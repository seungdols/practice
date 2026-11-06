# 13장: Host와 Engine (Host and Engine)

> 원본 PDF 264~268페이지 번역. 마지막 페이지에서 시작된 `StandardHost.map()` 코드는 다음 페이지에 이어진다. Java 코드는 원문을 유지한다.

## 개요 (Overview)

이 장에서는 호스트(host)와 엔진(engine)이라는 두 가지 주제를 다룬다. 동일한 Tomcat 배포 환경에서 둘 이상의 컨텍스트를 실행하려면 호스트를 사용한다. 이론적으로 컨텍스트가 하나뿐이라면 호스트가 필요하지 않다. `org.apache.catalina.Context` 인터페이스의 설명에도 다음과 같이 나와 있다.

- Context에 연결된 부모 Container는 일반적으로 Host이지만, 다른 구현일 수도 있고 필요하지 않다면 생략할 수도 있다.

하지만 실제 Tomcat 배포에서는 항상 호스트가 필요하다. 그 이유는 이 장 뒤쪽의 「호스트 없이 실행할 수 없는 이유(Why You Cannot Live without a Host)」 절에서 설명한다.

엔진은 Catalina 서블릿 엔진 전체를 나타낸다. 엔진을 사용한다면 엔진은 언제나 최상위 컨테이너가 된다. 엔진에 추가하는 자식 컨테이너는 보통 `org.apache.catalina.Host` 또는 `org.apache.catalina.Context`의 구현체다. 기본적으로 Tomcat 배포 환경에서는 엔진을 사용한다. 이때 엔진에는 기본 호스트(default host) 하나가 있다.

이 장에서는 `Host` 및 `Engine` 인터페이스와 관련된 클래스들을 설명한다. 먼저 `Host` 인터페이스를 살펴보고 `StandardHost`, Tomcat 4의 `StandardHostMapper`, `StandardHostValve` 클래스를 차례로 다룬다. 그런 다음 호스트를 최상위 컨테이너로 사용하는 예제 애플리케이션을 소개하면서 호스트에 관한 설명을 마친다. 두 번째 주제는 `Engine` 인터페이스에서 시작하여 `StandardEngine`과 `StandardEngineValve` 클래스로 이어진다. 마지막으로 엔진을 최상위 컨테이너로 사용하는 이 장의 두 번째 예제 애플리케이션을 살펴본다.

## Host 인터페이스 (The Host Interface)

호스트는 `org.apache.catalina.Host` 인터페이스로 표현된다. 이 인터페이스는 `Container` 인터페이스를 확장하며, 목록 13.1에 제시되어 있다.

**목록 13.1: Host 인터페이스**

```java
package org.apache.catalina;

public interface Host extends Container {
  public static final String ADD_ALIAS_EVENT = "addAlias";
  public static final String REMOVE_ALIAS_EVENT = "removeAlias";
  /**
   * Return the application root for this Host.
   * This can be an absolute
   * pathname, a relative pathname, or a URL.
   */
  public String getAppBase();

  /**
   * Set the application root for this Host. This can be an absolute
   * pathname, a relative pathname, or a URL.
   *
   * @param appBase The new application root
   */
  public void setAppBase(String appBase);

  /**
   * Return the value of the auto deploy flag.
   * If true, it indicates that
   * this host's child webapps should be discovred and automatically
   * deployed.
   */
  public boolean getAutoDeploy();

  /**
   * Set the auto deploy flag value for this host.
   *
   * @param autoDeploy The new auto deploy flag
   */
  public void setAutoDeploy(boolean autoDeploy);

  /**
   * Set the DefaultContext
   * for new web applications.
   *
   * @param defaultContext The new DefaultContext
   */
  public void addDefaultContext(DefaultContext defaultContext);

  /**
   * Retrieve the DefaultContext for new web applications.
   */

  public DefaultContext getDefaultContext();

  /**
   * Return the canonical, fully qualified, name of the virtual host
   * this Container represents.
   */
  public String getName();

  /**
   * Set the canonical, fully qualified, name of the virtual host
   * this Container represents.
   *
   * @param name Virtual host name
   *
   * @exception IllegalArgumentException if name is null
   */
  public void setName(String name);

  /**
   * Import the DefaultContext config into a web application context.
   *
   * @param context web application context to import default context
   */
  public void importDefaultContext(Context context);

  /**
   * Add an alias name that should be mapped to this same Host.
   *
   * @param alias The alias to be added
   */
  public void addAlias(String alias);

  /**
   * Return the set of alias names for this Host. If none are defined,
   * a zero length array is returned.
   */
  public String[] findAliases();

  /**
   * Return the Context that would be used to process the specified
   * host-relative request URI, if any; otherwise return
   * <code>null</code>.
   *
   * @param uri Request URI to be mapped
   */
  public Context map(String uri);

  /**
   * Remove the specified alias name from the aliases for this Host.
   * @param alias Alias name to be removed
   */
  public void removeAlias(String alias);
}
```

