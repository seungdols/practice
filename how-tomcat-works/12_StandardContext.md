# 12장: StandardContext

> 원본 PDF 246~249페이지 번역. 249페이지의 `start()` 코드 목록은 다음 페이지로 이어진다.

## 개요 (Overview)

앞 장들에서 보았듯이, 컨텍스트(context)는 웹 애플리케이션을 나타내며 하나 이상의 래퍼(wrapper)를 포함한다. 각 래퍼는 서블릿 정의 하나를 나타낸다. 그러나 컨텍스트에는 로더(loader)와 매니저(manager)를 비롯한 다른 컴포넌트도 필요하다. 이 장에서는 Catalina의 `Context` 인터페이스에 대한 표준 구현을 나타내는 `org.apache.catalina.core.StandardContext` 클래스를 설명한다.

먼저 `StandardContext` 객체를 생성하고 구성하는 방법을 살펴본다. 그런 다음 관련 클래스인 `StandardContextMapper`(Tomcat 4)와 `ContextConfig`를 설명한다. 이어서 HTTP 요청이 들어올 때마다 호출되는 메서드들의 순서를 살펴본다. 그다음 `StandardContext` 클래스의 중요한 속성들을 논의하면서 이 클래스를 다시 살펴본다. 마지막 절에서는 Tomcat 5의 `backgroundProcess` 메서드를 설명한다.

> **참고:** 이 장에는 별도의 예제 애플리케이션이 없다. `StandardContext`는 11장과 그 이전 장들에서 사용했다.

## StandardContext 구성 (StandardContext Configuration)

`StandardContext` 인스턴스를 생성한 뒤에는 들어오는 HTTP 요청을 처리할 수 있도록 `start` 메서드를 호출해야 한다. 여러 이유로 `StandardContext` 객체가 시작에 실패할 수 있다. 이런 일이 발생하면 해당 객체의 `available` 속성이 `false`로 설정된다. 이름에서 알 수 있듯이 `available` 속성은 `StandardContext`를 사용할 수 있는지 나타낸다.

`start` 메서드가 성공하려면 `StandardContext` 객체가 올바르게 구성되어 있어야 한다. Tomcat 배포 환경에서 `StandardContext`를 구성할 때는 여러 작업이 수행된다. 먼저 `%CATALINA_HOME%/conf` 디렉터리에 있으며 배포된 모든 애플리케이션에 적용되는 기본 `web.xml` 파일을 읽고 파싱할 수 있도록 `StandardContext`를 준비한다. 또한 애플리케이션 수준의 `web.xml` 파일을 처리할 수 있게 한다. 이 밖에도 인증기 밸브(authenticator valve)와 인증서 밸브(certificate valve)를 설치한다.

> **참고:** `StandardContext` 구성에 관한 자세한 내용은 15장에서 설명한다.

`StandardContext` 클래스의 속성 가운데 `configured`가 있다. 이는 `StandardContext` 인스턴스가 올바르게 구성되었는지를 나타내는 불리언 값이다. `StandardContext`는 이벤트 리스너를 구성 담당자로 사용한다. `StandardContext` 인스턴스의 `start` 메서드를 호출하면 여러 작업 가운데 하나로 생명주기 이벤트(lifecycle event)가 발생한다. 이 이벤트는 리스너를 호출하고, 리스너는 `StandardContext` 인스턴스를 구성한다. 구성이 성공하면 리스너는 `configured` 속성을 `true`로 설정한다. 그렇지 않으면 `StandardContext` 인스턴스는 시작을 거부하며 HTTP 요청을 처리할 수 없게 된다.

11장에서는 `StandardContext` 인스턴스에 추가한 생명주기 리스너의 구현을 보았다. 이 리스너의 타입은 `ch11.pyrmont.core.SimpleContextConfig`이며, 다른 작업은 하지 않고 `StandardContext`의 `configured` 속성을 `true`로 설정하기만 한다. 이를 통해 `StandardContext`가 올바르게 구성되었다고 믿도록 만든다. 실제 Tomcat 배포 환경에서 `StandardContext`를 구성하는 생명주기 리스너는 `org.apache.catalina.startup.ContextConfig` 타입이며, 15장에서 자세히 설명한다.

