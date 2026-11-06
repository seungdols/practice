# 11장 — StandardWrapper

> 번역 범위: 원본 PDF 219~224페이지. 다음 번역은 225페이지부터 이어진다.

5장에서 컨테이너에는 `engine`, `host`, `context`, `wrapper`라는 네 가지 유형이 있음을 배웠다. 앞 장에서는 간단한 `context`와 `wrapper`를 직접 만들어 보기도 했다. 일반적으로 하나의 `context`에는 하나 이상의 `wrapper`가 있으며, 각 `wrapper`는 서블릿 정의 하나를 나타낸다.

이번 장에서는 Catalina가 제공하는 `Wrapper` 인터페이스의 표준 구현을 살펴본다. 먼저 HTTP 요청마다 호출되는 메서드의 순서를 설명하고, 이어서 `javax.servlet.SingleThreadModel` 인터페이스를 다룬다. 마지막으로 `StandardWrapper`와 `StandardWrapperValve` 클래스를 설명한다. 이 장의 예제 애플리케이션은 `StandardWrapper` 인스턴스를 사용하여 서블릿을 나타낸다.

## 메서드 호출 순서 (Sequence of Methods Invocation)

HTTP 요청이 들어올 때마다 커넥터는 연결된 컨테이너의 `invoke` 메서드를 호출한다. 컨테이너는 다시 자식 컨테이너들의 `invoke` 메서드를 호출한다. 예를 들어 커넥터가 `StandardContext` 인스턴스와 연결되어 있다면, 커넥터는 그 인스턴스의 `invoke`를 호출하고, `StandardContext`는 자식 컨테이너(이 경우 `StandardWrapper` 유형)의 `invoke`를 호출한다.

그림 11.1은 커넥터가 HTTP 요청을 받았을 때 어떤 일이 일어나는지 보여준다. (5장에서 설명했듯이 컨테이너에는 하나 이상의 밸브가 있는 파이프라인이 존재한다.)

**그림 11.1: 메서드 호출 협력 다이어그램** — 원본 PDF 219페이지 참조.

- 커넥터는 요청 객체와 응답 객체를 생성한다.
- 커넥터는 `StandardContext` 인스턴스의 `invoke` 메서드를 호출한다.


<!-- 원본 PDF 220~224페이지 번역. 219페이지에서 이어짐. -->

- `StandardContext`의 `invoke` 메서드는 컨텍스트 파이프라인의 `invoke` 메서드를 호출한다. `StandardContext` 파이프라인의 기본 밸브는 `StandardContextValve`이므로, 파이프라인은 `StandardContextValve`의 `invoke`를 호출한다.
- `StandardContextValve`의 `invoke` 메서드는 요청을 처리할 적절한 래퍼를 찾아 그 래퍼의 `invoke`를 호출한다.
- `StandardWrapper`는 래퍼의 표준 구현이다. `StandardWrapper` 인스턴스의 `invoke`는 자신의 파이프라인에 있는 `invoke`를 호출한다.
- `StandardWrapper` 파이프라인의 기본 밸브는 `StandardWrapperValve`다. 따라서 `StandardWrapperValve`의 `invoke`가 호출된다. 이 메서드는 래퍼의 `allocate`를 호출하여 서블릿 인스턴스를 얻는다.
- `allocate`는 서블릿을 로드해야 하는 경우 `load` 메서드를 호출한다.
- `load` 메서드는 서블릿의 `init` 메서드를 호출한다.
- `StandardWrapperValve`는 서블릿의 `service` 메서드를 호출한다.

> **참고** `StandardContext` 생성자는 `StandardContextValve` 인스턴스를 기본 밸브로 설정한다.

```java
publicStandardContext() {

 super();
 pipeline.setBasic(new StandardContextValve());
 namingResources.setContainer(this);
}
```

> **참고** `StandardWrapper` 생성자는 `StandardWrapperValve` 인스턴스를 기본 밸브로 설정한다.

```java
public StandardWrapper() {
  super();
  pipeline.setBasic(new StandardWrapperValve());
}
```

이 장에서 관심을 두는 것은 서블릿이 호출되는 구체적인 방식이다. 따라서 `StandardWrapper`와 `StandardWrapperValve` 클래스를 살펴볼 것이다. 그전에 `javax.servlet.SingleThreadModel` 인터페이스를 먼저 알아보자. 이 인터페이스를 이해해야 래퍼가 서블릿을 로드하는 방식을 이해할 수 있다.

## SingleThreadModel

서블릿은 `javax.servlet.SingleThreadModel` 인터페이스를 구현할 수 있다. 이 인터페이스를 구현한 서블릿은 흔히 SingleThreadModel(STM) 서블릿이라고 부른다.

서블릿 명세에 따르면 이 인터페이스를 구현하는 목적은 서블릿이 한 번에 하나의 요청만 처리하도록 보장하는 것이다. Servlet 2.4 명세의 SRV.14.2.24절을 인용하면 다음과 같다(Servlet 2.3에서도 `SingleThreadModel` 인터페이스를 비슷하게 설명한다).

> 서블릿이 이 인터페이스를 구현하면 두 스레드가 동일한 서블릿 인스턴스의 `service` 메서드를 동시에 실행하지 않는다는 것이 보장된다. 서블릿 컨테이너는 단일 서블릿 인스턴스에 대한 접근을 동기화하거나, 서블릿 인스턴스 풀을 유지하면서 새로운 요청마다 사용 가능한 서블릿을 배정하여 이를 보장할 수 있다. 그러나 이 인터페이스는 정적 클래스 변수나 서블릿 범위 밖의 클래스 등 공유 자원에 서블릿이 접근하여 발생하는 동기화 문제까지 방지하지는 않는다.

많은 프로그래머는 이 설명을 자세히 읽지 않고 `SingleThreadModel`을 구현하면 서블릿이 스레드 안전(thread-safe)해진다고 생각한다. 하지만 그렇지 않다. 위 인용문을 다시 읽어 보자.