특히 중요한 것은 들어오는 요청을 처리할 적절한 컨텍스트를 반환하는 `map` 메서드다. 이 메서드의 구현은 다음 절에서 설명할 `StandardHost` 클래스에서 확인할 수 있다.

## StandardHost

`org.apache.catalina.core.StandardHost` 클래스는 `Host`의 표준 구현체다. 이 클래스는 `org.apache.catalina.core.ContainerBase` 클래스를 상속하고 `Host` 및 `Deployer` 인터페이스를 구현한다. `Deployer`는 17장에서 설명한다.

`StandardContext` 및 `StandardWrapper` 클래스와 마찬가지로, `StandardHost` 생성자는 파이프라인에 기본 밸브(basic valve)를 추가한다.

```java
public StandardHost() {
  super();
  pipeline.setBasic(new StandardHostValve());
}
```

보이는 것처럼 기본 밸브는 `org.apache.catalina.core.StandardHostValve`의 인스턴스다.

`StandardHost`가 시작될 때, 즉 `start` 메서드가 호출될 때 두 밸브인 `ErrorReportValve`와 `ErrorDispatcherValve`를 추가한다. 두 밸브는 모두 `org.apache.catalina.valves` 패키지에 속한다. Tomcat 4의 `StandardHost.start` 메서드는 목록 13.2와 같다.

**목록 13.2: StandardHost의 start 메서드**

```java
public synchronized void start() throws LifecycleException {
  // Set error report valve
  if ((errorReportValveClass != null)
      && (!errorReportValveClass.equals(""))) {
      try {
         Valve valve =
            (Valve) Class.forName(errorReportValveClass).newInstance();
         addValve(valve);
      }
      catch (Throwable t) {
         log(sm.getString
            ("StandardHost.invalidErrorReportValveClass",
              errorReportValveClass));
      }
    }
    // Set dispatcher valve
    addValve(new ErrorDispatcherValve());
    super.start();
}
```

**참고:** Tomcat 5의 `start` 메서드는 이와 비슷하지만 JXM 객체 이름을 구성하는 코드가 포함되어 있다는 점이 다르다. 이 내용은 20장에서 설명한다. (원문 표기: `JXM`.)

`errorReportValveClass`의 값은 `StandardHost` 클래스에서 다음과 같이 결정된다.

```java
private String errorReportValveClass =
   "org.apache.catalina.valves.ErrorReportValve";
```

요청이 들어올 때마다 호스트의 `invoke` 메서드가 호출된다. `StandardHost`에는 자체 `invoke` 구현이 없으므로 부모 클래스인 `ContainerBase`의 `invoke` 메서드가 호출된다. 이 메서드는 다시 `StandardHost`의 기본 밸브인 `StandardHostValve`의 `invoke` 메서드를 호출한다. `StandardHostValve.invoke`는 아래의 「StandardHostValve」 절에서 설명한다. `StandardHostValve.invoke`는 여러 작업 중 하나로 `StandardHost.map` 메서드를 호출하여 요청을 처리할 적절한 컨텍스트를 얻는다. `StandardHost`의 `map` 구현은 목록 13.3에 제시되어 있다.

**목록 13.3: StandardHost 클래스의 map 메서드 — 다음 페이지에서 계속**

```java
public Context map(String uri) {
  if (debug > 0)
      log("Mapping request URI '" + uri + "'");
  if (uri == null)
```

> **이어지는 부분:** 원본 PDF 268페이지는 `if (uri == null)`에서 끝난다. 나머지 코드는 269페이지부터 계속 번역한다.


## StandardHost.map 메서드 (목록 13.3 계속, PDF 269페이지)

