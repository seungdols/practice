# 15장: Digester

> 번역 범위: 원본 PDF 305~313페이지. 313페이지는 목록 15.3의 `Test01` 코드 중간에서 끝난다.

## 개요

앞 장들에서는 `Bootstrap` 클래스를 사용하여 커넥터, 컨텍스트, 래퍼 및 기타 구성 요소를 생성했다. 객체를 생성한 뒤에는 여러 객체의 `set` 메서드를 호출하여 서로 연결했다. 예를 들어 커넥터와 컨텍스트를 생성하려면 다음 코드를 사용한다.

```java
Connector connector = new HttpConnector();
Context context = new StandardContext();
```

커넥터를 컨텍스트와 연결하려면 다음 코드를 작성한다.

```java
connector.setContainer(context);
```

또한 해당 `set` 메서드를 호출하여 각 객체의 속성을 설정할 수 있다. 예를 들어 `Context` 객체의 `path`와 `docBase` 속성은 `setPath()`와 `setDocBase()`로 설정한다.

```java
context.setPath("/myApp") ;
context.setDocBase("myApp");
```

그뿐 아니라 구성 요소를 생성한 뒤 컨텍스트의 해당 `add` 메서드를 호출하여 `Context` 객체에 여러 구성 요소를 추가할 수 있다. 다음은 생명주기 리스너와 로더를 컨텍스트 객체에 추가하는 예다.

```java
LifecycleListener listener = new SimpleContextConfig();
((Lifecycle) context).addLifecycleListener(listener);
Loader loader = new WebappLoader();
context.setLoader(loader);
```

필요한 연결과 추가 작업을 모두 마치면, 애플리케이션 시작을 완료하기 위해 커넥터의 `initialize()`와 `start()`, 컨텍스트의 `start()`를 호출한다.

```java
connector.initialize();
((Lifecycle) connector).start ();
((Lifecycle) context).start();
```

이러한 애플리케이션 구성 방식에는 분명한 단점이 하나 있다. 모든 것이 **하드코딩**되어 있다는 점이다. 구성 요소 하나 또는 속성값 하나만 바꾸어도 `Bootstrap` 클래스를 다시 컴파일해야 한다. 다행히 Tomcat 설계자들은 `server.xml`이라는 XML 문서를 사용하는 더 우아한 구성 방식을 선택했다. `server.xml` 파일의 각 요소는 Java 객체로 변환되고, 요소의 속성(attribute)은 객체의 프로퍼티를 설정하는 데 사용된다. 따라서 Tomcat 설정을 변경하려면 `server.xml`만 편집하면 된다.

예를 들어 `server.xml`의 `Context` 요소는 컨텍스트를 나타낸다.

```xml
<context/>
```

`path`와 `docBase` 프로퍼티를 설정하려면 XML 요소의 속성을 사용한다.

```xml
<context docBase="myApp" path="/myApp"/>
```

Tomcat은 XML 요소를 Java 객체로 변환하기 위해 오픈 소스 라이브러리 **Digester**를 사용한다. 이 장의 첫 번째 절에서는 Digester를 설명한다.

다음 절에서는 웹 애플리케이션의 구성을 설명한다. 컨텍스트는 웹 애플리케이션을 나타내므로, 웹 애플리케이션을 구성한다는 것은 생성된 `Context` 인스턴스를 설정한다는 의미다. 웹 애플리케이션 설정에 사용하는 XML 파일은 `web.xml`이며, 애플리케이션의 `WEB-INF` 디렉터리에 있어야 한다.

## Digester

Digester는 Apache의 Jakarta 프로젝트에 속한 Commons 하위 프로젝트의 오픈 소스 프로젝트다. 원문에 제시된 다운로드 주소는 `http://jakarta.apache.org/commons/digester/`이다. Digester API는 `commons-digester.jar`에 포함된 세 패키지로 구성된다.

- `org.apache.commons.digester`: 임의의 XML 문서를 규칙 기반으로 처리하는 기능을 제공한다.
- `org.apache.commons.digester.rss`: 많은 뉴스 피드에서 사용하는 Rich Site Summary 형식과 호환되는 XML 문서를 Digester로 파싱하는 예를 제공한다.
- `org.apache.commons.digester.xmlrules`: Digester의 규칙을 XML 기반으로 정의하는 기능을 제공한다.

이 세 패키지의 모든 구성원을 다루지는 않는다. 대신 Tomcat에서 사용하는 몇 가지 중요한 타입에 집중한다. 먼저 Digester 라이브러리에서 가장 중요한 타입인 `Digester` 클래스를 살펴보자.

### Digester 클래스

`org.apache.commons.digester.Digester`는 Digester 라이브러리의 핵심 클래스다. XML 문서를 파싱할 때 사용한다. 문서의 각 요소에 대해 `Digester` 객체는 수행해야 할 작업이 있는지 확인한다. 프로그래머는 `parse()` 메서드를 호출하기 전에 Digester 인스턴스가 어떤 작업을 수행할지 결정한다.

Digester 객체가 XML 요소를 만났을 때 무엇을 해야 하는지는 어떻게 알려 줄까? **패턴(pattern)**을 정의하고 각 패턴에 하나 이상의 **규칙(rule)**을 연결하면 된다. XML 문서의 루트 요소는 요소 이름과 동일한 패턴을 갖는다. 목록 15.1의 XML 문서를 살펴보자.

**목록 15.1: example.xml 파일**

```xml
<?xml version="1.0" encoding="ISO-8859-1"?>
<employee firstName="Brian" lastName="May">
  <office>
    <address streeName="Wellington Street" streetNumber="110"/>
  </office>
</employee>
```

XML 문서의 루트 요소는 `employee`이고 패턴도 `employee`다. `office`는 `<employee>`의 하위 요소다. 하위 요소의 패턴은 부모 요소의 패턴 뒤에 `/`와 하위 요소 이름을 붙여 만든다. 따라서 `office` 요소의 패턴은 `employee/office`다. `address` 요소의 패턴은 다음 식으로 나타낼 수 있다.

```text
부모 요소의 패턴 + "/" + 요소 이름
```

`address`의 부모는 `<office>`이며 그 패턴은 `employee/office`다. 따라서 `<address>`의 패턴은 `employee/office/address`다.

XML 요소에서 패턴을 얻는 방법을 이해했으니 이제 규칙을 살펴보자.

규칙은 특정 패턴을 만났을 때 Digester가 수행해야 할 하나 또는 여러 작업을 지정한다. 규칙은 `org.apache.commons.digester.Rule` 클래스로 표현된다. `Digester` 클래스에는 0개 이상의 `Rule` 객체가 들어 있다. Digester 인스턴스 안에서 규칙과 대응하는 패턴은 `org.apache.commons.digester.Rules` 인터페이스로 표현되는 저장소에 보관된다. Digester 인스턴스에 규칙을 추가할 때마다 해당 `Rule` 객체가 `Rules` 객체에 추가된다.

`Rule` 클래스에는 `begin()`과 `end()`를 비롯한 여러 메서드가 있다. Digester가 XML을 파싱하면서 패턴에 맞는 시작 태그를 만나면 연결된 `Rule` 객체들의 `begin()`을 호출한다. 종료 태그를 만나면 `end()`를 호출한다.

목록 15.1의 `example.xml`을 파싱할 때 Digester는 다음 순서로 동작한다.

1. `employee` 시작 태그를 만나면 `employee` 패턴에 대한 규칙이 있는지 확인한다. 있다면 Digester에 처음 추가된 규칙부터 각 규칙의 `begin()`을 실행한다.
2. `office` 시작 태그를 만나면 `employee/office` 패턴에 대한 규칙을 확인하고, 존재하면 해당 `begin()`을 실행한다.
3. `address` 시작 태그를 만나면 `employee/office/address` 패턴에 대한 규칙을 확인하고, 발견된 규칙들의 `begin()`을 실행한다.
4. `address` 종료 태그를 만나면 일치하는 규칙들의 `end()`를 실행한다.
5. `office` 종료 태그를 만나면 일치하는 규칙들의 `end()`를 실행한다.
6. 마지막으로 `employee` 종료 태그를 만나면 일치하는 규칙들의 `end()`를 실행한다.