이제 `StandardContext`를 구성하는 것이 얼마나 중요한지 알았으므로, 생성자부터 시작해 `StandardContext` 클래스를 더 자세히 살펴보자.

## StandardContext 클래스의 생성자 (StandardContext Class's Constructor)

다음은 `StandardContext` 클래스의 생성자이다.

```java
public StandardContext() {
  super();
  pipeline.setBasic(new StandardContextValve());
  namingResources.setContainer(this);
}
```

생성자에서 가장 중요한 점은 `StandardContext`의 파이프라인에 `org.apache.catalina.core.StandardContextValve` 타입의 기본 밸브(basic valve)를 설정한다는 것이다. 이 밸브는 커넥터를 통해 들어오는 모든 HTTP 요청을 처리한다.

## StandardContext 시작 (Starting StandardContext)

`start` 메서드는 `StandardContext` 인스턴스를 초기화하고 생명주기 리스너가 이를 구성할 기회를 제공한다. 구성이 성공하면 리스너는 `configured` 속성을 `true`로 설정한다. 결국 `start` 메서드는 `available` 속성을 `true` 또는 `false`로 설정한다. `true`이면 `StandardContext` 인스턴스가 올바르게 구성되었고 관련된 모든 자식 컨테이너와 컴포넌트도 성공적으로 시작되었으므로 들어오는 HTTP 요청을 처리할 준비가 되었다는 뜻이다. `false`이면 그렇지 않다는 뜻이다.

`StandardContext` 클래스는 처음에 `false`로 설정되는 `configured`라는 불리언 값을 사용한다. 생명주기 리스너가 `StandardContext` 인스턴스를 구성하는 작업을 성공적으로 마치면 `StandardContext`의 `configured` 속성을 `true`로 설정한다. `start` 메서드의 끝부분에서 `StandardContext`는 `configured` 변수의 값을 확인한다. 이 값이 `true`이면 `StandardContext`가 성공적으로 시작된 것이다. 그렇지 않으면 `stop` 메서드를 호출하여 `start` 메서드가 시작했던 모든 컴포넌트를 중지한다.

Tomcat 4의 `StandardContext` 클래스에 있는 `start` 메서드는 목록 12.1과 같다.

**목록 12.1: Tomcat 4 `StandardContext` 클래스의 `start` 메서드**

```java
public synchronized void start() throws LifecycleException {
  if (started)
      throw new LifecycleException
         (sm.getString("containerBase.alreadyStarted", logName()));
  if (debug >= 1)
      log("Starting");

  // Notify our interested LifecycleListeners
  lifecycle.fireLifecycleEvent(BEFORE_START_EVENT, null);

  if (debug >= 1)
      log("Processing start(), current available=" + getAvailable());
  setAvailable(false);
  setConfigured(false);
  boolean ok = true;

  // Add missing components as necessary
  if (getResources() == null) { // (1) Required by Loader
      if (debug >= 1)
          log("Configuring default Resources");
      try {
          if ((docBase != null) && (docBase.endsWith(".war")))
              setResources(new WARDirContext());
          else
              setResources(new FileDirContext());
      }
      catch (IllegalArgumentException e) {
          log("Error initializing resources: " + e.getMessage());
          ok = false;
      }
  }
if (ok && (resources instanceof ProxyDirContext)) {
    DirContext dirContext =
        ((ProxyDirContext) resources).getDirContext();
    if ((dirContext != null)
        && (dirContext instanceof BaseDirContext)) {
        ((BaseDirContext) dirContext).setDocBase(getBasePath());
        ((BaseDirContext) dirContext).allocate();
    }
}
if (getLoader() == null) {          // (2) Required by Manager
    if (getPrivileged()) {
        if (debug >= 1)
            log("Configuring privileged default Loader");
        setLoader(new WebappLoader(this.getClass().getClassLoader()));
    }
    else {
        if (debug >= 1)
            log("Configuring non-privileged default Loader");
        setLoader(new WebappLoader(getParentClassLoader()));
    }
}
if (getManager() == null) {          // (3) After prerequisites
    if (debug >= 1)
        log("Configuring default Manager");
    setManager(new StandardManager());
}

// Initialize character set mapper
getCharsetMapper();

// Post work directory
postWorkDirectory();

// Reading the "catalina.useNaming" environment variable
String useNamingProperty = System.getProperty("catalina.useNaming");
if ((useNamingProperty != null)
    && (useNamingProperty.equals("false"))) {
    useNaming = false;
}

if (ok && isUseNaming()) {
    if (namingContextListener == null) {
        namingContextListener = new NamingContextListener();
        namingContextListener.setDebug(getDebug());
```