`SingleThreadModel`을 구현하면 두 스레드가 **같은 서블릿 인스턴스**의 `service`를 동시에 실행하지 않는 것은 사실이다. 그러나 성능을 높이기 위해 서블릿 컨테이너는 STM 서블릿의 인스턴스를 여러 개 생성할 수 있다. 즉, 서로 다른 인스턴스의 `service` 메서드는 동시에 실행될 수 있다. 따라서 서블릿이 정적 클래스 변수나 클래스 외부의 다른 자원에 접근해야 한다면 동기화 문제가 발생할 수 있다.

**잘못된 멀티스레드 안전성에 대한 믿음:** `SingleThreadModel` 인터페이스는 서블릿 프로그래머에게 멀티스레드 안전성에 대한 잘못된 확신을 주기 때문에 Servlet 2.4에서 사용 중단(deprecated)으로 지정되었다. 그러나 Servlet 2.3 및 Servlet 2.4 컨테이너는 여전히 이 인터페이스를 지원해야 한다.

> **참고** 원문은 `SingleThreadModel`에 관한 흥미로운 토론을 다음 주소에서 소개한다: `http://w4.metronet.com/~wjm/tomcat/ToFeb11/msg02655.html` (원문에 기재된 역사적 주소).

## StandardWrapper

`StandardWrapper` 객체의 주요 책임은 자신이 나타내는 서블릿을 로드하고 그 인스턴스를 할당하는 것이다. 하지만 `StandardWrapper`가 서블릿의 `service` 메서드를 직접 호출하지는 않는다. 이 작업은 `StandardWrapper` 인스턴스의 파이프라인에 있는 기본 밸브인 `StandardWrapperValve` 객체에 맡겨진다. `StandardWrapperValve`는 `StandardWrapper`의 `allocate`를 호출해 서블릿 인스턴스를 얻는다. 그런 다음 해당 인스턴스의 `service`를 호출한다.

`StandardWrapper`는 서블릿이 처음 요청될 때 서블릿 클래스를 로드한다. 서블릿을 동적으로 로드하므로 서블릿 클래스의 완전한 이름(fully qualified name)을 알아야 한다. 이를 위해 `StandardWrapper`의 `setServletClass` 메서드에 서블릿 클래스 이름을 전달한다. 또한 `setName` 메서드를 호출하여 해당 서블릿을 참조할 이름을 지정한다.

`StandardWrapperValve`가 서블릿 인스턴스를 요청할 때 `StandardWrapper`는 그 서블릿이 `SingleThreadModel` 인터페이스를 구현하는지 여부를 고려해야 한다.

`SingleThreadModel`을 구현하지 않는 서블릿이라면 `StandardWrapper`는 서블릿 클래스를 한 번 로드하고, 이후 요청에도 동일한 인스턴스를 계속 반환한다. 여러 스레드가 서블릿의 `service`를 호출해도 된다고 가정하기 때문에 서블릿 인스턴스를 여러 개 유지할 필요가 없다. 공유 자원에 대한 접근을 동기화해야 한다면 이는 서블릿 프로그래머의 책임이다.

STM 서블릿은 다르다. `StandardWrapper` 인스턴스는 두 스레드가 같은 STM 서블릿 인스턴스의 `service`를 동시에 실행하지 않도록 보장해야 한다. `StandardWrapper`가 STM 서블릿 인스턴스를 하나만 유지한다면 다음과 같은 방식으로 `service`를 호출할 수 있을 것이다.

```java
Servlet instance = <get an instance of the servlet>;
if ((servlet implementing SingleThreadModel>) {
    synchronized (instance) {
      instance.service(request, response);
    }
}
else {
      instance.service(request, response);
}
```

그러나 성능을 위해 `StandardWrapper`는 STM 서블릿 인스턴스의 풀(pool)을 유지한다.

래퍼는 서블릿 내부에서 얻어 사용할 수 있는 `javax.servlet.ServletConfig` 인스턴스를 준비하는 책임도 맡는다. 다음 두 절에서는 서블릿 인스턴스의 할당과 로딩을 설명한다.

## 서블릿 할당 (Allocating the Servlet)

앞에서 설명했듯이 `StandardWrapperValve`의 `invoke` 메서드는 요청된 서블릿 인스턴스를 얻기 위해 래퍼의 `allocate`를 호출한다. 따라서 `StandardWrapper` 클래스에는 이 메서드의 구현이 있어야 한다.

`allocate` 메서드의 시그니처는 다음과 같다.

```java
public javax.servlet.Servlet allocate() throws ServletException;
```

`allocate`는 요청된 서블릿의 인스턴스를 반환한다는 점에 주목하자.

STM 서블릿을 지원해야 하므로 `allocate` 메서드는 다소 복잡해진다. 실제로 이 메서드는 STM이 아닌 서블릿을 처리하는 부분과 STM 서블릿을 처리하는 부분, 이렇게 두 부분으로 구성된다. 첫 번째 부분의 뼈대는 다음과 같다.

```java
if (!singleThreadModel) {
    // returns a non-STM servlet instance
}
```

`singleThreadModel`은 해당 `StandardWrapper`가 나타내는 서블릿이 STM 서블릿인지 표시하는 불리언 값이다. 초기값은 `false`지만, `loadServlet` 메서드는 로드하는 서블릿을 검사하고 STM 서블릿이라면 이 값을 설정한다. `loadServlet`은 뒤의 「서블릿 로딩(Loading the Servlet)」 절에서 설명한다.

`singleThreadModel`이 `true`라면 `allocate` 메서드의 두 번째 부분이 실행된다. 두 번째 부분의 뼈대는 다음과 같다.

```java
synchronized (instancepool) {
  // returns an instance of the servlet from the pool
}
```

이제 첫 번째 부분과 두 번째 부분을 차례로 살펴보자.

STM이 아닌 서블릿을 위해 `StandardWrapper`는 `javax.servlet.Servlet` 타입의 `instance` 변수를 정의한다.

```java
private Servlet instance = null;
```

`allocate`는 `instance`가 `null`인지 확인한다. `null`이면 `loadServlet`을 호출하여 서블릿을 로드한다. 그런 다음 `countAllocated` 정수를 증가시키고 인스턴스를 반환한다.

```java
if (!singleThreadModel) {
    // Load and initialize our instance if necessary
    if (instance == null) {
        synchronized (this) {
            if (instance == null) {
```