어떤 규칙을 사용할 수 있을까? Digester에는 여러 규칙이 미리 정의되어 있다. 따라서 `Rule` 클래스를 자세히 이해하지 않아도 사용할 수 있다. 하지만 기본 규칙으로 충분하지 않다면 직접 규칙을 만들 수도 있다. 기본 제공 규칙에는 객체 생성, 프로퍼티값 설정 등을 위한 규칙이 포함된다.

### 객체 생성

특정 패턴을 발견했을 때 Digester 인스턴스가 객체를 생성하도록 하려면 `addObjectCreate()`를 호출한다. 이 메서드에는 네 가지 오버로드가 있으며, 자주 사용하는 두 가지의 시그니처는 다음과 같다.

```java
public void addObjectCreate(java.lang.String pattern,
  java.lang.Class clazz)

public void addObjectCreate(java.lang.String pattern,
  java.lang.String className)
```

패턴과 `Class` 객체 또는 클래스 이름을 전달한다. 예를 들어 `employee` 패턴을 만나면 `ex15.pyrmont.digestertest.Employee` 클래스의 `Employee` 객체를 생성하도록 하려면 다음 중 하나를 호출한다.

```java
digester.addObjectCreate("employee",
   ex15.pyrmont.digestertest.Employee.class);
```

또는 다음과 같이 호출한다.

```java
digester.addObjectCreate("employee",
   "ex15.pyrmont.digestertest.Employee");
```

나머지 두 `addObjectCreate()` 오버로드는 클래스 이름을 메서드 인수 대신 XML 요소에 정의할 수 있게 한다. 클래스 이름을 실행 시점에 결정할 수 있다는 점에서 매우 강력한 기능이다. 시그니처는 다음과 같다.

```java
public void addObjectCreate(java.lang.String pattern,
  java.lang.String className, java.lang.String attributeName)

public void addObjectCreate(java.lang.String pattern,
  java.lang.String attributeName, java.lang.Class clazz)
```

이 두 오버로드에서 `attributeName`은 생성할 클래스의 이름을 담고 있는 XML 요소의 속성 이름을 지정한다. 예를 들어 객체 생성 규칙을 추가하려면 다음 코드를 사용할 수 있다.

```java
digester.addObjectCreate("employee", null, "className");
```

여기서 속성 이름은 `className`이다. 그러면 XML 요소에 클래스 이름을 전달할 수 있다.

```xml
<employee firstName="Brian" lastName="May"
  className="ex15.pyrmont.digestertest.Employee">
```

또는 다음과 같이 `addObjectCreate()`에서 기본 클래스 이름을 정의할 수도 있다.

```java
digester.addObjectCreate("employee",
   "ex15.pyrmont.digestertest.Employee", "className");
```

`employee` 요소에 `className` 속성이 있으면 해당 속성값을 생성할 클래스 이름으로 사용한다. `className` 속성이 없으면 기본 클래스 이름을 사용한다.

`addObjectCreate()`가 생성한 객체는 내부 스택에 푸시된다. 생성된 객체를 확인(`peek`), 푸시(`push`), 팝(`pop`)하기 위한 여러 메서드도 제공된다.

### 프로퍼티 설정

또 다른 유용한 메서드는 `addSetProperties()`다. 이 메서드를 사용하면 Digester 객체가 객체의 프로퍼티를 설정하도록 할 수 있다. 오버로드 중 하나의 시그니처는 다음과 같다.

```java
public void addSetProperties(java.lang.String pattern)
```

이 메서드에는 패턴을 전달한다. 다음 코드를 살펴보자.

```java
digester.addObjectCreate("employee",
   "ex15.pyrmont.digestertest.Employee");
digester.addSetProperties("employee");
```

위 Digester 인스턴스에는 객체 생성과 프로퍼티 설정이라는 두 규칙이 있다. 둘 다 `employee` 패턴을 만나면 실행되도록 설정되어 있다. 규칙은 Digester 인스턴스에 추가한 순서대로 실행된다. XML 문서에 다음 요소가 있다고 하자.

```xml
<employee firstName="Brian" lastName="May">
```

첫 번째 규칙 덕분에 Digester는 `ex15.pyrmont.digestertest.Employee` 인스턴스를 먼저 생성한다. 그런 다음 두 번째 규칙에 따라 생성한 `Employee` 객체의 `setFirstName()`과 `setLastName()`을 호출하며 각각 `Brian`, `May`를 전달한다. `employee` 요소의 속성은 `Employee` 객체의 프로퍼티에 대응한다. `Employee` 클래스에 필요한 프로퍼티 중 하나라도 정의되어 있지 않으면 오류가 발생한다.

### 메서드 호출

Digester 클래스에서는 대응하는 패턴을 만났을 때 스택 맨 위 객체의 메서드를 호출하는 규칙을 추가할 수 있다. 이를 위한 메서드는 원문에 `addcallMethod`로 표기되어 있으며, 오버로드 중 하나의 시그니처는 다음과 같다.

```java
public void addCallMethod (java.lang.String pattern,
  java.lang.String methodName)
```

### 객체 사이의 관계 생성

Digester 인스턴스에는 객체를 임시로 저장하는 내부 스택이 있다. `addObjectCreate()`가 클래스를 인스턴스화하면 결과 객체를 스택에 푸시한다. 이 스택을 우물이라고 생각해 보자. 객체를 스택에 푸시하는 것은 우물과 지름이 같은 둥근 물체를 우물 안에 떨어뜨리는 것과 같다. 객체를 팝하는 것은 우물 맨 위의 물체를 들어 올리는 것과 같다.

`addObjectCreate()`를 두 번 호출하면 첫 번째 객체가 먼저 우물에 들어가고 그다음 두 번째 객체가 들어간다. `addSetNext()`는 첫 번째 객체의 지정된 메서드를 호출하면서 두 번째 객체를 인수로 전달하여 두 객체 사이의 관계를 생성하는 데 사용한다. 시그니처는 다음과 같다.

```java
public void addSetNext(java.lang.String pattern,
  java.lang.String methodName)
```

`pattern` 인수는 이 규칙을 실행할 패턴을 지정한다. `methodName`은 첫 번째 객체에서 호출할 메서드의 이름이다. 패턴은 `firstObject/secondObject` 형태여야 한다.

예를 들어 직원(employee)은 사무실(office)을 가질 수 있다. 직원과 사무실 사이의 관계를 만들려면 먼저 다음과 같이 두 개의 `addObjectCreate()`를 사용해야 한다.

```java
digester.addObjectCreate("employee",
   "ex15.pyrmont.digestertest.Employee");
digester.addObjectCreate("employee/office",
   "ex15.pyrmont.digestertest.Office");
```

첫 번째 `addObjectCreate()`는 `employee` 요소를 만나면 `Employee` 인스턴스를 생성한다. 두 번째는 `<employee>` 아래의 `<office>`를 만나면 `Office` 인스턴스를 생성한다.

두 메서드는 객체 두 개를 스택에 푸시한다. 이제 `Employee` 객체는 아래쪽에, `Office` 객체는 위쪽에 있다. 둘 사이의 관계를 만들려면 다음과 같이 `addSetNext()`로 또 다른 규칙을 정의한다.

```java
digester.addSetNext("employee/office", "addOffice");
```

여기서 `addOffice`는 `Employee` 클래스의 메서드다. 이 메서드는 `Office` 객체를 인수로 받아야 한다. 이 절의 두 번째 Digester 예제에서 `addSetNext()`의 사용법을 더 명확히 설명한다.

### XML 문서 검증

Digester가 파싱하는 XML 문서는 스키마를 기준으로 검증할 수 있다. XML 문서를 검증할지는 Digester의 `validating` 프로퍼티로 결정하며, 기본값은 `false`다.

검증을 수행할지 지정하는 `setValidating()`의 시그니처는 다음과 같다.

```java
public void setValidating(boolean validating)
```

XML 문서가 올바른 형식인지 검증하려면 `setValidating()`에 `true`를 전달한다.

### Digester 예제 1

첫 번째 예제는 Digester를 사용하여 객체를 동적으로 생성하고 프로퍼티를 설정하는 방법을 설명한다. Digester로 인스턴스화할 목록 15.2의 `Employee` 클래스를 살펴보자.

**목록 15.2: Employee 클래스**

