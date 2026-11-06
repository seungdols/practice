# 20장: JMX 기반 관리 (JMX-Based Management)

> 원본 PDF 438~442페이지 번역. 이 장은 계속된다.

19장에서는 Manager 애플리케이션을 살펴보았다. `ManagerServlet` 클래스가 `ContainerServlet` 인터페이스를 구현하여 Catalina 내부 객체에 접근하는 방법을 보여주었다. 이번 장에서는 Java Management Extensions(JMX 명세)를 이용해 Tomcat을 더욱 정교하게 관리하는 방법을 설명한다. JMX에 익숙하지 않은 독자를 위해 장의 앞부분에서 간략히 소개한다. 또한 Catalina가 다른 객체를 관리하는 객체인 Managed Bean을 더 쉽게 작성하기 위해 사용하는 Commons Modeler 라이브러리를 설명한다. Tomcat에서 JMX가 어떻게 사용되는지 이해할 수 있도록 예제도 제공한다.

## JMX 소개

`ContainerServlet` 인터페이스만으로도 Manager 애플리케이션이 Catalina 내부에 접근할 수 있는데, 굳이 JMX에 관심을 가져야 하는 이유는 무엇일까? JMX가 `ContainerServlet`보다 훨씬 큰 유연성을 제공하기 때문이다. Tomcat, JBoss, JONAS, Geronimo 등 많은 서버 기반 애플리케이션이 이 기술로 자원을 관리한다.

이 책을 집필할 당시 버전 1.2.1인 JMX 명세는 Java 객체를 관리하기 위한 개방형 표준을 정의한다. 예를 들어 Tomcat 4와 5에서는 JMX를 사용해 서블릿 컨테이너의 서버, 호스트, 컨텍스트, 밸브 등의 객체를 관리 애플리케이션에서 유연하고 쉽게 관리하도록 한다. Tomcat 개발자들은 관리 애플리케이션 역할을 하는 Admin 애플리케이션까지 작성했다.

JMX를 준수하는 관리 애플리케이션으로 관리할 수 있는 Java 객체를 **JMX 관리 가능 자원(JMX manageable resource)**이라고 한다. 관리 가능 자원은 애플리케이션, 구현체나 서비스, 장치, 사용자 등이 될 수도 있다. JMX 관리 가능 자원은 Java로 작성되거나 Java 래퍼를 제공한다.

Java 객체를 JMX 관리 가능 자원으로 만들려면 **Managed Bean(MBean)**이라는 다른 객체를 생성해야 한다. `org.apache.catalina.mbeans` 패키지에는 여러 MBean이 있다. `ConnectorMBean`, `StandardEngineMBean`, `StandardHostMBean`, `StandardContextMBean` 등이 그 예다. 이름에서 알 수 있듯 `ConnectorMBean`은 커넥터를, `StandardContextMBean`은 `org.apache.catalina.core.StandardContext` 인스턴스를 관리한다. 물론 원한다면 하나의 MBean으로 여러 Java 객체를 관리할 수도 있다.

MBean은 자신이 관리하는 Java 객체의 속성과 메서드를 관리 애플리케이션에 노출한다. 관리 애플리케이션은 관리 대상 Java 객체에 직접 접근하지 못한다. 따라서 관리 애플리케이션에서 호출할 수 있게 할 속성이나 메서드를 선택할 수 있다.

MBean 클래스를 작성했다면 인스턴스를 만들고 **MBean 서버(MBean server)**라는 또 다른 Java 객체에 등록해야 한다. MBean 서버는 애플리케이션에 있는 모든 MBean의 중앙 등록소다. 관리 애플리케이션은 MBean 서버를 통해 MBean에 접근한다. 이를 서블릿 애플리케이션에 비유하면 관리 애플리케이션은 웹 브라우저, MBean 서버는 서블릿 컨테이너, MBean은 서블릿 또는 JSP 페이지에 해당한다. 브라우저가 서블릿이나 JSP에 직접 접근하지 않고 서블릿 컨테이너를 거치는 것처럼, 관리 애플리케이션도 MBean 서버를 통해 MBean에 접근한다.

MBean에는 **표준(Standard), 동적(Dynamic), 개방형(Open), 모델(Model)**의 네 종류가 있다. 표준 MBean은 가장 작성하기 쉽지만 유연성은 가장 낮다. 나머지 세 종류는 더 유연하며, 특히 Catalina가 사용하는 모델 MBean에 관심을 둘 것이다. 먼저 표준 MBean을 살펴보면서 MBean을 작성하는 방식을 익힌 뒤 모델 MBean을 설명한다. 동적 MBean과 개방형 MBean은 이 장과 관련이 없으므로 생략한다. 더 알고 싶은 독자는 JMX 1.2.1 명세를 참고하기 바란다.

아키텍처 측면에서 JMX 명세는 **계측 계층(instrumentation level), 에이전트 계층(agent level), 분산 서비스 계층(distributed services level)**의 세 계층으로 나뉜다. MBean 서버는 에이전트 계층에, MBean은 계측 계층에 속한다. 분산 서비스 계층은 향후 JMX 명세 버전에서 다룰 예정이다.

계측 계층은 JMX 관리 가능 자원을 작성하는 표준, 즉 MBean을 작성하는 방법을 정의한다. 에이전트 계층은 에이전트를 생성하기 위한 명세를 제공한다. 에이전트는 MBean 서버와 MBean 처리 서비스를 캡슐화한다. 에이전트와 그 에이전트가 관리하는 MBean은 일반적으로 같은 JVM에 존재한다. JMX 명세에는 참조 구현이 있으므로 MBean 서버를 직접 작성할 필요가 없다. 참조 구현은 기본 MBean 서버를 생성하는 방법을 제공한다.

**참고:** 명세와 참조 구현은 `http://java.sun.com/products/JavaManagement/download.html`에서 다운로드할 수 있다. 이 책에 포함된 소프트웨어에도 들어 있는 오픈 소스 JMX 구현 MX4J는 `http://mx4j.sourceforge.net`에서 구할 수 있다.

**주의:** 이 책의 ZIP 파일에는 Tomcat 4.1.12에 포함된 `mx4j-jmx.jar` 대신 MX4J 2.0 베타 1을 담은 `mx4j.jar`가 들어 있다. 더 최신 JMX 버전(1.2.1)을 사용할 수 있도록 하기 위한 조치다.

## JMX API

참조 구현은 `javax.management` 패키지의 핵심 Java 라이브러리와 JMX 프로그래밍의 특정 영역을 위한 다른 패키지들로 구성된다. 이 절에서는 API의 중요한 타입 몇 가지를 설명한다.

### MBeanServer

`javax.management.MBeanServer` 인터페이스는 MBean 서버를 나타낸다. `MBeanServer` 인스턴스를 생성하려면 `javax.management.MBeanServerFactory` 클래스의 메서드 중 하나를 사용하면 된다. 원문에서는 그 예로 `createMBean` 메서드를 언급한다.

MBean을 `MBeanServer`에 등록하려면 `MBeanServer` 인스턴스의 `registerMBean` 메서드를 호출한다. 시그니처는 다음과 같다.

```java
public ObjectInstance registerMBean(java.lang.Object object,
  ObjectName name) throws InstanceAlreadyExistsException,
  MBeanRegistrationException, NotCompliantMBeanException
```

`registerMBean`에는 등록하려는 MBean 인스턴스와 `ObjectName` 인스턴스를 전달한다. `ObjectName`은 `HashMap`의 키처럼 MBean을 고유하게 식별한다. `registerMBean`은 `ObjectInstance`를 반환한다. `javax.management.ObjectInstance` 클래스는 MBean의 객체 이름과 클래스 이름을 캡슐화한다.

특정 패턴과 일치하는 MBean 하나 또는 여러 개를 검색하기 위해 `MBeanServer` 인터페이스는 `queryNames`와 `queryMBeans` 두 메서드를 제공한다. `queryNames`는 지정한 객체 이름 패턴과 일치하는 MBean의 객체 이름을 담은 `java.util.Set`을 반환한다. 메서드 시그니처는 다음과 같다.

```java
public java.util.Set queryNames(ObjectName name, QueryExp query)
```

`query` 인수는 필터링 조건을 지정한다.

`name` 인수가 `null`이거나 도메인과 키 속성이 지정되지 않으면 등록된 모든 MBean의 `ObjectName` 인스턴스를 반환한다. `query`가 `null`이면 필터링하지 않는다.

`queryMBeans`는 `queryNames`와 비슷하지만, 선택된 MBean의 `ObjectInstance` 객체를 담은 `java.util.Set`을 반환한다. 시그니처는 다음과 같다.

```java
public java.util.Set queryMBeans(ObjectName name, QueryExp query)
```

원하는 MBean의 객체 이름을 얻었다면 MBean이 노출하는 관리 대상 자원의 속성을 조작하거나 메서드를 호출할 수 있다.