## `start()` 메서드의 나머지 부분 — 원본 PDF 250~253페이지

다음은 목록 12.1에서 이어지는 `StandardContext.start()` 메서드의 코드다. 원문에 실린 코드 순서와 식별자를 유지했다.

```java
        namingContextListener.setName(getNamingContextName());
                  addLifecycleListener(namingContextListener);
    }
}

// Binding thread
ClassLoader oldCCL = bindThread();

// Standard container startup
if (debug >= 1)
    log("Processing standard container startup");

if (ok) {
    try {
       addDefaultMapper(this.mapperClass);
       started = true;

        // Start our subordinate components, if any
        if ((loader != null) && (loader instanceof Lifecycle))
            ((Lifecycle) loader).start();
        if ((logger != null) && (logger instanceof Lifecycle))
            ((Lifecycle) logger).start();

        // Unbinding thread
        unbindThread(oldCCL);

        // Binding thread
        oldCCL = bindThread();

        if ((cluster != null) && (cluster instanceof Lifecycle))
            ((Lifecycle) cluster).start();
        if ((realm != null) && (realm instanceof Lifecycle))
            ((Lifecycle) realm).start();
        if ((resources != null) && (resources instanceof Lifecycle))
            ((Lifecycle) resources).start();

        // Start our Mappers, if any
        Mapper mappers[] = findMappers();
        for (int i = 0; i < mappers.length; i++) {
           if (mappers[i] instanceof Lifecycle)
           ((Lifecycle) mappers[i]).start();
        }

      // Start our child containers, if any
      Container children[] = findChildren();
      for (int i = 0; i < children.length; i++) {
         if (children[i] instanceof Lifecycle)
         ((Lifecycle) children[i]).start();
      }

      // Start the Valves in our pipeline (including the basic),
      // if any
      if (pipeline instanceof Lifecycle)
          ((Lifecycle) pipeline).start();

      // Notify our interested LifecycleListeners
      lifecycle.fireLifecycleEvent(START_EVENT, null);

      if ((manager != null) && (manager instanceof Lifecycle))
          ((Lifecycle) manager).start();
    }
    finally {
       // Unbinding thread
       unbindThread(oldCCL);
    }
}

if (!getConfigured())
    ok = false;

// We put the resources into the servlet context
if (ok)
    getServletContext().setAttribute
      (Globals.RESOURCES_ATTR, getResources());

// Binding thread
       oldCCL = bindThread();

// Create context attributes that will be required
if (ok) {
    if (debug >= 1)
        log("Posting standard context attributes");
    postWelcomeFiles();
}

// Configure and call application event listeners and filters
if (ok) {
    if (!listenerStart())
            ok = false;
    }
    if (ok) {
        if (!filterStart())
            ok = false;
    }

    // Load and initialize all "load on startup" servlets
    if (ok)
        loadOnStartup(findChildren());

    // Unbinding thread
    unbindThread(oldCCL);

    // Set available status depending upon startup success
    if (ok) {
        if (debug >= 1)
            log("Starting completed");
        setAvailable(true);
    }
    else {
        log(sm.getString("standardContext.startFailed"));
        try {
            stop();
        }
        catch (Throwable t) {
            log(sm.getString("standardContext.startCleanup"), t);
        }
        setAvailable(false);
    }

    // Notify our interested LifecycleListeners
    lifecycle.fireLifecycleEvent(AFTER_START_EVENT, null);
}
```