```java
package ex15.pyrmont.digestertest;

import java.util.ArrayList;

public class Employee {
  private String firstName;
  private String lastName;
  private ArrayList offices = new ArrayList();

  public Employee () {
    System.out.println ("Creating Employee");
  }
  public String getFirstName() {
    return firstName;
  }
  public void setFirstName(String firstName) {
    System.out.println("Setting firstName : " + firstName);
    this.firstName = firstName;
  }
  public String getLastName() {
    return lastName;
  }
  public void setLastName(String lastName) {
    System.out.println("Setting lastName : " + lastName);
    this.lastName = lastName;
  }
  public void addOffice(Office office) {

    System.out.println("Adding Office to this employee");
    offices.add(office);
  }
  public ArrayList getOffices() {
    return offices;
  }
  public void printName() {
    System.out.println("My name is " + firstName + " " + lastName);
  }
}
```

`Employee` 클래스에는 `firstName`, `lastName`, `office`라는 세 가지 프로퍼티가 있다. `firstName`과 `lastName`은 문자열이고, `office`는 원문 설명상 `ex15.pyrmont.digester.Office` 타입이다. `office` 프로퍼티는 두 번째 Digester 예제에서 사용한다. 또한 `Employee`에는 이름과 성을 콘솔에 출력하는 `printName()` 메서드가 있다.

이제 Digester를 사용하고 `Employee` 객체 생성 및 프로퍼티 설정 규칙을 추가하는 테스트 클래스를 작성해 보자. 목록 15.3의 `Test01` 클래스를 사용할 수 있다.

**목록 15.3: Test01 클래스 — 원본 PDF 313페이지까지의 코드**

```java
package ex15.pyrmont.digestertest;

import java.io.File;
import org.apache.commons.digester.Digester;

public class Test01 {

    public static void main(String[] args) {
      String path = System.getProperty("user.dir") + File.separator +
         "etc";
      File file = new File(path, "employee1.xml");
```

> `Test01` 클래스의 나머지 코드는 원본 PDF 314페이지부터 이어진다. 원문 XML의 `streeName` 표기와 본문의 패키지 이름 차이도 임의로 수정하지 않았다.


## 목록 15.3: Test01 클래스 (계속) — PDF 314페이지

```java
         Digester digester = new Digester();
         // add rules
         digester.addObjectCreate("employee",
            "ex15.pyrmont.digestertest.Employee");
         digester.addSetProperties("employee");
         digester.addCallMethod("employee", "printName");

         try {
            Employee employee = (Employee) digester.parse(file);
            System.out.println("First name : " + employee.getFirstName());
            System.out.println("Last name : " + employee.getLastName());
         }
         catch(Exception e) {
            e.printStackTrace();
         }
    }
}
```

먼저 XML 문서의 위치를 나타내는 경로를 정의하고 `File` 클래스 생성자에 전달한다. 그런 다음 `Digester` 객체를 생성하고, 패턴이 `employee`인 세 가지 규칙을 추가한다.

```java
        digester.addObjectCreate("employee",
           "ex15.pyrmont.digestertest.Employee");
        digester.addSetProperties("employee");
        digester.addCallMethod("employee", "printName");
```

이어서 XML 문서를 참조하는 `File` 객체를 인수로 전달하여 `Digester` 객체의 `parse()` 메서드를 호출한다. `parse()` 메서드는 Digester 내부 스택의 첫 번째 객체를 반환한다.

```java
        Employee employee = (Employee) digester.parse(file);
```

이렇게 하면 Digester가 생성한 `Employee` 객체를 얻는다. 이 객체의 프로퍼티가 설정되었는지 확인하려면 `getFirstName()`과 `getLastName()`을 호출한다.

```java
          System.out.println("First name : " + employee.getFirstName());
          System.out.println("Last name : " + employee.getLastName());
```

목록 15.4는 루트 요소가 `employee`인 `employee1.xml` 문서다. 이 요소에는 `firstName`과 `lastName`이라는 두 속성이 있다.

### 목록 15.4: employee1.xml 파일

```xml
<?xml version="1.0" encoding="ISO-8859-1"?>
<employee firstName="Brian" lastName="May">
</employee>
```

`Test01` 클래스를 실행하면 다음 결과가 출력된다.

```text
Creating Employee
Setting firstName : Brian
Setting lastName : May
My name is Brian May
First name : Brian
Last name : May
```

이 과정에서 어떤 일이 일어나는지 살펴보자.

`Digester` 객체의 `parse()` 메서드를 호출하면 XML 문서를 열고 파싱을 시작한다. 먼저 Digester는 `employee` 시작 요소를 발견한다. 그러면 `employee` 패턴에 등록된 세 가지 규칙이 **추가된 순서대로** 실행된다. 첫 번째는 객체 생성 규칙이다. 따라서 Digester는 `Employee` 클래스를 인스턴스화하며, 생성자가 호출되어 `Creating Employee`가 출력된다.

두 번째 규칙은 `Employee` 객체의 속성을 설정한다. `employee` 요소에는 `firstName`, `lastName` 두 속성이 있다. 이 규칙은 해당 프로퍼티의 setter 메서드를 호출하므로 다음 문자열이 출력된다.

```text
Setting firstName : Brian
Setting lastName : May
```

세 번째 규칙은 `printName()`을 호출하며 다음 텍스트를 출력한다.

```text
My name is Brian May
```

마지막 두 줄은 `Employee` 객체의 `getFirstName()`과 `getLastName()`을 호출한 결과다.

```text
First name : Brian
Last name : May
```

## Digester 예제 2 — PDF 316~318페이지

두 번째 Digester 예제는 객체 두 개를 생성하고 그 사이에 관계를 설정하는 방법을 보여준다. 어떤 관계를 만들지도 직접 정의한다. 예를 들어 직원은 하나 이상의 사무실에서 근무한다. 사무실은 `Office` 클래스로 표현한다. `Employee` 객체와 `Office` 객체를 생성한 다음 두 객체 사이의 관계를 설정할 수 있다. 목록 15.5에 `Office` 클래스가 나와 있다.

### 목록 15.5: Office 클래스

```java
package ex15.pyrmont.digestertest;

public class Office {
  private Address address;
  private String description;
  public Office() {
     System.out.println("..Creating Office");
  }
  public String getDescription() {
     return description;
  }
  public void setDescription(String description) {
     System.out.println("..Setting office description : " +
description);
     this.description = description;
  }
  public Address getAddress() {
     return address;
  }
  public void setAddress(Address address) {
     System.out.println("..Setting office address : " + address);

        this.address = address;
    }
}
```

부모 객체의 메서드를 호출하여 관계를 생성한다. 이 예제는 목록 15.2의 `Employee` 클래스를 사용한다. `Employee` 클래스에는 `Office` 객체를 자신의 `offices` 컬렉션에 추가하는 `addOffice()` 메서드가 있다.

Digester를 사용하지 않는다면 Java 코드는 다음과 같다.

```java
Employee employee = new Employee();
Office office = new Office();
employee.addOffice(office);
```

사무실에는 주소가 있으며, 주소는 목록 15.6의 `Address` 클래스로 표현한다.

### 목록 15.6: Address 클래스

```java
package ex15.pyrmont.digestertest;

public class Address {
  private String streetName;
  private String streetNumber;
  public Adress () {
     System.out.println("....Creating Address");
  }
  public String getStreetName() {
     return streetName;
  }
  public void setStreetName(String streetName) {
     System.out.println("....Setting streetName : " + streetName);
     this.streetName = streetName;
  }
  public String getStreetNumber() {
     return streetNumber;
  }
  public void setStreetNumber(String streetNumber) {
     System.out.println("....Setting streetNumber : " + streetNumber);
     this.streetNumber = streetNumber;
  }
  public String toString() {
     return "...." + streetNumber + " " + streetName;
  }
}
```

사무실에 주소를 할당하려면 `Office` 클래스의 `setAddress()` 메서드를 호출하면 된다. Digester 없이 작성한 코드는 다음과 같다.

```java
Office office = new Office();
Address address = new Address();
office.setAddress (address);
```

두 번째 Digester 예제는 객체를 생성하고 객체 사이의 관계를 맺는 방법을 보여준다. 여기서는 `Employee`, `Office`, `Address` 클래스를 사용한다. 목록 15.7의 `Test02` 클래스는 Digester를 생성하고 규칙을 추가한다.

### 목록 15.7: Test02 클래스 (PDF 318페이지까지)