등록된 MBean의 메서드는 `MBeanServer` 인터페이스의 `invoke` 메서드로 호출할 수 있다. 등록된 MBean의 속성을 읽고 설정할 때는 각각 `getAttribute`, `setAttribute` 메서드를 사용한다.

### ObjectName

MBean 서버는 MBean의 등록소다. `HashMap`의 항목을 키로 고유하게 식별하듯, MBean 서버에 등록된 각 MBean도 객체 이름으로 고유하게 식별된다.

객체 이름은 `javax.management.ObjectName` 클래스로 표현한다. 객체 이름은 **도메인(domain)**과 **키/값 쌍들의 집합**이라는 두 부분으로 구성된다. 도메인은 문자열이며 빈 문자열일 수도 있다. 객체 이름에서는 도메인 뒤에 콜론을 쓰고, 그다음에 하나 이상의 키/값 쌍을 적는다. 키는 빈 문자열일 수 없으며 등호(`=`), 쉼표(`,`), 콜론(`:`), 별표(`*`), 물음표(`?`)를 포함할 수 없다. 같은 키는 하나의 객체 이름에서 한 번만 나타날 수 있다.

키와 값은 등호로 구분하고, 두 키/값 쌍은 쉼표로 구분한다. 다음은 두 개의 키를 가진 올바른 객체 이름이다.

```text
myDomain:type=Car,color=blue
```

`ObjectName` 인스턴스는 MBean 서버에서 MBean을 검색하기 위한 속성 패턴을 나타낼 수도 있다. 패턴인 `ObjectName`은 도메인 부분이나 키/값 쌍에 와일드카드를 사용한다. 패턴 `ObjectName`은 키가 하나도 없거나 여러 개일 수 있다.

## 표준 MBean(Standard MBeans)

표준 MBean은 가장 단순한 종류의 MBean이다. 표준 MBean을 사용하여 Java 객체를 관리하려면 다음 단계를 따른다.

1. 관리할 Java 클래스 이름 뒤에 `MBean` 접미사를 붙인 인터페이스를 생성한다. 예를 들어 관리할 클래스가 `Car`라면 인터페이스 이름은 `CarMBean`이다.
2. Java 클래스가 새로 만든 인터페이스를 구현하도록 수정한다.
3. `MBeanServer`를 포함하는 에이전트 클래스를 만든다.
4. MBean의 `ObjectName`을 생성한다.
5. `MBeanServer` 인스턴스를 생성한다.
6. MBean을 `MBeanServer`에 등록한다.

표준 MBean은 작성하기 가장 쉽지만, 사용하려면 기존 클래스를 수정해야 한다. 어떤 프로젝트에서는 클래스 수정이 괜찮지만, 다른 프로젝트에서는 특히 클래스 수가 많을 때 받아들이기 어렵다. 다른 종류의 MBean을 사용하면 클래스를 수정하지 않고도 객체를 관리할 수 있다.

표준 MBean의 예로, JMX를 통해 관리하고 싶은 다음 `Car` 클래스를 생각해 보자.

```java
package ex20.pyrmont.standardmbeantest;

public class Car {
  private String color = "red";

    public String getColor() {
      return color;
    }
    public void setColor(String color) {
         this.color = color;
    }
    public void drive() {
      System.out.println("Baby you can drive my car.");
    }
}
```

첫 단계는 이 클래스가 `CarMBean` 인터페이스를 구현하도록 수정하는 것이다. 변경한 `Car` 클래스는 **리스팅 20.1**에 제시되어 있다.

**리스팅 20.1: 수정한 Car 클래스**

```java
package ex20.pyrmont.standardmbeantest;

public class Car implements CarMBean {
  private String color = "red";

    public String getColor() {
      return color;
    }
    public void setColor(String color) {
      this.color = color;
    }
```

*리스팅 20.1의 나머지 코드는 원본 PDF 443페이지에서 이어진다.*

## 원본 PDF 443~447페이지 번역

*앞 페이지에서 이어지는 리스팅 20.1의 마지막 부분:*

```java
    public void drive() {
      System.out.println("Baby you can drive my car.");
    }
}
```

이제 리스팅 20.2의 `CarMBean` 인터페이스를 작성한다.

**리스팅 20.2: CarMBean 인터페이스**

```java
package ex20.pyrmont.standardmbeantest;

public interface CarMBean {
  public String getColor();
  public void setColor(String color);
  public void drive();
}
```

기본적으로 이 인터페이스에는 `Car` 클래스가 외부에 노출하도록 할 모든 메서드를 선언한다. 이 예제의 `CarMBean` 인터페이스는 `Car` 클래스에 있는 모든 메서드를 나열한다. 예를 들어 관리 애플리케이션에서 `drive` 메서드를 사용할 수 없게 하려면 `CarMBean` 인터페이스에서 해당 메서드 선언을 제거하기만 하면 된다.

마지막으로 리스팅 20.3은 표준 MBean을 생성하고 `Car` 객체를 관리하는 데 사용되는 `StandardAgent` 클래스를 보여 준다.

**리스팅 20.3: StandardAgent 클래스**

```java
package ex20.pyrmont.standardmbeantest;

import javax.management.Attribute;
import javax.management.ObjectName;
import javax.management.MBeanServer;
import javax.management.MBeanServerFactory;

public class StandardAgent {
  private MBeanServer mBeanServer = null;
  public StandardAgent() {
     mBeanServer = MBeanServerFactory.createMBeanServer();
  }
  public MBeanServer getMBeanServer() {
     return mBeanServer;
  }
  public ObjectName createObjectName(String name) {
     ObjectName objectName = null;
     try {
        objectName = new ObjectName(name);
     }
     catch (Exception e) {
     }
     return objectName;
  }
  private void createStandardBean(ObjectName objectName,
     String managedResourceClassName) {
     try {
        mBeanServer.createMBean(managedResourceClassName, objectName);
     }
     catch(Exception e) {
     }
  }

  public static void main(String[] args) {
    StandardAgent agent = new StandardAgent();
    MBeanServer mBeanServer = agent.getMBeanServer();
    String domain = mBeanServer.getDefaultDomain();
    String managedResourceClassName =
       "ex20.pyrmont.standardmbeantest.Car";
    ObjectName objectName = agent.createObjectName(domain + ":type=" +
       managedResourceClassName);
    agent.createStandardBean(objectName, managedResourceClassName);

    // manage MBean
    try {
       Attribute colorAttribute = new Attribute("Color","blue");
       mBeanServer.setAttribute(objectName, colorAttribute);
       System.out.println(mBeanServer.getAttribute(objectName,
          "Color"));
       mBeanServer.invoke(objectName,"drive",null,null);
    }
    catch (Exception e) {
       e.printStackTrace();
    }
  }
}
```

`StandardAgent` 클래스는 MBean 서버를 인스턴스화하고 이를 사용해 `CarMBean`을 등록하는 에이전트다. 먼저 살펴볼 것은 `mBeanServer` 변수다. `StandardAgent` 생성자는 이 변수에 `MBeanServer`를 할당하며, `MBeanServerFactory` 클래스의 `createMBeanServer` 메서드를 호출한다.

```java
public StandardAgent() {
  mBeanServer = MBeanServerFactory.createMBeanServer();
}
```

`createMBeanServer` 메서드는 JMX 참조 구현이 제공하는 기본 `MBeanServer` 객체를 반환한다. 숙련된 JMX 개발자라면 자체 `MBeanServer` 구현을 제공하고 싶을 수도 있지만, 이 책에서는 그러한 구현을 다루지 않는다.

리스팅 20.3의 `StandardAgent` 클래스에 있는 `createObjectName` 메서드는 전달받은 문자열 인수를 바탕으로 `ObjectName` 인스턴스를 반환한다. `StandardAgent`의 `createStandardBean` 메서드는 `MBeanServer`의 `createMBean` 메서드를 호출한다. `createMBean`은 관리 대상 리소스의 클래스 이름과, 생성되는 MBean을 고유하게 식별하는 `ObjectName` 인스턴스를 인수로 받는다. 또한 생성된 MBean을 `MBeanServer`에 등록한다. 표준 MBean은 특정 명명 규칙을 따르므로 `createMBean`에 MBean 유형의 이름을 별도로 제공할 필요가 없다. 관리 대상 리소스의 클래스 이름이 `Car`라면 해당 MBean은 `CarMBean`이다.

`StandardAgent`의 `main` 메서드는 먼저 `StandardAgent` 인스턴스를 만들고, `getMBeanServer`를 호출해 그 안에 있는 `MBeanServer` 인스턴스의 참조를 얻는다.

```java
StandardAgent agent = new StandardAgent();
MBeanServer mBeanServer = agent.getMBeanServer();
```

그런 다음 `CarMBean`에 사용할 `ObjectName`을 생성한다. `ObjectName`의 도메인에는 `MBeanServer`의 기본 도메인을 사용한다. 도메인 뒤에는 `type`이라는 키를 추가하고, 그 값으로 관리 대상 리소스의 완전한 클래스 이름을 사용한다.