> **참고:** Tomcat 5의 `start()` 메서드는 Tomcat 4의 메서드와 비슷하다. 그러나 JMX 관련 코드가 일부 포함되어 있어 JMX를 이해하지 못하면 의미를 파악하기 어렵다. JMX는 20장에서 설명한다. JMX가 처음이라면 20장을 읽은 뒤 Tomcat 5의 `start()` 메서드를 살펴보는 것이 좋다.

목록 12.1에서 볼 수 있듯이 `start()` 메서드는 다음 작업을 수행한다.

- `BEFORE_START` 이벤트를 발생시킨다.
- `available` 속성을 `false`로 설정한다.
- `configured` 속성을 `false`로 설정한다.
- 리소스(`resources`)를 설정한다.
- 로더(`loader`)를 설정한다.
- 매니저(`manager`)를 설정한다.
- 문자 집합 매퍼(character set mapper)를 초기화한다.
- 이 컨텍스트에 연결된 다른 컴포넌트를 시작한다.
- 자식 컨테이너인 래퍼(wrapper)를 시작한다.
- 파이프라인을 시작한다.
- 매니저를 시작한다.
- `START` 이벤트를 발생시킨다. 이때 리스너인 `ContextConfig`가 몇 가지 설정 작업을 수행한다(15장 참조). 설정에 성공하면 `ContextConfig`는 `StandardContext` 인스턴스의 `configured` 속성을 `true`로 설정한다.
- `configured` 속성의 값을 확인한다. `true`라면 `postWelcomePages` 메서드를 호출하고, 시작 시 로드해야 하는 자식 래퍼를 로드한 다음 `available` 속성을 `true`로 설정한다. `configured` 변수가 `false`라면 `stop()` 메서드를 호출한다.
- `AFTER_START` 이벤트를 발생시킨다.

## `invoke()` 메서드

Tomcat 4에서 `StandardContext`의 `invoke()` 메서드는 연결된 커넥터가 호출한다. `StandardContext`가 호스트의 자식 컨테이너인 경우에는 호스트의 `invoke()` 메서드가 호출한다.

`StandardContext`의 `invoke()` 메서드는 먼저 애플리케이션을 재로딩하고 있는지 확인한다. 재로딩 중이라면 작업이 끝날 때까지 기다린다. 그런 다음 부모 클래스인 `ContainerBase`의 `invoke()` 메서드를 호출한다. 목록 12.2는 `StandardContext` 클래스의 `invoke()` 메서드를 보여 준다.

**목록 12.2: `StandardContext` 클래스의 `invoke()` 메서드**

```java
public void invoke(Request request, Response response)
  throws IOException, ServletException {

  // Wait if we are reloading
  while (getPaused()) {
    try {
       Thread.sleep(1000);
    }
    catch (InterruptedException e) {
       ;
    }
  }
```




### 목록 12.2의 나머지 코드 (원본 PDF 254페이지)

```java
    // Normal request processing
    if (swallowOutput) {
        try {
           SystemLogHandler.startCapture();
           super.invoke(request, response);
        }
        finally {
           String log = SystemLogHandler.stopCapture();
           if (log != null && log.length() > 0) {
               log(log);
           }
        }
    }
    else {
        super.invoke(request, response);
    }
}
```

`getPaused` 메서드는 `paused` 속성의 값을 반환한다. 애플리케이션을 재로딩하는 중에는 이 속성이 `true`이다. 애플리케이션 재로딩은 다음 하위 절에서 설명한다.

Tomcat 5의 `StandardContext` 클래스에는 `invoke` 메서드의 구현이 없다. 따라서 `ContainerBase` 클래스의 `invoke` 메서드가 실행된다. 애플리케이션 재로딩 여부를 확인하는 작업은 `StandardContextValve` 클래스의 `invoke` 메서드로 옮겨졌다.

## StandardContextMapper