```java
package ex15.pyrmont.digestertest;

import java.io.File;
import java.util.*;
import org.apache.commons.digester.Digester;

public class Test02 {

  public static void main(String[] args) {
    String path = System.getProperty("user.dir") + File.separator +
       "etc";
    File file = new File(path, "employee2.xml");
    Digester digester = new Digester();
    // add rules
    digester.addObjectCreate("employee",
       "ex15.pyrmont.digestertest.Employee");
    digester.addSetProperties("employee");
    digester.addObjectCreate("employee/office",
       "ex15.pyrmont.digestertest.Office");
    digester.addSetProperties("employee/office");
    digester.addSetNext("employee/office", "addOffice");
    digester.addObjectCreate("employee/office/address",
       "ex15.pyrmont.digestertest.Address");
    digester.addSetProperties("employee/office/address");
    digester.addSetNext("employee/office/address", "setAddress");
    try {
       Employee employee = (Employee) digester.parse(file);
       ArrayList offices = employee.getOffices();
       Iterator iterator = offices.iterator();
       System.out.println(
          "-------------------------------------------------");
       while (iterator.hasNext()) {
          Office office = (Office) iterator.next();
          Address address = office.getAddress();
          System.out.println(office.getDescription());
          System.out.println("Address : " +
             address.getStreetNumber() + " " + address.getStreetName());
          System.out.println(" -------------------------------");
```

> **원문 표기 주의:** 목록 15.6의 생성자 이름은 원문에서 `Adress`로 표기되어 있다. 클래스 이름 `Address`와 일치하지 않으므로 그대로 컴파일하면 오류가 발생한다. 번역에서는 원문 코드를 수정하지 않았다.
>
> 목록 15.7의 `Test02` 코드는 PDF 319페이지에서 계속된다.

## PDF 319~323페이지 — Digester 예제 2의 결과와 Rule 클래스, RuleSet

### 예제 2: `Test02`의 나머지 코드 (PDF 319페이지)

앞 페이지의 목록 15.7은 다음 코드로 끝난다.

```java
          }
        }
        catch(Exception e) {
          e.printStackTrace();
        }
    }
}
```

Digester가 실제로 동작하는 모습을 확인하려면 목록 15.8의 XML 문서 `employee2.xml`을 사용하면 된다.

**목록 15.8: `employee2.xml` 파일**

```xml
<?xml version="1.0" encoding="ISO-8859-1"?>
<employee firstName="Freddie" lastName="Mercury">
  <office description="Headquarters">
    <address streetName="Wellington Avenue" streetNumber="223"/>
  </office>
  <office description="Client site">
    <address streetName="Downing Street" streetNumber="10"/>
  </office>
</employee>
```

`Test02` 클래스를 실행하면 다음과 같은 결과가 나온다.

```text
Creating Employee
Setting firstName : Freddie
Setting lastName : Mercury
..Creating Office
..Setting office description : Headquarters
....Creating Address
....Setting streetName : Wellington Avenue
....Setting streetNumber : 223
..Setting office address : ....223 Wellington Avenue
Adding Office to this employee
..Creating Office
..Setting office description : Client site
....Creating Address
....Setting streetName : Downing Street
....Setting streetNumber : 10
..Setting office address : ....10 Downing Street
Adding Office to this employee
-------------------------------------------------
Headquarters
Address : 223 Wellington Avenue
--------------------------------
Client site
Address : 10 Downing Street
--------------------------------
```

### `Rule` 클래스 (PDF 320~321페이지)

`Rule` 클래스에는 여러 메서드가 있지만 가장 중요한 두 메서드는 `begin`과 `end`이다. `Digester` 인스턴스가 XML 요소의 시작 부분을 만나면 해당 요소와 패턴이 일치하는 모든 `Rule` 객체의 `begin` 메서드를 호출한다. `Rule` 클래스의 `begin` 메서드 시그니처는 다음과 같다.

```java
public void begin(org.xml.sax.Attributes attributes)
  throws java.lang.Exception
```

`Digester` 인스턴스가 XML 요소의 끝 부분을 만나면 패턴이 일치하는 모든 `Rule` 인스턴스의 `end` 메서드를 호출한다. `Rule` 클래스의 `end` 메서드 시그니처는 다음과 같다.

```java
public void end() throws java.lang.Exception
```

앞선 예제에서 `Digester` 객체는 어떻게 이런 일을 해낼까? `Digester`의 `addObjectCreate`, `addCallMethod`, `addSetNext` 및 기타 메서드를 호출할 때마다 간접적으로 `Digester` 클래스의 `addRule` 메서드를 호출하게 된다. 이 메서드는 `Rule` 객체와 그에 대응하는 패턴을 `Digester` 내부의 `Rules` 컬렉션에 추가한다.

`addRule` 메서드의 시그니처는 다음과 같다.

```java
public void addRule(java.lang.String pattern, Rule rule)
```

`Digester` 클래스에서 `addRule` 메서드의 구현은 다음과 같다.

```java
public void addRule(String pattern, Rule rule) {
  rule.setDigester(this);
  getRules().add(pattern, rule);
}
```

`Digester` 클래스 소스 코드에서 오버로딩된 `addObjectCreate` 메서드들을 살펴보자.

```java
public void addObjectCreate(String pattern, String className) {
  addRule(pattern, new ObjectCreateRule(className));
}
public void addObjectCreate(String pattern, Class clazz) {
  addRule(pattern, new ObjectCreateRule(clazz));
}
public void addObjectCreate(String pattern, String className,
  String attributeName) {
  addRule(pattern, new ObjectCreateRule(className, attributeName));
}
public void addObjectCreate(String pattern,
  String attributeName, Class clazz) {
  addRule(pattern, new ObjectCreateRule(attributeName, clazz));
}
```

네 가지 오버로드 모두 `addRule` 메서드를 호출한다. `addRule`의 두 번째 인자로 인스턴스가 생성되는 `ObjectCreateRule` 클래스는 `Rule` 클래스의 하위 클래스이다. `ObjectCreateRule` 클래스의 `begin`과 `end` 메서드 구현을 살펴보자.

```java
public void begin(Attributes attributes) throws Exception {
  // Identify the name of the class to instantiate
  String realClassName = className;
  if (attributeName != null) {
      String value = attributes.getValue(attributeName);

      if (value != null) {
          realClassName = value;
      }
    }
    if (digester.log.isDebugEnabled()) {
        digester.log.debug("[ObjectCreateRule]{" + digester.match +
        "}New " + realClassName);
    }

    // Instantiate the new object and push it on the context stack
    Class clazz = digester.getClassLoader().loadclass(realClassName);
    Object instance = clazz.newInstance();
    digester.push(instance);
}

public void end() throws Exception {
    Object top = digester.pop();
    if (digester.log.isDebugEnabled()) {
        digester.log.debug("[ObjectCreateRule]{" + digester.match +
           "} Pop " + top.getdass().getName());
      }
}
```

`begin` 메서드의 마지막 세 줄은 객체의 인스턴스를 생성한 다음 `Digester` 내부 스택에 넣는다. `end` 메서드는 스택에서 객체를 꺼낸다.

`Rule` 클래스의 다른 하위 클래스들도 비슷하게 작동한다. 각 규칙이 내부적으로 어떻게 동작하는지 더 자세히 알고 싶다면 소스 코드를 열어보면 된다.

> **원문 코드 주의:** 위 코드의 `loadclass` 및 `getdass()`는 PDF 원문 표기대로 유지했다. 실제 Java API 명칭과 다를 수 있다.

### Digester 예제 3: `RuleSet` 사용하기 (PDF 322~323페이지)

`Digester` 인스턴스에 규칙을 추가하는 또 다른 방법은 `addRuleSet` 메서드를 호출하는 것이다. 이 메서드의 시그니처는 다음과 같다.

```java
public void addRuleSet(RuleSet ruleSet)
```

`org.apache.commons.digester.RuleSet` 인터페이스는 `Rule` 객체들의 집합을 나타낸다. 이 인터페이스에는 `addRuleInstance`와 `getNamespaceURI`라는 두 메서드가 정의되어 있다. `addRuleInstance`의 시그니처는 다음과 같다.

```java
public void addRuleInstance(Digester digester)
```