```java
String domain = mBeanServer.getDefaultDomain();
String managedResourceClassName =
  "ex20.pyrmont.standardmbeantest.Car";
ObjectName objectName = agent.createObjectName(domain + ":type=" +
  managedResourceClassName);
```

이어서 `main` 메서드는 객체 이름과 관리 대상 리소스의 클래스 이름을 전달하여 `createStandardBean` 메서드를 호출한다.

```java
agent.createStandardBean(objectName, managedResourceClassName);
```

그다음 `main` 메서드는 `CarMBean` 인스턴스를 통해 `Car` 객체를 관리한다. `Color` 속성을 나타내는 `colorAttribute`라는 `Attribute` 객체를 만들고 값을 `blue`로 설정한다. 이후 `objectName`과 `colorAttribute`를 전달해 `setAttribute`를 호출한다. 마지막으로 `MBeanServer` 객체의 `invoke` 메서드를 사용해 `drive` 메서드를 실행한다.

```java
// manage MBean
try {
   Attribute colorAttribute = new Attribute("Color","blue");
   mBeanServer.setAttribute(objectName, colorAttribute);
   System.out.println(mBeanServer.getAttribute(objectName,
      "Color"));
   mBeanServer.invoke(objectName,"drive",null,null);
}
```

`StandardAgent` 클래스를 실행하면 콘솔에 다음과 같이 표시된다.

```text
blue
Baby you can drive my car.
```

여기서 Java 객체를 관리하는 데 굳이 JMX가 필요한 이유가 궁금할 수 있다. `StandardAgent` 클래스에서도 `Car` 객체에 직접 접근할 수 있기 때문이다. 물론 직접 접근할 수 있다. 그러나 중요한 점은 객체의 어떤 기능을 외부에 공개할지 선택하고, 공개하고 싶지 않은 다른 `public` 메서드는 숨길 수 있다는 것이다. 또한 이 장의 뒤쪽에 나오는 「애플리케이션(The Application)」 절에서 보겠지만, `MBeanServer`는 관리 대상 객체와 관리 애플리케이션 사이의 계층 역할을 한다.

### 모델 MBean(Model MBeans)

모델 MBean은 유연성을 제공한다. 표준 MBean보다 프로그래밍하기 어렵지만, 객체를 관리 가능하게 만들기 위해 기존 Java 클래스를 수정할 필요가 없다. 기존 클래스를 변경할 수 없는 상황이라면 모델 MBean을 사용하는 편이 훨씬 바람직하다.

모델 MBean을 사용하는 방식은 표준 MBean과 다르다. 표준 MBean으로 리소스를 관리할 때는 관리 대상 리소스가 구현해야 하는 인터페이스를 직접 작성한다. 반면 모델 MBean을 사용할 때는 인터페이스를 작성하지 않는다. 대신 모델 MBean을 나타내는 `javax.management.modelmbean.ModelMBean` 인터페이스가 이미 제공된다. 이 인터페이스의 구현체만 있으면 된다. 다행히 JMX에는 `ModelMBean`의 기본 구현인 `javax.management.modelmbean.RequiredModelMBean` 클래스가 포함되어 있다. `RequiredModelMBean`이나 그 하위 클래스의 인스턴스를 생성하면 된다.

**참고:** `ModelMBean` 인터페이스는 다른 방식으로 구현할 수도 있다. 예를 들어 다음 절에서 설명할 Commons Modeler 라이브러리는 `RequiredModelMBean`을 상속하지 않는 자체 구현 클래스를 제공한다.

모델 MBean을 작성할 때 가장 어려운 부분은 관리 대상 리소스의 어떤 속성과 연산을 에이전트에 공개할지 `ModelMBean` 객체에 알려 주는 것이다. 이를 위해 `javax.management.modelmbean.ModelMBeanInfo` 객체를 생성한다. `ModelMBeanInfo` 객체는 에이전트에 공개할 생성자, 속성, 연산, 이벤트 리스너를 설명한다. `ModelMBeanInfo` 객체를 만드는 과정은 번거로울 수 있지만(이 절의 예제 참조), 일단 만들고 나면 `ModelMBean` 객체에 연결하기만 하면 된다.

`RequiredModelMBean`을 `ModelMBean` 구현체로 사용할 때 `ModelMBeanInfo`와 연결하는 방법은 두 가지다.

1. `RequiredModelMBean` 생성자에 `ModelMBeanInfo` 객체를 전달한다.
2. `RequiredModelMBean` 객체의 `setModelMBeanInfo` 메서드에 `ModelMBeanInfo` 객체를 전달한다.

`ModelMBean`을 생성한 후에는 `ModelMBean` 인터페이스의 `setManagedResource` 메서드를 호출해 관리 대상 리소스를 연결해야 한다. 이 메서드의 시그니처는 다음과 같다.

```java
public void setManagedResource(java.lang.Object managedResource,
  java.lang.String managedResourceType) throws MBeanException,
  RuntimeOperationsException, InstanceNotFoundException,
  InvalidTargetObjectTypeException
```

`managedResourceType` 인수의 값은 `ObjectReference`, `Handle`, `IOR`, `EJBHandle`, `RMIReference` 중 하나가 될 수 있다. 현재 지원되는 값은 `ObjectReference`뿐이다.

그런 다음 당연히 `ObjectName`을 생성하고 모델 MBean을 MBean 서버에 등록해야 한다.



## 원본 PDF 448~452페이지 번역

이 절에서는 표준 MBean 예제에서 사용한 것과 동일한 `Car` 객체를 모델 MBean으로 관리하는 예제를 제시한다. 그러나 예제를 살펴보기 전에, 관리 대상 리소스가 공개할 속성과 연산을 기술하는 `ModelMBeanInfo` 인터페이스부터 알아보자.

### MBeanInfo와 ModelMBeanInfo

> **원문 표기 참고:** `ModelMBeanInfo` 설명 문단에는 `javax.management.mbean.ModelMBeanInfo`라고 인쇄되어 있지만, 실제 API 패키지는 `javax.management.modelmbean.ModelMBeanInfo`입니다. 번역문은 실제 API 경로를 사용했습니다.

`javax.management.modelmbean.ModelMBeanInfo` 인터페이스는 `ModelMBean`이 공개하는 생성자, 속성, 연산 및 리스너를 설명한다. 생성자는 `javax.management.modelmbean.ModelMBeanConstructorInfo`, 속성은 `javax.management.modelmbean.ModelMBeanAttributeInfo`, 연산은 `javax.management.modelmbean.ModelMBeanOperationInfo`, 리스너는 `javax.management.modelmbean.ModelMBeanNotificationInfo` 클래스로 표현된다. 이 장에서는 속성과 연산에만 관심을 둔다.

JMX는 `ModelMBeanInfo`의 기본 구현인 `javax.management.modelmbean.ModelMBeanInfoSupport` 클래스를 제공한다. 이 예제에서 사용할 생성자의 시그니처는 다음과 같다.

```java
public ModelMBeanInfoSupport(java.lang.String className,
  java.lang.String description, ModelMBeanAttributeInfo[] attributes,
  ModelMBeanConstructorInfo[] constructors,
  ModelMBeanOperationInfo[] operations,
  ModelMBeanNotificationInfo[] notifications)
```

`ModelMBeanAttributeInfo` 객체는 다음 생성자를 사용해 만든다.

```java
public ModelMBeanAttributeInfo(java.lang.String name,
  java.lang.String type, java.lang.String description,
  boolean isReadable, boolean isWritable,
  boolean isIs, Descriptor descriptor)
  throws RuntimeOperationsException
```

매개변수는 다음과 같다.

- `name`: 속성의 이름.
- `type`: 속성의 타입 또는 클래스 이름.
- `description`: 속성에 대한 설명.
- `isReadable`: 속성에 getter 메서드가 있으면 `true`, 그렇지 않으면 `false`.
- `isWritable`: 속성에 setter 메서드가 있으면 `true`, 그렇지 않으면 `false`.
- `isIs`: 속성에 `is` 형식의 getter가 있으면 `true`, 그렇지 않으면 `false`.
- `descriptor`: 이 속성 인스턴스에 적절한 메타데이터가 들어 있는 `Descriptor` 인스턴스. `null`이면 기본 디스크립터가 생성된다.

`ModelMBeanOperationInfo` 객체는 다음 생성자로 만든다.

```java
public ModelMBeanOperationInfo(java.lang.String name,
  java.lang.String description, MBeanParameterInfo[] signature,
  java.lang.String type, int impact, Descriptor)
  throws RuntimeOperationsException
```

각 매개변수의 의미는 다음과 같다.

- `name`: 메서드의 이름.
- `description`: 연산에 대한 설명.
- `signature`: 메서드 매개변수를 설명하는 `MBeanParameterInfo` 객체 배열.
- `type`: 메서드 반환값의 타입.
- `impact`: 메서드의 영향 유형. 값은 `INFO`, `ACTION`, `ACTION_INFO`, `UNKNOWN` 중 하나다.
- `descriptor`: 해당 `MBeanOperationInfo` 인스턴스에 관한 메타데이터가 들어 있는 `Descriptor` 인스턴스.