요청이 들어올 때마다 `StandardContext`의 파이프라인에 있는 기본 밸브의 `invoke` 메서드가 호출된다. `StandardContext`의 기본 밸브는 `org.apache.catalina.core.StandardContextValve` 클래스로 표현된다. `StandardContextValve.invoke`가 가장 먼저 해야 할 일은 요청을 처리할 래퍼를 얻는 것이다.

Tomcat 4에서 `StandardContextValve` 인스턴스는 자신을 포함하는 `StandardContext`를 살펴본다. `StandardContextValve`는 컨텍스트의 매퍼(mapper)를 사용하여 적절한 래퍼를 찾는다. 래퍼를 얻으면 해당 래퍼의 `invoke` 메서드를 호출한다. `StandardContextValve`의 동작을 자세히 살펴보기 전에, 이 절에서는 매퍼 컴포넌트를 소개한다.

`StandardContext`의 부모 클래스인 `ContainerBase`는 기본 매퍼를 추가하는 `addDefaultMapper` 메서드를 다음과 같이 정의한다.

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

`StandardContext` 클래스는 `start` 메서드에서 `mapperClass` 변수를 전달하면서 `addDefaultMapper` 메서드를 호출한다.

```java
public synchronized void start() throws LifecycleException {
  ...
  if (ok) {
      try {
         addDefaultMapper(this.mapperClass);
  ...
}
```

`StandardContext` 클래스는 `mapperClass` 변수를 다음과 같이 정의한다.

```java
private String mapperClass =
   "org.apache.catalina.core.StandardContextMapper";
```

그런 다음 매퍼의 `setContainer` 메서드를 호출하고 컨테이너 인스턴스를 전달하여 매퍼를 컨테이너와 연결해야 한다. `StandardContext`에서 사용하는 `org.apache.catalina.Mapper` 인터페이스의 구현 클래스는 `org.apache.catalina.core.StandardContextMapper`이다. `setContainer` 메서드에서 확인할 수 있듯이 `StandardContextMapper`는 컨텍스트에만 연결할 수 있다.

```java
public void setContainer(Container container) {
  if (!(container instanceof StandardContext))
      throw new IllegalArgumentException
         (sm.getstring("httpContextMapper.container"));
  context = (StandardContext) container;
}
```

매퍼에서 가장 중요한 메서드는 HTTP 요청을 처리할 자식 컨테이너를 반환하는 `map`이다. 메서드 선언은 다음과 같다.

```java
public Container map(Request request, boolean update)
```

`StandardContextMapper`의 `map` 메서드는 요청을 처리할 래퍼를 반환한다. 적절한 래퍼를 찾지 못하면 `null`을 반환한다.

이 절의 앞부분에서 설명했듯이, `StandardContextValve` 인스턴스는 HTTP 요청이 들어올 때마다 `org.apache.catalina.Request` 객체를 전달하면서 컨텍스트의 `map` 메서드를 호출한다. 부모 클래스인 `ContainerBase`의 `map` 메서드는 먼저 `findMapper`를 호출하여 특정 프로토콜의 매퍼를 얻고, 이어서 그 매퍼의 `map` 메서드를 호출한다.

```java
// Select the Mapper we will use
Mapper mapper = findMapper(request.getRequest().getProtocol());
if (mapper == null)
    return (null);
// Use this Mapper to perform this mapping
return (mapper.map(request, update));
```

`StandardContextMapper`의 `map` 메서드는 먼저 매핑 대상인 컨텍스트 상대 URI를 식별한다.

```java
// Identify the context-relative URI to be mapped
String contextPath =
  ((HttpServletRequest) request.getRequest()).getContextPath();
String requestURI = ((HttpRequest) request).getDecodedRequestURI();

String relativeURI = requestURI.substring(contextPath.length());
```

그다음 다음 네 가지 매칭 규칙을 적용하여 래퍼를 찾는다.

```java
// Apply the standard request URI mapping rules from the specification
Wrapper wrapper = null;
String servletPath = relativeURI;
String pathInfo = null;
String name = null;
```

**규칙 1 — 정확히 일치하는 매핑(Exact Match)**

