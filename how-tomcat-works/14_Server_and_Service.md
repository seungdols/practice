# 14장: Server와 Service

## 개요 (PDF 283페이지)

앞선 장들에서는 커넥터와 컨테이너의 인스턴스를 생성한 다음 서로 연결하여 서블릿 컨테이너를 구성하는 방법을 살펴보았다. 하지만 사용할 수 있는 커넥터는 하나뿐이었고, 이 커넥터는 8080 포트에서 HTTP 요청을 처리하는 역할만 했다. 예를 들어 HTTPS 요청을 처리할 커넥터를 추가할 수는 없었다.

또한 이전 장들에 함께 제공된 모든 애플리케이션에는 한 가지 부족한 점이 있다. 바로 서블릿 컨테이너를 시작하고 중지하기 위한 적절한 메커니즘이다. 이 장에서는 이러한 메커니즘을 제공하면서 여러 다른 기능도 지원하는 두 컴포넌트, 즉 `Server`와 `Service`를 살펴본다.

## Server

`org.apache.catalina.Server` 인터페이스는 Catalina 서블릿 컨테이너 전체를 나타내며, 다른 모든 컴포넌트를 포괄한다. 서버는 전체 시스템을 시작하고 중지하는 편리한 메커니즘을 제공하므로 특히 유용하다. 이제 커넥터와 컨테이너를 각각 따로 시작할 필요가 없다.

시작 및 중지 메커니즘은 다음과 같이 동작한다. 서버를 시작하면 서버에 포함된 모든 컴포넌트가 시작된다. 그런 다음 서버는 종료 명령이 올 때까지 무기한 기다린다. 시스템을 종료하려면 지정된 포트로 종료 명령을 보낸다. 서버가 이 명령을 수신하고 올바른 종료 명령임을 확인하면 모든 컴포넌트를 중지한다.

서버는 `Service`라는 또 다른 컴포넌트를 사용하여 컨테이너와 하나 이상의 커넥터 같은 컴포넌트를 포함한다. `Service`는 이 장의 뒷부분에 있는 「Service」 절에서 설명한다.

`Server` 인터페이스는 목록 14.1에 나와 있다.

**목록 14.1: `Server` 인터페이스 (다음 페이지에서 계속)**

```java
package org.apache.catalina;

import org.apache.catalina.deploy.NamingResources;

public interface Server {
  /**
   * Return descriptive information about this Server implementation
```

> 원본 PDF 283페이지의 마지막에서 코드가 끊기므로 나머지는 284페이지부터 이어진다.

## `Server` 인터페이스 (PDF 284페이지, 목록 14.1 계속)

앞 페이지에서 시작한 `Server` 인터페이스의 나머지 선언은 다음과 같다. Java 코드와 주석은 원문 표기를 유지했다.

```java
 * and the corresponding version number, in the format
 * <code>&lt;description&gt;/&lt;version&gt;</code>.
 */
public String getInfo();
/**
 * Return the global naming resources.
 */
public NamingResources getGlobalNamingResources();
/**
 * Set the global naming resources.
 *
 * @param namingResources The new global naming resources
 */
public void setGlobalNamingResources
   (NamingResources globalNamingResources);
/**
 * Return the port number we listen to for shutdown commands.
 */
public int getPort();
/**
 * Set the port number we listen to for shutdown commands.
 *
 * @param port The new port number
 */
public void setPort(int port);
/**
 * Return the shutdown command string we are waiting for.
 */
public String getShutdown();
/**
 * Set the shutdown command we are waiting for.
 *
 * @param shutdown The new shutdown command
 */
public void setShutdown(String shutdown);
/**
 * Add a new Service to the set of defined Services.
 *
 * @param service The Service to be added
 */
public void addService(Service service);
/**
 * Wait until a proper shutdown command is received, then return.
 */
public void await();
/**
 * Return the specified Service (if it exists); otherwise return
 * <code>null</code>.
 *
 * @param name Name of the Service to be returned
 */
public Service findService(String name);
/**
 * Return the set of Services defined within this Server.
 */
public Service[] findServices();
/**
 * Remove the specified Service from the set associated from this
 * Server.
 *
 * @param service The Service to be removed
 */
public void removeService(Service service);
/**
 * Invoke a pre-startup initialization. This is used to allow
 * onnectors to bind to restricted ports under Unix operating
 * environments.
 *
 * @exception LifecycleException If this server was already
 * initialized.
 */
public void initialize() throws LifecycleException;
}
```