### ModelMBean 예제

이 예제에서는 모델 MBean을 사용해 `Car` 객체를 관리하는 방법을 보여 준다. `Car` 클래스는 리스팅 20.4에 제시되어 있다.

**리스팅 20.4: Car 클래스**

```java
package ex20.pyrmont.modelmbeantest1;

public class Car {
  private String color = "red";
  public String getColor() {
     return color;
  }
  public void setColor(String color) {
     this.color = color;
  }
  public void drive() {
     System.out.println("Baby you can drive my car.");
  }
}
```

모델 MBean에서는 표준 MBean과 달리 인터페이스를 작성할 필요가 없다. `RequiredMBean` 클래스를 인스턴스화하기만 하면 된다(원문의 표기이며, 실제 예제에서는 `RequiredModelMBean`을 사용한다). 리스팅 20.5에는 MBean을 생성하고 `Car` 객체를 관리하는 `ModelAgent` 클래스가 나와 있다.

**리스팅 20.5: ModelAgent 클래스 — 452페이지까지 수록된 부분**

```java
package ex20.pyrmont.modelmbeantest1;

import javax.management.Attribute;
import javax.management.Descriptor;
import javax.management.MalformedObjectNameException;
import javax.management.MBeanOperationInfo;
import javax.management.MBeanParameterInfo;
import javax.management.MBeanServer;
import javax.management.MBeanServerFactory;
import javax.management.ObjectName;
import javax.management.modelmbean.DescriptorSupport;
import javax.management.modelmbean.ModelMBean;
import javax.management.modelmbean.ModelMBeanAttributeInfo;
import javax.management.modelmbean.ModelMBeanInfo;
import javax.management.modelmbean.ModelMBeanInfoSupport;
import javax.management.modelmbean.ModelMBeanOperationInfo;
import javax.management.modelmbean.RequiredModelMBean;

public class ModelAgent {

    private String MANAGED_CLASS_NAME =
       "ex20.pyrmont.modelmbeantest1.Car";
    private MBeanServer mBeanServer = null;

    public ModelAgent() {
      mBeanServer = MBeanServerFactory.createMBeanServer();
    }

    public MBeanServer getMBeanServer() {
      return mBeanServer;
    }

    private ObjectName createObjectName(String name) {
       ObjectName objectName = null;
       try {
          objectName = new ObjectName(name);
       }
       catch (MalformedObjectNameException e) {
          e.printStackTrace();
       }
       return objectName;
    }

    private ModelMBean createMBean(ObjectName objectName,
       String mbeanName) {
       ModelMBeanInfo mBeanInfo = createModelMBeanInfo(objectName,
           mbeanName);
       RequiredModelMBean modelMBean = null;
       try {
          modelMBean = new RequiredModelMBean(mBeanInfo);
       }
       catch (Exception e) {
          e.printStackTrace();
       }
       return modelMBean;
    }

    private ModelMBeanInfo createModelMBeanInfo(ObjectName
       inMbeanObjectName, String inMbeanName) {
       ModelMBeanInfo mBeanInfo = null;
       ModelMBeanAttributeInfo[] attributes = new
          ModelMBeanAttributeInfo[1];
       ModelMBeanOperationInfo[] operations = new
          ModelMBeanOperationInfo[3];
       try {
          attributes[0] = new ModelMBeanAttributeInfo("Color",
             "java,lang.String",
             "the color.", true, true, false, null);
          operations[0] = new ModelMBeanOperationInfo("drive",
             "the drive method",
             null, "void", MBeanOperationInfo.ACTION, null);
          operations[1] = new ModelMBeanOperationInfo("getColor",
             "get color attribute",
             null, "java.lang.String", MBeanOperationInfo.ACTION, null);

          Descriptor setColorDesc = new DescriptorSupport(new String[] {
            "name=setColor", "descriptorType=operation",
            "class=" + MANAGED_CLASS_NAME, "role=operation"});
          MBeanParameterInfo[] setColorParams = new MBeanParameterInfo[] {
            (new MBeanParameterInfo("new color", "java.lang.String",
            "new Color value") )} ;
          operations[2] = new ModelMBeanOperationInfo("setColor",
            "set Color attribute", setColorParams, "void",
            MBeanOperationInfo.ACTION, setColorDesc);

          mBeanInfo = new ModelMBeanInfoSupport(MANAGED_CLASS_NAME,
           null, attributes, null, operations, null);
        }
        catch (Exception e) {
          e.printStackTrace();
        }
        return mBeanInfo;
    }

    public static void main(String[] args) {
      ModelAgent agent = new ModelAgent();
      MBeanServer mBeanServer = agent.getMBeanServer();
      Car car = new Car();
      String domain = mBeanServer.getDefaultDomain();
      ObjectName objectName = agent.createObjectName(domain +
         ":type=MyCar");
      String mBeanName = "myMBean";

      ModelMBean modelMBean = agent.createMBean(objectName, mBeanName);
      try {
         modelMBean.setManagedResource(car, "ObjectReference");
         mBeanServer.registerMBean(modelMBean, objectName);
      }
      catch (Exception e) {
      }

      // manage the bean
      try {
         Attribute attribute = new Attribute("Color", "green");
         mBeanServer.setAttribute(objectName, attribute);

         String color = (String) mBeanServer.getAttribute(objectName,
           "Color");
         System.out.println("Color:" + color);

         attribute = new Attribute("Color", "blue");
```

**원문 확인 사항:** `"java,lang.String"`은 원본 PDF에 쉼표로 인쇄되어 있으므로 그대로 옮겼다. 실행 코드로 사용하려면 원본 예제와 대조할 필요가 있다. 리스팅 20.5는 453페이지에서 계속된다.

---

## 원본 PDF 453~462페이지 번역

### 리스팅 20.5의 나머지 코드 (453페이지)

```java
            mBeanServer.setAttribute(objectName, attribute);
            color = (String) mBeanServer.getAttribute(objectName, "Color");
            System.out.println("Color:" + color);
            mBeanServer.invoke(objectName, "drive", null, null);
          }
          catch (Exception e) {
            e.printStackTrace();
          }
    }
}
```

앞서 살펴본 것처럼 모델 MBean을 작성하려면 많은 작업이 필요하다. 특히 관리 대상 리소스가 외부에 노출하는 모든 속성과 연산을 선언해야 한다. 다음 절에서는 모델 MBean을 더 빠르게 작성하도록 돕는 Commons Modeler 라이브러리를 살펴본다.

### Commons Modeler

Commons Modeler 라이브러리는 Apache Software Foundation의 Jakarta 프로젝트에 속한다. 이 라이브러리는 모델 MBean을 손쉽게 작성할 방법을 제공한다. 가장 큰 장점은 `ModelMBeanInfo` 객체를 생성하는 코드를 직접 작성하지 않아도 된다는 것이다.

앞선 예제에서 `RequiredModelMBean` 인스턴스를 생성하려면 `ModelMBeanInfo` 객체를 만들고 이를 생성자에 전달해야 했다.

```java
ModelMBeanInfo mBeanInfo = createModelMBeanInfo(objectName,
 mbeanName);
RequiredModelMBean modelMBean = null;
try {
    modelMBean = new RequiredModelMBean(mBeanInfo);
}
...
```

`ModelMBeanInfo` 객체는 MBean이 노출하는 속성과 연산을 설명한다. 따라서 `createModelMBeanInfo` 메서드를 작성하는 일은 번거롭다. 모든 속성과 연산을 나열하고 그 정보를 `ModelMBeanInfo`에 전달해야 하기 때문이다.

Commons Modeler를 사용하면 `ModelMBeanInfo` 객체를 직접 만들 필요가 없다. 대신 모델 MBean에 관한 설명을 `org.apache.catalina.modeler.ManagedBean` 객체에 담는다. MBean의 속성과 연산을 노출하기 위한 코드를 직접 작성할 필요도 없다. 생성하려는 MBean들을 나열한 **MBean 디스크립터 파일(XML 문서)**을 작성하면 된다. 각 MBean에 대해 MBean 클래스와 관리 대상 리소스 클래스의 완전한 이름, 그리고 외부에 노출할 속성과 연산을 기록한다.

그런 다음 `org.apache.commons.modeler.Registry` 인스턴스를 사용하여 이 XML 문서를 읽는다. 그러면 레지스트리는 MBean 서버 인스턴스와 디스크립터의 XML 요소에 해당하는 모든 `ManagedBean` 인스턴스를 생성한다.

이후 `ManagedBean` 인스턴스의 `createMBean` 메서드를 호출하여 모델 MBean을 생성한다. 나머지 절차는 앞에서 살펴본 방식과 같다. `ObjectName` 인스턴스를 만들고 MBean을 MBean 서버에 등록한다.