```java
      return (null);

    // Match on the longest possible context path prefix
    if (debug > 1)
        log(" Trying the longest context path prefix");
    Context context = null;
    String mapuri = uri;
    while (true) {
        context = (Context) findChild(mapuri);
        if (context != null)
            break;
        int slash = mapuri.lastIndexOf('/');
        if (slash < 0)
            break;
        mapuri = mapuri.substring(0, slash);
    }

    // If no Context matches, select the default Context
    if (context == null) {
        if (debug > 1)
            log(" Trying the default context");
        context = (Context) findChild("");
    }

    // Complain if no Context has been selected

    if (context == null) {
        log(sm.getString("standardHost.mappingError", uri));
        return (null);
    }

    // Return the mapped Context (if any)
    if (debug > 0)
        log(" Mapped to context '" + context.getPath() + "'");
    return (context);
}
```

Tomcat 4의 `ContainerBase` 클래스에도 다음 시그니처를 가진 `map` 메서드가 정의되어 있다는 점에 주목하자.

```java
public Container map(Request request, boolean update);
```

Tomcat 4에서 `StandardHostValve`의 `invoke` 메서드는 `ContainerBase`의 `map` 메서드를 호출하고, 이 메서드는 다시 `StandardHost`의 `map` 메서드를 호출한다. Tomcat 5는 매퍼 컴포넌트를 사용하지 않으며, 요청 객체에서 올바른 컨텍스트를 얻는다.

## StandardHostMapper

Tomcat 4에서는 `StandardHost`의 부모 클래스인 `ContainerBase`가 `start` 메서드에서 `addDefaultMapper`를 호출하여 기본 매퍼를 생성한다. 기본 매퍼의 유형은 `mapperClass` 속성으로 지정된다. 다음은 `ContainerBase`의 `addDefaultMapper` 메서드다.

```java
protected void addDefaultMapper(String mapperClass) {
  // Do we need a default Mapper?
  if (mapperClass == null)
      return;
  if (mappers.size() >= 1)
      return;

    // Instantiate and add a default Mapper
    try {
       Class clazz = Class.forName(mapperClass);
       Mapper mapper = (Mapper) clazz.newInstance();
       mapper.setProtocol("http");
       addMapper(mapper);
    }
    catch (Exception e) {
       log(sm.getString("containerBase.addDefaultMapper", mapperClass),
          e);
    }
}
```

`StandardHost` 클래스는 `mapperClass` 변수를 다음과 같이 정의한다.

```java
private String mapperClass =
   "org.apache.catalina.core.StandardHostMapper";
```

또한 `StandardHost` 클래스의 `start` 메서드는 본문 마지막에 `super.start()`를 호출하므로 기본 매퍼가 생성되도록 보장한다.

**참고:** Tomcat 4의 `StandardContext` 클래스는 기본 매퍼를 생성할 때 약간 다른 방식을 사용한다. `start` 메서드에서 `super.start()`를 호출하지 않는다. 대신 `StandardContext`의 `start` 메서드가 `mapperClass` 변수를 전달하여 `addDefaultMapper` 메서드를 호출한다. (원문에는 `standardContext`, `supoer.start()`, `Standardcontext`라는 표기가 있다.)

`StandardHostMapper`에서 가장 중요한 메서드는 당연히 `map`이다. 구현은 다음과 같다.

```java
public Container map(Request request, boolean update) {
  // Has this request already been mapped?
  if (update && (request.getContext() != null))
      return (request.getContext());

    // Perform mapping on our request URI
    String uri = ((HttpRequest) request).getDecodedRequestURI();
    Context context = host.map(uri);

    // Update the request (if requested) and return the selected Context
    if (update) {
        request.setContext(context);
        if (context != null)
            ((HttpRequest) request).setContextPath(context.getPath());
        else
            ((HttpRequest) request).setContextPath(null);
    }
    return (context);
}
```

`map` 메서드는 단순히 `Host`의 `map` 메서드를 호출한다는 점에 주목하자!

## StandardHostValve

`org.apache.catalina.core.StandardHostValve` 클래스는 `StandardHost`의 기본 밸브다. HTTP 요청이 들어오면 이 클래스의 `invoke` 메서드(목록 13.4)가 호출된다.

**목록 13.4: StandardHostValve의 invoke 메서드**