`shutdown` 속성에는 `Server` 인스턴스를 종료하기 위해 전송해야 하는 문자열이 저장된다. `port` 속성은 서버가 종료 명령을 기다리는 포트를 정의한다. `addService()`를 호출하면 서버에 `Service` 객체를 추가할 수 있고, 나중에 `removeService()`를 호출하여 제거할 수도 있다. `findServices()`는 서버에 추가된 모든 서비스를 반환한다. `initialize()`에는 서버를 시작하기 전에 실행해야 하는 코드가 들어 있다.

## `StandardServer` (PDF 285페이지)

`org.apache.catalina.core.StandardServer` 클래스는 `Server` 인터페이스의 표준 구현이다. 여기서는 이 클래스가 제공하는 종료 메커니즘에 특히 관심을 둔다. 이것이 이 클래스의 가장 중요한 기능이기도 하다. 여러 메서드는 서버 구성을 새로운 `server.xml` 파일에 저장하는 작업과 관련되지만, 여기서는 다루지 않는다. 관심 있는 독자라면 이 메서드들도 어렵지 않게 이해할 수 있을 것이다.

하나의 `Server`에는 0개 이상의 서비스가 포함될 수 있다. `StandardServer`는 `addService()`, `removeService()`, `findServices()` 메서드를 구현한다.

`StandardServer`의 생명주기와 관련된 메서드는 `initialize()`, `start()`, `stop()`, `await()` 네 가지다. 다른 컴포넌트와 마찬가지로 서버를 초기화하고 시작한다. 그런 다음 `await()`를 호출하고, 그 호출이 끝난 후 `stop()`을 호출한다. `await()`는 8085 포트(또는 다른 설정 포트)에서 종료 명령을 수신하기 전까지 반환하지 않는다. `await()`가 반환되면 `stop()`이 실행되어 모든 하위 컴포넌트를 중지한다. 이 장의 예제 애플리케이션에서는 이러한 종료 메커니즘을 구현하는 방법을 배우게 된다.

다음 절에서는 `initialize()`, `start()`, `stop()`, `await()`를 각각 살펴본다.

### `initialize()` 메서드

`initialize()`는 `Server` 인스턴스에 추가된 서비스들을 초기화한다. 목록 14.2는 Tomcat 4의 `StandardServer` 클래스에 구현된 `initialize()` 메서드다.

**목록 14.2: `initialize()` 메서드**

```java
public void initialize() throws LifecycleException {
  if (initialized)
      throw new LifecycleException (
         sm.getString("StandardServer.initialize.initialized"));
  initialized = true;

    // Initialize our defined Services
    for (int i = 0; i < services.length; i++) {
       services[i].initialize();
    }
}
```

`initialize()`는 서버가 두 번 초기화되지 않도록 `initialized`라는 불리언 변수를 사용한다. Tomcat 5의 `initialize()`도 비슷하지만, JMX(20장에서 설명) 관련 코드가 추가되어 있다. `stop()`은 `initialized` 값을 초기화하지 않는다. 따라서 서버를 중지했다가 다시 시작해도 `initialize()`가 다시 호출되지 않는다.

### `start()` 메서드 (PDF 286~287페이지)

서버를 시작하려면 `start()`를 호출한다. `StandardServer`의 이 메서드는 모든 서비스를 시작하며, 각 서비스는 다시 커넥터와 컨테이너 같은 다른 컴포넌트들을 시작한다. 목록 14.3에 `start()`가 나와 있다.

**목록 14.3: `start()` 메서드**