이제 MBean 디스크립터 파일의 형식을 살펴본 뒤, Modeler에서 가장 중요한 세 클래스인 `Registry`, `ManagedBean`, `BaseModelMBean`을 설명한다.

> **참고:** Tomcat 4는 현재는 더 이상 권장되지 않는 메서드를 포함한 구버전 Modeler를 사용한다. 이 장에서는 `org.apache.catalina.mbeans` 패키지의 MBean을 이해할 수 있도록 Tomcat 4에 포함된 버전을 사용한다.

### MBean 디스크립터

MBean 디스크립터는 MBean 서버가 관리하는 모델 MBean들을 설명하는 XML 문서다. 문서는 다음 헤더로 시작한다.

```xml
<?xml version="1.0"?>
<!DOCTYPE mbeans-descriptors PUBLIC
 "-//Apache Software Foundation//DTD Model MBeans Configuration File"
 "http://jakarta.apache.org/commons/dtds/mbeans-descriptors.dtd">
```

그 뒤에는 루트 요소 `mbeans-descriptors`가 온다.

```xml
<mbeans-descriptors>
...
</mbeans-descriptors>
```

시작 태그와 종료 태그 사이에는 모델 MBean을 각각 나타내는 `mbean` 요소들이 들어간다. `mbean` 요소에는 속성, 연산, 생성자, 알림을 나타내는 요소가 포함될 수 있다. 다음 소절에서는 Tomcat의 MBean 디스크립터를 이해하는 데 필요한 요소들을 설명한다.

#### `mbean`

`mbean` 요소는 모델 MBean을 설명하며, 해당 `ModelMBeanInfo` 객체를 생성하는 데 필요한 정보를 포함한다. 정의는 다음과 같다.

```xml
<!ELEMENT mbean (descriptor?, attribute*, constructor*, notification*,
operation*)>
```

이 정의에 따르면 `mbean` 요소에는 선택적인 `descriptor` 요소 하나, 0개 이상의 `attribute` 요소, 0개 이상의 `constructor` 요소, 0개 이상의 `notification` 요소, 0개 이상의 `operation` 요소를 넣을 수 있다.

`mbean` 요소에 지정할 수 있는 속성은 다음과 같다.

- `className`: `ModelMBean` 구현 클래스의 완전한 Java 클래스 이름. 지정하지 않으면 `org.apache.commons.modeler.BaseModelMBean`을 사용한다.
- `description`: 모델 MBean에 대한 설명.
- `domain`: 관리 빈이 생성한 `ModelMBean`을 `ObjectName`으로 등록할 때 사용할 MBean 서버 도메인.
- `group`: 유사한 MBean 구현 클래스들을 묶어 선택할 때 사용할 수 있는 선택적 그룹 이름.
- `name`: 모델 MBean을 고유하게 식별하는 이름. 일반적으로 해당 서버 구성 요소의 기본 클래스 이름을 사용한다.
- `type`: 관리 대상 리소스 구현 클래스의 완전한 Java 클래스 이름.

#### `attribute`

`attribute` 요소는 MBean의 JavaBeans 프로퍼티를 설명한다. 선택적인 `descriptor` 요소를 포함할 수 있으며 다음 속성을 가진다.

- `description`: 속성에 대한 설명.
- `displayName`: 표시 이름.
- `getMethod`: 해당 프로퍼티의 getter 메서드.
- `is`: `is` 형태의 getter를 사용하는 불리언 속성인지 나타내는 값. 기본값은 `false`다.
- `name`: JavaBeans 프로퍼티 이름.
- `readable`: 관리 애플리케이션이 읽을 수 있는지 나타내는 불리언 값. 기본값은 `true`다.
- `setMethod`: 해당 프로퍼티의 setter 메서드.
- `type`: 속성의 완전한 Java 클래스 이름.
- `writeable`: 관리 애플리케이션이 값을 쓸 수 있는지 나타내는 불리언 값. 기본값은 `true`다.

#### `operation`

`operation` 요소는 관리 애플리케이션에 공개되는 모델 MBean의 `public` 메서드를 설명한다. 0개 이상의 `parameter` 하위 요소를 가질 수 있으며 다음 속성을 사용한다.

- `description`: 연산에 대한 설명.
- `impact`: 메서드의 영향 유형. `ACTION`(쓰기와 유사), `ACTION-INFO`(쓰기와 읽기 모두와 유사), `INFO`(읽기와 유사), `UNKNOWN` 중 하나다.
- `name`: 공개 메서드 이름.
- `returnType`: 반환형의 완전한 Java 클래스 이름.

#### `parameter`

`parameter` 요소는 생성자 또는 연산에 전달되는 매개변수를 설명한다.

- `description`: 매개변수 설명.
- `name`: 매개변수 이름.
- `type`: 매개변수의 완전한 Java 클래스 이름.

### `mbean` 요소 예제

Catalina에는 여러 모델 MBean이 포함되어 있으며, 모두 `org.apache.catalina.mbeans` 패키지의 `mbean-descriptors.xml` 파일에 선언되어 있다. 리스팅 20.6은 Tomcat 4의 `StandardServer` MBean 선언을 보여 준다.

**리스팅 20.6: StandardServer MBean 선언**

```xml
<mbean name="StandardServer"
  className="org.apache.catalina.mbeans.StandardServerMBean"
  description="Standard Server Component"
  domain="Catalina"
  group="Server"
  type="org.apache.catalina.core.StandardServer">

  <attribute name="debug"
    description="The debugging detail level for this component"
    type="int"/>
  <attribute name="managedResource"
    description="The managed resource this MBean is associated with"
    type="java.lang.Object"/>
  <attribute name="port"
    description="TCP port for shutdown messages"
    type="int"/>
  <attribute name="shutdown"
    description="Shutdown password"
    type="java.lang.String"/>
  <operation name="store"
    description="Save current state to server.xml file"
    impact="ACTION"
    returnType="void">
  </operation>
</mbean>
```

리스팅 20.6의 `mbean` 요소는 `StandardServer`라는 고유한 이름으로 모델 MBean을 선언한다. 이 MBean은 `org.apache.catalina.mbeans.StandardServerMBean`으로 표현되며 `org.apache.catalina.core.StandardServer` 인스턴스를 관리한다. 도메인은 `Catalina`, 그룹은 `Server`다.

이 모델 MBean은 내부의 네 `attribute` 요소에서 설명하는 `debug`, `managedResource`, `port`, `shutdown` 프로퍼티를 노출한다. 또한 `operation` 요소에서 설명하는 `store` 메서드 하나를 노출한다.

### 사용자 정의 모델 MBean 클래스 작성

Commons Modeler를 사용할 때는 `mbean` 요소의 `className` 속성에 모델 MBean 유형을 정의한다. 기본적으로 Commons Modeler는 `org.apache.commons.modeler.BaseModelMBean` 클래스를 사용한다. 하지만 다음과 같은 상황에서는 `BaseModelMBean`을 확장할 필요가 있다.

1. 관리 대상 리소스의 프로퍼티나 메서드를 재정의하려는 경우.
2. 관리 대상 리소스에 정의되지 않은 속성이나 연산을 추가하려는 경우.

Catalina는 `org.apache.catalina.mbeans` 패키지에 `BaseModelMBean`의 여러 하위 클래스를 제공한다. 이 클래스들은 곧 살펴본다.

### Registry

API의 중심은 `org.apache.commons.modeler.Registry` 클래스다. 이 클래스를 사용하면 다음 작업을 수행할 수 있다.

- `javax.management.MBeanServer` 인스턴스를 얻는다. 따라서 `javax.management.MBeanServerFactory`의 `createMBeanServer` 메서드를 직접 호출할 필요가 없다.
- `loadRegistry` 메서드로 MBean 디스크립터 파일을 읽는다.
- 모델 MBean을 구성하는 데 사용할 `ManagedBean` 객체를 생성한다.

### ManagedBean

`ManagedBean` 객체는 모델 MBean을 설명하며 `javax.management.MBeanInfo` 객체를 대신한다.

### BaseModelMBean

`org.apache.commons.modeler.BaseModelMBean` 클래스는 `javax.management.modelmbean.ModelMBean` 인터페이스를 구현한다. 이 클래스를 사용하면 `javax.management.modelmbean.RequiredModelMBean` 클래스를 직접 사용할 필요가 없다.

이 클래스에서 특히 유용한 필드는 모델 MBean이 관리하는 리소스를 나타내는 `resource` 필드다. 정의는 다음과 같다.

```java
protected java.lang.Object resource;
```

### Modeler API 사용하기

관리하려는 객체의 `Car` 클래스는 리스팅 20.7에 나와 있다.

**리스팅 20.7: Car 클래스**

```java
package ex20.pyrmont.modelmbeantest2;

public class Car {
  public Car() {
     System.out.println("Car constructor");
  }
  private String color = "red";

  public String getColor() {
    return color;
  }
  public void setColor(String color) {
    this.color = color;
  }

  public void drive() {
    System.out.println("Baby you can drive my car.");
  }
}
```