> **이어짐:** 위 코드는 원본 PDF 224페이지 끝에서 중단되며, 225페이지에서 계속된다. 코드의 나머지를 임의로 보충하지 않았다.

## `allocate()` 메서드의 나머지 부분 (원본 PDF 225~226페이지)

앞 페이지에서 시작한 `allocate()`의 첫 번째 코드 부분은 다음과 같이 이어진다.

```java
                try {
                   instance = loadServlet();
                }
                catch (ServletException e) {
                   throw e;
                }
                catch (Throwable e) {
                   throw new ServletException
                      (sm.getString("standardWrapper.allocate"), e);
                }
            }
        }
      }
      if (!singleThreadModel) {
          if (debug >= 2)
              log(" Returninq non-STM instance");
          countAllocated++;
          return (instance);
      }
  }
```

`StandardWrapper`가 나타내는 서블릿이 STM 서블릿이면 `allocate()`는 풀에서 인스턴스를 반환하려고 시도한다. `java.util.Stack` 타입의 `instancePool` 변수는 STM 서블릿 인스턴스가 들어 있는 스택을 참조한다.

```java
private Stack instancePool = null;
```

이 변수는 다음 절에서 설명할 `loadServlet()` 메서드 안에서 인스턴스화된다.

`allocate()`는 STM 서블릿 인스턴스의 개수가 지정된 최대 개수를 넘지 않는 한 인스턴스를 할당한다. `maxInstances` 정수에는 STM 인스턴스의 최대 개수가 저장되며, 기본값은 20이다.

```java
private int maxInstances = 20;
```

현재 STM 인스턴스 개수를 추적하기 위해 `StandardWrapper`는 `nInstances` 정수를 사용한다.

```java
private int nInstances = 0;
```

다음은 `allocate()` 메서드의 두 번째 부분이다.

```java
  synchronized (instancePool) {
    while (countAllocated >= nInstances) {
        // Allocate a new instance if possible, or else wait
        if (nInstances < maxInstances) {
            try {
               instancePool.push(loadServlet());
               nInstances++;
            }
            catch (ServletException e) {
               throw e;
            }
            catch (Throwable e) {
               throw new ServletException
                 (sm.getString("StandardWrapper.allocate"), e);
            }
        }
        else {
            try {
               instancePool.wait();
            }
            catch (InterruptedException e) {
               ;
            }
        }
    }
    if (debug >= 2)
        log(" Returning allocated STM instance");
    countAllocated++;
    return (Servlet) instancePool.pop();

  }
```

위 코드는 `while` 반복문을 사용하여 `nInstances`가 `countAllocated` 이하인 동안 기다린다. 반복문 안에서 `allocate()`는 `nInstances` 값을 검사한다. 이 값이 `maxInstances`보다 작으면 `loadServlet()`을 호출하고 새 인스턴스를 풀에 넣은 뒤 `nInstances`를 증가시킨다. `nInstances`가 `maxInstances` 이상이면 `instancePool` 스택의 `wait()` 메서드를 호출하여 인스턴스가 스택으로 반환될 때까지 기다린다.

> **원문 확인:** 위의 비교 조건과 변수 표기는 책의 설명 및 코드에 나온 그대로 옮겼다.

## 서블릿 로딩(Loading the Servlet) (원본 PDF 227~229페이지)

`StandardWrapper` 클래스는 `Wrapper` 인터페이스의 `load()` 메서드를 구현한다. `load()`는 서블릿을 로드하고 `javax.servlet.ServletConfig` 인스턴스를 전달하여 서블릿의 `init()` 메서드를 호출하는 `loadServlet()`을 실행한다. `loadServlet()`의 동작을 살펴보자.

먼저 `loadServlet()`은 현재 `StandardWrapper`가 STM 서블릿을 나타내는지 확인한다. STM이 아니면서 `instance`가 `null`이 아니라면, 즉 서블릿이 이미 로드되어 있다면 해당 인스턴스를 그대로 반환한다.

```java
  // Nothing to do if we already have an instance or an instance pool
  if (!singleThreadModel && (instance != null))
      return instance;
```

`instance`가 `null`이거나 STM 서블릿이라면 메서드의 나머지 부분을 실행한다.

먼저 `System.out`과 `System.err`의 출력을 캡처한다. 그러면 나중에 `javax.servlet.ServletContext`의 `log()` 메서드를 이용해 메시지를 기록할 수 있다.

```java
  PrintStream out = System.out;
  SystemLogHandler.startCapture();
```

다음으로 `javax.servlet.Servlet` 타입의 `servlet` 변수를 정의한다. 이 변수는 `loadServlet()`이 반환할, 로드된 서블릿 인스턴스를 나타낸다.

```java
  Servlet servlet = null;
```

`loadServlet()`은 서블릿 클래스를 로드하는 역할을 한다. 클래스 이름은 미리 `servletClass` 클래스 변수에 지정되어 있어야 한다. 메서드는 이 변수의 값을 문자열 `actualClass`에 할당한다.

```java
String actualClass = servletclass;
```

하지만 Catalina는 JSP 컨테이너이기도 하므로, `loadServlet()`은 요청된 서블릿이 JSP 페이지인지도 확인해야 한다. JSP 페이지라면 그 페이지의 실제 클래스 이름을 알아내려고 시도한다.

```java
   if ((actualClass == null) && (jspFile != null)) {
       Wrapper jspWrapper = (Wrapper)
       ((Context) getParent()).findChild(Constants.JSP_SERVLET_NAME);
       if (jspWrapper != null)
           actualClass = jspWrapper.getServletClass();
   }
```

JSP 페이지에 대응하는 서블릿 이름을 찾지 못하면 `servletclass` 변수의 값을 사용한다. 그러나 `StandardWrapper`의 `setServletClass()` 메서드를 호출하여 `servletclass`가 설정되지 않았다면 예외가 발생하고 메서드의 나머지 부분은 실행되지 않는다.

```java
   // Complain if no servlet class has been specified
   if (actualClass == null) {
       unavailable(null);
       throw new ServletException
         (sm.getString("StandardWrapper.notClass", getName()));
   }
```