```java
public void start() throws LifecycleException {
  // Validate and update our current component state
  if (started)
      throw new LifecycleException
         (sm.getString("standardServer.start.started"));
  // Notify our interested LifecycleListeners
  lifecycle.fireLifecycleEvent(BEFORE_START_EVENT, null);
  lifecycle.fireLifecycleEvent(START_EVENT, null);
  started = true;
  // Start our defined Services
  synchronized (services) {
      for (int i = 0; i < services.length; i++) {
         if (services[i] instanceof Lifecycle)
             ((Lifecycle) services[i]).start();
      }
  // Notify our interested LifecycleListeners
  lifecycle.fireLifecycleEvent(AFTER_START_EVENT, null);
}
```

`start()`는 서버가 두 번 시작되지 않도록 `started` 불리언 변수를 사용한다. `stop()`은 이 변수를 다시 초기화한다.

### `stop()` 메서드

`stop()`은 서버를 중지한다. 목록 14.4에 이 메서드가 나와 있다.

**목록 14.4: `stop()` 메서드**

```java
public void stop() throws LifecycleException {
  // Validate and update our current component state
  if (!started)
      throw new LifecycleException
         (sm.getString("standardServer.stop.notStarted"));
  // Notify our interested LifecycleListeners
  lifecycle.fireLifecycleEvent(BEFORE_STOP_EVENT, null);
  lifecycle.fireLifecycleEvent(STOP_EVENT, null);
  started = false;

  // Stop our defined Services
  for (int i = 0; i < services.length; i++) {
      if (services[i] instanceof Lifecycle)
          ((Lifecycle) services[i]).stop();
  }
  // Notify our interested LifecycleListeners
  lifecycle.fireLifecycleEvent(AFTER_STOP_EVENT, null);
}
```

`stop()`을 호출하면 모든 서비스가 중지되고 `started` 불리언 값이 초기화된다. 따라서 나중에 서버를 다시 시작할 수 있다.

### `await()` 메서드 (PDF 288페이지, 다음 페이지에서 계속)

`await()`는 Tomcat 배포 전체를 중지하는 메커니즘을 담당한다. 목록 14.5에 이 메서드가 나와 있다.

**목록 14.5: `await()` 메서드 — 시작 부분**

```java
/**
 * Wait until a proper shutdown command is received, then return.
 */
public void await() {
  // Set up a server socket to wait on
  ServerSocket serverSocket = null;
  try {
     serverSocket = new ServerSocket(port, 1,
        InetAddress.getByName("127.0.0.1"));
  }
  catch (IOException e) {
     System.err.println("StandardServer.await: create[" +
        port + "]: " + e);
     e.printStackTrace();
     System.exit(1);
```

> 목록 14.5의 코드는 원본 PDF 289페이지에서 계속된다. 원문에 보이는 코드의 일부 문법·중괄호 이상은 임의로 수정하지 않았다.

## `await()` 메서드의 나머지 구현 (PDF 289~290페이지)

다음은 목록 14.5의 이어지는 코드다. 서버는 연결을 받아 종료 명령을 읽고, 올바른 명령이 들어올 때까지 대기한다.

```java
}
// Loop waiting for a connection and a valid command
while (true) {
  // Wait for the next connection
  Socket socket = null;
  InputStream stream = null;
  try {
     socket = serverSocket.accept();
     socket.setSoTimeout(10 * 1000); // Ten seconds
     stream = socket.getInputStream();
  }
  catch (AccessControlException ace) {
     System.err.println("StandardServer.accept security exception: "
        + ace.getMessage());
     continue;
  }
  catch (IOException e) {
     System.err.println("StandardServer.await: accept: " + e);
     e.printStackTrace();
     System.exit(1);
  }

  // Read a set of characters from the socket
  StringBuffer command = new StringBuffer();
  int expected = 1024; // Cut off to avoid DoS attack
  while (expected < shutdown.length()) {
     if (random == null)
         random = new Random(System.currentTimeMillis());
     expected += (random.nextInt() % 1024);
  }
  while (expected > 0) {
     int ch = -1;
     try {
       ch = stream, read();
    }
    catch (IOException e) {
        System.err.println("StandardServer.await: read: " + e);
        e.printStackTrace();
        ch = -1;
    }
    if (ch < 32) // Control character or EOF terminates loop
        break;
    command.append((char) ch);
    expected--;
  }

  // Close the socket now that we are done with it
  try {
     socket.close();
  }
  catch (IOException e) {
     ;
  }

  // Match against our command string
  boolean match = command.toString().equals(shutdown);
  if (match) {
      break;
  }
  else
      System.err.println("StandardServer.await: Invalid command '" +
        command.toString() + "' received");
}

// Close the server socket and return
try {
   serverSocket.close();
}
catch (IOException e) {
   ;
}
}
```