Commons Modeler를 사용하면 관리 대상 객체의 속성과 연산을 코드에 직접 하드코딩하지 않는다. 대신 모델 MBean의 디스크립터 역할을 하는 XML 문서에 나열한다. 이 예제에서는 리스팅 20.8의 `car-mbean-descriptor.xml` 파일을 사용한다.

**리스팅 20.8: car-mbean-descriptor.xml 파일**

```xml
<?xml version="1.0"?>
<!DOCTYPE mbeans-descriptors PUBLIC
 "-//Apache Software Foundation//DTD Model MBeans Configuration File"
 "http://jakarta.apache.org/commons/dtds/mbeans-descriptors.dtd">

<mbeans-descriptors>
  <mbean name="myMBean"
    className="javax.management.modelmbean.RequiredModelMBean"
    description="The ModelMBean that manages our Car object"
    type="ex20.pyrmont.modelmbeantest.Car">

    <attribute name="Color"
      description="The car color"
      type="java.lang.String"/>
    <operation name="drive"
      description="drive method"
      impact="ACTION"
      returnType="void">
      <parameter name="driver" description="the driver parameter"
        type="java.lang.String"/>
    </operation>
  </mbean>
</mbeans-descriptors>
```

이제 리스팅 20.9의 에이전트 클래스(`ModelAgent.java`)가 필요하다.

**리스팅 20.9: ModelAgent 클래스**

```java
package ex20.pyrmont.modelmbeantest2;

import java.io.InputStream;
import java.net.URL;
import javax.management.Attribute;
import javax.management.MalformedObjectNameException;
import javax.management.MBeanServer;
import javax.management.ObjectName;
import javax.management.modelmbean.ModelMBean;

import org.apache.commons.modeler.ManagedBean;
import org.apache.commons.modeler.Registry;

public class ModelAgent {
  private Registry registry;
  private MBeanServer mBeanServer;

  public ModelAgent() {
    registry = createRegistry();
    try {
       mBeanServer = Registry.getServer();
    }
    catch (Throwable t) {
       t.printStackTrace(System.out);
       System.exit(1);
    }
  }

  public MBeanServer getMBeanServer() {
    return mBeanServer;
  }

  public Registry createRegistry() {
    Registry registry = null;
    try {
       URL url = ModelAgent.class.getResource
          ("/ex20/pyrmont/modelmbeantest2/car-mbean-descriptor.xml");
       InputStream stream = url.openStream();
       Registry.loadRegistry(stream);
       stream.close();
       registry = Registry.getRegistry();
    }
    catch (Throwable t) {
      System.out.println(t.toString());
    }
    return (registry);
  }

  public ModelMBean createModelMBean(String mBeanName)
    throws Exception {
    ManagedBean managed = registry.findManagedBean(mBeanName);
    if (managed == null) {
        System.out.println("ManagedBean null");
        return null;
    }
    ModelMBean mbean = managed.createMBean();
    ObjectName objectName = createObjectName();
    return mbean;
  }

  private ObjectName createObjectName() {
    ObjectName objectName = null;
    String domain = mBeanServer.getDefaultDomain();
    try {
       objectName = new ObjectName(domain + ":type=MyCar");
    }
    catch (MalformedObjectNameException e) {
       e.printStackTrace();
    }
    return objectName;
  }

  public static void main(String[] args) {
    ModelAgent agent = new ModelAgent();
    MBeanServer mBeanServer = agent.getMBeanServer();
    Car car = new Car();
    System.out.println("Creating ObjectName");
    ObjectName objectName = agent.createObjectName();
    try {
       ModelMBean modelMBean = agent.createModelMBean("myMBean");
       modelMBean.setManagedResource(car, "ObjectReference");
       mBeanServer.registerMBean(modelMBean, objectName);
    }
    catch (Exception e) {
       System.out.println(e.toString());
    }
    // manage the bean
    try {
       Attribute attribute = new Attribute("Color", "green");
       mBeanServer.setAttribute(objectName, attribute);
       String color = (String) mBeanServer.getAttribute(objectName,
          "Color");
       System.out.println("Color:" + color);

       attribute = new Attribute("Color", "blue");
       mBeanServer.setAttribute(objectName, attribute);
       color = (String) mBeanServer.getAttribute(objectName, "Color");
       System.out.println("Color:" + color);
       mBeanServer.invoke(objectName, "drive", null, null);
    }
    catch (Exception e) {
       e.printStackTrace();
    }
  }
}
```

Commons Modeler를 사용하면 에이전트 클래스가 얼마나 짧아지는지 알 수 있다.

### Catalina의 MBean

이 장의 시작 부분에서 설명했듯이 Catalina는 `org.apache.catalina.mbeans` 패키지에 여러 MBean 클래스를 제공한다. 이 MBean들은 모두 직접 또는 간접적으로 `org.apache.commons.modeler.BaseModelMBean` 클래스를 확장한다. 다음 절에서는 Tomcat 4에서 중요한 세 가지 MBean 클래스를 살펴본다. **설명은 PDF 463페이지에서 이어진다.**

> **원문 보존 참고:** 리스팅 20.8의 `type` 경로는 리스팅 20.7의 패키지와 다르며, `drive` 연산의 매개변수 선언도 `Car.drive()` 시그니처와 일치하지 않는다. 번역본은 원문 표기를 유지했다. 실행 시 원본 소스와 비교해야 한다.


## 원본 PDF 463~476페이지 번역 — 20장 JMX 기반 관리 (완결)

 Catalina가 제공하는 모델 MBean에는 `ClassNameMBean`, `StandardServerMBean`, `MBeanFactory`가 있다. 이 세 클래스를 이해하면 Catalina의 다른 모델 MBean 클래스도 어렵지 않게 이해할 수 있다. 또한 `org.apache.catalina.mbeans` 패키지의 `MBeanUtil` 클래스도 설명한다.

 ### ClassNameMBean

`org.apache.catalina.mbeans.ClassNameMBean` 클래스는 `org.apache.commons.modeler.BaseModelMBean`을 확장한다. 관리 대상 리소스의 클래스 이름을 나타내는 `className` 속성을 제공한다. 원문은 이 속성을 쓰기 전용이라고 설명하지만, 아래 리스팅에는 `getClassName()` 접근자가 제시되어 있다.

**리스팅 20.10: `ClassNameMBean` 클래스**

 ```java
package org.apache.catalina.mbeans;

import javax.management.MBeanException;
import javax.management.RuntimeOperationsException;
import org.apache.commons.modeler.BaseModelMBean;

public class ClassNameMBean extends BaseModelMBean {
  public ClassNameMBean()
  throws MBeanException, RuntimeOperationsException {
     super();
  }
  public String getClassName() {
     return (this.resource.getClass().getName());

    }
}
```

 `ClassNameMBean`은 관리 대상 리소스 자체에는 없는 속성을 제공하기 위해 작성한 `BaseModelMBean` 하위 클래스의 예다. `mbeans-descriptors.xml` 파일에 정의된 많은 `mbean` 요소가 이 클래스를 모델 MBean 유형으로 사용한다.

 ### StandardServerMBean

`StandardServerMBean` 클래스는 `org.apache.commons.modeler.BaseModelMBean`을 확장하여 `org.apache.catalina.core.StandardServer` 인스턴스를 관리한다. 리스팅 20.11은 관리 대상 리소스의 메서드, 즉 `store()` 메서드를 재정의하는 모델 MBean의 예다. 관리 애플리케이션이 `store()`를 호출하면 관리 대상 `StandardServer` 객체의 메서드가 아니라 `StandardServerMBean`의 `store()` 메서드가 실행된다.

**리스팅 20.11: `StandardServerMBean` 클래스**

 ```java
package org.apache.catalina.mbeans;

import javax.management.InstanceNotFoundException;
import javax.management.MBeanException;
import javax.management.MBeanServer;
import javax.management.RuntimeOperationsException;
import org.apache.catalina.Server;
import org.apache.catalina.ServerFactory;
import org.apache.catalina.core.StandardServer;
import org.apache.commons.modeler.BaseModelMBean;

public class StandardServerMBean extends BaseModelMBean {
  private static MBeanServer mserver = MBeanUtils.createServer();
  public StandardServerMBean()
     throws MBeanException, RuntimeOperationsException {
     super();
  }

    public synchronized void stored throws InstanceNotFoundException,
      MBeanException, RuntimeOperationsException {

        Server server = ServerFactory.getServer();
        if (server instanceof StandardServer) {
            try {
               ((StandardServer) server).store();
            }
            catch (Exception e) {
               throw new MBeanException(e, "Error updating conf/server.xml");
            }

        }
    }
}
```

 따라서 `StandardServerMBean`은 관리 대상 리소스의 메서드를 재정의하기 위해 `BaseModelMBean`을 상속하는 모델 MBean 클래스의 예다.

 ### MBeanFactory