```java
// Rule 1 -- Exact Match
if (wrapper == null) {
    if (debug >= 2)
        context.log(" Trying exact match");
    if (!(relativeURI.equals("/")))
        name = context.findServletMapping(relativeURI);
    if (name != null)
        wrapper = (Wrapper) context.findChild(name);
    if (wrapper != null) {
        servletPath = relativeURI;
        pathInfo = null;
    }
}
```

**규칙 2 — 접두사 매핑(Prefix Match)**

```java
// Rule 2 -- Prefix Match
if (wrapper == null) {
    if (debug >= 2)
        context.log(" Trying prefix match");
    servletPath = relativeURI;
    while (true) {
        name = context.findServletMapping(servletPath + "/*");
        if (name != null)
            wrapper = (Wrapper) context.findChild(name);
        if (wrapper != null) {
            pathInfo = relativeURI.substring(servletPath.length());
            if (pathInfo.length() == 0)
                pathInfo = null;
            break;
        }
        int slash = servletPath.lastIndexOf('/');
        if (slash < 0)
            break;
        servletPath = servletPath.substring(0, slash);
    }
}
```

**규칙 3 — 확장자 매핑(Extension Match)**

```java
// Rule 3 -- Extension Match
if (wrapper == null) {
    if (debug >= 2)
        context.log(" Trying extension match");
    int slash = relativeURI.lastIndexOf('/');
    if (slash >= 0) {
        String last = relativeURI.substring(slash);
        int period = last.lastIndexOf('.');
        if (period >= 0) {
            String pattern = "*" + last.substring(period);

            name = context.findServletMapping(pattern);
            if (name != null)
                wrapper = (Wrapper) context.findChild(name);
            if (wrapper != null) {
                servletPath = relativeURI;
                pathInfo = null;
            }
        }
    }
}
```

**규칙 4 — 기본 매핑(Default Match)**

```java
// Rule 4 -- Default Match
if (wrapper == null) {
    if (debug >= 2)
        context.log(" Trying default match");
    name = context.findServletMapping("/");
    if (name != null)
        wrapper = (Wrapper) context.findChild(name);
    if (wrapper != null) {
        servletPath = relativeURI;
        pathInfo = null;
    }
}
```

컨텍스트는 서블릿 매핑과 같은 정보를 어떻게 알고 있을까? 11장의 `Bootstrap` 클래스가 `StandardContext`에 다음 두 가지 서블릿 매핑을 추가했다는 사실을 떠올려 보자.

```java
context.addServletMapping("/Primitive", "Primitive");
context.addServletMapping("/Modern", "Modern");
```

또한 래퍼를 컨텍스트의 자식으로 추가했다.

```java
context.addChild(wrapper1);
context.addChild(wrapper2);
```

Tomcat 5에서는 `Mapper` 인터페이스와 관련 클래스가 제거되었다. 실제로 `StandardContextValve` 클래스의 `invoke` 메서드는 요청 객체에서 적절한 래퍼를 가져온다.

```java
Wrapper wrapper = request.getWrapper();
```

> 원문 PDF 254~258페이지 번역. 코드의 영문 주석 및 식별자는 원문을 유지했다.

## 클래스 재로딩 지원 (Support for Reloading)

이는 매핑 정보가 요청 객체에 캡슐화되어 있음을 의미한다.

`StandardContext` 클래스는 애플리케이션의 재로딩을 허용할지 나타내는 `reloadable` 속성을 정의한다. 재로딩이 활성화되어 있으면 `web.xml` 파일이 변경되거나 `WEB-INF/classes` 디렉터리의 파일 중 하나가 다시 컴파일될 때 애플리케이션이 재로딩된다.

`StandardContext` 클래스는 애플리케이션을 재로딩하기 위해 로더에 의존한다. Tomcat 4에서 `StandardContext`의 `Loader` 구현 클래스인 `WebappLoader`에는 `WEB-INF` 디렉터리에 있는 모든 클래스 파일과 JAR 파일의 타임스탬프를 확인하는 스레드가 있다. 이 스레드를 시작하기 위해 필요한 일은 `WebappLoader`의 `setContainer` 메서드를 호출해 `StandardContext`와 연결하는 것뿐이다. 다음은 Tomcat 4의 `WebappLoader` 클래스에 있는 `setContainer` 메서드 구현이다.