이 시점에는 서블릿 클래스 이름이 결정되었으므로 `loadServlet()`은 로더를 얻는다. 로더가 없으면 예외를 던지고 메서드를 종료한다.

```java
   // Acquire an instance of the class loader to be used
   Loader loader = getLoader();
   if (loader == null) {
       unavailable(null);
       throw new ServletException
       (sm.getString("StandardWrapper.missingLoader", getName()));
   }
```

로더를 찾았다면 `loadServlet()`은 로더의 `getClassLoader()` 메서드를 호출하여 `ClassLoader`를 얻는다.

```java
   ClassLoader classLoader = loader.getClassLoader();
```

Catalina는 `org.apache.catalina` 패키지에 속하는 특수 서블릿을 제공한다. 이러한 서블릿은 서블릿 컨테이너의 내부에 접근할 수 있다. 특수 서블릿이라면, 즉 `isContainerProvidedServlet()`이 `true`를 반환하면, Catalina 내부에 접근할 수 있도록 `classLoader` 변수에 다른 `ClassLoader` 인스턴스를 할당한다.

```java
   // Special case class loader for a container provided servlet
   if (isContainerProvidedServlet(actualClass)) {
       ClassLoader = this.getClass().getClassLoader();
       log(sm.getString
          ("standardWrapper.containerServlet", getName()));
   }
```

이제 클래스 로더와 서블릿 이름을 확보했으므로 `loadServlet()`은 서블릿을 로드할 수 있다.

```java
   // Load the specified servlet class from the appropriate class
   // loader
   Class classClass = null;
   try {
      if (ClassLoader != null) {
         System.out.println("Using classLoader.loadClass");
         classClass = classLoader.loadClass(actualClass);
       }
       else {
          System.out.println("Using forName");
          classClass = Class.forName(actualClass);
       }
   }
   catch (ClassNotFoundException e) {
       unavailable(null);
       throw new ServletException
         (sm.getstring("standardWrapper.missingClass", actualClass), e);
   }
   if (classClass == null) {
       unavailable(null);
       throw new ServletException
         (sm.getString("standardWrapper.missingClass", actualClass));
   }
```

그런 다음 서블릿 인스턴스를 생성할 수 있다.

```java
   // Instantiate and initialize an instance of the servlet class
   // itself
   try {
      servlet = (Servlet) classClass.newInstance();
   }
   catch (ClassCastException e) {
      unavailable(null);
      // Restore the context ClassLoader
      throw new ServletException
         (sm.getString("standardWrapper.notServlet", actualClass), e);
   }
   catch (Throwable e) {
      unavailable(null);
      // Restore the context ClassLoader
      throw new ServletException
         (sm.getstring("standardWrapper.instantiate", actualClass), e);

   }
```

하지만 `loadServlet()`은 서블릿을 초기화하기 전에 `isServletAllowed()`를 호출하여 해당 서블릿을 로드해도 되는지 검사한다.

```java
   // Check if loading the servlet in this web application should be
   // allowed
```

> **다음 페이지로 이어짐:** `isServletAllowed()` 검사 코드는 PDF 230페이지에서 계속된다. 위 코드에 보이는 `ClassLoader`/`classLoader`, `getstring`/`getString` 등의 대소문자 차이는 원문 표기를 그대로 유지했다.

## `loadServlet()`의 나머지 처리 (원본 PDF 230~231페이지)

```java
   if (!isServletAllowed(servlet)) {
       throw new SecurityException
          (sm.getString("standardWrapper.privilegedServlet",
            actualClass));
   }
```

보안 검사를 통과하면, 서블릿이 `ContainerServlet`인지 확인한다. `ContainerServlet`은 `org.apache.catalina.ContainerServlet` 인터페이스를 구현하여 Catalina의 내부 기능에 접근할 수 있는 서블릿이다. 해당하는 경우 `loadServlet()`은 `ContainerServlet`의 `setWrapper()` 메서드를 호출하면서 현재 `StandardWrapper` 인스턴스를 전달한다.

```java
   // Special handling for ContainerServlet instances
   if ((servlet instanceof ContainerServlet) &&
       isContainerProvidedServlet(actualClass)) {
       ((ContainerServlet) servlet).setWrapper(this);
   }
```

다음으로 `loadServlet()`은 `BEFORE_INIT_EVENT`를 발생시키고 서블릿의 `init()` 메서드를 호출한다.

```java
   try {
      instanceSupport.fireInstanceEvent(
         InstanceEvent.BEFORE_INIT_EVENT, servlet);
      servlet.init(facade);
```

`init()`에 전달하는 `facade` 변수는 `javax.servlet.ServletConfig` 객체를 참조한다. 이 객체가 어떻게 만들어지는지는 이 장의 뒤쪽에 있는 **ServletConfig 객체 생성** 절에서 살펴본다.

`loadOnStartup` 변수에 정수 값이 설정되어 있고 해당 서블릿이 JSP 페이지라면, 서블릿의 `service()` 메서드도 호출한다.

```java
   // Invoke jspInit on JSP pages
   if ((loadOnStartup > 0) && (jspFile != null)) {
       // Invoking jspInit
       HttpRequestBase req = new HttpRequestBase();
       HttpResponseBase res = new HttpResponseBase();
       req.setServletPath(jspFile};
       req.setQueryString("jsp_precompile=true");
       servlet.service(req, res);
   }
```

그다음 `AFTER_INIT_EVENT` 이벤트를 발생시킨다.

```java
      instanceSupport.firelnstanceEvent (InstanceEvent.AFTER_INIT_EVENT,
         servlet);
```

현재 `StandardWrapper`가 나타내는 서블릿이 STM 서블릿이라면, 서블릿 인스턴스를 인스턴스 풀에 추가할 예정이다. 따라서 `instancePool` 변수가 아직 `null`이면 `Stack` 객체를 할당한다.

```java
       // Register our newly initialized instance
       singleThreadModel = servlet instanceof SingleThreadModel;
       if (singleThreadModel) {
           if (instancePool == null)
               instancePool = new Stack();
       }
       fireContainerEvent("load", this);
  }
```