```java
public void invoke(Request request, Response response,
  ValveContext valveContext)
  throws IOException, ServletException {

    // Validate the request and response object types

    if (!(request.getRequest() instanceof HttpServletRequest) ||
        !(response.getResponse() instanceof HttpServletResponse)) {
        return;      // NOTE - Not much else we can do generically
    }

    // Select the Context to be used for this Request
    StandardHost host = (StandardHost) getContainer();
    Context context = (Context) host.map(request, true);
    if (context == null) {
        ((HttpServletResponse) response.getResponse()).sendError
        (HttpServletResponse.SC_INTERNAL_SERVER_ERROR,
        sm.getstring("StandardHost.noContext"));
        return;
    }

    // Bind the context CL to the current thread
    Thread.currentThread().setContextClassLoader
      (context.getLoader().getClassLoader());

    // Update the session last access time for our session (if any)
    HttpServletRequest hreq = (HttpServletRequest) request.getRequest();
    String sessionId = hreq.getRequestedSessionId();
    if (sessionId != null) {
        Manager manager = context.getManager();
        if (manager != null) {
            Session session = manager.findSession(sessionId);
            if ((session != null) && session.isValid())
                session.access();
            }
        }

    // Ask this Context to process this request
    context.invoke(request, response);
}
```

Tomcat 4의 `invoke` 메서드는 `StandardHost`의 `map` 메서드를 호출하여 적절한 컨텍스트를 얻는다.

```java
    // Select the Context to be used for this Request
    StandardHost host = (StandardHost) getContainer();
    Context context = (Context) host.map(request, true);
```

**참고:** 위에서 `Context` 객체를 얻는 과정에는 호출이 한 번 왕복한다. 위 `map` 메서드는 두 개의 인수를 받는데, 이는 `ContainerBase` 클래스에 정의된 `map` 메서드다. `ContainerBase`의 `map` 메서드는 자식 객체(이 경우 `StandardHost` 인스턴스)에서 적절한 매퍼를 찾은 다음 그 매퍼의 `map` 메서드를 호출한다.

그런 다음 `invoke` 메서드는 요청 객체와 연결된 세션 객체를 얻어 `access` 메서드를 호출한다. `access` 메서드는 마지막 접근 시간을 갱신한다. 다음은 `org.apache.catalina.session.StandardSession` 클래스의 `access` 메서드다.

```java
public void access() {
  this.isNew = false;
  this.lastAccessedTime = this.thisAccessedTime;
  this.thisAccessedTime = System.currentTimeMillis();
}
```

마지막으로 `invoke` 메서드는 컨텍스트의 `invoke` 메서드를 호출하여 해당 컨텍스트가 요청을 처리하도록 한다.

## Host 없이 살 수 없는 이유 (Why You Cannot Live without a Host)

각 컨텍스트를 `ContextConfig`로 구성하려면 Tomcat 4와 Tomcat 5의 배포 환경에 호스트가 반드시 있어야 한다. 이유는 다음과 같다.

`ContextConfig`에는 애플리케이션의 `web.xml` 파일 위치가 필요하다. `applicationConfig` 메서드에서 `web.xml`을 열려고 시도한다. 다음은 `applicationConfig` 메서드의 일부다.

```java
synchronized (webDigester) {
  try {
     URL url =
         servletContext.getResource(Constants.ApplicationWebXml);
     InputSource is = new InputSource(url.toExternalForm());
     is.setByteStream(stream);
     ...
     webDigester.parse(is);
     ...
```

여기서 `Constants.ApplicationWebXml`은 `web.xml` 파일의 상대 경로인 `/WEB-INF/web.xml`이고, `servletContext`는 `javax.servlet.ServletContext`를 구현하는 `org.apache.catalina.core.ApplicationContext` 타입의 객체다.

다음은 `ApplicationContext`의 `getResource` 메서드다. **코드는 다음 PDF 페이지에서 이어진다.**

```java
public URL getResource(String path)
  throws MalformedURLException {

  DirContext resources = context.getResources();
  if (resources != null) {
```

> 번역 범위: 원본 PDF 269~273페이지. `getResource` 메서드의 나머지는 274페이지부터 이어진다.


## `ApplicationContext.getResource()`의 나머지와 Host가 필요한 이유

앞에서 소개한 `getResource()` 구현은 리소스에 접근할 때 컨텍스트의 정보를 사용한다. 이어지는 코드는 다음과 같다.

```java
     String fullPath = context.getName() + path;
     // this is the problem. Host must not be null
     String hostName = context.getParent().getName();
```