> **원문 주의:** 위 코드는 PDF에 인쇄된 표기를 따른다. 특히 `stream, read()`는 Java 문법상 이상이 있지만, 원문의 오류를 임의로 수정하지 않았다. 앞 페이지의 코드와 이어서 읽어야 한다.

`await()` 메서드는 8085 포트에서 기다리는 `ServerSocket` 객체를 만든 다음 `while` 루프 안에서 `accept()` 메서드를 호출한다. `accept()`는 지정된 포트(8085)로 메시지가 들어와야 반환된다. 이후 각 메시지를 종료 문자열(`shutdown`)과 비교한다. 문자열이 일치하면 `while` 루프를 빠져나오고 서버 소켓을 닫는다. 일치하지 않으면 루프에 남아 다음 메시지를 기다린다.

## Service (PDF 290~292페이지)

`org.apache.catalina.Service` 인터페이스는 서비스를 나타낸다. 서비스는 **하나의 컨테이너와 여러 커넥터**를 보유할 수 있다. 커넥터는 원하는 만큼 추가할 수 있으며, 모든 커넥터는 해당 컨테이너와 연결된다. 목록 14.6은 `Service` 인터페이스를 보여준다.

**목록 14.6: `Service` 인터페이스**

```java
package org.apache.catalina;

public interface Service {

  /**
   * Return the <code>Container</code> that handles requests for all
   * <code>Connectors</code> associated with this Service.
   */
  public Container getContainer();

  /**
   * Set the <code>Container</code> that handles requests for all
   * <code>Connectors</code> associated with this Service.
   *
   * @param container The new Container
   */
  public void setContainer(Container container);

  /**
   * Return descriptive information about this Service implementation
   * and the corresponding version number, in the format
   * <code>&lt;description&gt;/&lt;version&gt;</code>.
   */
  public String getInfo();

  /**
   * Return the name of this Service.
   */
  public String getName();

  /**
   * Set the name of this Service.
   *
   * @param name The new service name
   */
  public void setName(String name);

  /**
   * Return the <code>Server</code> with which we are associated
   * (if any).
   */
  public Server getServer();

  /**
   * Set the <code>Server</code> with which we are associated (if any).
   *
   * @param server The server that owns this Service
   */
  public void setServer(Server server);

  /**
   * Add a new Connector to the set of defined Connectors,
   * and associate it with this Service's Container.
   *
   * @param connector The Connector to be added
   */
  public void addConnector(Connector connector);

  /**
   * Find and return the set of Connectors associated with
   * this Service.
   */
  public Connector[] findConnectors();

  /**
   * Remove the specified Connector from the set associated from this
   * Service. The removed Connector will also be disassociated
   * from our Container.
   *
   * @param connector The Connector to be removed
   */
  public void removeConnector(Connector connector);

  /**
   * Invoke a pre-startup initialization. This is used to
   * allow connectors to bind to restricted ports under
   * Unix operating environments.
   *
   * @exception LifecycleException If this server was
   * already initialized.
   */
  public void initialize() throws LifecycleException;
}
```

## StandardService (PDF 292~293페이지)

`org.apache.catalina.core.StandardService` 클래스는 `Service`의 표준 구현이다. `StandardService`의 `initialize()` 메서드는 서비스에 추가된 모든 커넥터를 초기화한다. `StandardService`는 `Service` 인터페이스뿐 아니라 `org.apache.catalina.Lifecycle` 인터페이스도 구현한다. 이 클래스의 `start()` 메서드는 컨테이너와 모든 커넥터를 시작한다.

### 컨테이너와 커넥터