`finally` 블록에서는 `System.out`과 `System.err`의 출력 캡처를 중지하고, 로딩 과정에서 발생한 메시지를 `ServletContext`의 `log()` 메서드로 기록한다.

```java
  finally {
     String log = SystemLogHandler.stopCapture();
     if (log != null && log.length() > 0) {
         if (getServletContext() != null) {
             getServletContext().log(log);
         }
         else {
             out.println(log);
         }
     }
  }
```

마지막으로 `loadServlet()`은 서블릿 인스턴스를 반환한다.

```java
  return servlet;
```

## ServletConfig 객체 (원본 PDF 231~233페이지)

`StandardWrapper` 클래스의 `loadServlet()` 메서드는 서블릿을 로드한 뒤 해당 서블릿의 `init()` 메서드를 호출한다. `init()`에는 `javax.servlet.ServletConfig` 인스턴스가 전달된다. 그렇다면 `StandardWrapper`는 이 `ServletConfig` 객체를 어디에서 얻는 것일까?

답은 `StandardWrapper` 클래스 자체에 있다. 이 클래스는 `Wrapper` 인터페이스뿐 아니라 `javax.servlet.ServletConfig` 인터페이스도 구현한다.

`ServletConfig` 인터페이스에는 `getServletContext`, `getServletName`, `getInitParameter`, `getInitParameterNames`라는 네 가지 메서드가 있다. 이제 `StandardWrapper`에서 각각 어떻게 구현하는지 살펴보자.

> **참고:** `StandardWrapper`는 서블릿의 `init()`에 자기 자신을 직접 전달하지 않는다. 대신 `StandardWrapperFacade` 인스턴스로 자신을 감싸서 서블릿 개발자가 `StandardWrapper`의 공개 메서드 대부분에 접근하지 못하게 한다. 이 장의 뒤쪽에 있는 **StandardWrapperFacade** 절을 참고하자.

### `getServletContext()`

메서드 선언은 다음과 같다.

```java
public ServletContext getServletContext()
```

`StandardWrapper` 인스턴스는 반드시 `StandardContext`의 자식 컨테이너여야 한다. 즉, 부모가 `StandardContext`여야 한다. `StandardContext` 객체의 `getServletContext()`를 호출하면 `ServletContext` 객체를 얻을 수 있다. `StandardWrapper`에서의 구현은 다음과 같다.

```java
public ServletContext getServletContext() {
  if (parent == null)
      return (null);
  else if (!(parent instanceof Context))
      return (null);
  else
      return (((Context) parent).getServletContext());
}
```

> **참고:** 이제 서블릿 정의를 나타내는 래퍼를 독립적으로 배포할 수 없다는 점을 알 수 있다. `ServletConfig.getServletContext()`가 `ServletContext` 인스턴스를 반환할 수 있으려면 래퍼는 반드시 컨텍스트 안에 있어야 한다.

### `getServletName()`

이 메서드는 서블릿 이름을 반환한다. 선언은 다음과 같다.

```java
public java.lang.String getServletName()
```

`StandardWrapper`에서의 구현은 다음과 같다.

```java
public String getServletName() {
  return (getName());
}
```

이 메서드는 `StandardWrapper`의 부모 클래스인 `ContainerBase`의 `getName()`을 호출할 뿐이다. `ContainerBase`의 구현은 다음과 같다.

```java
public String getName() {
  return (name);
}
```

`setName()`을 호출하여 `name`의 값을 설정할 수 있다. 앞에서 `StandardWrapper` 인스턴스의 `setName()`을 호출할 때 서블릿 이름을 전달했던 것을 떠올려 보자.

### `getInitParameter()`

이 메서드는 지정한 초기화 매개변수의 값을 반환한다. 선언은 다음과 같다.

```java
public java.lang.String getInitParameter(java.lang.String name)
```

`StandardWrapper`는 초기화 매개변수를 `parameters`라는 `HashMap`에 저장한다.

```java
private HashMap parameters = new HashMap();
```

매개변수의 이름과 값을 `addInitParameter()`에 전달하면 `parameters`에 저장된다.

```java
public void addInitParameter(String name, String value) {
  synchronized (parameters) {
     parameters.put(name, value);
  }
  fireContainerEvent("addInitParameter", name);
}
```

`StandardWrapper`의 `getInitParameter()` 구현은 다음과 같다.

```java
public String getInitParameter(String name) {
  return (findInitParameter(name));
}
```

`findInitParameter()`는 매개변수 이름을 받아 `parameters`의 `get()` 메서드를 호출한다.

```java
public String findInitParameter(String name) {

    synchronized (parameters) {
      return ((String) parameters.get(name));
    }
}
```

### `getInitParameterNames()` (원본 PDF 234페이지)

이 메서드는 모든 초기화 매개변수 이름을 담은 `Enumeration`을 반환한다. 선언은 다음과 같다.

```java
public java.util.Enumeration getInitParameterNames()
```

`StandardWrapper`에서의 구현은 다음과 같다.

```java
public Enumeration getInitParameterNames() {
  synchronized (parameters) {
     return (new Enumerator(parameters.keyset()));
  }
}
```

`Enumerator` 클래스는 `java.util.Enumeration`을 구현하며 `org.apache.catalina.util` 패키지에 속한다.

## 부모와 자식 컨테이너 (원본 PDF 234페이지)

래퍼는 개별 서블릿을 위한 컨테이너를 나타낸다. 따라서 래퍼는 자식 컨테이너를 가질 수 없으며 `addChild()`를 호출해서도 안 된다. 호출하면 `java.lang.IllegalStateException`이 발생한다. `StandardWrapper`의 `addChild()` 구현은 다음과 같다.

```java
public void addChild(Container child) {
  throw new IllegalStateException
     (sm.getString("StandardWrapper.notChild"));
}
```

래퍼의 부모는 반드시 `Context`의 구현체여야 한다. `setParent()`에 `Context`가 아닌 컨테이너를 전달하면 `java.lang.IllegalArgumentException`이 발생한다. 이 설명과 코드는 다음 페이지에서 이어진다.

> **원문 표기 참고:** 코드에 나타난 `setServletPath(jspFile}` 및 `firelnstanceEvent`, `keyset()` 등의 표기는 원본 PDF에 인쇄된 대로 옮겼다. 실제 컴파일 가능한 코드와 다를 수 있다.