마지막 줄을 보면 `ContextConfig`가 컨텍스트를 구성하려면 해당 컨텍스트에 부모 컨테이너, 즉 호스트가 있어야 한다는 사실이 명확해진다. `web.xml` 파일을 파싱하는 방법은 15장 「Digester」에서 배운다. 요약하면, 자체 `ContextConfig` 클래스를 작성하지 않는 한 호스트가 필요하다.

## 애플리케이션 1 (Application 1)

이 장의 첫 번째 애플리케이션은 호스트를 최상위 컨테이너로 사용하는 방법을 보여준다. 이 애플리케이션은 `ex13.pyrmont.core.SimpleContextConfig`와 `ex13.pyrmont.startup.Bootstrap1` 두 클래스를 사용한다. `SimpleContextConfig`는 11장에서 복사한 클래스다. 다음 목록 13.5는 `Bootstrap1` 클래스다.

**목록 13.5: `Bootstrap1` 클래스**

```java
package ex13.pyrmont.startup;

import ex13.pyrmont.core.SimpleContextConfig;
import org.apache.catalina.Connector;
import org.apache.catalina.Context;
import org.apache.catalina.Host;
import org.apache.catalina.Lifecycle;
import org.apache.catalina.LifecycleListener;
import org.apache.catalina.Loader;
import org.apache.catalina.Wrapper;
import org.apache.catalina.connector.http.HttpConnector;
import org.apache.catalina.core.StandardContext;
import org.apache.catalina.core.StandardHost;
import org.apache.catalina.core.StandardWrapper;
import org.apache.catalina.loader.WebappLoader;

public final class Bootstrap1 {
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
     context.setPath("/app1");
     context.setDocBase("app1");
     context.addChild(wrapper1);
     context.addChild(wrapper2);
     LifecycleListener listener = new SimpleContextConfig();
     ((Lifecycle) context).addLifecycleListener(listener);
     Host host = new StandardHost();
     host.addChild(context);
     host.setName("localhost");
     host.setAppBase("webapps");
     Loader loader = new WebappLoader();
     context.setLoader(loader);
     // context.addServletMapping(pattern, name);

     context.addServletMapping("/Primitive", "Primitive");
     context.addServletMapping("/Modern", "Modern");
     connector.setContainer(host);
     try {
        connector.initialize();
        ((Lifecycle) connector).start();
        ((Lifecycle) host).start();
        // make the application wait until we press a key.
        System.in.read();
        ((Lifecycle) host).stop();
     }
     catch (Exception e) {
        e.printStackTrace();
     }
  }
}
```

### 애플리케이션 실행 (Running the Applications)

Windows에서는 작업 디렉터리에서 다음 명령을 입력해 애플리케이션을 실행한다.

```text
java -classpath ./lib/servlet.jar;./lib/commons-
collections.jar;./lib/commons—digester.jar;./
ex13.pyrmont.startup.Bootstrap1
```

Linux에서는 라이브러리 경로를 구분할 때 콜론(`:`)을 사용한다.

```text
java -classpath ./lib/servlet.jar:./lib/commons-
collections.jar:./lib/commons-digester.jar:./
ex13.pyrmont.startup.Bootstrap1
```

`PrimitiveServlet`을 호출하려면 브라우저에서 다음 URL을 사용한다.

```text
http://localhost:8080/app1/Primitive
```

`ModernServlet`을 호출하려면 다음 URL을 사용한다.

```text
http://localhost:8080/app1/Modern
```

> 원문 명령은 PDF에 표시된 줄바꿈과 대시 표기를 유지했다. 실제 셸에서 실행할 때는 클래스패스와 클래스 이름을 환경에 맞게 한 줄로 연결해야 한다.

## Engine 인터페이스 (The Engine Interface)

`org.apache.catalina.Engine` 인터페이스는 엔진을 나타낸다. 엔진은 Catalina 서블릿 엔진 전체를 나타낸다. 여러 가상 호스트를 지원하려면 엔진을 사용하는 것이 좋다. 실제로 일반적인 Tomcat 배포 환경에서는 엔진을 사용한다.

**목록 13.6: `Engine` 인터페이스**