`StandardService` 인스턴스에는 두 종류의 구성 요소가 있다. **컨테이너 하나와 하나 이상의 커넥터**다. 여러 커넥터를 사용할 수 있으므로 Tomcat은 여러 프로토콜을 처리할 수 있다. 예를 들어 커넥터 하나는 HTTP 요청을 처리하고 다른 커넥터는 HTTPS 요청을 처리할 수 있다.

`StandardService` 클래스는 `container` 변수를 `Container` 인스턴스의 객체 참조로 사용하고, `connectors` 배열에 모든 커넥터를 보관한다.

```java
private Container container = null;
private Connector connectors[] = new Connector[0];
```

서비스에 컨테이너를 연결하려면 목록 14.7의 `setContainer()` 메서드를 사용한다.

**목록 14.7: `setContainer()` 메서드 — PDF 293페이지에 수록된 부분**

```java
public void setContainer(Container container) {
  Container oldContainer = this.container;
  if ((oldContainer != null) && (oldContainer instanceof Engine))
      ((Engine) oldContainer).setService(null);
  this.container = container;
  if ((this.container != null) && (this.container instanceof Engine))
    ((Engine) this.container).setService(this);
  if (started && (this.container != null) &&
      (this.container instanceof Lifecycle)) {
      try {
         ((Lifecycle) this.container).start();
      }
      catch (LifecycleException e) {
         ;
```

> 목록 14.7의 나머지 코드는 원본 PDF 294페이지에서 이어진다.


## `StandardService`의 컨테이너와 커넥터 관리 (PDF 294~296페이지)

**목록 14.7: `setContainer()` 메서드의 나머지 코드**

앞 페이지에서 시작한 `setContainer()`는 새 컨테이너를 서비스의 각 커넥터에 전달한다. 서비스가 이미 시작된 상태라면 기존 컨테이너를 중지하며, 마지막으로 `container` 속성 변경 이벤트를 발생시켜 관심 있는 리스너에 알린다. 아래 코드는 앞 페이지의 `catch` 블록부터 이어진다.

```java
}
    }
    synchronized (connectors) {
        for (int i = 0; i < connectors.length; i++)
           connectors[i].setContainer(this.container);
    }
    if (started && (oldContainer != null) &&
        (oldContainer instanceof Lifecycle)) {
        try {
           ((Lifecycle) oldContainer).stop();
        }
        catch (LifecycleException e) {
           ;
        }
    }

    // Report this property change to interested listeners
    support.firePropertyChange("container", oldContainer,
      this.container);
}
```

서비스와 연결된 컨테이너는 해당 서비스에서 사용할 수 있는 각 `Connector` 객체의 `setContainer()` 메서드에 전달된다. 이로써 컨테이너와 개별 커넥터 사이의 연관 관계가 만들어진다.

`Service` 객체에 커넥터를 추가하려면 `addConnector()`를 사용하고, 제거하려면 `removeConnector()`를 호출한다. 두 메서드는 각각 목록 14.8과 목록 14.9에 제시되어 있다.

### 목록 14.8: `addConnector()` 메서드

```java
public void addConnector(Connector connector) {
  synchronized (connectors) {
     connector.setContainer(this.container);
     connector.setService(this);
     Connector results[] = new Connector[connectors.length + 1];
     System.arraycopy(connectors, 0, results, 0, connectors.length);
     results[connectors.length] = connector;
     connectors = results;

      if (initialized) {
          try {
             connector.initialize();
          }
        catch (LifecycleException e) {
          e.printStackTrace(System.err);
        }
    }

    if (started && (connector instanceof Lifecycle)) {
        try {
           ((Lifecycle) connector).start();
        }
        catch (LifecycleException e) {
           ;
        }
    }

    // Report this property change to interested listeners
    support.firePropertyChange("connector", null, connector);
    }
}
```

### 목록 14.9: `removeConnector()` 메서드