`MBeanFactory` 클래스는 Catalina의 여러 리소스를 관리하는 모델 MBean을 생성하는 팩터리 객체를 나타낸다. 또한 이러한 MBean을 삭제하는 메서드도 제공한다. 예를 들어 리스팅 20.12의 `createStandardContext()` 메서드를 살펴보자.

**리스팅 20.12: `createStandardContext()` 메서드**

 ```java
public String createStandardContext(String parent,
  String path, String docBase) throws Exception {
  // Create a new StandardContext instance
  StandardContext context = new StandardContext();
  path = getPathStr(path);
  context.setPath(path);
  context.setDocBase(docBase);
  ContextConfig contextConfig = new ContextConfig();
  context.addLifecycleListener(contextConfig);

    // Add the new instance to its parent component
    ObjectName pname = new ObjectName(parent);
    Server server = ServerFactory.getServer();
    Service service =
      server.findService(pname.getKeyProperty("service"));
    Engine engine = (Engine) service.getContainer();
    Host host = (Host) engine.findChild(pname.getKeyProperty("host"));

    // Add context to the host
    host.addChild(context);

    // Return the corresponding MBean name
    ManagedBean managed = registry.findManagedBean("StandardContext");

    ObjectName oname =
```

 ### MBeanUtil

`org.apache.catalina.mbeans.MBeanUtil`은 Catalina 객체를 관리하는 여러 MBean을 생성하거나 삭제하고, 객체 이름을 생성하는 정적 메서드를 제공하는 유틸리티 클래스다. 예를 들어 리스팅 20.13의 `createMBean()`은 `org.apache.catalina.Server` 객체를 관리하는 모델 MBean을 만든다.

**리스팅 20.13: `Server` 객체를 관리하는 모델 MBean을 만드는 `createMBean()` 메서드**

 ```java
public static ModelMBean createMBean(Server server) throws Exception {
  String mname = createManagedName(server);
  ManagedBean managed = registry.findManagedBean(mname);
  if (managed == null) {
      Exception e = new Exception(
        "ManagedBean is not found with "+mname);
      throw new MBeanException(e);
  }
  String domain = managed.getDomain();
  if (domain == null)
      domain = mserver.getDefaultDomain();
  ModelMBean mbean = managed.createMBean(server);
  ObjectName oname = createObjectName(domain, server);
  mserver.registerMBean(mbean, oname);
  return (mbean);
}
```

 ### Catalina의 MBean 생성

Catalina의 모델 MBean 몇 가지를 살펴보았으므로, 이제 이 MBean들이 어떻게 생성되고 관리 애플리케이션에서 사용할 수 있게 되는지 알아보자. Tomcat의 구성 파일인 `server.xml`은 `Server` 요소 안에 다음 `Listener` 요소를 정의한다.

 ```xml
<Server port="8005" shutdown="SHUTDOWN" debug="0">
  <Listener
    c1assName="org.apache.catalina.mbeans.ServerLifecycleListener"
    debug="0"/>
...
```

 이 설정은 서버를 나타내는 `org.apache.catalina.core.StandardServer` 객체에 `org.apache.catalina.mbeans.ServerLifecycleListener` 유형의 리스너를 추가한다. `StandardServer` 인스턴스가 시작되면 `start()` 메서드에서 `START_EVENT` 이벤트를 발생시킨다.

 ```java
...
  lifecycle.fireLifecycleEvent(START_EVENT, null);
  ...
}
```

 `StandardServer` 객체가 중지되면 `stop()` 메서드에서 `STOP_EVENT` 이벤트가 발생한다.

 ```java
...
  lifecycle.fireLifecycleEvent(STOP_EVENT, null);
  ...
}
```

 이 이벤트들은 `ServerLifecycleListener` 클래스의 `lifecycleEvent()` 메서드를 실행시킨다. 리스팅 20.14를 보자.

**리스팅 20.14: `ServerLifecycleListener`의 `lifecycleEvent()` 메서드**

 ```java
public void lifecycleEvent(LifecycleEvent event) {
  Lifecycle lifecycle = event.getLifecycle();
  if (Lifecycle.START_EVENT.equals(event.getType())) {
      if (lifecycle instanceof Server) {
          // Loading additional MBean descriptors
          loadMBeanDescriptors();
          createMBeans();
      }
  }
  else if (Lifecycle.STOP_EVENT.equals(event.getType())) {
      if (lifecycle instanceof Server) {
          destroyMBeans();
      }
  }
  else if (Context.RELOAD_EVENT.equals(event.getType())) {
      if (lifecycle instanceof StandardContext) {
          StandardContext context = (StandardContext)lifecycle;
          if (context.getPrivileged()) {
              context.getServletContext().setAttribute
                (Globals.MBEAN_REGISTRY_ATTR,
              MBeanUtils.createRegistry());
              context.getServletContext().setAttribute
                (Globals.MBEAN_SERVER_ATTR,
              MBeanUtils.createServer());
          }
      }
  }
}
```

 `createMBeans()`는 Catalina의 모든 MBean을 생성하는 메서드다. 앞 절에서 설명한 모델 MBean 클래스 `MBeanFactory`의 인스턴스를 생성하는 것으로 시작한다.

**리스팅 20.15: `ServerLifecycleListener`의 `createMBeans()` 메서드**

 ```java
protected void createMBeans() {
  try {
     MBeanFactory factory = new MBeanFactory();
     createMBeans(factory);
     createMBeans(serverFactory.getserver());
  }
  catch (MBeanException t) {
     Exception e = t.getTargetException();
     if (e == null)
         e = t;
     log("createMBeans: MBeanException", e);
  }
  catch (Throwable t) {
     log("createMBeans: Throwable", t);
  }
}
```

 첫 번째 `createMBeans()` 메서드는 `MBeanUtil` 클래스를 사용하여 `MBeanFactory`의 `ObjectName`을 만들고 MBean 서버에 등록한다. 두 번째 `createMBeans()`는 `org.apache.catalina.Server` 객체를 인수로 받아 그 객체의 모델 MBean을 생성한다. 리스팅 20.16의 메서드를 따라가 보면 흥미롭다.

**리스팅 20.16: `Server` 객체의 MBean을 만드는 `createMBeans()` 메서드**

 ```java
protected void createMBeans(Server server) throws Exception {
  // Create the MBean for the Server itself
  if (debug >= 2)
      log("Creating MBean for Server " + server);
  MBeanUtils.createMBean(server);
  if (server instanceof StandardServer) {
      ((StandardServer) server).addPropertyChangeListener(this);
  }

    // Create the MBeans for the global NamingResources (if any)
    NamingResources resources = server.getGlobalNamingResources();
    if (resources != null) {
        createMBeans(resources);
    }

    // Create the MBeans for each child Service
    Service services[] = server.findServices();
    for (int i = 0; i < services.length; i++) {
       // FIXME - Warp object hierarchy not currently supported
       if (services[i].getContainer().getClass().getName().equals
           ("org.apache.catalina.connector.warp.WarpEngine")) {
           if (debug >= 1) {

            log("Skipping MBean for Service " + services[i]);
          }
          continue;
        }
        createMBeans(services[i]);
    }
}
```

 리스팅 20.16에서 `StandardServer` 인스턴스의 모든 `Service` 객체를 순회하는 `for` 루프 안에 `createMBeans(services[i]);` 호출이 있다는 점에 주목하자. 이 메서드는 서비스의 MBean 인스턴스를 생성하고, 서비스에 속한 모든 커넥터와 엔진의 MBean을 생성하는 `createMBeans()` 메서드를 호출한다. 서비스 MBean을 생성하는 메서드는 리스팅 20.17과 같다.