```java
package org.apache.catalina;

public interface Engine extends Container {
  /**
   * Return the default hostname for this Engine.
   */
  public String getDefaultHost();
  /**
   * Set the default hostname for this Engine.
   *
   * @param defaultHost The new default host
   */
  public void setDefaultHost(String defaultHost);
  /**
   * Retrieve the JvmRouteId for this engine.
   */
  public String getJvmRoute();
  /**
   * Set the JvmRouteId for this engine.
   *
   * @param jvmRouteId the (new) JVM Route ID. Each Engine within a
   * cluster must have a unique JVM Route ID.
   */
  public void setJvmRoute(String jvmRouteId);
  /**
   * Return the <code>Service</code> with which we are associated (if
   * any).
   */
  public Service getService();
  /**
   * Set the <code>Service</code> with which we are associated (if
   * any).
   *
   * @param service The service that owns this Engine
   */
  public void setService(Service service);
  /**
   * Set the DefaultContext
   * for new web applications.
   *
   * @param defaultContext The new DefaultContext
   */
  public void addDefaultContext(DefaultContext defaultContext);
  /**
   * Retrieve the DefaultContext for new web applications.
   */
  public DefaultContext getDefaultContext();
  /**
   * Import the DefaultContext config into a web application context.
   *
   * @param context web application context to import default context
   */
  public void importDefaultContext(Context context);
}
```

`Engine`에는 기본 호스트를 지정하거나 기본 컨텍스트를 추가할 수 있다. 또한 엔진은 서비스와 연결될 수 있다. 서비스는 14장에서 다룬다.

## StandardEngine

`org.apache.catalina.core.StandardEngine`은 `Engine` 인터페이스의 표준 구현이다. `StandardContext`, `StandardHost`와 비교하면 `StandardEngine` 클래스는 비교적 작다. 다음 생성자에서 볼 수 있듯이, `StandardEngine` 인스턴스를 만들면 기본 밸브가 추가된다.

```java
public StandardEngine() {
  super();
  pipeline.setBasic(new StandardEngineValve());
}
```

최상위 컨테이너인 `StandardEngine`에는 자식 컨테이너가 존재한다. 이때 자식 컨테이너는 반드시 호스트여야 한다. 호스트가 아닌 컨테이너를 추가하려고 하면 예외가 발생한다. 다음은 `StandardEngine`의 `addChild()` 메서드다.

```java
public void addChild(Container child) {
  if (!(child instanceof Host))
      throw new IllegalArgumentException
         (sm.getString("StandardEngine.notHost"));
  super.addChild(child);
}
```

엔진은 최상위 컨테이너이므로 부모를 가질 수 없다. 부모를 설정하려고 하면 예외가 발생한다. 다음은 `StandardEngine`의 `setParent()` 메서드다.

```java
public void setParent(Container container) {
  throw new IllegalArgumentException
   (sm.getString("standardEngine.notParent"));
}
```

## StandardEngineValve

`org.apache.catalina.core.StandardEngineValve`는 `StandardEngine`의 기본 밸브다. 다음 목록 13.7은 `StandardEngineValve`의 `invoke()` 메서드를 보여준다.

**목록 13.7: `StandardEngineValve.invoke()` 메서드 — 다음 페이지에서 계속**

```java
public void invoke(Request request, Response response,
  ValveContext valveContext)
  throws IOException, ServletException {
  // Validate the request and response object types
  if (!(request.getRequest() instanceof HttpServletRequest) ||
      !(response.getResponse() instanceof HttpServletResponse)) {
```

> 번역 범위: 원본 PDF 274~278페이지. `StandardEngineValve.invoke()`의 나머지 코드는 279페이지부터 이어진다.

## StandardEngineValve.invoke()의 나머지 구현 (PDF 279페이지)

앞 페이지의 목록 13.7에서 이어지는 코드다. 다음 코드는 앞서 열린 `if` 블록부터 시작한다.

```java
        return;      // NOTE - Not much else we can do generically
    }
    // Validate that any HTTP/1.1 request included a host header
    HttpServletRequest hrequest = (HttpServletRequest) request;
    if ("HTTP/1.1".equals(hrequest.getProtocol()) &&
        (hrequest.getServerName() == null)) {
        ((HttpServletResponse) response.getResponse()).sendError
        (HttpServletResponse.SC_BAD_REQUEST,
           sm.getString("standardEngine.noHostHeader",
        request.getRequest().getServerName()));
        return;
    }
    // Select the Host to be used for this Request
    StandardEngine engine = (StandardEngine) getContainer();
    Host host = (Host) engine.map(request, true);
    if (host == null) {
        ((HttpServletResponse) response.getResponse()).sendError
           (HttpServletResponse.SC_BAD_REQUEST,
           sm.getString("standardEngine.noHost",
           request.getRequest().getServerName()));
        return;
    }
    // Ask this Host to process this request
    host.invoke(request, response);
}
```