```java
public void removeConnector(Connector connector) {
  synchronized (connectors) {
  int j = -1;
  for (int i = 0; i < connectors.length; i++) {
      if (connector == connectors [i]) {
          j = i;
          break;
      }
  }
  if (j < 0)
      return;
  if (started && (connectors[j] instanceof Lifecycle)) {
      try {

          ((Lifecycle) connectors[j]).stop();
        }
        catch (LifecycleException e) {
          ;
        }
    }
    connectors[j].setContainer(null);
    connector.setService(null);
    int k = 0;
    Connector results[] = new Connector[connectors.length - 1];
    for (int i = 0; i < connectors.length; i++) {
       if (i != j)
           results[k++] = connectors[i];
    }
    connectors = results;

    // Report this property change to interested listeners
    support.firePropertyChange("connector", connector, null);
    }
}
```

`addConnector()` 메서드는 추가된 커넥터를 초기화하고 시작한다.

## 생명주기 메서드 (PDF 296~298페이지)

생명주기 메서드에는 `Lifecycle` 인터페이스에서 상속한 `start()`와 `stop()`, 그리고 `initialize()`가 포함된다. `initialize()`는 서비스에 속한 각 커넥터의 `initialize()` 메서드를 호출한다. 목록 14.10에 이 메서드가 제시되어 있다.

### 목록 14.10: `StandardService.initialize()` 메서드

```java
public void initialize() throws LifecycleException {
  if (initialized)
      throw new LifecycleException (
         sm.getString("StandardService.initialize.initialized"));
  initialized = true;

    // Initialize our defined Connectors
    synchronized (connectors) {
      for (int i = 0; i < connectors.length; i++) {
         connectors[i].initialize();
      }
    }
}
```

`start()` 메서드는 서비스에 연결된 컨테이너와 서비스에 추가된 모든 커넥터를 시작한다. 이 메서드는 목록 14.11에 제시되어 있다.

### 목록 14.11: `start()` 메서드

```java
public void start() throws LifecycleException {
  // Validate and update our current component state
  if (started) {

        throw new LifecycleException
          (sm.getString("standardService.start.started"));
    }

    // Notify our interested LifecycleListeners
    lifecycle.fireLifecycleEvent(BEFORE_START_EVENT, null);

    System.out.println
        (sm.getString("standardService.start.name", this.name));
    lifecycle.fireLifecycleEvent(START_EVENT, null);
    started = true;

    // Start our defined Container first
    if (container != null) {
        synchronized (container) {
          if (container instanceof Lifecycle) {
              ((Lifecycle) container).start();
          }
        }
    }

    // Start our defined Connectors second
    synchronized (connectors) {
      for (int i = 0; i < connectors.length; i++) {
         if (connectors[i] instanceof Lifecycle)
             ((Lifecycle) connectors[i]).start();
      }
    }

    // Notify our interested LifecycleListeners
    lifecycle.fireLifecycleEvent(AFTER_START_EVENT, null);
}
```

`stop()` 메서드는 연결된 컨테이너와 모든 커넥터를 중지한다. 이 메서드는 목록 14.12에 제시되어 있다.

### 목록 14.12: `stop()` 메서드

```java
public void stop() throws LifecycleException {
  // Validate and update our current component state
  if (!started) {
      throw new LifecycleException
         (sm.getString("standardService.stop.notStarted"));
  }

    // Notify our interested LifecycleListeners
    lifecycle.fireLifecycleEvent(BEFORE_STOP_EVENT, null);

    lifecycle.fireLifecycleEvent(STOP_EVENT, null);

    System.out.println
      (sm.getString("standardService.stop.name", this.name));
    started = false;

    // Stop our defined Connectors first
    synchronized (connectors) {

        for (int i = 0; i < connectors.length; i++) {
           if (connectors[i] instanceof Lifecycle)
               ((Lifecycle) connectors[i]).stop();
        }
    }

    // Stop our defined Container second
    if (container != null) {
        synchronized (container) {
          if (container instanceof Lifecycle) {
              ((Lifecycle) container).stop();
          }
        }
    }

    // Notify our interested LifecycleListeners
    lifecycle.fireLifecycleEvent(AFTER_STOP_EVENT, null);
}
```

`start()`는 먼저 컨테이너를 시작하고 그다음 커넥터를 시작한다. 반대로 `stop()`은 먼저 커넥터를 중지한 다음 컨테이너를 중지한다. 각 단계 전후로 관련 생명주기 이벤트가 발생한다.