## 부모 컨테이너 설정 (원본 PDF 235페이지)

`StandardWrapper`의 부모 컨테이너는 `Context`여야 한다. 다음 `setParent()` 구현은 `null`이 아닌 부모가 `Context`의 인스턴스인지 검사하고, 조건에 맞지 않으면 `IllegalArgumentException`을 던진다. 검사를 통과하면 상위 클래스의 `setParent()`를 호출한다.

```java
public void setParent(Container container) {
  if ((container != null) && !(container instanceof Context))
      throw new IllegalArgumentException
        (sm.getString("standardWrapper.notContext"));
  super.setParent(container);
}
```

## StandardWrapperFacade (원본 PDF 235~236페이지)

`StandardWrapper` 인스턴스는 자신이 로딩한 서블릿의 `init()` 메서드를 호출한다. `init()`에는 `javax.servlet.ServletConfig`가 필요하다. `StandardWrapper` 자체가 `ServletConfig` 인터페이스를 구현하므로 이론적으로는 자기 자신을 `init()`에 전달할 수 있다. 그러나 서블릿이 `StandardWrapper`의 공개 메서드 대부분에 접근하지 못하도록 숨겨야 한다. 이를 위해 `StandardWrapper`는 자신을 `StandardWrapperFacade` 인스턴스로 감싼다. 그림 11.2는 두 클래스의 관계를 보여준다. 두 클래스 모두 `javax.servlet.ServletConfig` 인터페이스를 구현한다.

> **그림 11.2:** `StandardWrapper`와 `StandardWrapperFacade`의 관계 — 원본 PDF 235페이지 참조.

`StandardWrapper` 클래스에는 자신을 `StandardWrapperFacade` 생성자에 전달하는 다음 코드가 있다.

```java
private StandardWrapperFacade facade = new StandardWrapperFacade(this);
```

`StandardWrapperFacade` 클래스는 `ServletConfig` 타입의 클래스 수준 변수 `config`를 제공한다.

```java
private ServletConfig config = null;
```

`StandardWrapper` 객체 내부에서 `StandardWrapperFacade` 인스턴스를 생성할 때, 생성자는 그 `StandardWrapper` 객체를 전달받아 `config` 변수에 대입한다.

```java
public StandardWrapperFacade(StandardWrapper config) {
  super();
  this.config = (ServletConfig) config;
}
```

따라서 `StandardWrapper`가 서블릿 인스턴스의 `init()` 메서드를 호출할 때는 `StandardWrapperFacade` 인스턴스를 전달한다. 서블릿 내부에서 `ServletConfig`의 `getServletName()`, `getInitParameter()`, `getInitParameterNames()`를 호출하면, 해당 호출은 단순히 `StandardWrapper`에 구현된 대응 메서드로 전달된다.

```java
public String getServletName() {
  return config.getServletName();
}
public String getInitParameter(String name) {
  return config.getInitParameter(name);
}
public Enumeration getInitParameterNames() {
  return config.getInitParameterNames();
}
```

`getServletContext()` 호출은 조금 더 복잡하다.

```java
public ServletContext getServletContext() {
  ServletContext theContext = config.getServletContext();
  if ((theContext != null) && (theContext instanceof
ApplicationContext))
      theContext = ((ApplicationContext) theContext).getFacade();
  return (theContext);
}
```

이 메서드는 `StandardWrapper` 클래스의 `getServletContext()`를 호출하지만, `ServletContext` 객체 자체 대신 해당 객체의 퍼사드(façade)를 반환한다.

## StandardWrapperValve (원본 PDF 237~238페이지)

`StandardWrapperValve` 클래스는 `StandardWrapper` 인스턴스의 기본 밸브다. 이 밸브는 다음 두 가지 작업을 수행한다.

- 해당 서블릿에 연결된 모든 필터를 실행한다.
- 요청을 처리할 서블릿의 `service()` 메서드를 호출한다.

이를 위해 `StandardWrapperValve`는 `invoke()` 메서드에서 다음 작업을 수행해야 한다.

1. `StandardWrapper.allocate()`를 호출하여 해당 래퍼가 나타내는 서블릿 인스턴스를 얻는다.
2. 비공개 메서드 `createFilterChain()`을 호출해 필터 체인을 생성한다.
3. 필터 체인의 `doFilter()`를 호출한다. 이 과정에는 서블릿의 `service()` 호출도 포함된다.
4. 필터 체인을 해제한다.
5. 래퍼의 `deallocate()`를 호출한다.
6. 서블릿을 영구적으로 사용할 수 없는 경우 래퍼의 `unload()`를 호출한다.

다음은 `invoke()` 메서드의 핵심 부분이다.

```java
  // Allocate a servlet instance to process this request
  try {
      if (!unavailable) {
          servlet = wrapper.allocate();
      }
  }
  ...
  // Acknowlege the request
  try {
      response.sendAcknowledgement();
  }
  ...
  // Create the filter chain for this request
  ApplicationFilterChain filterChain = createFilterChain(request,
      servlet);
  // Call the filter chain for this request
  // This also calls the servlet's servicet() method
  try {
      String jspFile = wrapper.getJspFile();
      if (jspFile != null)
          sreq.setAttribute(Globals.JSP_FILE_ATTR, jspFile);
      else
          sreq.removeAttribute(Globals.JSP_FILE_ATTR);
      if ((servlet != null) && (filterChain != null)) {
          filterChain.doFilter(sreq, sres);
      }
      sreq.removeAttribute(Globals.JSP_FILE_ATTR);
  }
  ...
  // Release the filter chain (if any) for this request
  try {
      if (filterChain != null)
          filterChain.release();
  }
  ...
  // Deallocate the allocated servlet instance
  try {
      if (servlet != null) {
          wrapper.deallocate(servlet);
      }
  }
  ...
  // If this servlet has been marked permanently unavailable,
  // unload it and release this instance
  try {
      if ((servlet != null) && (wrapper.getAvailable() ==
          Long.MAX_VALUE)) {
          wrapper.unload();
      }
  }
  ...
```