`addRuleInstance` 메서드는 현재 `RuleSet`에 정의된 `Rule` 객체들의 집합을 인자로 전달된 `Digester` 인스턴스에 추가한다.

`getNamespaceURI`는 이 `RuleSet`에서 생성되는 모든 `Rule` 객체에 적용할 네임스페이스 URI를 반환한다. 시그니처는 다음과 같다.

```java
public java.lang.String getNamespaceURI()
```

따라서 `Digester` 객체를 생성한 다음 `RuleSet` 객체를 만들고, 이를 `Digester`의 `addRuleSet` 메서드에 전달할 수 있다.

편의를 위한 기본 클래스인 `RuleSetBase`는 `RuleSet`을 구현한다. `RuleSetBase`는 `getNamespaceURI`를 구현해 둔 추상 클래스다. 따라서 `addRuleInstances` 메서드만 구현하면 된다.

예를 들어 앞선 예제의 `Test02` 클래스를 수정해 목록 15.9의 `EmployeeRuleSet` 클래스를 도입해 보자.

**목록 15.9: `EmployeeRuleSet` 클래스**

```java
package ex15.pyrmont.digestertest;

import org.apache.commons.digester.Digester;
import org.apache.commons.digester.RuleSetBase;

public class EmployeeRuleSet extends RuleSetBase {
  public void addRuleInstances(Digester digester) {
     // add rules
     digester.addObjectCreate("employee",
        "ex15.pyrmont.digestertest.Employee");
     digester.addSetProperties("employee");
     digester.addObjectCreate("employee/office",
        "ex15.pyrmont.digestertest.Office");
     digester.addSetProperties("employee/office");
     digester.addSetNext("employee/office", "addOffice");
     digester.addObjectCreate("employee/office/address",
        "ex15.pyrmont.digestertest.Address");
     digester.addSetProperties("employee/office/address");
     digester.addSetNext("employee/office/address", "setAddress");
  }
}
```

`EmployeeRuleSet` 클래스의 `addRuleInstances` 구현이 `Test02` 클래스와 동일한 규칙들을 `Digester`에 추가한다는 점에 주목하자. 목록 15.10의 `Test03` 클래스는 `EmployeeRuleSet` 인스턴스를 생성한 다음 앞서 생성한 `Digester`에 이를 추가한다.

**목록 15.10: `Test03` 클래스 (PDF 323페이지부터 시작)**

```java
package ex15.pyrmont.digestertest;

import java.io.File;
import java.util.ArrayList;
import java.util.Iterator;
import org.apache.commons.digester.Digester;

public class Test03 {

  public static void main(String[] args) {
    String path = System.getProperty("user.dir") +
```

> 목록 15.10의 나머지 코드는 **PDF 324페이지**에서 이어진다.


## 목록 15.10: `Test03` 클래스 (계속, PDF 324페이지)

```java
           File.separator + "etc";
        File file = new File(path, "employee2.xml");
        Digester digester = new Digester();
        digester.addRuleSet(new EmployeeRuleSet());
        try {
           Employee employee = (Employee) digester.parse(file);
           ArrayList offices = employee.getOffices();
           Iterator iterator = offices.iterator();
           System.out.println(
              "-------------------------------------------------");
           while (iterator.hasNext()) {
              Office office = (Office) iterator.next();
              Address address = office.getAddress();
              System.out.println(office.getDescription());
              System.out.println("Address : " +
                 address.getStreetNumber() + " " + address.getStreetName());
              System.out.println ("-------------------------------");
           }
        }
        catch(Exception e) {
           e.printStackTrace ();
        }
    }
}
```

`Test03` 클래스를 실행하면 `Test02` 클래스와 동일한 출력이 생성된다. 하지만 `Test03`의 코드가 더 짧다는 점에 주목하자. `Rule` 객체를 추가하는 코드가 이제 `EmployeeRuleSet` 클래스 안에 숨겨져 있기 때문이다.

뒤에서 살펴보겠지만 Catalina는 서버와 다른 구성 요소를 초기화할 때 `RuleSetBase`의 하위 클래스를 사용한다. 다음 절에서는 `Digester`가 Catalina에서 얼마나 중요한 역할을 하는지 알아본다.

## `ContextConfig` (PDF 324~325페이지)

다른 종류의 컨테이너와 달리 `StandardContext`에는 반드시 리스너가 있어야 한다. 이 리스너는 `StandardContext` 인스턴스를 구성하고, 구성이 성공적으로 끝나면 `StandardContext`의 `configured` 변수를 `true`로 설정한다. 앞 장들에서는 `SimpleContextConfig` 클래스를 `StandardContext`의 리스너로 사용했다. 이 클래스는 `StandardContext`의 `start` 메서드가 계속 실행될 수 있도록 `configured` 변수를 설정하는 것이 유일한 목적인 매우 단순한 클래스였다.

실제 Tomcat 배포 환경에서 `StandardContext`의 표준 리스너는 `org.apache.catalina.startup.ContextConfig` 클래스의 인스턴스다. 단순한 `SimpleContextConfig`와 달리 `ContextConfig`는 `StandardContext`가 정상적으로 작동하는 데 꼭 필요한 여러 작업을 수행한다. 예를 들어 `StandardContext`와 연결된 `ContextConfig` 인스턴스는 인증기 밸브(authenticator valve)를 `StandardContext`의 파이프라인에 설치한다. 또한 `org.apache.catalina.valves.CertificateValve` 유형의 인증서 밸브도 파이프라인에 추가한다.

더 중요한 점은 `ContextConfig` 인스턴스가 기본 `web.xml` 파일과 애플리케이션의 `web.xml` 파일을 읽고 파싱하여 XML 요소를 Java 객체로 변환한다는 것이다. 기본 `web.xml` 파일은 `CATALINA_HOME`의 `conf` 디렉터리에 있다. 이 파일은 기본 서블릿을 정의하고 매핑하며, 파일 확장자를 MIME 유형과 연결하고, 기본 세션 타임아웃을 정의하고, 시작 페이지(welcome file)를 나열한다. 지금 이 파일을 열어 내용을 확인해 보자.

애플리케이션 `web.xml`은 애플리케이션의 `WEB-INF` 디렉터리에 있는 구성 파일이다. 두 파일 모두 필수는 아니다. 어느 파일도 찾지 못하더라도 `ContextConfig`는 처리를 계속한다.

`ContextConfig`는 각각의 `servlet` 요소에 대해 `StandardWrapper` 인스턴스를 생성한다. 따라서 이 장의 예제 애플리케이션에서 볼 수 있듯이 구성이 훨씬 쉬워진다. 더 이상 래퍼를 직접 인스턴스화할 필요가 없다.

그러므로 부트스트랩 클래스의 적절한 위치에서 `ContextConfig` 클래스를 인스턴스화하고 `org.apache.catalina.Lifecycle` 인터페이스의 `addLifecycleListener` 메서드를 호출하여 `StandardContext`에 추가해야 한다.

```java
LifecycleListener listener = new ContextConfig();
((Lifecycle) context).addLifecycleListener(listener);
```

`StandardContext`는 시작할 때 다음 이벤트를 발생시킨다.

- `BEFORE_START_EVENT`
- `START_EVENT`
- `AFTER_START_EVENT`

중지할 때는 다음 이벤트를 발생시킨다.

- `BEFORE_STOP_EVENT`
- `STOP_EVENT`
- `AFTER_STOP_EVENT`

`ContextConfig` 클래스는 `START_EVENT`와 `STOP_EVENT`, 두 이벤트에 반응한다. `StandardContext`가 이벤트를 발생시킬 때마다 `lifecycleEvent` 메서드가 호출된다. 이 메서드는 목록 15.11에 나와 있다. `stop` 메서드를 더 쉽게 이해할 수 있도록 목록 15.11에 주석을 추가했다.

**목록 15.11: `ContextConfig`의 `lifecycleEvent` 메서드**

```java
public void lifecycleEvent(LifecycleEvent event) {
  // Identify the context we are associated with
  try {
      context = (Context) event.getLifecycle();
      if (context instanceof StandardContext) {
          int contextDebug = ((StandardContext) context).getDebug();
          if (contextDebug > this.debug)
              this.debug = contextDebug;
      }
  }
  catch (ClassCastException e) {
      log(sm.getString("contextConfig.cce", event.getLifecycle()), e);
      return;
  }
  // Process the event that has occurred
  if (event.getType().equals(Lifecycle.START_EVENT))
      start();
  else if (event.getType().equals(Lifecycle.STOP_EVENT))
      stop();
}
```