> 다음 절 **The Application**은 원본 PDF 298페이지 마지막에서 제목만 등장하며, 본문은 299페이지부터 이어진다. 원문 Java 코드는 수정하지 않았다.

## 애플리케이션 (원본 PDF 299~304페이지)

이 애플리케이션은 서버와 서비스를 사용하는 방법을 보여 준다. 특히 `StandardServer` 클래스의 시작 및 종료 메커니즘을 활용하는 방법을 설명한다. 애플리케이션에는 세 개의 클래스가 있다. 첫 번째 `SimpleContextConfig`는 13장의 애플리케이션에서 복사한 것이고, 나머지 두 클래스는 `Bootstrap`과 `Stopper`다. `Bootstrap`은 애플리케이션을 시작하고 `Stopper`는 종료하는 데 사용한다.

### Bootstrap 클래스

`Bootstrap` 클래스는 목록 14.13과 같다.

**목록 14.13: Bootstrap 클래스**

```java
package ex14.pyrmont.startup;

import ex14.pyrmont.core.SimpleContextConfig;
import org.apache.catalina.Connector;
import org.apache.catalina.Context;
import org.apache.catalina.Engine;
import org.apache.catalina.Host;
import org.apache.catalina.Lifecycle;
import org.apache.catalina.LifecycleException;
import org.apache.catalina.LifecycleListener;
import org.apache.catalina.Loader;
import org.apache.catalina.Server;
import org.apache.catalina.Service;
import org.apache.catalina.Wrapper;
import org.apache.catalina.connector.http.HttpConnector;

import org.apache.catalina.core.StandardContext;
import org.apache.catalina.core.StandardEngine;
import org.apache.catalina.core.StandardHost;
import org.apache.catalina.core.StandardServer;
import org.apache.catalina.core.StandardService;
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
     Service service = new StandardService();
     service.setName("Stand-alone Service");
     Server server = new StandardServer();
     server.addService(service);
     service.addConnector(connector);
     // StandardService class's setContainer method calls
     // its connectors' setContainer method
     service.setContainer(engine);
     // Start the new server
     if (server instanceof Lifecycle) {
         try {
            server.initialize();
            ((Lifecycle) server).start();
            server.await();
            // the program waits until the await method returns,
            // i.e. until a shutdown command is received.
         }
         catch (LifecycleException e) {
            e.printStackTrace(System.out);
         }
     }

     // Shut down the server
     if (server instanceof Lifecycle) {
         try {
            ((Lifecycle) server).stop();
         }
         catch (LifecycleException e) {
            e.printStackTrace(System.out);
         }
     }
  }
}
```

`Bootstrap`의 `main()` 메서드 앞부분은 13장의 예제와 비슷하다. 커넥터, 두 개의 래퍼, 컨텍스트, 호스트, 엔진을 생성한다. 이어서 래퍼를 컨텍스트에, 컨텍스트를 호스트에, 호스트를 엔진에 추가한다. 그러나 커넥터를 최상위 컨테이너인 엔진과 직접 연결하지는 않는다. 대신 `Service` 객체를 만들고 이름을 지정한 뒤 `Server` 객체를 생성하여 서비스를 서버에 추가한다.

```java
Service service = new StandardService();
service.setName("Stand-alone Service");
Server server = new StandardServer();
server.addService(service);
```

그다음 `main()`은 커넥터와 엔진을 서비스에 추가한다.

```java
service.addConnector(connector);
service.setContainer(engine);
```

커넥터를 서비스에 추가하면 커넥터는 해당 서비스의 컨테이너와 연결된다.

이어서 `Server`의 `initialize()`와 `start()`를 호출한다. 이 과정에서 커넥터가 초기화되고 커넥터와 컨테이너가 시작된다.

```java
if (server instanceof Lifecycle) {
    try {
       server.initialize();
       ((Lifecycle) server).start();
```