**리스팅 20.17: `Service` MBean을 생성하는 `createMBeans()` 메서드**

 ```java
protected void createMBeans(Service service) throws Exception {
  // Create the MBean for the Service itself
  if (debug >= 2)
      log("Creating MBean for Service " + service);
  MBeanUtils.createMBean(service);
  if (service instanceof StandardService) {
    ((StandardService) service).addPropertyChangeListener(this);
  }

    // Create the MBeans for the corresponding Connectors
    Connector connectors[] = service.findConnectors();
    for (int j = 0; j < connectors.length; j++) {
       createMBeans(connectors[j]);
    }

    // Create the MBean for the associated Engine and friends
    Engine engine = (Engine) service.getContainer();
    if (engine != null) {
        createMBeans(engine);
    }
}
```

 예상할 수 있듯이 `createMBeans(engine)` 메서드는 호스트의 MBean을 생성하는 `createMBeans()`를 호출한다.

 ```java
// Create the MBean for the Engine itself
  if (debug >= 2) {
      log("Creating MBean for Engine " + engine);
  }
  MBeanUtils.createMBean(engine);
  ...

    Container hosts[] = engine.findChildren();
    for (int j = 0; j < hosts.length; j++) {
        createMBeans((Host) hosts[j]);
    }
    ...
}
```

 이어서 `createMBeans(host)`는 다음과 같이 `ContextMBean`을 생성한다.

 ```java
...
  MBeanUtils.createMBean(host);
  ...
  Container contexts[] = host.findChildren();
  for (int k = 0; k < contexts.length; k++) {
      createMBeans((Context) contexts[k]);
  }
  ...
}
```

 `createMBeans(context)` 메서드는 다음과 같다.

 ```java
...
  MBeanUtils.createMBean(context);
  ...
  context.addContainerListener(this);
  if (context instanceof StandardContext) {
    ((StandardContext) context).addPropertyChangeListener(this);
    ((StandardContext) context).addLifecycleListener(this);
  }

    // If the context is privileged, give a reference to it
    // in a servlet context attribute
    if (context.getPrivileged()) {
        context.getServletContext().setAttribute
          (Globals.MBEAN_REGISTRY_ATTR, MBeanUtils.createRegistry());
        context.getServletContext().setAttribute
          (Globals.MBEAN_SERVER_ATTR, MBeanUtils.createServer());
    }
    ...
}
```

 컨텍스트의 `privileged` 속성이 `true`이면 웹 애플리케이션의 `ServletContext` 객체에 두 속성을 만들어 저장한다. 이 속성들의 키는 `Globals.MBEAN_REGISTRY_ATTR`과 `Globals.MBEAN_SERVER_ATTR`이다. 다음은 `org.apache.catalina.Globals` 클래스의 일부다.

 ```java
* The servlet context attribute under which the managed bean Registry
 * will be stored for privileged contexts (if enabled).
 */

public static final String MBEAN_REGISTRY_ATTR =
  "org.apache.catalina.Registry";

/**
 * The servlet context attribute under which the MBeanServer will be
 * stored for privileged contexts (if enabled).
 */
public static final String MBEAN_SERVER_ATTR =
  "org.apache.catalina.MBeanServer";
```

 `MBeanUtils.createRegistry()`는 `Registry` 인스턴스를 반환한다. `MBeanUtils.createServer()`는 Catalina의 모든 MBean이 등록되는 `javax.management.MBeanServer` 인스턴스를 반환한다. 즉, `privileged` 속성이 `true`인 웹 애플리케이션에서는 이 `Registry`와 `MBeanServer` 객체를 얻을 수 있다. 다음 절에서는 Tomcat을 관리하는 JMX 관리 애플리케이션을 만드는 방법을 설명한다.

 ### 애플리케이션

여기서 만드는 애플리케이션은 Tomcat 관리용 웹 애플리케이션이다. 단순하지만 Catalina가 노출하는 MBean을 어떻게 사용하는지 전반적인 개념을 이해하기에는 충분하다. Catalina에 있는 모든 `ObjectName` 인스턴스를 나열하거나, 현재 실행 중인 모든 컨텍스트를 표시하고 그중 하나를 제거할 수 있다.

먼저 리스팅 20.18의 애플리케이션 디스크립터를 만든다. 이 파일은 `%CATALINA_HOME%/webapps` 디렉터리에 배치해야 한다.

**리스팅 20.18: `myadmin.xml` 파일**

 ```xml
<Context path="/myadmin" docBase="../server/webapps/myadmin" debug="8"
privileged="true" reloadable="true">
</Context>
```

 주의할 점은 `Context` 요소의 `privileged` 속성을 반드시 `true`로 설정해야 한다는 것이다. `docBase` 속성은 애플리케이션의 위치를 지정한다. 이 애플리케이션은 리스팅 20.19의 서블릿 하나로 구성된다.

**리스팅 20.19: `MyAdminServlet` 클래스**

 ```java
package myadmin;

import java.io.IOException;

import java.io.PrintWriter;
import java.net.URLEncoder;
import java.util.Iterator;
import java.util.Set;
import javax.management.MBeanServer;
import javax.management.ObjectName;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServlet;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.commons.modeler.Registry;

public class MyAdminServlet extends HttpServlet {
  private Registry registry;
  private MBeanServer mBeanServer;

  public void init() throws ServletException {
    registry = (Registry)
        getServletContext().getAttribute("org.apache.catalina.Registry");
    if(registry == null) {
        System.out.println("Registry not available");
        return;
    }
    mBeanServer = (MBeanServer) getServletContext().getAttribute(
        "org.apache.catalina.MBeanServer");
    if (mBeanServer==null) {
        System.out.println("MBeanServer not available");
        return;
    }
  }

  public void doGet(HttpServletRequest request,
    HttpServletResponse response)
    throws ServletException, IOException {

    response.setContentType("text/html");
    PrintWriter out = response.getWriter();
    if (registry==null || mBeanServer==null) {
        out.println("Registry or MBeanServer not found");
        return;
    }

    out.println("<html><head></head><body>");
    String action = request.getParameter("action");
    if ("listAllManagedBeans".equals(action)) {
        listAllManagedBeans(out);
    }
    else if ("listAllContexts".equals(action)) {
        listAllContexts(out);
    }
    else if ("removeContext".equals(action)) {
        String contextObjectName =
           request.getParameter("contextObjectName");
        removeContext(contextObjectName, out);
    }
    else {

        out.println("Invalid command");
    }
    out.println("</body></html>");
}

private void listAllManagedBeans(PrintWriter out) {
   String[] managedBeanNames = registry.findManagedBeans();
   for (int i=0; i<managedBeanNames.length; i++) {
      out.print(managedBeanNames[i] + "<br/>");
   }
}

private void listAllContexts(PrintWriter out) {
   try {
      ObjectName objName = new ObjectName("Catalina:type=Context,*");
      Set set = mBeanServer.queryNames(objName, null);
      Iterator it = set.iterator();
      while (it.hasNext()) {
         ObjectName obj = (ObjectName) it.next();
         out.print(obj +
           " <a href=?action=removeContext&contextObjectName=" +
           URLEncoder.encode (obj.toString(), "UTF-8") +
           ">remove</a><br/>");
      }
   }
   catch (Exception e) {
      out.print(e.toString());
        }
    }

    private void removeContext(String contextObjectName,
       PrintWriter out) {
       try {
          ObjectName mBeanFactoryObjectName = new
              ObjectName("Catalina:type=MBeanFactory");
          if (mBeanFactoryObjectName!=null) {
              String operation = "removeContext";
              String[] params = new String[1];
              params[0] = contextObjectName;
              String signature[] = { "java.lang.String" };
              try {
                 mBeanServer.invoke(mBeanFactoryObjectName, operation,
                    params, signature);
                 out.println("context removed");
              }
              catch (Exception e) {
                 out.print(e.toString());
              }
          }

        }
        catch (Exception e) {
        }
    }
}
```

 마지막으로 리스팅 20.20의 애플리케이션 배포 디스크립터가 필요하다.

**리스팅 20.20: `web.xml` 파일**

 ```xml
<?xml version="1.0" encoding="ISO-8859-1"?>

<!DOCTYPE web-app
    PUBLIC "-//Sun Microsystems, Inc.//DTD Web Application 2.3//EN"
    "http://java.sun.com/dtd/web-app_2_3.dtd">

<web-app>
  <servlet>
    <servlet-name>myAdmin</servlet-name>
    <servlet-class>myadmin.MyAdminServlet</servlet-class>
  </servlet>
  <servlet-mapping>
    <servlet-name>myAdmin</servlet-name>
    <url-pattern>/myAdmin</url-pattern>
  </servlet-mapping>
</web-app>
```

 모든 `ObjectName` 인스턴스를 나열하려면 다음 URL을 사용한다.

`http://localhost:8080/myadmin/myAdmin?action=listAllMBeans`

MBean 객체의 목록이 표시된다. 다음은 그중 처음 여섯 개다.

```text
MemoryUserDatabase
DigestAuthenticator
BasicAuthenticator
UserDatabaseRealm
SystemErrLogger
Group
```

모든 컨텍스트를 나열하려면 다음 URL을 사용한다.

`http://localhost:8080/myadmin/myAdmin?action=listAllContexts`

실행 중인 모든 애플리케이션이 표시된다. 어느 하나를 제거하려면 `remove` 하이퍼링크를 클릭한다.

 ### 요약

이 장에서는 JMX를 사용하여 Tomcat을 관리하는 방법을 배웠다. 네 가지 MBean 유형 가운데 두 가지를 소개했으며, Catalina가 제공하는 MBean을 사용하는 간단한 관리 애플리케이션을 개발했다.

 > **원문 코드 확인 사항:** 원본 PDF의 일부 예제에는 `c1assName`(숫자 1), `stored` 메서드 선언, `serverFactory.getserver()` 등의 오타로 보이는 표기가 있다. 또한 예제 URL의 `listAllMBeans`와 서블릿의 `listAllManagedBeans` 명령 이름이 서로 다르다. 위 코드와 URL은 원문 표기를 보존했으므로 실행 전 확인해야 한다. PDF 텍스트 추출에 따른 줄바꿈도 점검할 것.