`lifecycleEvent` 메서드의 끝부분에서 볼 수 있듯이, 이 메서드는 `start` 또는 `stop` 메서드를 호출한다. `start` 메서드는 목록 15.12에 나와 있다. 이 메서드의 본문에서 `defaultConfig`와 `applicationConfig` 메서드를 호출한다는 점에 주목하자. 두 메서드는 다음 절들에서 설명한다.

**목록 15.12: `ContextConfig`의 `start` 메서드 (PDF 326~327페이지)**

```java
private synchronized void start() {
   if (debug > 0)
       log(sm.getString("ContextConfig.start"));
   // reset the configured boolean
   context.setConfigured(false);
   // a flag that indicates whether the process is still
   // going smoothly
   ok = true;
   // Set properties based on DefaultContext
   Container container = context.getParent();
   if( !context.getOverride() ) {
       if( container instanceof Host ) {
           ((Host)container).importDefaultContext(context);
           container = container.getParent();
       }
       if( container instanceof Engine ) {
           ((Engine)container).importDefaultContext(context);
       }
   }

   // Process the default and application web.xml files
   defaultConfig();
   applicationConfig();
   if (ok) {
       validateSecurityRoles();
   }

   // Scan tag library descriptor files for additional listener classes
   if (ok) {
       try {
          tldScan();
       }
       catch (Exception e) {
          log(e.getMessage(), e);
          ok = false;
       }
   }
   // Configure a certificates exposer valve, if required
   if (ok)
       certificatesConfig();

   // Configure an authenticator if we need one
   if (ok)
       authenticatorConfig();
   // Dump the contents of this pipeline if requested
   if ((debug >= 1) && (context instanceof ContainerBase)) {
       log("Pipline Configuration:");
       Pipeline pipeline = ((ContainerBase) context).getPipeline();
       Valve valves[] = null;
       if (pipeline != null)
           valves = pipeline.getValves();
       if (valves != null) {
           for (int i = 0; i < valves.length; i++) {
              log(" " + valves[i].getInfo());
           }
       }
       log("======================");
   }
   // Make our application available if no problems were encountered
   if (ok)
       context.setConfigured(true);
   else {
       log(sm.getString("contextConfig.unavailable"));
       context.setConfigured(false);
   }
}
```

> 원문 코드의 `"Pipline Configuration:"` 표기는 수정하지 않았다.

## `defaultConfig` 메서드 (PDF 328페이지)

`defaultConfig` 메서드는 `%CATALINA_HOME%/conf` 디렉터리의 기본 `web.xml` 파일을 읽고 파싱한다. `defaultConfig` 메서드는 목록 15.13에 제시되어 있다.

**목록 15.13: `defaultConfig` 메서드 (시작)**

```java
private void defaultConfig() {
   // Open the default web.xml file, if it exists
   File file = new File(Constants.DefaultWebXml);
   if (!file.isAbsolute())
       file = new File(System.getProperty("catalina.base"),
           Constants.DefaultWebXml);
   FileInputStream stream = null;
   try {
       stream = new FileInputStream(file.getCanonicalPath());
       stream.close();
       stream = null;
   }
   catch (FileNotFoundException e) {
       log(sm.getString("contextConfig.defaultMissing"));
       return;
   }
   catch (IOException e) {
       log(sm.getString("contextConfig.defaultMissing"), e);
       return;
   }
   // Process the default web.xml file
```

> 목록 15.13의 나머지 코드는 **원본 PDF 329페이지**에서 이어진다.

## `defaultConfig` 메서드 (계속, PDF 329페이지)

**목록 15.13: `defaultConfig` 메서드 (앞 페이지에서 계속)**

```java
    synchronized (webDigester) {
       try {
          InputSource is =
              new InputSource("file://" + file.getAbsolutePath());
          stream = new FileInputStream(file);
          is.setByteStream(stream);
          webDigester.setDebug(getDebug());
          if (context instanceof StandardContext)
              ((StandardContext) context).setReplaceWelcomeFiles(true);
          webDigester.clear();
          webDigester.push(context);
          webDigester.parse(is);
     }
     catch (SAXParseException e) {
        log(sm.getString("contextConfig.defaultParse"), e);
        log(sm.getString("contextConfig.defaultPosition",
            "" + e.getLineNumber(), "" + e.getColumnNumber()));
        ok = false;
     }
     catch (Exception e) {
        log(sm.getString("contextConfig.defaultParse"), e);
        ok = false;
     }
     finally {
        try {
            if (stream != null) {
                stream.close();
            }
        }
        catch (IOException e) {
            log(sm.getString("contextConfig.defaultClose"), e);
        }
     }

    }
}
```

`defaultConfig` 메서드는 먼저 기본 `web.xml` 파일을 참조하는 `File` 객체를 생성한다.

```java
File file = new File(Constants.DefaultWebXml);
```

`DefaultWebXml`의 값은 `org.apache.catalina.startup.Constants` 클래스에서 다음과 같이 정의되어 있다.

```java
public static final String DefaultWebXml = "conf/web.xml";
```

그다음 `defaultConfig` 메서드는 `web.xml` 파일을 처리한다. `webDigester` 객체를 잠근 뒤 파일을 파싱한다.

```java
  synchronized (webDigester) {
    try {
       InputSource is =
           new InputSource("file://" + file.getAbsolutePath());
       stream = new FileInputStream(file);
       is.setByteStream(stream);
       webDigester.setDebug(getDebug());
       if (context instanceof StandardContext)
           ((StandardContext) context).setReplaceWelcomeFiles(true);
       webDigester.clear();
       webDigester.push(context);
       webDigester.parse(is);
```

`webDigester` 객체 변수는 `web.xml` 파일을 처리하는 규칙들이 등록된 `Digester` 인스턴스를 참조한다. 이에 대해서는 이 절 뒤쪽의 「웹 Digester 생성(Creating Web Digester)」에서 설명한다.

## `applicationConfig` 메서드 (PDF 330~331페이지)

`applicationConfig` 메서드는 `defaultConfig`와 비슷하지만, 애플리케이션의 배포 서술자(deployment descriptor)를 처리한다는 차이가 있다. 배포 서술자는 애플리케이션 디렉터리의 `WEB-INF` 디렉터리에 위치한다.

`applicationConfig` 메서드는 목록 15.14에 나와 있다.

**목록 15.14: `ContextConfig`의 `applicationConfig` 메서드**

```java
private void applicationConfig() {
   // Open the application web.xml file, if it exists
   InputStream stream = null;
   ServletContext servletContext = context.getServletContext();
   if (servletContext != null)
       stream = servletContext.getResourceAsStream
   (Constants.ApplicationWebXml);
   if (stream == null) {
        log(sm.getString("contextConfig.applicationMissing"));
        return;
    }

    // Process the application web.xml file
    synchronized (webDigester) {
      try {
         URL url =
            servletContext.getResource(Constants.ApplicationWebXml);

          InputSource is = new InputSource(url.toExternalForm());
          is.setByteStream(stream);
          webDigester.setDebug(getDebug());
          if (context instanceof StandardContext) {
              ((StandardContext) context).setReplaceWelcomeFiles(true);
          }
          webDigester.clear();
          webDigester.push(context);
          webDigester.parse(is);
        }
        catch (SAXParseException e) {
           log(sm.getString("contextConfig.applicationParse"), e);
           log(sm.getString("contextConfig.applicationPosition",
              "" + e.getLineNumber(),
              "" + e.getColumnNumber()));
           ok = false;
        }
        catch (Exception e) {
           log(sm.getString("contextConfig.applicationParse"), e);
           ok = false;
        }
        finally {
           try {
              if (stream != null) {
                  stream.close();
              }
           }
           catch (IOException e) {
              log(sm.getString("contextConfig.applicationClose"),e);
           }
        }
    }
}
```

## 웹 Digester 생성 (Creating Web Digester, PDF 331~332페이지)

`ContextConfig` 클래스에는 `webDigester`라는 `Digester` 객체 참조가 있다.

```java
private static Digester webDigester = createWebDigester();
```