```java
public void setContainer(Container container) {
  // Deregister from the old Container (if any)
  if ((this.container != null) && (this.container instanceof Context))
      ((Context) this.container).removePropertyChangeListener(this);
  // Process this property change
  Container oldContainer = this.container;
  this.container = container;
  support.firePropertyChange("container", oldContainer,
      this.container);
  // Register with the new Container (if any)
  if ((this.container!=null) && (this.container instanceof Context)) {
      setReloadable( ((Context) this.container).getReloadable() );
      ((Context) this.container).addPropertyChangeListener(this);
  }
}
```

마지막 `if` 블록을 살펴보자. 컨테이너가 컨텍스트라면 `setReloadable` 메서드가 호출된다. 즉, `WebappLoader` 인스턴스의 `reloadable` 속성값이 `StandardContext` 인스턴스의 `reloadable` 속성값과 같아진다.

다음은 `WebappLoader` 클래스의 `setReloadable` 메서드 구현이다.

```java
public void setReloadable(boolean reloadable) {
  // Process this property change
  boolean oldReloadable = this.reloadable;
  this.reloadable = reloadable;
  support.firePropertyChange("reloadable",
      new Boolean(oldReloadable), new Boolean(this.reloadable));
  // Start or stop our background thread if required
  if (!started)
      return;
  if (!oldReloadable && this.reloadable)
      threadStart();
  else if (oldReloadable && !this.reloadable)
      threadStop();
}
```

`reloadable` 속성이 `false`에서 `true`로 바뀌면 `threadStart` 메서드가 호출된다. 반대로 `true`에서 `false`로 바뀌면 `threadStop` 메서드가 호출된다. `threadStart` 메서드는 `WEB-INF`에 있는 클래스 파일과 JAR 파일의 타임스탬프를 지속적으로 확인하는 전용 스레드를 시작한다. `threadStop` 메서드는 이 스레드를 중지한다.

Tomcat 5에서는 클래스의 타임스탬프를 확인해 재로딩 여부를 판단하는 작업을 `backgroundProcess` 메서드가 수행한다. 다음 절에서 이 메서드를 살펴본다.

## backgroundProcess 메서드 (The backgroundProcess Method)

컨텍스트에는 로더나 매니저와 같은 다른 컴포넌트의 도움이 필요하다. 이러한 컴포넌트는 백그라운드 처리를 담당하는 별도 스레드를 필요로 하는 경우가 많다. 예를 들어 자동 재로딩을 지원하는 로더에는 `WEB-INF`의 모든 클래스 파일과 JAR 파일의 타임스탬프를 주기적으로 확인하는 스레드가 필요하다. 매니저에는 자신이 관리하는 세션 객체의 만료 시간을 확인하는 스레드가 필요하다. Tomcat 4에서는 이러한 컴포넌트가 각각 자체 스레드를 갖게 된다.

자원을 절약하기 위해 Tomcat 5는 다른 방식을 사용한다. 모든 백그라운드 작업이 동일한 스레드를 공유한다. 컴포넌트나 컨테이너가 어떤 작업을 주기적으로 수행해야 한다면, 자신의 `backgroundProcess` 메서드에 해당 코드를 작성하기만 하면 된다.

공유 스레드는 `ContainerBase` 객체에서 생성된다. `ContainerBase` 클래스는 컨테이너가 시작될 때, 즉 `start` 메서드에서 `threadStart` 메서드를 호출한다.

```java
protected void threadStart() {
  if (thread != null)
      return;
  if (backgroundProcessorDelay <= 0)
      return;
  threadDone = false;
  String threadName = "ContainerBackgroundProcessor[" + toString() +
      "]";
  thread = new Thread(new ContainerBackgroundProcessor(), threadName);
  thread.setDaemon(true);
  thread.start();
}
```