그다음 서버의 `await()` 메서드를 호출한다. 이 메서드는 서버가 **8085 포트**에서 종료 명령을 기다리게 한다고 본문에 설명되어 있다. 이 시점에 커넥터는 이미 실행 중이며 기본 포트인 **8080**에서 HTTP 요청을 기다린다.

```java
server.await() ;
```

올바른 종료 명령이 도착하기 전까지 `await()`는 반환하지 않는다. 명령을 수신하면 `main()`은 서버의 `stop()`을 호출하여 나머지 구성 요소를 중지한다.

이제 서버를 종료하는 데 사용하는 `Stopper` 클래스를 살펴보자.

### Stopper 클래스

앞 장의 애플리케이션에서는 컨테이너를 갑자기 종료하거나 키를 눌러 종료했다. `Stopper`는 Catalina 서버를 더 정돈된 방식으로 종료할 수 있게 한다. 또한 생명주기를 지원하는 모든 구성 요소의 `stop()` 메서드가 호출되도록 보장한다.

**목록 14.14: Stopper 클래스**

```java
package ex14.pyrmont.startup;

import java.io.OutputStream;
import java.io.IOException;
import java.net.Socket;

public class Stopper {
  public static void main(String[] args) {
     // the following code is taken from the Stop method of
     // the org.apache.catalina.startup.Catalina class
     int port = 8005;
     try {
        Socket socket = new Socket("127.0.0.1", port);
        OutputStream stream = socket.getOutputStream();
        String shutdown = "SHUTDOWN";
        for (int i = 0; i < shutdown.length(); i++)
           stream.write(shutdown.charAt(i));
        stream.flush();
        stream.close();
        socket.close();
        System.out.println("The server was successfully shut down.");
     }
     catch (IOException e) {
        System.out.println("Error. The server has not been started.");
     }
  }
}
```

`Stopper.main()`은 `Socket`을 생성하고 올바른 종료 명령인 `SHUTDOWN` 문자열을 포트로 전송한다. 실행 중인 Catalina 서버가 있다면 종료된다. **원문 주의:** 본문은 이 포트를 `8085`라고 설명하지만, 바로 위 코드에서는 `8005`를 사용한다. 두 값을 임의로 통일하지 않고 원문대로 옮겼다.

### 애플리케이션 실행

Windows에서는 작업 디렉터리에서 다음과 같이 `Bootstrap`을 실행한다. 원문에서 여러 줄로 나뉜 클래스 경로는 한 줄로 표시했다.

```text
java -classpath ./lib/servlet.jar;./lib/commons-collections.jar;./lib/commons-digester.jar;./lib/naming-factory.jar;./lib/naming-common.jar;./ ex14.pyrmont.startup.Bootstrap
```

Linux에서는 라이브러리 사이를 콜론(`:`)으로 구분한다.

```text
java -classpath ./lib/servlet.jar:./lib/commons-collections.jar:./lib/commons-digester.jar:./lib/naming-factory.jar:./lib/naming-common.jar:./ ex14.pyrmont.startup.Bootstrap
```

브라우저에서 `PrimitiveServlet`을 호출하려면 다음 URL을 사용한다.

```text
http://localhost:8080/app1/Primitive
```

`ModernServlet`은 다음 URL로 호출한다.

```text
http://localhost:8080/app1/Modern
```

Windows와 Linux 모두 작업 디렉터리에서 다음 명령으로 종료 프로그램을 실행한다.

```text
java ex14.pyrmont.startup.Stopper
```

실제 Catalina 배포 환경에서는 `Stopper`가 제공하는 기능이 `Bootstrap` 클래스 안에 포함되어 있다는 점에 유의하자.

### 요약

이 장에서는 Catalina의 두 가지 중요한 구성 요소인 **서버(server)**와 **서비스(service)**를 설명했다. 서버는 Catalina 배포 환경을 정상적인 절차에 따라 시작하고 종료하는 메커니즘을 제공한다는 점에서 특히 유용하다. 서비스는 하나의 컨테이너와 하나 이상의 커넥터를 캡슐화한다. 이 장의 예제는 서버와 서비스를 사용하는 방법뿐 아니라 `StandardServer`의 종료 메커니즘을 활용하는 방법도 보여 준다.