이 `Digester`는 기본 `web.xml`과 애플리케이션의 `web.xml` 파일을 파싱하는 데 사용된다. `web.xml` 파일을 처리할 규칙들은 `createWebDigester` 메서드를 호출할 때 추가된다. 이 메서드는 목록 15.15에 제시되어 있다.

**목록 15.15: `createWebDigester` 메서드**

```java
private static Digester createWebDigester() {
   URL url = null;
   Digester webDigester = new Digester();
   webDigester.setValidating(true);
   url = ContextConfig.class.getResource(
      Constants.WebDtdResourcePath_22);
   webDigester.register(Constants.WebDtdPublicId_22,
      url.toString());
   url = ContextConfig.class.getResource(
      Constants.WebDtdResourcePath_23);
   webDigester.register(Constants.WebDtdPublicId_23,
   url.toString());
   webDigester.addRuleSet(new WebRuleSet());
   return (webDigester);
}
```

`createWebDigester` 메서드는 `org.apache.catalina.startup.WebRuleSet` 인스턴스를 전달하여 `webDigester`의 `addRuleSet` 메서드를 호출한다는 점에 주목하자. `WebRuleSet`은 `org.apache.commons.digester.RuleSetBase` 클래스의 하위 클래스이다. 서블릿 애플리케이션 배포 서술자의 문법을 알고 있고, 이 장 앞부분의 Digester 절을 읽었다면 그 작동 방식을 이해할 수 있을 것이다.

`WebRuleSet` 클래스는 목록 15.16에 제시되어 있다. 지면을 절약하기 위해 `addRuleInstances` 메서드의 일부는 생략했다.

## `WebRuleSet` 클래스 (PDF 332~333페이지)

**목록 15.16: `WebRuleSet` 클래스 (시작)**

```java
package org.apache.catalina.startup;

import java.lang.reflect.Method;
import org.apache.catalina.Context;
import org.apache.catalina.Wrapper;
import org.apache.catalina.deploy.SecurityConstraint;
import org.apache.commons.digester.Digester;
import org.apache.commons.digester.Rule;
import org.apache.commons.digester.RuleSetBase;
import org.xml.sax.Attributes;

/**
 * <p><strong>RuleSet</strong> for processing the contents of a web
application
 * deployment descriptor (<code>/WEB-INF/web.xml</code>) resource.</p>
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.1 $ $Date: 2001/10/17 00:44:02 $
 */

public class WebRuleSet extends RuleSetBase {
  // ------------------------------------- Instance Variables
  /**
   * The matching pattern prefix to use for recognizing our elements.
   */
  protected String prefix = null;

  // ------------------------------------------- Constructor
  /**
    * Construct an instance of this <code>RuleSet</code> with
    * the default matching pattern prefix.
    */
  public WebRuleSet () {
     this("");
  }

  /**
   * Construct an instance of this <code>RuleSet</code> with
   * the specified matching pattern prefix.
   *
   * @param prefix Prefix for matching pattern rules (including the
   * trailing slash character)
   */
  public WebRuleSet(String prefix) {
```

> `WebRuleSet(String prefix)` 생성자의 나머지 구현은 원본 PDF **334페이지**에서 이어진다. 코드 주석은 원문의 Java 소스 그대로 보존했다.


## `WebRuleSet`의 규칙 등록과 내부 규칙 클래스 (원본 PDF 334~337페이지)

앞 페이지에서 시작된 `WebRuleSet(String prefix)` 생성자는 네임스페이스 URI를 `null`로 지정하고, 전달받은 매칭 패턴 접두사를 `prefix` 필드에 저장한다.

`addRuleInstances(Digester digester)`는 이 `RuleSet`에서 정의한 `Rule` 인스턴스를 지정한 `Digester`에 추가한다. 네임스페이스 URI가 있으면 해당 URI와 연관 지어 등록하며, 이 메서드는 `Digester` 인스턴스에서만 호출해야 한다.

다음 코드는 원문의 발췌를 그대로 보존한 것이다. `...`는 원문에서 생략된 부분이며, PDF 텍스트 추출에서 줄이 분리된 문자열과 오탈자도 임의로 수정하지 않았다.

```java
    super();
    this.namespaceURI = null;
    this.prefix = prefix;
}

// ------------------------------------------- Public Methods
/**
 * <p>Add the set of Rule instances defined in this RuleSet to the
 * specified <code>Digester</code> instance, associating them with
 * our namespace URI (if any). This method should only be called
 * by a Digester instance.</p>
 *
 * @param digester Digester instance to which the new Rule instances
 * should be added.
 */
public void addRuleInstances(Digester digester) {
   digester.addRule(prefix + "web-app",
       new SetPublicIdRule(digester, "setPublicId"));
   digester.addCallMethod(prefix + "web-app/context-param",
       "addParameter", 2);
   digester.addCallParam(prefix +
       "web-app/context-param/param-name", 0);
   digester.addCallParam(prefix +
       "web-app/context-param/param-value", 1);
   digester.addCallMethod(prefix + "web-app/display-name",
       "setDisplayName", 0);
   digester.addRule(prefix + "web-app/distributable",
       new SetDistributableRule(digester));
   ...
   digester.addObjectCreate(prefix + "web-app/filter",
       "org.apache.catalina.deploy.FilterDef");
   digester.addSetNext(prefix + "web-app/filter", "addFilterDef",

       "org.apache.catalina.deploy.FilterDef");
    digester.addCallMethod(prefix + "web-app/filter/description",
       "setDescription", 0);
    digester.addCallMethod(prefix + "web-app/filter/display-name",
       "setDisplayName", 0);
    digester.addCallMethod(prefix + "web-app/filter/filter-class",
       "setFilterClass", 0);
    digester.addCallMethod(prefix + "web-app/filter/filter-name",
       "setFilterName", 0);
    digester.addCallMethod(prefix + "web-app/filter/large-icon",
       "setLargeIcon", 0);

digester.addCallMethod(prefix + "web-app/filter/small-icon",
    "setSmallIcon", 0);
digester.addCallMethod(prefix + "web-app/filter/init-param",
    "addInitParameter", 2);
digester.addCallParam(prefix +
    "web-app/filter/init-param/param-name", 0);
digester.addCallParam(prefix +
    "web-app/filter/init-param/param-value", 1);
digester.addObjectCreate(prefix + "web-app/filter-mapping",
    "org.apache.catalina.deploy.FilterMap");
digester.addSetNext(prefix + "web-app/filter-mapping",
    "addFilterMap", "org.apache.catalina.deploy.FilterMap");
digester.addCallMethod(prefix +
    "web-app/filter-mapping/filter-name", "setFilterName", 0);
digester.addCallMethod(prefix +
    "web-app/filter-mapping/servlet-name", "setServletName", 0);
digester.addCallMethod(prefix +
    "web-app/filter-mapping/url-pattern", "setURLPattern", 0);
digester.addCallMethod (prefix +
    "web-app/listener/listener-class", "addApplicationListener", 0);
...
digester.addRule(prefix + "web-app/servlet",
    new WrapperCreateRule(digester));
digester.addSetNext(prefix + "web-app/servlet",
    "addChild", "org.apache.catalina.Container");
digester.addCallMethod(prefix + "web-app/servlet/init-param",
    "addInitParameter", 2);
digester.addCallParam(prefix +
    "web-app/servlet/init-param/param-name", 0);
digester.addCallParam(prefix +
    "web-app/servlet/init-param/param-value", 1);
digester.addCallMethod(prefix + "web-app/servlet/jsp-file",
    "setJspFile", 0);
digester.addCallMethod(prefix +
    "web-app/servlet/load-on-startup", "setLoadOnStartupString", 0);
digester.addCallMethod(prefix +
    "web-app/servlet/run-as/role-name", "setRunAs", 0);
digester.addCallMethod(prefix +
    "web-app/servlet/security-role-ref", "addSecurityReference", 2);
digester.addCallParam(prefix +
    "web-app/servlet/security-role-ref/role-link", 1);
digester.addCallParam(prefix +
    "web-app/servlet/security-role-ref/role-name", 0);
digester.addCallMethod(prefix + "web-app/servlet/servlet-class",

        "setServletdass", 0);

      digester.addCallMethod(prefix + "web-app/servlet/servlet-name",
         "setName", 0);
      digester.addCallMethod(prefix + "web-app/servlet-mapping",
         "addServletMapping", 2);
      digester.addCallParam(prefix +
         "web-app/servlet-mapping/servlet-name"/ 1);
      digester.addCallParam(prefix +
         "web-app/servlet-mapping/url-pattern", 0);
      digester.addCallMethod (prefix +
         "web-app/session-config/session-timeout", "setSessionTimeout", 1,
         new Class[] { Integer.TYPE });
      digester.addCallParam(prefix +
         "web-app/session-config/session-timeout", 0);
      digester.addCallMethod(prefix + "web-app/taglib",
         "addTaglib", 2);
      digester.addCallParam(prefix + "web-app/taglib/taglib-location",
1);
      digester.addCallParam(prefix + "web-app/taglib/taglib-uri", 0);
      digester.addCallMethod(prefix +
         "web-app/welcome-file-list/welcome-file", "addWelcomeFile", 0);
  }
}
// --------------------------------------------- Private Classes

/**
  * A Rule that calls the <code>setAuthConstraint(true)</code> method of
  * the top item on the stack, which must be of type
  * <code>org.apache.catalina.deploy.SecurityConstraint</code>.
  */
final class SetAuthConstraintRule extends Rule {
   public SetAuthConstraintRule(Digester digester) {
      super(digester);
   }
   public void begin(Attributes attributes) throws Exception {
      SecurityConstraint securityConstraint =
        (SecurityConstraint) digester.peek();
      securityConstraint.setAuthConstraint(true);
      if (digester.getDebug() > 0)
          digester.log("Calling
SecurityConstraint.setAuthConstraint(true)");
   }
}

...
final class WrapperCreateRule extends Rule {
    public WrapperCreateRule(Digester digester) {
      super(digester);
    }
    public void begin(Attributes attributes) throws Exception {
      Context context =
          (Context) digester.peek(digester.getCount() - 1);
      Wrapper wrapper = context.createWrapper();
          digester.push(wrapper);
      if (digester.getDebug() > 0)
          digester.log("new " + wrapper.getClass().getName());
    }

    public void end() throws Exception {
      Wrapper wrapper = (Wrapper) digester.pop();
      if (digester.getDebug() > 0)
          digester.log("pop " + wrapper.getclass().getName());
    }
}
```