`threadStart` 메서드는 `java.lang.Runnable`을 구현한 `ContainerBackgroundProcessor` 클래스의 인스턴스를 전달하여 새 스레드를 생성한다. 목록 12.3은 `ContainerBackgroundProcessor` 클래스를 보여 준다.

**목록 12.3: `ContainerBackgroundProcessor` 클래스**

```java
protected class ContainerBackgroundProcessor implements Runnable {
  public void run() {
     while (!threadDone) {
       try {
           Thread.sleep(backgroundProcessorDelay * 1000L);
       }
       catch (InterruptedException e) {
           ;
       }
       if (!threadDone) {
           Container parent = (Container) getMappingObject();
           ClassLoader cl =
               Thread.currentThread().getContextClassLoader();
           if (parent.getLoader() != null) {
               cl = parent.getLoader().getClassLoader();
           }
           processChildren(parent, cl);
       }
     }
  }

  protected void processChildren(Container container, ClassLoader cl) {
      try {
         if (container.getLoader() != null) {
             Thread.currentThread().setContextClassLoader
               (container.getLoader().getClassLoader());
         }
         container.backgroundProcess();
      }
      catch (Throwable t) {
         log.error("Exception invoking periodic operation: ", t);
      }
      finally {
           Thread.currentThread().setContextClassLoader(cl);
      }
      Container[] children = container.findChildren();
      for (int i = 0; i < children.length; i++) {
           if (children[i].getBackgroundProcessorDelay() <= 0) {
               processChildren(children[i], cl);
           }
      }
  }
}
```

`ContainerBackgroundProcessor` 클래스는 `ContainerBase`의 내부 클래스다. 이 클래스의 `run` 메서드에는 `processChildren` 메서드를 주기적으로 호출하는 `while` 루프가 있다. `processChildren` 메서드는 해당 컨테이너의 `backgroundProcess` 메서드를 호출하고, 이어서 각 자식 컨테이너에 대해 `processChildren`을 호출한다. `ContainerBase`의 자식 클래스는 `backgroundProcess` 메서드를 구현함으로써 클래스 타임스탬프나 세션 만료 시간을 확인하는 등의 주기적인 작업을 스레드에서 실행할 수 있다. 목록 12.4는 Tomcat 5의 `StandardContext` 클래스에 있는 `backgroundProcess` 메서드를 보여 준다.

**목록 12.4: `StandardContext` 클래스의 `backgroundProcess` 메서드**

```java
public void backgroundProcess() {
  if (!started)
      return;
  count = (count + 1) % managerChecksFrequency;
  if ((getManager() != null) && (count == 0)) {
      try {
          getManager().backgroundProcess();
      }
      catch ( Exception x ) {
          log.warn("Unable to perform background process on manager", x);
      }
  }
  if (getloader() != null) {
      if (reloadable && (getLoader().modified())) {
          try {
             Thread.currentThread().setContextClassloader
                (StandardContext.class.getClassLoader());
             reload();
          }
          finally {
             if (getLoader() != null) {
                 Thread.currentThread().setContextClassLoader
                    (getLoader().getClassLoader());
             }
          }
      }
      if (getLoader() instanceof WebappLoader) {
          ((WebappLoader) getLoader()).closeJARs(false);
      }
  }
}
```

이제 `StandardContext`가 자신과 연결된 매니저와 로더의 주기적인 작업을 어떻게 지원하는지 알 수 있을 것이다.

## 요약 (Summary)

이 장에서는 `StandardContext` 클래스와 관련 클래스에 대해 배웠다. 또한 `StandardContext` 인스턴스가 어떻게 설정되는지, 들어오는 HTTP 요청마다 내부에서 어떤 일이 일어나는지도 살펴보았다. 마지막 절에서는 Tomcat 5의 `backgroundProcess` 구현을 설명했다.

> 원본 PDF 259~263페이지. Java 코드의 식별자 및 원문에 보이는 대소문자 표기(`getloader`, `setContextClassloader` 등)는 수정하지 않았다.