`invoke()` 메서드는 먼저 요청 및 응답 객체의 유형이 올바른지 확인한다. 그런 다음 요청을 처리할 `Host` 인스턴스를 얻는다. `Host`는 `Engine`의 `map()` 메서드를 호출하여 얻는다. `Host`를 찾으면 해당 호스트의 `invoke()` 메서드를 호출한다.

## 애플리케이션 2 (PDF 279~281페이지)

이 장의 두 번째 애플리케이션은 `Engine`을 최상위 컨테이너로 사용하는 방법을 보여준다. 이 애플리케이션에서는 `ex13.pyrmont.core.SimpleContextConfig`와 `ex13.pyrmont.startup.Bootstrap2`라는 두 클래스를 사용한다. `Bootstrap2` 클래스는 목록 13.8에 제시되어 있다.

**목록 13.8: `Bootstrap2` 클래스**

```java
package ex13.pyrmont.startup;
//Use engine
import ex13.pyrmont.core.SimpleContextConfig;
import org.apache.catalina.Connector;
import org.apache.catalina.Context;
import org.apache.catalina.Engine;
import org.apache.catalina.Host;
import org.apache.catalina.Lifecycle;
import org.apache.catalina.LifecycleListener;
import org.apache.catalina.Loader;
import org.apache.catalina.Wrapper;
import org.apache.catalina.connector.http.HttpConnector;
import org.apache.catalina.core.StandardContext;
import org.apache.catalina.core.StandardEngine;
import org.apache.catalina.core.StandardHost;
import org.apache.catalina.core.StandardWrapper;
import org.apache.catalina.loader.WebappLoader;

public final class Bootstrap2 {
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
     context.setPath("/app1");
     context.setDocBase("app1");
     context.addChild(wrapper1);
     context.addChild(wrapper2);
     LifecycleListener listener = new SimpleContextConfig();
     ((Lifecycle) context).addLifecycleListener(listener);
     Host host = new StandardHost();
     host.addChild(context);
     host.setName("localhost");
     host.setAppBase("webapps");
     Loader loader = new WebappLoader();
     context.setLoader(loader);
     // context.addServletMapping(pattern, name);
     context.addServletMapping("/Primitive", "Primitive");
     context.addServletMapping("/Modern", "Modern");
     Engine engine = new StandardEngine();
     engine.addChild(host);
     engine.setDefaultHost("localhost");
     connector.setContainer(engine);
     try {
        connector.initialize();
        ((Lifecycle) connector).start();
        ((Lifecycle) engine).start();
        // make the application wait until we press a key.
        System.in.read();
        ((Lifecycle) engine).stop();

     }
     catch (Exception e) {
       e.printStackTrace();
     }
  }
}
```

## 애플리케이션 실행 (PDF 281~282페이지)

Windows에서 애플리케이션을 실행하려면 작업 디렉터리에서 다음 명령을 입력한다.

```text
java -classpath ./lib/servlet.jar;./lib/commons-
collections.jar;./lib/commons-digester.jar;./
ex13.pyrmont.startup.Bootstrap2
```

Linux에서는 라이브러리 사이의 구분자로 콜론(`:`)을 사용한다.

```text
java -classpath ./lib/servlet.jar:./lib/commons-
collections.jar:./lib/commons-digester.jar:./
ex13.pyrmont.startup.Bootstrap2
```

`PrimitiveServlet`을 호출하려면 브라우저에서 다음 URL을 사용한다.

```text
http://localhost:8080/app1/Primitive
```

`ModernServlet`을 호출하려면 다음 URL을 사용한다.

```text
http://localhost:8080/app1/Modern
```

> 실행 명령의 줄바꿈은 원본 PDF의 표기를 따랐다. 실제 터미널에서는 클래스패스와 클래스 이름을 한 줄로 입력해야 한다.

## 요약 (PDF 282페이지)

이 장에서는 두 가지 컨테이너 유형인 `Host`와 `Engine`을 배웠다. 또한 두 컨테이너와 관련된 클래스들을 설명했다. `Host`와 `Engine`을 각각 최상위 컨테이너로 사용하는 방법을 보여주는 두 가지 애플리케이션도 소개했다.

**13장 번역 완료.**