이 규칙들은 `web.xml` 요소를 Catalina 객체와 메서드에 연결한다. `web-app/context-param`의 이름과 값은 `addCallParam()`으로 수집하여 `addParameter()`에 전달한다. 필터 정의는 `FilterDef` 객체로 생성한 뒤 `addFilterDef()`로 부모 객체에 연결하며, 필터 매핑은 `FilterMap` 객체로 생성하여 `addFilterMap()`으로 등록한다. 서블릿 요소를 처리할 때는 `WrapperCreateRule`로 `Wrapper`를 만들고 `addChild()`로 컨텍스트에 추가한다.

`SetAuthConstraintRule`은 스택 맨 위의 `SecurityConstraint` 객체에 `setAuthConstraint(true)`를 호출하는 규칙이다. `WrapperCreateRule`의 `begin()`은 컨텍스트의 `createWrapper()`를 호출하여 래퍼를 만들고 스택에 넣는다. `end()`는 해당 래퍼를 스택에서 꺼낸다.

## 애플리케이션 (원본 PDF 337~338페이지)

이 장의 애플리케이션은 `ContextConfig` 인스턴스를 리스너로 사용하여 `StandardContext` 객체를 구성하는 방법을 보여 준다. 애플리케이션은 **`Bootstrap` 클래스 하나**로 구성되며, 그 코드는 **목록 15.17**에 제시되어 있다.

### 목록 15.17: `Bootstrap` 클래스

```java
package ex15.pyrmont.startup;

import org.apache.catalina.Connector;
import org.apache.catalina.Container;
import org.apache.catalina.Context;
import org.apache.catalina.Host;
import org.apache.catalina.Lifecycle;
import org.apache.catalina.LifecycleListener;
import org.apache.catalina.Loader;
import org.apache.catalina.connector.http.HttpConnector;
import org.apache.catalina.core.StandardContext;
import org.apache.catalina.core.StandardHost;
import org.apache.catalina.loader.WebappLoader;

import org.apache.catalina.startup.ContextConfig;

public final class Bootstrap {

  // invoke: http://localhost:8080/app1/Modern or
  // http://localhost:8080/app2/Primitive
  // note that we don't instantiate a Wrapper here,
  // ContextConfig reads the WEB-INF/classes dir and loads all
  // servlets.
  public static void main(String[] args) {
    System.setProperty("catalina.base",
       System.getProperty("user.dir"));
    Connector connector = new HttpConnector();
    Context context = new StandardContext();
    // StandardContext's start method adds a default mapper
    context.setPath("/app1");
    context.setDocBase("app1");
    LifecycleListener listener = new ContextConfig();
    ((Lifecycle) context).addLifecycleListener(listener);
    Host host = new StandardHost();
    host.addChild(context);
    host.setName("localhost");
    host.setAppBase("webapps");

     Loader loader = new WebappLoader();
     context.setLoader(loader);
     connector.setContainer(host);
     try {
        connector.initialize();
        ((Lifecycle) connector).start();
        ((Lifecycle) host).start();
        Container[] c = context.findChildren();
        int length = c.length;
        for (int i=0; i<length; i++) {
           Container child = c[i];
           System.out.println(child.getName());
        }
        // make the application wait until we press a key.
        System.in.read();
        ((Lifecycle) host) .stop();
     }
     catch (Exception e) {
        e.printStackTrace();
     }
```

이 예제는 `ContextConfig`를 컨텍스트의 생명주기 리스너로 등록한다. 따라서 부트스트랩 코드에서 `Wrapper`를 직접 생성하지 않는다. 대신 `ContextConfig`가 웹 애플리케이션의 배포 설정을 읽어 서블릿을 구성한다. 예제 주석에 제시된 호출 URL은 `http://localhost:8080/app1/Modern`과 `http://localhost:8080/app2/Primitive`이다. `Host`와 커넥터를 시작한 후 자식 컨테이너의 이름을 출력하며, 키 입력을 기다렸다가 호스트를 종료한다.

> **원문 표기 주의:** 발췌 코드의 `setServletdass`, `getclass()`, `servlet-name"/ 1` 등은 원문에 나타난 형태를 그대로 남겼다. 실행 가능한 코드로 사용하려면 원본 소스와 대조해야 한다.

## 애플리케이션 실행 (PDF 339페이지)

Windows에서 애플리케이션을 실행하려면 작업 디렉터리에서 다음 명령을 입력한다.

```text
java -classpath ./lib/servlet.jar;./lib/commons-
collections.jar;./lib/commons-digester.jar;./lib/commons-
logging.jar;./lib/commons-beanutils.jar;./
ex15.pyrmont.startup.Bootstrap
```

Linux에서는 라이브러리 사이의 구분자로 콜론(`:`)을 사용한다.

```text
java -classpath ./lib/servlet.jar:./lib/commons-
collections.jar:./lib/commons-digester.jar:./lib/commons-
logging.jar:./lib/commons-beanutils.jar:./
ex15.pyrmont.startup.Bootstrap
```

`PrimitiveServlet`을 호출하려면 브라우저에서 다음 URL을 사용한다.

```text
http://localhost:8080/app1/Primitive
```

`ModernServlet`을 호출하려면 다음 URL을 사용한다.

```text
http://localhost:8080/app1/Modern
```

## 요약

Tomcat은 여러 구성 방식으로 사용된다. `server.xml` 파일을 사용하여 쉽게 구성할 수 있는 것은 XML 요소를 Java 객체로 변환하는 `Digester` 객체 덕분이다. 또한 서블릿/JSP 애플리케이션을 구성할 때는 `web.xml` 문서를 사용한다. Tomcat은 이 `web.xml` 문서를 분석하고 XML 문서의 요소를 바탕으로 `Context` 객체를 구성할 수 있어야 한다. 이 문제 역시 `Digester`가 간결하고 효과적으로 해결한다.

> **원문 코드 표기:** 위 실행 명령은 책의 줄바꿈과 표기를 보존한 것이다. 실제 터미널에서 실행할 때는 줄바꿈으로 나뉜 클래스패스와 클래스명을 환경에 맞게 한 줄로 이어야 한다.