여기서 가장 중요한 부분은 `createFilterChain()` 호출과 필터 체인의 `doFilter()` 호출이다. `createFilterChain()`은 `ApplicationFilterChain` 인스턴스를 만들고, `StandardWrapper`가 나타내는 서블릿에 적용할 모든 필터를 추가한다. `ApplicationFilterChain` 클래스는 이 장 뒤쪽의 「ApplicationFilterChain」 절에서 자세히 설명한다. 다만 이 클래스를 완전히 이해하려면 `FilterDef`와 `ApplicationFilterConfig` 클래스를 먼저 이해해야 한다. 두 클래스는 각각 「FilterDef」와 「ApplicationFilterConfig」 절에서 설명한다.

> **원문 코드 주의:** 위 발췌 코드의 `Acknowlege`, `servicet()` 같은 표기는 원본에 인쇄된 그대로 유지했다. `...`는 원문에서 생략한 코드 구간을 나타낸다.

## FilterDef (원본 PDF 238~239페이지)

`org.apache.catalina.deploy.FilterDef` 클래스는 배포 서술자(deployment descriptor)의 `filter` 요소에 정의된 필터를 나타낸다. 목록 11.1은 이 클래스를 보여준다.

**목록 11.1: `FilterDef` 클래스**

```java
package org.apache.catalina.deploy;

import java.util.HashMap;
import java.util.Map;

public final class FilterDef {
  /**
    * The description of this filter.
    */
  private String description = null;
  public String getDescription() {
     return (this.description);
  }
  public void setDescription(String description) {
     this.description = description;
  }

  /**
    * The display name of this filter.
    */
  private String displayName = null;
  public String getDisplayName() {
     return (this.displayName);
  }
  public void setDisplayName(String displayName) {
     this.displayName = displayName;
  }

  /**

    * The fully qualified name of the Java class that implements this
    * filter.
    */
  private String filterClass = null;
  public String getFilterClass() {
     return (this.filterClass);
  }
  public void setFilterclass(String filterClass) {
     this.filterClass = filterClass;
  }

  /**
   * The name of this filter, which must be unique among the filters
   * defined for a particular web application.
   */
  private String filterName = null;
```

> **이어짐:** 목록 11.1의 `FilterDef` 클래스 구현은 원본 PDF 240페이지에서 계속된다. `setFilterclass`의 대소문자도 원문 표기를 유지했다.


## `FilterDef` 클래스 구현의 나머지 부분 (원본 PDF 240페이지)

다음은 앞 페이지에서 시작한 목록 11.1의 나머지 코드다.

```java
public String getFilterName() {
  return (this.filterName);
}
public void setFilterName(String filterName) {
  this.filterName = filterName;
}

/**
  * The large icon associated with this filter.
  */
private String largeIcon = null;
public String getLargeIcon() {
   return (this.largeIcon);
}
public void setLargeIcon(String largeIcon) {
   this.largeIcon = largeIcon;
}

/**
  * The set of initialization parameters for this filter, keyed by
  * parameter name.
  */
private Map parameters = new HashMap();
public Map getParameterMap() {
   return (this.parameters);
}

/**
  * The small icon associated with this filter.
  */
private String smallIcon = null;
public String getSmallIcon() {
   return (this.smallIcon);
}
public void setSmallIcon(String smallIcon) {
   this.smallIcon = smallIcon;
}

public void addInitParameter(String name, String value) {
  parameters.put(name, value);

}
/**
  * Render a String representation of this object.
  */
public String toString() {
   StringBuffer sb = new StringBuffer("FilterDef[");
   sb.append("filterName=");
   sb.append(this.filterName);
   sb.append(", filterClass=");
   sb.append(this.filterClass);
   sb.append("]");
   return (sb.toString());
}
}
```

`FilterDef` 클래스의 각 속성은 `filter` 요소 아래에 나타날 수 있는 하위 요소를 나타낸다. 이 클래스에는 `parameters`라는 `Map`도 있으며, 해당 필터의 모든 초기화 매개변수를 저장한다. `addInitParameter` 메서드는 초기화 매개변수의 이름과 값으로 이루어진 한 쌍을 이 맵에 추가한다.

## `ApplicationFilterConfig` (원본 PDF 241~242페이지)

`org.apache.catalina.core.ApplicationFilterConfig` 클래스는 `javax.servlet.FilterConfig` 인터페이스를 구현한다. `ApplicationFilterConfig`는 웹 애플리케이션이 처음 시작될 때 생성되는 필터 인스턴스를 관리한다.

`ApplicationFilterConfig` 객체를 만들려면 `org.apache.catalina.Context` 객체와 `FilterDef` 객체를 생성자에 전달한다.

```java
public ApplicationFilterConfig(Context context, FilterDef filterDef)
  throws ClassCastException, ClassNotFoundException,
     IllegalAccessException, InstantiationException, ServletException
```

`Context` 객체는 웹 애플리케이션을 나타내며, `FilterDef` 객체는 필터 정의를 나타낸다. `ApplicationFilterConfig` 클래스의 `getFilter` 메서드는 `javax.servlet.Filter` 객체를 반환한다. 이 메서드는 필터 클래스를 로드하고 인스턴스를 생성한다.

```java
Filter getFilter() throws ClassCastException, ClassNotFoundException,
   IllegalAccessException, InstantiationException, ServletException {

    // Return the existing filter instance, if any
    if (this.filter != null)
        return (this.filter);

    // Identify the class loader we will be using
    String filterClass = filterDef.getFilterClass();
    ClassLoader classLoader = null;
    if (filterClass.startsWith("org.apache.catalina."))
        classLoader = this.getClass().getClassLoader();
    else
        classLoader = context.getLoader().getClassLoader();

    ClassLoader oldCtxClassLoader =
      Thread.currentthread().getContextClassLoader();

    // Instantiate a new instance of this filter and return it
    Class clazz = classLoader.loadClass(filterClass);
    this.filter = (Filter) clazz.newInstance();
    filter.init(this);
    return (this.filter);
}
```

> 원문 코드에는 `Thread.currentthread()`라고 표기되어 있다. 위 코드는 원문의 대소문자를 그대로 보존했다.

## `ApplicationFilterChain` (원본 PDF 242~243페이지)

`org.apache.catalina.core.ApplicationFilterChain` 클래스는 `javax.servlet.FilterChain` 인터페이스의 구현체다. `StandardWrapperValve` 클래스의 `invoke` 메서드는 이 클래스의 인스턴스를 만들고 `doFilter` 메서드를 호출한다. `ApplicationFilterChain`의 `doFilter` 메서드는 체인에 있는 첫 번째 필터의 `doFilter` 메서드를 호출한다. `Filter` 인터페이스의 `doFilter` 메서드 시그니처는 다음과 같다.

```java
public void doFilter(ServletRaquest request, ServletResponse response,
  FilterChain chain) throws java.io.IOException, ServletException
```

`ApplicationFilterChain` 클래스의 `doFilter` 메서드는 필터의 `doFilter` 메서드에 세 번째 인수로 자기 자신을 전달한다.

필터는 자신의 `doFilter` 메서드에서 `FilterChain` 객체의 `doFilter` 메서드를 명시적으로 호출하여 다음 필터가 실행되도록 할 수 있다. 다음은 필터의 `doFilter` 구현 예제다.

```java
public void doFilter(ServletRequest request, ServletResponse response,
  FilterChain chain) throws IOException, ServletException {
  // do something here
  ...
  chain.doFilter(request, response);
}
```

위 코드에서 보듯 `doFilter` 메서드의 마지막 줄은 `FilterChain`의 `doFilter` 메서드를 호출한다. 현재 필터가 체인의 마지막 필터라면 이 호출로 요청 대상 서블릿의 `service` 메서드가 실행된다. 반대로 필터가 `chain.doFilter`를 호출하지 않으면 다음 필터는 실행되지 않는다.

> 원문 첫 번째 메서드 시그니처의 `ServletRaquest` 표기는 원문 그대로 유지했다.

## 애플리케이션 (원본 PDF 243~244페이지)

애플리케이션은 `ex11.pyrmont.core.SimpleContextConfig`와 `ex11.pyrmont.startup.Bootstrap`의 두 클래스로 구성된다. `SimpleContextConfig` 클래스는 이전 장 애플리케이션에서 사용한 것의 복사본이다. `Bootstrap` 클래스는 목록 11.2에 제시되어 있다.

**목록 11.2: `Bootstrap` 클래스**

```java
package ex11.pyrmont.startup;
//use StandardWrapper
import ex11.pyrmont.core.SimpleContextConfig;
import org.apache.catalina.Connector;
import org.apache.catalina.Context;
import org.apache.catalina.Lifecycle;
import org.apache.catalina.LifecycleListener;
import org.apache.catalina.Loader;
import org.apache.catalina.Wrapper;
import org.apache.catalina.connector.http.HttpConnector;
import org.apache.catalina.core.StandardContext;
import org.apache.catalina.core.StandardWrapper;
import org.apache.catalina.loader.WebappLoader;

public final class Bootstrap {
  public static void main(String[] args) {
     System.setProperty("catalina.base",
        System.getProperty("user.dir"));
     Connector connector = new HttpConnector();
     Wrapper wrapper1 = new StandardWrapper();
     wrapper1.setName("Primitive");
     wrapper1.setServletClass("PrimitiveServlet");
     Wrapper wrapper2 = new StandardWrapper();
     wrapper2.setName("Modern");
     wrapper2.setServletClass("ModernServlet");

     Context context = new StandardContext();
     // StandardContext's start method adds a default mapper
     context.setPath("/myApp");
     context.setDocBase("myApp");
     LifecycleListener listener = new SimpleContextConfig();
     ((Lifecycle) context).addLifecycleListener(listener);
     context.addChild(wrapper1);
     context.addChild(wrapper2);
     // for simplicity, we don't add a valve, but you can add
     // valves to context or wrapper just as you did in Chapter 6
     Loader loader = new WebappLoader();
     context.setLoader(loader);
     // context.addServletMapping(pattern, name);
     context.addServletMapping("/Primitive", "Primitive");
     context.addServletMapping("/Modern", "Modern");
     // add ContextConfig. This listener is important because it
     // configures StandardContext (sets configured to true), otherwise
     // StandardContext won't start

     connector.setContainer(context);
     try {
        connector.initialize();
        ((Lifecycle) connector).start();
        ((Lifecycle) context).start();
        // make the application wait until we press a key.
        System.in.read();
        ((Lifecycle) context).stop();
     }
     catch (Exception e) {
        e.printStackTrace();
     }
  }
}
```

`Bootstrap` 클래스는 `StandardContext` 인스턴스를 만들고 이름을 `myApp`으로 지정한다. 그런 다음 `StandardContext`에 `Primitive`와 `Modern`이라는 두 `StandardWrapper` 인스턴스를 추가한다.

## 애플리케이션 실행 (원본 PDF 244페이지)

Windows에서 애플리케이션을 실행하려면 작업 디렉터리에서 다음 명령을 입력한다.

```text
java -classpath ./lib/servlet.jar;./lib/commons-collections.jar;./
ex11.pyrmont.startup.Bootstrap
```

Linux에서는 두 라이브러리를 구분할 때 세미콜론 대신 콜론(`:`)을 사용한다.



## 애플리케이션 실행 — 계속 (원본 PDF 245페이지)

Linux에서 애플리케이션을 실행하려면 다음 명령을 사용한다.

```text
java -classpath ./lib/servlet.jar:./lib/commons-collections.jar:./
ex11.pyrmont.startup.Bootstrap
```

`PrimitiveServlet`을 호출하려면 브라우저에서 다음 URL을 사용한다.

```text
http://localhost:8080/Primitive
```

`ModernServlet`을 호출하려면 다음 URL을 사용한다.

```text
http://localhost:8080/Modern
```

## 요약

이 장에서는 Catalina의 `Wrapper` 인터페이스에 대한 표준 구현인 `StandardWrapper` 클래스를 배웠다. 또한 필터 및 필터 관련 클래스도 살펴보았다. 마지막에는 `StandardWrapper` 클래스를 사용하는 애플리케이션을 소개했다.

**11장 끝 — 원본 PDF 245페이지.**
