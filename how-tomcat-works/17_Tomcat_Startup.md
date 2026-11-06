# 17장: Tomcat 시작 (Tomcat Startup)

> 원본 PDF 349~353페이지 번역. Java 코드 및 기술 식별자는 원문 표기를 유지했다. 마지막 코드는 다음 페이지에서 계속된다.

## 개요

이 장에서는 `org.apache.catalina.startup` 패키지의 두 클래스인 `Catalina`와 `Bootstrap`을 중심으로 Tomcat의 시작 과정을 살펴본다. `Catalina` 클래스는 `Server` 객체를 시작하고 종료하며, Tomcat 설정 파일인 `server.xml`을 파싱하는 데 사용된다. `Bootstrap` 클래스는 `Catalina` 인스턴스를 만들고 그 `process` 메서드를 호출하는 진입점이다. 이론적으로 두 클래스를 하나로 합칠 수도 있다. 하지만 Tomcat을 여러 실행 방식으로 지원하기 위해 여러 부트스트랩 클래스가 제공된다. 예를 들어 앞서 언급한 `Bootstrap` 클래스는 Tomcat을 독립 실행형 애플리케이션으로 실행할 때 사용한다. 다른 클래스인 `org.apache.catalina.startup.BootstrapService`는 Tomcat을 Windows NT 서비스로 실행할 때 사용한다.

사용자 편의를 위해 Tomcat에는 서블릿 컨테이너를 쉽게 시작하고 종료할 수 있는 배치 파일과 셸 스크립트도 포함되어 있다. 이러한 파일을 이용하면 `Bootstrap` 클래스를 실행하기 위한 `java.exe` 프로그램의 옵션을 기억할 필요가 없다. 대신 알맞은 배치 파일이나 셸 스크립트만 실행하면 된다.

이 장의 첫 번째 절에서는 `Catalina` 클래스를, 두 번째 절에서는 `Bootstrap` 클래스를 다룬다. 내용을 이해하려면 서버와 서비스를 다룬 14장, Digester를 다룬 15장, 종료 훅을 다룬 16장을 먼저 읽어 두어야 한다. 또한 Windows와 Unix/Linux에서 Tomcat을 실행하는 방법을 각각 설명한다. 한 절에서는 Windows에서 Tomcat을 시작하고 종료하는 배치 파일을, 다른 절에서는 Unix/Linux의 셸 스크립트를 설명한다.

## Catalina 클래스

`org.apache.catalina.startup.Catalina` 클래스는 시작을 담당하는 클래스이다. 이 클래스에는 `%CATALINE_HOME%/conf` 디렉터리에 있는 `server.xml` 파일을 파싱하는 `Digester`가 포함되어 있다. 이 `Digester`에 추가되는 규칙을 이해하면 원하는 방식으로 Tomcat을 설정할 수 있다.

또한 `Catalina` 클래스는 `Service`를 갖는 `Server` 객체를 캡슐화한다. 15장에서 언급했듯이 `Service` 객체에는 하나의 컨테이너와 하나 이상의 커넥터가 포함된다. `Catalina`를 이용해 `Server` 객체를 시작하고 종료한다.

Tomcat을 실행하려면 `Catalina` 클래스를 인스턴스화한 뒤 `process` 메서드를 호출한다. 이 메서드를 호출할 때 적절한 인수를 전달해야 한다. Tomcat을 시작하려면 첫 번째 인수로 `start`를, 종료 명령을 보내려면 `stop`을 사용한다. `-help`, `-config`, `-debug`, `-nonaming` 같은 인수도 사용할 수 있다.

> **참고:** `nonaming` 인수가 있으면 JNDI 네이밍을 지원하지 않도록 지정한다. Tomcat에서 JNDI 네이밍을 지원하는 방법에 대해서는 `org.apache.naming` 패키지를 참고한다.

`Catalina` 클래스 자체에도 진입점인 `main` 메서드가 있지만, 일반적으로는 `Bootstrap` 클래스가 `Catalina`를 생성하고 `process` 메서드를 호출하도록 한다. 다음 절에서는 부트스트랩 클래스 중 하나를 설명한다. 이 절에서도 `Bootstrap`이 수행하는 작업을 알아볼 수 있다.

Tomcat 4의 `Catalina` 클래스에 있는 `process` 메서드는 목록 17.1과 같다.

**목록 17.1: Catalina 클래스의 process 메서드**

```java
public void process(String args[]) {
  setCatalinaHome();
  setCatalinaBase();
  try {
     if (arguments(args))
         execute();
  }
  catch (Exception e) {
     e.printStackTrace(System.out);
  }
}
```

`process` 메서드는 `catalina.home`과 `catalina.base`라는 두 시스템 속성을 설정한다. `catalina.home`의 기본값은 `user.dir` 속성값이다. `catalina.base`에는 `catalina.home`의 값이 할당된다. 따라서 두 속성 모두 `user.dir` 속성값을 갖는다.

> **참고:** `user.dir` 시스템 속성은 사용자의 작업 디렉터리, 즉 `java` 명령을 실행한 디렉터리를 가리킨다. 시스템 속성 목록은 J2SE 1.4 API 명세 문서의 `java.lang.System` 클래스에 있는 `getProperties` 메서드를 참고한다.

그런 다음 `process` 메서드는 인수 목록을 전달하면서 `arguments` 메서드를 호출한다. 목록 17.2의 `arguments` 메서드는 명령행 인수를 처리하고, `Catalina` 객체가 처리를 계속해야 하면 `true`를 반환한다.

**목록 17.2: arguments 메서드**

```java
protected boolean arguments(String args[]) {
  boolean isConfig = false;
  if (args.length < 1) {
      usage();
      return (false);
  }
  for (int i = 0; i < args.length; i++) {
      if (isConfig) {
          configFile = args[i];
          isConfig = false;
      }
      else if (args[i].equals("-config")) {
          isConfig = true;
      }
      else if (args[i].equals("-debug")) {
          debug = true;
      }
      else if (args[i].equals("-nonaming")) {
          useNaming = false;
      }
      else if (args[i].equals("-help")) {
          usage();
          return (false);
      }
      else if (args[i].equals("start")) {
          starting = true;
      }
      else if (args[i].equals("stop")) {
          stopping = true;
      }
      else {
          usage();
          return (false);
      }
  }
  return (true);
}
```

`process` 메서드는 `arguments` 메서드의 반환값을 검사하고, 반환값이 `true`이면 `execute` 메서드를 호출한다. `execute` 메서드는 목록 17.3과 같다.

**목록 17.3: execute 메서드**

```java
protected void execute() throws Exception {
  if (starting)
      start();
  else if (stopping)
      stop();
}
```

`execute` 메서드는 Tomcat을 시작하는 `start` 메서드 또는 종료하는 `stop` 메서드를 호출한다. 두 메서드는 이어지는 소절에서 설명한다.

> **참고:** Tomcat 5에는 더 이상 `execute` 메서드가 없다. `process` 메서드에서 `start` 또는 `stop` 메서드를 직접 호출한다.

### start 메서드

`start` 메서드는 Tomcat 설정 파일인 `server.xml`을 처리하기 위해 `Digester` 인스턴스를 생성한다. XML 파일을 파싱하기 전에 `Digester`의 `push` 메서드를 호출하면서 현재 `Catalina` 객체를 전달한다. 그러면 `Catalina` 객체가 `Digester` 내부 객체 스택의 첫 번째 객체가 된다. 파싱이 끝나면 `server` 변수가 `Server` 객체를 참조하게 되며, 기본적으로 그 타입은 `org.apache.catalina.core.StandardServer`이다.

이후 `start` 메서드는 `Server` 객체의 `initialize`와 `start` 메서드를 호출한다. 이어서 `Server` 객체의 `await` 메서드를 호출하여 종료 명령을 기다리는 스레드를 사용하도록 한다. `await` 메서드는 종료 명령을 받을 때까지 반환하지 않는다. `await`가 반환되면 `Catalina`의 `start` 메서드는 `Server` 객체의 `stop` 메서드를 호출한다. 그 결과 `Server` 객체와 다른 모든 컴포넌트가 종료된다. 또한 사용자가 애플리케이션을 갑자기 종료하더라도 `Server` 객체의 `stop` 메서드가 실행되도록 종료 훅을 사용한다.

`start` 메서드는 목록 17.4에 나와 있다.

**목록 17.4: start 메서드 — 앞부분 (다음 페이지에서 계속)**

```java
protected void start() {
// Create and execute our Digester
Digester digester = createStartDigester();
File file = configFile();
try {
   InputSource is =
      new InputSource("file://" + file.getAbsolutePath());
   FileInputStream fis = new FileInputStream(file);
   is.setByteStream(fis);
   digester.push(this);
   digester.parse(is);
   fis.close();
}
catch (Exception e) {
   System.out.println("Catalina.start: " + e);
   e.printstackTrace(System.out);
   System.exit(1);
}

// Setting additional variables
if (!useNaming) {
    System.setProperty("catalina.useNaming", "false");
}
else {
    System.setProperty("catalina.useNaming", "true");
    String value = "org.apache.naming";
    String oldValue =
        System.getProperty(javax.naming.Context.URL_PKG_PREFIXES);
    if (oldValue != null) {
        value = value + ":" + oldValue;
    }
    System.setProperty(javax.naming.Context.URL_PKG_PREFIXES, value);
    value = System.getProperty
        (javax.naming.Context.INITIAL_CONTEXT_FACTORY);
    if (value == null) {
        System.setProperty
           (javax.naming.Context.INITIAL_CONTEXT_FACTORY,
           "org.apache.naming.java.javaURLContextFactory");
    }
}

// If a SecurityManager is being used, set properties for
// checkPackageAccess() and checkPackageDefinition
if( System.getSecurityManager() != null ) {
    String access = Security.getProperty("package.access");
```

> **원문 표기 참고:** 본문에는 `%CATALINE_HOME%`이라고 인쇄되어 있으며, 코드에는 `e.printstackTrace(System.out)`가 나온다. 위에서는 원문을 그대로 보존했다.

## 원본 PDF 354~358페이지 번역

### `start()` 메서드 (계속)

앞 페이지에서 시작한 `start()` 메서드의 나머지 코드다. 보안 관리자가 사용되는 경우 패키지 접근 및 정의 관련 보안 속성을 설정하고, 표준 출력과 표준 오류 출력을 사용자 정의 `PrintStream`으로 교체한다. 그런 다음 서버를 초기화하고 시작하며, 종료 훅을 등록한 후 종료 명령을 기다린다. 서버가 종료될 때는 종료 훅을 제거하고 서버의 `stop()` 메서드를 호출한다.

```java
    if( access != null && access.length() > 0 )
        access += ",";
    else
        access = "sun.,";
    Security.setProperty("package.access",
    access + "org.apache.catalina.,org.apache.jasper.");
    String definition = Security.getProperty("package.definition");
    if( definition != null && definition.length() > 0 )
        definition += ",";
    else
        definition = "sun.,";
    Security.setProperty("package.definition",

    // FIX ME package "javax." was removed to prevent HotSpot
    // fatal internal errors
    definition + "java.,org.apache.catalina.,org.apache.jasper.");
}

// Replace System.out and System.err with a custom PrintStream
SystemLogHandler log = new SystemLogHandler(System.out);
System.setOut(log);
System.setErr(log);

Thread shutdownHook = new CatalinaShutdownHook();

// Start the new server
if (server instanceof Lifecycle) {
    try {
       server.initialize();
       ((Lifecycle) server).start();
       try {
          // Register shutdown hook
          Runtime.getRuntime().addShutdownHook(shutdownHook);
       }
       catch (Throwable t) {
          // This will fail on JDK 1.2. Ignoring, as Tomcat can run
          // fine without the shutdown hook.
       }
       // Wait for the server to be told to shut down
       server.await();
    }
    catch (LifecycleException e) {
       System.out.println("Catalina.start: " + e);
       e.printStackTrace(System.out);
       if (e.getThrowable() != null) {
           System.out.println("----- Root Cause -----");
           e.getThrowable().printStackTrace(System.out);
       }
    }
}

// Shut down the server
if (server instanceof Lifecycle) {
    try {
       try {
           // Remove the ShutdownHook first so that server.stop()
           // doesn't get invoked twice
           Runtime.getRuntime().removeShutdownHook(shutdownHook);
       }
       catch (Throwable t) {
           // This will fail on JDK 1.2. Ignoring, as Tomcat can run
           // fine without the shutdown hook.
       }
       ((Lifecycle) server).stop();
    }
    catch (LifecycleException e) {
       System.out.println("Catalina.stop: " + e);
       e.printStackTrace(System.out);
       if (e.getThrowable() != null) {
           System.out.println("----- Root Cause -----");
           e.getThrowable().printStackTrace(System.out);
       }
    }
}
}
```

### `stop()` 메서드

`stop()` 메서드는 Catalina를 중지하고 `Server` 객체가 종료되도록 한다. **리스팅 17.5**는 `stop()` 메서드를 보여 준다.

**리스팅 17.5: `stop()` 메서드**

```java
protected void stop() {
  // Create and execute our Digester
  Digester digester = createStopDigester();
  File file = configFile();
  try {
     InputSource is =
        new InputSource("file://" + file.getAbsolutePath());
     FileInputStream fis = new FileInputStream(file);
     is.setByteStream(fis);
     digester.push(this);
     digester.parse(is);
     fis.close();
  }
  catch (Exception e) {
     System.out.println("Catalina.stop: " + e);
     e.printStackTrace(System.out);
     System.exit(1);
  }

  // Stop the existing server
  try {
     Socket socket = new Socket("127.0.0.1", server.getPort());
     OutputStream stream = socket.getoutputStream();
     String shutdown = server.getShutdown();
     for (int i = 0; i < shutdown.length(); i++)
        stream.write(shutdown.charAt(i));
     stream.flush();
     stream.close();
     socket.close();
  }
  catch (IOException e) {
     System.out.println("Catalina.stop: " + e);
     e.printStackTrace(System.out);
     System.exit(1);
  }
}
```

`stop()` 메서드는 `createStopDigester()`를 호출하여 `Digester` 인스턴스를 생성한다. 그런 다음 `Digester`의 `push()` 메서드를 호출해 현재 `Catalina` 객체를 내부 스택에 넣고 설정 파일을 파싱한다. `Digester`에 추가되는 규칙은 뒤의 **「Stop Digester」** 절에서 설명한다.

이어서 실행 중인 `Server` 객체에 종료 명령을 전송하여 서버를 중지한다.

> **원문 표기:** 리스팅 17.5의 `socket.getoutputStream()`은 원문에 인쇄된 그대로다. Java의 일반적인 메서드 표기는 `getOutputStream()`이다.

### Start Digester

`Catalina`의 `createStartDigester()` 메서드는 `Digester` 인스턴스를 만들고 `server.xml` 파일을 파싱하는 데 사용할 규칙을 추가한다. 이 XML 문서는 Tomcat을 설정하는 데 사용되며 `%CATALINE_HOME%/conf` 디렉터리에 있다. `Digester`에 추가되는 규칙을 이해하는 것은 Tomcat 설정 방식을 이해하는 데 핵심적이다.

**리스팅 17.6: `createStartDigester()` 메서드**

```java
protected Digester createStartDigester() {

  // Initialize the digester
  Digester digester = new Digester();
  if (debug)
      digester.setDebug(999);
  digester.setValidating(false);

  // Configure the actions we will be using
  digester.addObjectCreate("Server",
     "org.apache.catalina.core.StandardServer", "className");
  digester.addSetProperties("Server");
  digester.addSetNext("Server", "setServer",
     "org.apache.catalina.Server");

  digester.addObjectCreate("Server/GlobalNamingResources",
     "org.apache.catalina.deploy.NamingResources");
  digester.addSetProperties("Server/GlobalNamingResources");
  digester.addSetNext("Server/GlobalNamingResources",
     "setGlobalNamingResources",
     "org.apache.catalina.deploy.NamingResources");

  digester.addObjectCreate("Server/Listener", null, "className");
  digester.addSetProperties("Server/Listener");
  digester.addSetNext("Server/Listener",
     "addLifecycleListener",
     "org.apache.catalina.LifecycleListener");

  digester.addObjectCreate("Server/Service",
     "org.apache.catalina.core.StandardService", "className");
  digester.addSetProperties("Server/Service");
  digester.addSetNext("Server/Service", "addService",
     "org.apache.catalina.Service");

  digester.addObjectCreate("Server/Service/Listener",
     null, "className");
  digester.addSetProperties("Server/Service/Listener");
  digester.addSetNext("Server/Service/Listener",
     "addLifecycleListener", "org.apache.catalina.LifecycleListener");

  digester.addObjectCreate("Server/Service/Connector",
     "org.apache.catalina.connector.http.HttpConnector",
     "className");
  digester.addSetProperties("Server/Service/Connector");
  digester.addSetNext("Server/Service/Connector",
     "addConnector", "org.apache.catalina.Connector");

  digester.addObjectCreate("Server/Service/Connector/Factory",
     "org.apache.catalina.net.DefaultServerSocketFactory",
     "className");
  digester.addSetProperties("Server/Service/Connector/Factory");
  digester.addSetNext("Server/Service/Connector/Factory",
     "setFactory", "org.apache.catalina.net.ServerSocketFactory");

  digester.addObjectCreate("Server/Service/Connector/Listener",
     null, "className");
  digester.addSetProperties("Server/Service/Connector/Listener");
  digester.addSetNext("Server/Service/Connector/Listener",
     "addLifecycleListener", "org.apache.catalina.LifecycleListener");

  // Add RuleSets for nested elements
  digester.addRuleSet(
     new NamingRuleSet("Server/GlobalNamingResources/"));
  digester.addRuleSet(new EngineRuleSet("Server/Service/"));
  digester.addRuleSet(new HostRuleSet("Server/Service/Engine/"));
  digester.addRuleSet(new
     ContextRuleSet("Server/Service/Engine/Default"));
  digester.addRuleSet(
     new NamingRuleSet("Server/Service/Engine/DefaultContext/"));
  digester.addRuleSet(
     new ContextRuleSet("Server/Service/Engine/Host/Default"));
  digester.addRuleSet(
     new NamingRuleSet("Server/Service/Engine/Host/DefaultContext/"));
```



## `createStartDigester()`의 나머지 코드 (원본 PDF 359페이지)

다음은 앞 페이지에서 시작된 리스팅 17.6의 마지막 부분이다.

```java
    digester.addRuleSet(
       new ContextRuleSet("Server/Service/Engine/Host/"));
    digester.addRuleSet(
       new NamingRuleSet("Server/Service/Engine/Host/Context/"));
    digester.addRule("Server/Service/Engine",
       new SetParentClassLoaderRule(digester, parentClassLoader));

    return (digester);
}
```

`createStartDigester` 메서드는 `org.apache.commons.digester.Digester` 클래스의 인스턴스를 생성한 다음 규칙을 추가한다.

처음 세 규칙은 `server.xml` 파일의 `server` 요소를 위한 것이다. 이미 알고 있듯이 `server` 요소는 루트 요소다. 다음은 `Server` 패턴에 대한 규칙이다.

```java
    digester.addObjectCreate("Server",
       "org.apache.catalina.core.StandardServer", "className");
    digester.addSetProperties("Server");
    digester.addSetNext("Server", "setServer",
       "org.apache.catalina.Server");
```

첫 번째 규칙은 `server` 요소를 만나면 Digester가 `org.apache.catalina.core.StandardServer`의 인스턴스를 생성해야 한다는 뜻이다. 다만 `server` 요소에 `className` 속성이 있다면 예외다. 이때는 `className` 속성의 값으로 지정된 클래스를 대신 인스턴스화해야 한다.

두 번째 규칙은 이름이 일치하는 속성값을 이용해 `Server` 객체의 프로퍼티를 채운다.

세 번째 규칙은 `Server` 객체가 스택에 들어가고, 스택의 다음 객체인 `Catalina` 인스턴스와 연결되도록 `setServer` 메서드를 호출하게 한다. 그렇다면 `Catalina` 인스턴스가 어떻게 Digester의 객체 스택에 들어 있을까? `start` 메서드는 `server.xml`을 파싱하기 전에 Digester의 `push` 메서드를 호출한다는 점을 기억하자.

```java
digester.push (this);
```

이 코드 한 줄은 `Catalina` 객체를 Digester 내부의 객체 스택에 넣는다.

이제 메서드를 직접 읽어 보면 나머지 규칙도 이해할 수 있을 것이다. 이해하기 어렵다면 15장을 다시 읽어 보자.

## 종료용 Digester (Stop Digester) — 원본 PDF 360페이지

`createStopDigester` 메서드는 `Server` 객체를 정상적으로 종료하기 위한 `Digester` 객체를 반환한다. 리스팅 17.7에 이 메서드가 나와 있다.

**리스팅 17.7: `stop` 메서드**

```java
protected Digester createStopDigester() {
  // Initialize the digester
  Digester digester = new Digester();
  if (debug)
      digester.setDebug(999);

    // Configure the rules we need for shutting down

    digester.addObjectCreate("Server",
       "org.apache.catalina.core.StandardServer", "className");
    digester.addSetProperties("Server");
    digester.addSetNext("Server", "setServer",
       "org.apache.catalina.Server");
    return (digester);
}
```

시작용 Digester와 달리 종료용 Digester는 루트 요소에만 관심이 있다.

## `Bootstrap` 클래스 — 원본 PDF 360~363페이지

`org.apache.catalina.startup.Bootstrap` 클래스는 Tomcat을 시작하기 위한 진입점을 제공하는 클래스 중 하나다. `startup.bat` 또는 `startup.sh` 파일을 실행하면 실제로는 이 클래스의 `main` 메서드가 호출된다. `main` 메서드는 클래스 로더 세 개를 생성하고 `Catalina` 클래스를 인스턴스화한 다음, `Catalina`의 `process` 메서드를 호출한다.

`Bootstrap` 클래스는 리스팅 17.8에 나와 있다.

**리스팅 17.8: `Bootstrap` 클래스 (363페이지까지의 원문 코드)**

```java
package org.apache.catalina.startup;

import java.io.File;
import java.lang.reflect.Method;

/**
 * Boostrap loader for Catalina. This application constructs a
 * class loader for use in loading the Catalina internal classes
 * (by accumulating all of the JAR files found in the "server"
 * directory under "catalina.home"), and starts the regular execution
 * of the container. The purpose of this roundabout approach is to
 * keep the Catalina internal classes (and any other classes they
 * depend on, such as an XML parser) out of the system
 * class path and therefore not visible to application level classes.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.36 $ $Date: 2002/04/01 19:51:31 $
 */

public final class Bootstrap {
  /**
   * Debugging detail level for processing the startup.
   */
  private static int debug = 0;

  /**
   * The main program for the bootstrap.
   *
   * @param args Command line arguments to be processed
   */

  public static void main(String args[]) {

    // Set the debug flag appropriately
    for (int i = 0; i < args.length; i++) {
       if ("-debug".equals(args[i]))
           debug = 1;
    }

    // Configure catalina.base from catalina.home if not yet set
    if (System.getProperty("catalina.base") == null)
        System.setProperty("catalina.base", getCatalinaHome());

    // Construct the class loaders we will need
    ClassLoader commonLoader = null;
    ClassLoader catalinaLoader = null;
    ClassLoader sharedLoader = null;
    try {
       File unpacked[] = new File[1];
       File packed[] = new File[1];
       File packed2[] = new File[2];
       ClassLoaderFactory.setDebug(debug);

      unpacked[0] = new File(getCatalinaHome(),
        "common" + File.separator + "classes");
      packed2[0] = new File(getCatalinaHome(),
        "common" + File.separator + "endorsed");
      packed2[l] = new File(getCatalinaHome(),
        "common" + File.separator + "lib");
      commonLoader =
        ClassLoaderFactory.createClassLoader(unpacked, packed2, null);

      unpacked[0] = new File(getCatalinaHome(),
        "server" + File.separator + "classes");
      packed[0] = new File(getCatalinaHome(),
        "server" + File.separator + "lib");
      catalinaLoader =
        ClassLoaderFactory.createClassLoader(unpacked, packed,
        commonLoader);

      unpacked[0] = new File(getCatalinaBase(),
        "shared" + File.separator + "classes");
      packed[0] = new File(getCatalinaBase(),
        "shared" + File.separator + "lib");
      sharedLoader =
        ClassLoaderFactory.createClassLoader(unpacked, packed,
        commonLoader);
    }
    catch (Throwable t) {
      log('Class loader creation threw exception", t);
      System.exit(1);
    }

    Thread.currentThread().setContextClassLoader(catalinaLoader);

    // Load our startup class and call its process() method
    try {

      SecurityClassLoad.securityClassLoad(catalinaLoader);

      // Instantiate a startup class instance
      if (debug >= 1)
          log("Loading startup class");
      Class startupClass =
          catalinaLoader.loadClass
             ("org.apache.catalina.startup.Catalina");
      Object startupInstance = startupClass.newInstance();

      // Set the shared extensions class loader
      if (debug >= 1)
          log("Setting startup class properties");
      String methodName = "setParentClassLoader";
      Class paramTypes[] = new Class[1];
      paramTypes[0] = Class.forName("java.lang.ClassLoader");
      Object paramValues[] = new Object[1];
      paramValues[0] = sharedLoader;
      Method method =
          startupInstance.getClass().getMethod(methodName, paramTypes);
      method.invoke(startupInstance, paramValues);

      // Call the process() method
      if (debug >= 1)
          log("Calling startup class process() method");
      methodName = "process";
      paramTypes = new Class[1];
      paramTypes[0] = args.getClass();
      paramValues = new Object[l];
      paramValues[0] = args;
      method =
          startupInstance.getClass().getMethod(methodName, paramTypes);
      method.invoke(startupInstance, paramValues);
    }
    catch (Exception e) {
      System.out.println("Exception during startup processing");
      e.printStackTrace(System.out);
      System.exit(2);
    }
}

/**
 * Get the value of the catalina.home environment variable.
 */
private static String getCatalinaHome() {
   return System.getProperty("catalina.home",
```

> **원문 주의:** 위 코드는 원본 PDF의 표기를 보존했다. `packed2[l]`, `new Object[l]`의 `l`과 `log('Class loader creation threw exception", t);`의 따옴표는 원문에서 보이는 형태이며, 그대로 컴파일 가능한 코드라는 의미는 아니다. `getCatalinaHome()` 구현은 다음 PDF 페이지에서 이어진다.

## PDF 364~368페이지 번역

### `Bootstrap` 클래스의 나머지 메서드

앞 페이지에서 시작한 `getCatalinaHome()` 메서드의 나머지 코드와 `Bootstrap` 클래스의 끝부분은 다음과 같다.

```java
          System.getProperty("user.dir"));
    }

    /**
      * Get the value of the catalina.base environment variable.
      */
    private static String getCatalinaBase() {
       return System.getProperty("catalina.base", getCatalinaHome());
    }

    /**
      * Log a debugging detail message.
      *
      * @param message The message to be logged
      */
    private static void log(String message) {
        System.out.print("Bootstrap: ");
        System.out.println(message);
    }

    /**
      * Log a debugging detail message with an exception.
      *
      * @param message The message to be logged
      * @param exception The exception to be logged
      */
    private static void log(String message, Throwable exception) {
        log(message);
        exception.printStackTrace(System.out);
    }

}
```

`Bootstrap` 클래스에는 네 개의 정적 메서드가 있다. 두 개의 `log` 메서드와 `getCatalinaHome`, `getCatalinaBase` 메서드다. `getCatalinaHome`의 구현은 다음과 같다.

```java
return System.getProperty("catalina.home",
  System.getProperty("user.dir"));
```

기본적으로 이 코드는 `catalina.home`에 값이 미리 설정되어 있지 않으면 `user.dir` 속성값을 반환한다는 뜻이다.

`getCatalinaBase` 메서드의 구현은 다음과 같다.

```java
return System.getProperty("catalina.base", getCatalinaHome());
```

이는 `catalina.base` 속성이 없으면 `catalina.home`의 값을 반환한다는 뜻이다.

`getCatalinaHome`과 `getCatalinaBase`는 모두 `Bootstrap` 클래스의 `main` 메서드에서 호출된다.

### 세 가지 클래스 로더

`Bootstrap` 클래스의 `main` 메서드는 서로 다른 용도로 사용할 클래스 로더 세 개도 생성한다. 여러 클래스 로더를 사용하는 주된 이유는 웹 애플리케이션의 클래스(서블릿과 기타 보조 클래스)가 `WEB-INF/classes` 및 `WEB-INF/lib` 디렉터리 밖의 클래스에 접근하여 실행하는 것을 제한하기 위해서다. 다만 `%CATALINE_HOME%/common/lib` 디렉터리에 배포된 JAR 클래스는 허용된다.

세 클래스 로더는 다음과 같이 선언한다.

```java
// Construct the class loaders we will need

ClassLoader commonLoader = null;
ClassLoader catalinaLoader = null;
ClassLoader sharedLoader = null;
```

그다음 각 클래스 로더에 접근 가능한 경로를 지정한다. `commonLoader`는 `%CATALINA_HOME%/common/classes`, `%CATALINA_HOME%/common/endorsed`, `%CATALINA_HOME%/common/lib`의 세 디렉터리에서 Java 클래스를 로드할 수 있다.

```java
try {
   File unpacked[] = new File[1];
   File packed[] = new File[1];
   File packed2[] = new File[2];
   ClassLoaderFactory.setDebug(debug);

   unpacked[0] = new File(getCatalinaHome(),
     "common" + File.separator + "classes");
   packed2[0] = new File(getCatalinaHome(),
     "common" + File.separator + "endorsed");
   packed2[1] = new File(getCatalinaHome(),
     "common" + File.separator + "lib");
   commonLoader =
     ClassLoaderFactory.createClassLoader(unpacked, packed2, null);
```

`catalinaLoader`는 Catalina 서블릿 컨테이너를 실행하는 데 필요한 클래스를 로드한다. `%CATALINA_HOME%/server/classes`와 `%CATALINA_HOME%/server/lib`뿐 아니라 `commonLoader`가 접근할 수 있는 모든 디렉터리에서 Java 클래스를 로드할 수 있다.

```java
   unpacked[0] = new File(getCatalinaHome(),
     "server" + File.separator + "classes");
   packed[0] = new File(getCatalinaHome(),
     "server" + File.separator + "lib");
   catalinaLoader =
     ClassLoaderFactory.createClassLoader(unpacked, packed,
     commonLoader);
```

`sharedLoader`는 `%CATALINA_HOME%/shared/classes`와 `%CATALJNA_HOME%/shared/lib` 디렉터리, 그리고 `commonLoader`가 접근할 수 있는 모든 디렉터리에 접근할 수 있다. 이후 `sharedLoader`는 Tomcat에 배포된 각 `Context`에 속하는 웹 애플리케이션 클래스 로더의 부모 클래스 로더로 지정된다.

```java
   unpacked[0] = new File(getCatalinaBase(),
     "shared" + File.separator + "classes");
   packed[0] = new File(getCatalinaBase(),
     "shared" + File.separator + "lib");
   sharedLoader =

     ClassLoaderFactory.createClassLoader(unpacked, packed,
     commonLoader);
}
catch (Throwable t) {
  log('Class loader creation threw exception", t);
  System.exit(1);
}
```

`sharedLoader`는 Catalina 내부 클래스나 `CLASSPATH` 환경 변수에 정의된 클래스 경로에는 접근할 수 없다는 점에 유의하자. 클래스 로더의 작동 방식에 대한 자세한 설명은 8장을 참조하라.

세 클래스 로더를 생성한 뒤 `main` 메서드는 `Catalina` 클래스를 로드하고 인스턴스를 생성하여 `startupInstance` 변수에 할당한다.

```java
Class startupClass =
  catalinaLoader.loadClass
    ("org.apache.catalina.startup.Catalina");
Object startupInstance = startupClass.newInstance();
```

그런 다음 `sharedLoader`를 인수로 전달해 `setParentClassLoader` 메서드를 호출한다.

```java
// Set the shared extensions class loader
if (debug >= 1)
    log("Setting startup class properties");
String methodName = "setParentClassLoader";
Class paramTypes[] = new Class[1];
paramTypes[0] = Class.forName("java.lang.ClassLoader");
Object paramValues[] = new Object[1];
paramValues[0] = sharedLoader;
Method method =
    startupInstance.getClass().getMethod(methodName, paramTypes);
method.invoke(startupInstance, paramValues);
```

마지막으로 `main` 메서드는 `Catalina` 객체의 `process` 메서드를 호출한다.

```java
// Call the process() method
if (debug >= 1)
    log("Calling startup class process() method");
methodName = "process";
paramTypes = new Class[1];
paramTypes[0] = args.getClass();
paramValues = new Object[1];
paramValues[0] = args;
method =
    startupInstance.getClass().getMethod(methodName, paramTypes);
method.invoke(startupInstance, paramValues);
```

### Windows에서 Tomcat 실행하기

앞 절에서 배운 것처럼 Tomcat을 독립 실행형 애플리케이션으로 실행하려면 `Bootstrap` 클래스를 호출한다. Windows 플랫폼에서는 `startup.bat` 배치 파일을 실행해 Tomcat을 시작하고 `shutdown.bat` 배치 파일을 실행해 종료한다. 두 배치 파일은 모두 `%CATALINA_HOME%/bin` 디렉터리에 있다.

이 절에서는 이 배치 파일들을 살펴본다. 배치 파일에 나타나는 DOS 명령줄에 익숙하지 않다면 첫 번째 하위 절인 「배치 파일 작성 입문」을 읽어보라.

### 배치 파일 작성 입문

이 절은 Tomcat을 시작하고 종료하는 배치 파일을 이해할 수 있도록 배치 파일의 기초를 소개한다. 특히 `rem`, `if`, `echo`, `goto`, 레이블(`label`) 등의 명령을 설명한다. 이 주제를 포괄적으로 다루려는 것은 아니므로, 더 자세한 내용은 다른 자료를 참고해야 한다.

우선 배치 파일의 확장자는 반드시 `.bat`여야 한다. Windows 탐색기에서 배치 파일을 두 번 클릭하거나 DOS 콘솔에서 명령을 입력해 실행할 수 있다. 실행되면 첫 줄부터 마지막 줄까지 각 명령문이 해석된다. Tomcat의 배치 파일에서 사용하는 언어 요소를 다음 하위 절에서 설명한다.

> **참고:** DOS 명령과 환경 변수는 대소문자를 구분하지 않는다.

#### `rem`

`rem` 명령은 주석을 작성하는 데 사용한다. `rem`으로 시작하는 줄은 무시되며 처리되지 않는다.

#### `pause`

`pause` 명령은 배치 파일의 처리를 일시 중지하고 사용자에게 키를 누르라는 메시지를 표시한다. 사용자가 키를 누르면 처리가 계속된다.

#### `echo`

이 명령은 뒤에 오는 텍스트를 DOS 콘솔에 표시한다. 예를 들어 다음 코드는 콘솔에 `Hello World`를 출력하고 일시 중지한다. 메시지를 표시하자마자 콘솔이 닫히지 않도록 `pause` 명령이 필요하다.

```bat
echo Hello World
pause
```

환경 변수의 값을 출력하려면 변수 이름을 `%` 기호로 감싼다. 예를 들어 다음 명령은 `myVar`의 값을 출력한다.

```bat
echo %myVar%.
```

운영체제의 이름을 출력하려면 다음 명령을 사용한다(명령 자체는 다음 PDF 페이지에서 이어진다).

> **원문 표기 주의:** 원본에는 `%CATALINE_HOME%`, `%CATALJNA_HOME%` 및 `log('Class loader creation threw exception", t);`처럼 일관되지 않은 표기가 나타난다. 여기서는 원문을 보존했으며, 실제 실행 시에는 각각의 경로와 Java 문자열 리터럴을 확인해야 한다.

## 원본 PDF 369~373페이지 번역

운영체제의 이름을 출력하려면 다음 명령을 사용한다.

```bat
cho %OS%
```

> **원문 표기:** 원본에는 `cho %OS%`라고 인쇄되어 있다. 일반적인 배치 명령은 `echo %OS%`이다.

#### `echo off`

`echo off`는 배치 파일에 있는 명령줄 자체가 표시되지 않도록 한다. 실행 결과만 화면에 표시된다. 하지만 `echo off` 명령 자체는 여전히 화면에 표시된다. 이 명령까지 표시되지 않도록 하려면 `@echo off`를 사용한다.

```bat
echo off
```

#### `@echo off`

`@echo off`는 `echo off`와 비슷하지만, `echo off` 명령 자체도 표시되지 않도록 한다.

```bat
@echo off
```

#### `set`

이 명령은 사용자가 정의한 환경 변수 또는 이름이 지정된 환경 변수를 설정하는 데 사용한다. 배치 파일에서 설정한 환경 변수는 메모리에 일시적으로 존재하며 배치 파일 실행이 끝나면 사라진다.

예를 들어 다음 `set` 명령은 `THE_KING`이라는 환경 변수를 만들고 값을 `Elvis`로 지정한 다음 콘솔에 표시한다.

```bat
set THE_KING=Elvis
echo %THE_KING%
pause
```

> **참고:** 변수 값을 참조하려면 변수 이름을 `%` 문자로 감싸야 한다. 예를 들어 `echo %the_king%`는 `THE_KING` 변수의 값을 표시한다.

#### `label` (레이블)

콜론(`:`)을 사용하여 레이블을 정의한다. 이후 `goto` 명령에 해당 레이블을 전달하면 처리가 레이블 다음 줄로 이동한다. 다음은 `end`라는 레이블이다.

```bat
:end
```

자세한 예는 `goto` 명령을 참고하자.

#### `goto`

`goto` 명령은 배치 파일의 실행 위치를 지정된 레이블 바로 다음 줄로 강제로 이동시킨다. 다음 예를 살펴보자.

```bat
echo Start
goto end
echo I can guarantee this line will not be executed
:end
echo End
pause
```

첫 번째 줄에서 `Start`를 출력한 뒤 `goto` 명령을 실행하면 제어가 `end` 레이블 바로 다음 줄로 이동한다. 따라서 세 번째 줄은 건너뛴다.

#### `if`

`if`는 조건을 검사하는 데 사용한다. 다음 세 가지 방식으로 사용할 수 있다.

1. 변수의 값을 검사한다.
2. 파일이 존재하는지 검사한다.
3. 오류 값을 검사한다.

변수 값을 검사하려면 다음 형식을 사용한다.

```bat
if variable==value nextCommand
```

예를 들어 다음 `if` 문은 `myVar`의 값이 `3`인지 검사한다. 값이 `3`이면 콘솔에 `Correct`를 출력한다.

```bat
set myVar=3
if %myVar%==3 echo Correct
```

이 명령들을 실행하면 `myVar`의 값이 평가되고 `Correct`가 출력된다.

파일이 존재하는지 검사하려면 다음 형식을 사용한다.

```bat
if exist c:\temp\myFile.txt goto start
```

`c:\temp` 디렉터리에 `myFile.txt`가 있으면 제어가 `start` 레이블 다음 줄로 이동한다.

`not` 키워드를 사용하여 조건을 부정할 수도 있다.

#### `not`

`not` 키워드는 조건을 부정한다. 예를 들어 다음 명령은 `myVar`의 값이 `3`이 *아닐 때* `Correct`를 출력한다.

```bat
set myVar=3
if not %myVar%==3 echo Correct
pause
```

다음 명령은 `c:\temp` 디렉터리에 `myFile.txt` 파일이 없으면 `end` 레이블로 이동한다.

```bat
if not exist c:\temp\myFile.txt goto end
```

#### `exist`

`exist` 키워드는 `if` 문과 함께 사용하여 파일의 존재 여부를 검사한다. 예제는 앞의 `if` 절을 참고하자.

### 매개변수 받기 (Accepting Parameters)

배치 파일에 매개변수를 전달할 수 있다. 첫 번째 매개변수는 `%1`, 두 번째 매개변수는 `%2` 등으로 참조한다.

예를 들어 다음 명령은 첫 번째 매개변수를 콘솔에 출력한다.

```bat
echo %1
```

배치 파일 이름이 `test.bat`이고 `test Hello` 명령으로 실행하면 `Hello`가 출력된다.

다음 배치 파일은 첫 번째 매개변수의 값을 검사한다. 값이 `start`이면 `Starting application`, `stop`이면 `Stopping application`을 출력한다. 둘 다 아니면 `Invalid parameter`를 출력한다.

```bat
echo off
if %1==start goto start

if %1==stop goto stop
goto invalid
:start
echo Starting application
goto end

:stop
echo Stopping application
goto end

:invalid
echo Invalid parameter

:end
```

배치 파일에 매개변수가 전달되었는지 확인하려면 `%1`을 빈 문자열과 비교한다. 예를 들어 다음 배치 파일은 매개변수가 없으면 콘솔에 `No parameter`를 출력한다.

```bat
if "%1"=="" echo No parameter
```

위 명령은 다음과 같다.

```bat
if ""%1""=="""" echo No parameter
```

> **원문 표기 주의:** 위의 두 번째 비교식은 원문에 제시된 그대로이며, 실제 배치 파일에서 사용하기 전에 따옴표 구성을 확인하는 편이 좋다.

#### `shift`

`shift` 명령은 매개변수의 위치를 하나씩 앞으로 이동시킨다. 즉, `%2`의 값이 `%1`로, `%3`의 값이 `%2`로 복사되는 식이다. 다음 예를 보자.

```bat
echo off
shift
echo %1
echo %2
```

이 배치 파일에 `a`, `b`, `c`라는 세 매개변수를 전달하여 실행하면 다음 결과를 얻는다.

```text
b
c
```

`shift` 이후에는 `%0`을 사용하여 첫 번째 매개변수를 참조할 수 있다. 마지막 매개변수는 사라진다.

#### `call`

`call` 명령은 다른 명령을 호출하는 데 사용한다.

#### `setLocal`

배치 파일에서 `setLocal`을 사용하면 현재 배치 파일을 실행하는 동안 변경한 환경 변수가 해당 배치 파일 내부에서만 유효하도록 지정할 수 있다. 변경된 환경 변수의 값은 배치 파일이 끝나거나 `endLocal` 명령을 만나면 이전 값으로 복원된다.

#### `start`

새 Windows 콘솔을 연다. 다음과 같이 새 창의 제목을 전달할 수 있다.

```bat
start "Title"
```

제목 뒤에 명령을 지정하면 새 콘솔에서 곧바로 실행된다.

```bat
start "Title" commandName
```

### `catalina.bat` 배치 파일

`catalina.bat` 배치 파일은 Tomcat을 시작하고 종료하는 데 사용한다. Tomcat을 더 간편하게 시작하고 종료하도록 `startup.bat`과 `shutdown.bat`이라는 두 파일도 제공한다. 이 두 파일은 적절한 매개변수를 전달하여 `catalina.bat`을 호출한다.

`%CATALINA_HOME%` 아래의 `bin` 디렉터리에서 다음 구문으로 `catalina.bat`을 호출해야 한다.

```bat
catalina command
```

또는 `%CATALINE_HOME%`에서 다음과 같이 호출할 수 있다.

```bat
bin\catalina command
```

> **원문 표기:** 두 번째 경로는 원문에서 `%CATALINE_HOME%`으로 표기되지만 일반적인 Tomcat 환경 변수 이름은 `%CATALINA_HOME%`이다.

두 경우 모두 `command`에 지정할 수 있는 값은 다음과 같다.

- `debug`: 디버거에서 Catalina를 시작한다.
- `debug -security`: 보안 관리자를 사용하면서 Catalina를 디버깅한다.

> **이어지는 부분:** `catalina.bat`에 전달할 수 있는 나머지 명령과 스크립트 설명은 원본 PDF 374페이지부터 계속된다.

## PDF 374~378페이지 번역 — `catalina.bat` 실행 명령과 배치 스크립트

### `catalina.bat`에 전달할 수 있는 나머지 명령

- `embedded`: Catalina를 임베디드 모드로 시작한다.
- `jpda start`: JPDA 디버거에서 Catalina를 시작한다.
- `run`: 현재 창에서 Catalina를 시작한다.
- `run -security`: 보안 관리자를 사용하여 현재 창에서 Catalina를 시작한다.
- `start`: 별도 창에서 Catalina를 시작한다.
- `start -security`: 보안 관리자를 사용하여 별도 창에서 Catalina를 시작한다.
- `stop`: Catalina를 종료한다.

예를 들어 별도 창에서 Catalina를 시작하려면 다음 명령을 사용한다.

```bat
catalina start
```

`catalina.bat` 배치 파일은 **리스팅 17.9**에 나와 있다.

### 리스팅 17.9: `catalina.bat` 파일

아래는 원본 PDF 374~378페이지에 수록된 배치 파일의 해당 부분이다. **명령과 주석은 원문 언어를 유지했다.** PDF 텍스트 추출 과정에서 긴 명령행과 주석이 여러 줄로 분리되어 있으므로, 실제 실행용 파일로 사용하려면 원본 배치 파일과 대조해야 한다. 리스팅은 379페이지에서 계속된다.

```bat
@echo off
if "%OS%" == "Windows_NT" setlocal
rem
-------------------------------------------------------------------
--------
rem Start/Stop Script for the CATALINA Server
rem
rem Environment Variable Prequisites
rem
rem CATALINA_HOME May point at your Catalina "build" directory.
rem
rem CATALINA_BASE (Optional) Base directory for resolving dynamic
portions
rem                        of a Catalina installation. If not present,
resolves to
rem                        the same directory that CATALINA_HOME points to.
rem
rem CATALINA_OPTS (Optional) Java runtime options used when the
"start",
rem                        "stop", or "run" command is executed.
rem
rem CATALINA_TMPDIR (Optional) Directory path location of temporary
directory
rem                        the JVM should use (java.io.tmpdir). Defaults to
rem                        %CATALINA_BASE%\temp.
rem
rem JAVA_HOME                 Must point at your Java Development Kit
installation.
rem
rem JAVA_OPTS       (Optional) Java runtime options used when the
"start",
rem               "stop", or "run" command is executed.
rem
rem JSSE_HOME       (Optional) May point at your Java Secure Sockets
Extension
rem               (JSSE) installation, whose JAR files will be
added to the
rem               system class path used to start Tomcat.
rem
rem JPDA_TRANSPORT (Optional) JPDA transport used when the "jpda
start"
rem               command is executed. The default is "dt_shmem".
rem

rem JPDA_ADDRESS             (Optional) Java runtime options used when the
"jpda start"
rem                        command is executed. The default is "jdbconn".
rem
rem $Id: catalina.bat,v 1.3 2002/08/04 18:19:43 patrickl Exp $
rem
-------------------------------------------------------------------
--------

rem Guess CATALINA_HOME if not defined
if not "%CATALINA_HOME%" == "" goto gotHome
set CATALINA_HOME=.
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
set CATALINA_HOME=..
:gotHome
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
echo The CATALINA_HOME environment variable is not defined correctly
echo This environment variable is needed to run this program
goto end
:okHome

rem Get standard environment variables
if exist "%CATALINA_HOME%\bin\setenv.bat" call
"%CATALINA_HOME%\bin\setenv.bat"

rem Get standard Java environment variables
if exist "%CATALINA_HOME%\bin\setclasspath.bat" goto okSetclasspath
echo Cannot find %CATALINA_HOME%\bin\setclasspath.bat
echo This file is needed to run this program
goto end
:okSetclasspath
set BASEDIR=%CATALINA_HOME%
call "%CATALINA_HOME%\bin\setclasspath.bat"

rem Add on extra jar files to CLASSPATH
if "%JSSE_HOME%" == "" goto noJsse
set
CLASSPATH=%CLASSPATH%;%JSSE_HOME%\lib\jcert.jar;%JSSE_HOME%\lib\jnet
.ja
r;%JSSE_HOME%\lib\jsse.jar
:noJsse
set CLASSPATH=%CLASSPATH%;%CATALINA_HOME%\bin\bootstrap.jar

if not "%CATALINA_BASE%" == "" goto gotBase
set CATALINA_BASE=%CATALINA_HOME%
:gotBase

if not "%CATALINA_TMPDIR%" == "" goto gotTmpdir
set CATALINA_TMPDIR=%CATALINA_BASE%\temp
:gotTmpdir

rem ----- Execute The Requested Command -------------------------------
--------

echo Using CATALINA_BASE: %CATALINA_BASE%
echo Using CATALINA_HOME: %CATALINA_HOME%
echo Using CATALINA_TMPDIR: %CATALINA_TMPDIR%
echo Using JAVA_HOME:       %JAVA_HOME%

set _EXECJAVA=%_RUNJAVA%
set MAINCLASS=org.apache.catalina.startup.Bootstrap
set ACTION=start
set SECURITY_POLICY_FILE=
set DEBOG_OPTS=
set JPDA=

if not ""%1"" == ""jpda"" goto noJpda
set JPDA=jpda
if not "%JPDA_TRANSPORT%" == "" goto gotJpdaTransport
set JPDA_TRANSPORT=dt_shmem
:gotJpdaTransport
if not "%JPDA_ADDRESS%" == "" goto gotJpdaAddress
set JPDA_ADDRESS=jdbconn
:gotJpdaAddress
shift
:noJpda

if ""%1"" == ""debug"" goto doDebug
if ""%1"" == ""embedded"" goto doEmbedded
if ""%1"" == ""run"" goto doRun
if ""%1"" == ""start"" goto doStart
if ""%1"" == ""stop"" goto doStop

echo Usage: catalina ( commands ... )
echo commands:
echo    debug              Start Catalina in a debugger
echo debug -security Debug Catalina with a security manager
echo embedded               Start Catalina in embedded mode
echo jpda start          Start Catalina under JPDA debugger
echo run                   Start Catalina in the current window
echo run -security       Start in the current window with security
manager
echo start                Start Catalina in a separate window
echo start -security Start in a separate window with security
manager
echo stop                  Stop Catalina
goto end

:doDebug
shift
set _EXECJAVA=%_RUNJDB%
set DEBUG_OPTS=-sourcepath "%CATALINA_HOME%\..\..\jakarta-tomcat-
4.0\catalina\src\share"
if not ""%1"" == ""-security"" goto execCmd
shift
echo Using Security Manager
set SECURITY_POLICY_FILE=%CATALINA_BASE%\conf\catalina.policy
goto execCmd

:doEmbedded
shift
set MAINCLASS=org.apache.catalina.startup.Embedded
goto execCmd

:doRun
shift
if not ""%1"" == ""-security"" goto execCmd
shift
echo Using Security Manager
set SECURITY_POLICY_FILE=%CATALINA_BASE%\conf\catalina.policy
goto execCmd

:doStart
shift
if not "%OS%" == "Windows_NT" goto noTitle
set _EXECJAVA=start "Tomcat" %_RUNJAVA%
goto gotTitle
:noTitle
set _EXECJAVA=start %_RUNJAVA%
:gotTitle
if not ""%1"" == ""-security"" goto execCmd
shift
echo Using Security Manager
set SECURITY_POLICY_FILE=%CATALINA_BASE%\conf\catalina.policy
goto execCmd

:doStop
shift
set ACTION=stop
goto execCmd

:execCmd
rem Get remaining unshifted command line arguments and save them in the
set CMD_LINE_ARGS=
:setArgs
if ""%1""=="""" goto doneSetArgs
set CMD_LINE_ARGS=%CMD_LINE_ARGS% %1
shift
goto setArgs
:doneSetArgs

rem Execute Java with the applicable properties
if not "%JPDA%" == "" goto dojpda
if not "%SECURITY_POLICY_FILE%" == "" goto doSecurity
%_EXECJAVA% %JAVA_OPTS% %CATALINA_OPTS% %DEBUG_OPTS% -
Djava.endorsed.dirs="%JAVA_ENDORSED_DIRS%" -classpath "%CLASSPATH%" -
Dcatalina.base="%CATALINA_BASE%" -Dcatalina.home="%CATALINA_HOME%" -
Djava.io.tmpdir="%CATALINA_TMPDIR%" %MAINCLASS% %CMD_LINE_ARGS%
%ACTION%
goto end
:doSecurity
```

**구조 설명:** 이 부분에서는 `CATALINA_HOME`을 찾아 검증하고, `setenv.bat` 및 `setclasspath.bat`을 호출해 환경을 준비한다. 필요한 JAR을 클래스 경로에 추가하고 `CATALINA_BASE`와 임시 디렉터리의 기본값을 설정한다. 이어서 첫 번째 인수에 따라 디버깅·임베디드·현재 창 실행·새 창 실행·종료 경로로 분기한다. 마지막에는 남은 인수를 모아 Java 실행 명령에 전달한다. 보안 관리자와 JPDA를 사용하는 실제 실행 분기는 다음 페이지에 이어진다.

## PDF 379~383페이지 — `catalina.bat`의 실행 흐름

> 원본 PDF 379페이지의 첫 부분은 앞 페이지에서 시작한 리스팅 17.9의 끝부분이다. 이 부분의 배치 명령은 PDF 줄바꿈 때문에 원본과 대조해야 하며, 아래에서는 이어지는 본문 설명과 명령 예제를 번역한다.

`catalina.bat` 파일은 먼저 `@echo off`를 호출해 명령 자체가 화면에 표시되지 않도록 한다. 그런 다음 `OS` 환경 변수의 값이 `Windows_NT`인지 확인한다. 이는 사용자가 Windows NT, Windows 2000 또는 Windows XP를 사용한다는 뜻이다. 이 경우 `setLocal`을 호출하여 환경 변수 변경 사항이 이 배치 파일 내부에만 적용되도록 한다.

```bat
if "%OS%" == "Windows_NT" setlocal
```

다음으로 `CATALINA_HOME` 변수가 아직 없으면 그 값을 설정한다. Tomcat을 실행하기 위해 이 변수를 반드시 미리 설정할 필요는 없으므로, 기본적으로는 `CATALINA_HOME`이 존재하지 않을 수 있다.

`CATALINA_HOME`을 찾을 수 없다면 배치 파일이 호출된 디렉터리를 기준으로 추정한다. 먼저 설치 디렉터리에서 `catalina.bat`을 실행했다고 가정한다. 그렇다면 `bin` 디렉터리에서 해당 파일을 찾을 수 있어야 한다.

```bat
if not "%CATALINA_HOME%" == "" goto gotHome
set CATALINA_HOME=.
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
```

현재 디렉터리의 `bin` 하위 디렉터리에서 `catalina.bat`을 찾지 못했다면, 설치 디렉터리에서 실행된 것이 아니다. 배치 파일은 다시 추정한다. 이번에는 설치 디렉터리 아래의 `bin` 디렉터리에서 실행했다고 가정하고, `bin`의 상위 디렉터리를 `CATALINA_HOME`으로 설정한 뒤 `bin`에 `catalina.bat`이 존재하는지 확인한다.

```bat
set CATALINA_HOME=..
:gotHome
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
```

추정이 맞으면 `okHome`으로 이동한다. 그렇지 않으면 `CATALINA_HOME`이 올바르게 설정되지 않았다는 오류 메시지를 표시하고, 배치 파일 끝의 `end` 레이블로 이동한다.

```bat
echo The CATALINA_HOME environment variable is not defined correctly
echo This environment variable is needed to run this program
goto end
```

`CATALINA_HOME`이 올바르게 정의되었다면, `setenv.bat`이 존재할 때 이를 호출해 필요한 환경 변수를 설정한다. 파일이 없더라도 오류 메시지는 발생하지 않는다.

```bat
:okHome
rem Get standard environment variables
if exist "%CATALINA_HOME%\bin\setenv.bat" call "%CATALINA_HOME%\bin\setenv.bat"
```

다음으로 `setclasspath.bat` 파일이 존재하는지 확인한다. 찾을 수 없다면 오류 메시지를 출력하고 `end`로 이동하여 배치 파일을 종료한다.

```bat
if exist "%CATALINA_HOME%\bin\setclasspath.bat" goto okSetclasspath
echo Cannot find %CATALINA_HOME%\bin\setclasspath.bat
echo This file is needed to run this program
goto end
```

`setclasspath.bat` 파일을 찾으면 `BASEDIR`을 `CATALINA_HOME` 값으로 설정한 다음, 클래스 경로를 구성하기 위해 `setclasspath.bat`을 호출한다.

```bat
:okSetclasspath
set BASEDIR=%CATALINA_HOME%
call "%CATALINA_HOME%\bin\setclasspath.bat"
```

`setclasspath.bat`은 `JAVA_HOME` 환경 변수가 올바르게 정의되었는지 확인하고, `catalina.bat`의 나머지 부분에서 사용할 다음 변수들을 설정한다.

```bat
set JAVA_ENDORSED_DIRS=%BASEDIR%\common\endorsed
set CLASSPATH=%JAVA_HOME%\lib\tools.jar
set _RUNJAVA="%JAVA_HOME%\bin\java"
set _RUNJAVAW="%JAVA_HOME%\bin\javaw"
set _RUNJDB="%JAVA_HOME%\bin\jdb"
set _RUNJAVAC="%JAVA_HOME%\bin\javac"
```

이어서 `catalina.bat`은 Java Secure Socket Extension(JSSE)이 설치되어 있고 `JSSE_HOME` 변수가 올바르게 설정되었는지 확인한다. `JSSE_HOME`이 있으면 해당 경로를 `CLASSPATH`에 추가한다.

```bat
if "%JSSE_HOME%" == "" goto noJsse
set CLASSPATH=%CLASSPATH%;%JSSE_HOME%\lib\jcert.jar;%JSSE_HOME%\lib\jnet.jar;%JSSE_HOME%\lib\jsse.jar
```

`JSSE_HOME`을 찾지 못하면 다음 줄로 진행하여 `bin` 디렉터리의 `bootstrap.jar`를 `CLASSPATH`에 추가한다.

```bat
:noJsse
set CLASSPATH=%CLASSPATH%;%CATALINA_HOME%\bin\bootstrap.jar
```

다음으로 `CATALINA_BASE` 값을 확인한다. 변수가 없으면 새로 만들고 `CATALINA_HOME` 값을 할당한다.

```bat
if not "%CATALINA_BASE%" == "" goto gotBase
set CATALINA_BASE=%CATALINA_HOME%
:gotBase
```

그런 다음 `CATALINA_BASE` 아래의 임시 디렉터리를 나타내는 `CATALINA_TMPDIR` 값을 확인한다.

```bat
if not "%CATALINA_TMPDIR%" == "" goto gotTmpdir
set CATALINA_TMPDIR=%CATALINA_BASE%\temp
:gotTmpdir
```

이어서 여러 변수의 값을 출력한다.

```bat
echo Using CATALINA_BASE: %CATALINA_BASE%
echo Using CATALINA_HOME: %CATALINA_HOME%
echo Using CATALINA_TMPDIR: %CATALINA_TMPDIR%
echo Using JAVA_HOME:       %JAVA_HOME%
```

그런 다음 `setclasspath.bat`에서 설정한 `_RUNJAVA` 값을 `_EXECJAVA`에 할당한다. `_RUNJAVA`의 값은 `"%JAVA_HOME%\bin\java"`이며, 다시 말해 `JAVA_HOME` 아래 `bin` 디렉터리의 `java.exe` 프로그램을 가리킨다.

```bat
set _EXECJAVA=%_RUNJAVA%
```

이어서 다음 변수들을 설정한다.

```bat
set MAINCLASS=org.apache.catalina.startup.Bootstrap
set ACTION=start
set SECURITY_POLICY_FILE=
set DEBUG_OPTS=
set JPDA=
```

`catalina.bat`은 첫 번째 인수가 `jpda`(Java Platform Debugger Architecture)인지 확인한다. 그렇다면 `JPDA` 변수를 `jpda`로 설정하고, `JPDA_TRANSPORT` 및 `JPDA_ADDRESS` 변수를 확인한 다음 인수를 한 칸 이동시킨다.

```bat
if not ""%1"" == ""jpda"" goto noJpda
set JPDA=jpda
if not "%JPDA_TRANSPORT%" == "" goto gotJpdaTransport
set JPDA_TRANSPORT=dt_shmem
:gotJpdaTransport
if not "%JPDA_ADDRESS%" == "" goto gotJpdaAddress
set JPDA_ADDRESS=jdbconn
:gotJpdaAddress
shift
```

대부분의 경우 JPDA를 사용하지 않으므로, 첫 번째 인수는 `debug`, `embedded`, `run`, `start`, `stop` 가운데 하나여야 한다.

```bat
:noJpda
if ""%1"" == ""debug"" goto doDebug
if ""%1"" == ""embedded"" goto doEmbedded
if ""%1"" == ""run"" goto doRun
if ""%1"" == ""start"" goto doStart
if ""%1"" == ""stop"" goto doStop
```

첫 번째 인수가 올바르지 않거나 인수가 없다면 사용법을 출력하고 종료한다.

```text
Usage: catalina ( commands ... )
commands:
 debug             Start Catalina in a debugger
 debug -security   Debug Catalina with a security manager
 embedded          Start Catalina in embedded mode
 jpda start        Start Catalina under JPDA debugger
 run               Start Catalina in the current window
 run -security     Start in the current window with security manager
 start             Start Catalina in a separate window
 start -security   Start in a separate window with security manager
 stop              Stop Catalina
```

첫 번째 인수가 `start`라면 `doStart`로, `stop`이라면 `doStop`으로 이동하며 다른 인수도 해당 레이블로 분기한다.

`doStart` 레이블에서는 `shift` 명령으로 다음 인수가 있는지 확인한다. 다음 인수가 있다면 `-security`여야 하며, 그렇지 않으면 무시된다. `-security`라면 다시 `shift`를 호출하고 `SECURITY_POLICY_FILE`을 `%CATALINA_BASE%\conf\catalina.policy`로 설정한다.

```bat
:doStart
shift
if not "%OS%" == "Windows_NT" goto noTitle
set _EXECJAVA=start "Tomcat" %_RUNJAVA%
goto gotTitle
:noTitle
set _EXECJAVA=start %_RUNJAVA%
:gotTitle
if not ""%1"" == ""-security"" goto execCmd
shift
echo Using Security Manager
set SECURITY_POLICY_FILE=%CATALINA_BASE%\conf\catalina.policy
goto execCmd
```

종료 경로인 `doStop`에서는 인수를 이동시키고 `ACTION`을 `stop`으로 설정한 뒤 `execCmd`로 이동한다.

```bat
:doStop
shift
set ACTION=stop
goto execCmd
```

`execCmd`에서는 아직 소비하지 않은 명령행 인수를 모아 `CMD_LINE_ARGS`에 저장한다.

```bat
:execCmd
rem Get remaining unshifted command line arguments and save them in the
set CMD_LINE_ARGS=
:setArgs
if ""%1""=="""" goto doneSetArgs
set CMD_LINE_ARGS=%CMD_LINE_ARGS% %1
shift
goto setArgs
:doneSetArgs
```

마지막으로 적용 가능한 속성을 지정하여 Java를 실행한다. `JPDA`가 설정되어 있으면 `dojpda`로, 보안 정책 파일이 지정되어 있으면 `doSecurity`로 이동한다. 그렇지 않으면 일반 실행 명령을 사용한다.

```bat
rem Execute Java with the applicable properties
if not "%JPDA%" == "" goto dojpda
if not "%SECURITY_POLICY_FILE%" == "" goto doSecurity
%_EXECJAVA% %JAVA_OPTS% %CATALINA_OPTS% %DEBUG_OPTS% -Djava.endorsed.dirs="%JAVA_ENDORSED_DIRS%" -classpath "%CLASSPATH%" -Dcatalina.base="%CATALINA_BASE%" -Dcatalina.home="%CATALINA_HOME%" -Djava.io.tmpdir="%CATALINA_TMPDIR%" %MAINCLASS% %CMD_LINE_ARGS% %ACTION%
goto end
:doSecurity
```

> **원문 보존 주의:** 위 배치 명령은 책에 실린 역사적 Tomcat 버전의 예제다. 특히 PDF에 나타난 `rem ... in the`처럼 끝이 잘린 주석, `""%1""` 표기, 긴 명령행의 개행은 원본 리스팅 및 실제 배치 파일과 대조해야 한다. 이 부분은 현대 Windows/Tomcat에서 그대로 실행 가능하다고 보장하지 않는다. 379페이지 상단에 이어진 보안·JPDA 실행 분기 리스팅은 앞선 번역 구간에 수록되어 있다.

## 원본 PDF 384~388페이지 번역

### `catalina.bat`의 실행 명령 마무리

```bat
set SECURITY_POLICY_FILE=%CATALINA_BASE%\conf\catalina.policy
```

이 단계에서 `_EXECJAVA`의 값은 다음 둘 중 하나다.

```bat
start "Tomcat" "%JAVA_HOME%\bin\java"
start "%JAVA_HOME%\bin\java"
```

그다음 `execCmd` 레이블로 이동한다.

```bat
goto execCmd
```

`execCmd` 레이블 아래의 명령은 아직 `shift`로 처리하지 않은 나머지 명령행 인수를 가져와 `CMD_LINE_ARGS`에 저장한 뒤 `doneSetArgs`로 이동한다.

```bat
:execCmd
set CMD_LINE_ARGS=
:setArgs
if ""%1""=="""" goto doneSetArgs
set CMD_LINE_ARGS=%CMD_LINE_ARGS% %1
shift
goto setArgs
```

`doneSetArgs` 뒤에는 다음 명령들이 있다.

```bat
:doneSetArgs
rem Execute Java with the applicable properties
if not "%JPDA%" == "" goto doJpda
if not "%SECURITY_POLICY_FILE%" == "" goto doSecurity
%_EXECJAVA% %JAVA_OPTS% %CATALINA_OPTS% %DEBUG_OPTS% -Djava.endorsed.dirs="%JAVA_ENDORSED_DIRS%" -classpath "%CLASSPATH%" -Dcatalina.base="%CATALINA_BASE%" -Dcatalina.home="%CATALINA_HOME%" -Djava.io.tmpdir="%CATALINA_TMPDIR%" %MAINCLASS% %CMD_LINE_ARGS% %ACTION%
```

예를 들어 저자의 컴퓨터에서 `catalina start`를 호출하면, 앞의 긴 명령행(`%_EXECJAVA%`로 시작하여 `%ACTION%`으로 끝나는 부분)은 다음과 같이 바뀐다.

```bat
start "Tomcat" "C:\j2sdk1.4.2_02\bin\java" -Djava.endorsed.dirs="..\common\endorsed" -classpath "C:\j2sdk1.4.2_02\lib\tools.jar;..\bin\bootstrap.jar" -Dcatalina.base=".." -Dcatalina,home=".." -Djava.io.tmpdir="..\temp" org.apache.catalina.startup.Bootstrap start
```

`catalina.bat`에 다른 인수를 전달하여 호출했을 때 어떤 명령이 실행될지도 이제 알아낼 수 있을 것이다.

> **원문 주의:** 원본 PDF에는 긴 명령행이 여러 줄로 나뉘어 있고, 예제의 `-Dcatalina,home`에는 쉼표가 들어 있다. 위에서는 명령행을 한 줄로 모았지만 원문의 표기를 수정하지 않았다.

### Windows에서 Tomcat 시작하기

리스팅 17.10에 제시된 `startup.bat`는 `catalina.bat`를 호출하기 위한 간편한 스크립트다. 이 파일은 `start`라는 인수 하나를 전달하여 `catalina.bat`를 호출한다.

**리스팅 17.10: `startup.bat` 파일**

```bat
@echo off
if "%OS%" == "Windows_NT" setlocal
rem ----------------------------------------------------------------
rem Start script for the CATALINA Server
rem
rem $Id: startup.bat,v 1.4 2002/08/04 18:19:43 patrickl Exp $
rem ----------------------------------------------------------------

rem Guess CATALINA_HOME if not defined
if not "%CATALINA_HOME%" == "" goto gotHome
set CATALINA_HOME=.
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
set CATALINA_HOME=..
:gotHome
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
echo The CATALINA_HOME environment variable is not defined correctly
echo This environment variable is needed to run this program
goto end
:okHome

set EXECUTABLE=%CATALINA_HOME%\bin\catalina.bat

rem Check that target executable exists
if exist "%EXECUTABLE%" goto okExec
echo Cannot find %EXECUTABLE%
echo This file is needed to run this program
goto end
:okExec

rem Get remaining unshifted command line arguments and save them in the
set CMD_LINE_ARGS=
:setArgs
if ""%1""=="""" goto doneSetArgs
set CMD_LINE_ARGS=%CMD_LINE_ARGS% %1
shift
goto setArgs
:doneSetArgs

call "%EXECUTABLE%" start %CMD_LINE_ARGS%

:end
```

모든 `rem` 명령과 `echo` 명령을 제거하면 다음과 같다.

```bat
if "%OS%" == "Windows_NT" setlocal
if not "%CATALINA_HOME%" == "" goto gotHome
set CATALINA_HOME=.
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
set CATALINA_HOME=..
:gotHome
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
goto end
:okHome
set EXECUTABLE=%CATALINA_HOME%\bin\catalina.bat
if exist "%EXECUTABLE%" goto okExec
goto end
:okExec

rem Get remaining unshifted command line arguments and save them in the
set CMD_LINE_ARGS=
:setArgs
if ""%1""=="""" goto doneSetArgs
set CMD_LINE_ARGS=%CMD_LINE_ARGS% %1
shift
goto setArgs
:doneSetArgs

call "%EXECUTABLE%" start %CMD_LINE_ARGS%

:end
```

> **원문 주의:** 책은 모든 `rem` 명령을 제거했다고 설명하지만, 축약된 코드에는 `rem Get remaining ...` 줄이 그대로 남아 있다.

### Windows에서 Tomcat 종료하기

`shutdown.bat`는 `stop` 인수를 전달하여 `catalina.bat`를 호출하기 위한 간편한 스크립트다. 리스팅 17.11에 `shutdown.bat`가 나와 있다.

**리스팅 17.11: `shutdown.bat` 파일**

```bat
@echo off
if "%OS%" == "Windows_NT" setlocal
rem ---------------------------------------------------------------------------
rem Stop script for the CATALINA Server
rem
rem $Id: shutdown.bat,v 1.3 2002/08/04 18:19:43 patrickl Exp $
rem ---------------------------------------------------------------------------

rem Guess CATALINA_HOME if not defined
if not "%CATALINA_HOME%" == "" goto gotHome
set CATALINA_HOME=.
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
set CATALINA_HOME=..
:gotHome
if exist "%CATALINA_HOME%\bin\catalina.bat" goto okHome
echo The CATALINA_HOME environment variable is not defined correctly
echo This environment variable is needed to run this program
goto end
:okHome

set EXECUTABLE=%CATALINA_HOME%\bin\catalina.bat

rem Check that target executable exists
if exist "%EXECUTABLE%" goto okExec

echo Cannot find %EXECUTABLE%
echo This file is needed to run this program
goto end
:okExec

rem Get remaining unshifted command line arguments and save them in the
set CMD_LINE_ARGS=
:setArgs
if ""%1""=="""" goto doneSetArgs
set CMD_LINE_ARGS=%CMD_LINE_ARGS% %1
shift
goto setArgs
:doneSetArgs

call "%EXECUTABLE%" stop %CMD_LINE_ARGS%
:end
```

> **원문 주의:** PDF 텍스트 추출에서는 `rem` 구분선이 여러 줄로 분리되어 있다. 위에서는 읽기 쉽도록 구분선을 한 줄로 정리했으며, 실행 명령은 유지했다.

### Unix/Linux에서 Tomcat 실행하기

Tomcat에는 Unix/Linux 환경에서 Tomcat을 시작하고 종료하기 위한 셸 스크립트가 포함되어 있다. 이 스크립트들은 `.sh` 확장자를 가지며 `%CATALINA_HOME%`의 `bin` 디렉터리에 있다. 이 절에서는 `catalina.sh`, `startup.sh`, `shutdown.sh`, `setclasspath.sh` 네 가지를 다룬다.

먼저 셸 스크립트에 익숙하지 않거나 관련 지식을 복습해야 하는 독자를 위해 셸 스크립트의 기본 사항을 소개한다. 이어서 `catalina.sh`, `startup.sh`, `shutdown.sh`를 설명한다. `setclasspath.sh`는 `catalina.sh`가 호출하므로 `catalina.sh`를 설명하는 하위 절에서 간단히 다룬다.

#### Unix/Linux 셸 스크립트 작성 입문

여기서 소개하는 셸 스크립트의 기초 지식은 Tomcat에 포함된 셸 스크립트, 특히 `catalina.sh`, `startup.sh`, `shutdown.sh`, `setclasspath.sh`를 읽을 수 있도록 돕기 위한 것이다. 이 주제를 완전히 다루는 설명은 아니므로, 더 자세한 내용은 다른 자료를 참고해야 한다.

셸 파일은 물리적으로 텍스트 파일이다. `vi`나 다른 텍스트 편집기로 작성할 수 있다. 다음 구문을 사용하여 파일의 권한 모드를 변경하고 실행 권한을 부여해야 한다.

```sh
$ chmod +x scriptName
$ chmod 755 scriptName
```

이렇게 하면 파일 소유자에게 읽기·쓰기·실행 권한을, 그룹과 다른 사용자에게 읽기·실행 권한을 부여한다.

그다음 다음 명령 중 하나로 스크립트를 실행할 수 있다.

```sh
bash scriptName
sh scriptName
./scriptName
```

스크립트는 인터프리터에 의해 한 줄씩 실행된다. 셸 스크립트에는 어떤 확장자를 붙여도 되고, 확장자가 없어도 된다. 다만 `.sh` 확장자가 가장 일반적이다.

이제 Tomcat 셸 스크립트를 이해하는 데 충분한 몇 가지 일반적인 명령을 살펴보자. (다음 페이지에서 계속)

---

## 원본 PDF 389~393페이지 번역

### Unix/Linux 셸 스크립트의 기본 명령 (계속)

#### 주석(comment)

`#` 기호는 해당 텍스트가 주석이며 명령으로 해석되지 않아야 함을 나타낸다. `#`는 줄의 맨 앞에 올 수 있으며, 이 경우 줄 전체가 주석이 된다.

```sh
# This is a comment
```

줄 중간에 올 수도 있다. 이 경우 `#` 오른쪽의 텍스트가 주석이다. 예를 들면 다음과 같다.

```sh
echo Hello # print Hello
```

#### clear

`clear` 명령은 화면을 지운다. 예를 들어 다음 스크립트는 먼저 화면을 지우고 문자열을 출력한다.

```sh
clear
echo Shell scripts are useful
```

#### exit

`exit` 명령은 셸 스크립트를 종료한다. 일반적으로 종료 상태값이 뒤따르는데, `0`은 정상적으로 실행되었음을, `0`이 아닌 값은 비정상 종료(오류 발생 가능성)를 의미한다. 따라서 셸에서 문제가 발생하여 종료하려면 다음과 같이 작성한다.

```sh
exit 1
```

#### echo

`echo`는 문자열을 화면에 출력한다. 예를 들어 다음 명령은 콘솔에 `Hello World`를 출력한다.

```sh
echo Hello World
```

### 함수 호출(Calling A Function)

마침표(`.`)를 사용하여 함수를 호출하거나 다른 셸 스크립트를 실행할 수 있다. 예를 들어 다음 명령은 실행 중인 스크립트와 같은 디렉터리에 있는 `test.sh`를 호출한다.

```sh
. ./test.sh
```

### 시스템 변수와 사용자 정의 변수(System and User Defined Variables)

변수 이름은 영숫자 문자 또는 밑줄로 시작해야 한다. 등호(`=`)를 사용하여 변수에 값을 지정한다. 예를 들어 다음 줄은 `myVar`에 `Tootsie`를 저장한다.

```sh
myVar=Tootsie
```

등호 앞뒤에는 공백이 없어야 한다. 변수 이름은 대소문자를 구별한다.

변수에 NULL 값을 지정하려면 빈 문자열을 사용하거나 등호 오른쪽을 비워 둔다.

```sh
myVar=
myVar=""
```

변수의 값을 참조하려면 변수 이름 앞에 달러 기호(`$`)를 붙인다. 다음은 `myVar`의 값을 화면에 출력하는 방법이다.

```sh
echo $myVar
```

Unix/Linux 운영체제는 여러 시스템 변수를 제공한다. 예를 들어 `HOME`은 현재 사용자의 홈 디렉터리, `PWD`는 사용자의 현재 위치, `PATH`는 명령 실행 시 검색할 경로를 나타낸다.

> **주의:** 시스템 변수를 변경했을 때 어떤 영향이 발생하는지 정확히 알지 못한다면 값을 변경하지 않아야 한다.

#### expr

`expr`은 수식을 계산하는 명령이다. 원문에서는 계산식을 역따옴표(back quote)로 둘러싸 사용한다. 역따옴표 키는 보통 키보드의 숫자 `1` 키 왼쪽에 있다. 다음은 산술식을 사용하는 셸 스크립트다.

```sh
sum=`expr 100 + 200`
echo $sum
```

이 코드는 사용자 정의 변수 `sum`을 만들고 `100 + 200`의 결과인 `300`을 할당한다. 스크립트를 실행하면 콘솔에 `300`이 출력된다.

다른 예는 다음과 같다.

```sh
echo `expr 200 + 300`
```

콘솔에는 다음 결과가 출력된다.

```text
500
```

특수 표현식 `uname`은 운영체제 이름을 반환한다. 예를 들어 Linux를 사용한다면 다음 명령은 콘솔에 `Linux`를 출력한다.

```sh
echo `uname`
```

특수 표현식 `dirname filePath`는 파일이 속한 디렉터리를 반환한다. 예를 들어 `dirname /home/user1/test.sh`는 `/home/user1`을 반환한다.

### 매개변수 접근(Accessing Parameters)

함수에 인수를 전달할 수 있는 것처럼 셸 스크립트에도 매개변수를 전달할 수 있다. `$1`은 첫 번째 매개변수, `$2`는 두 번째 매개변수를 가리키며 이후에도 같은 방식으로 이어진다. `$#`은 매개변수의 개수를, `$@`는 모든 매개변수를 나타낸다.

#### shift

이 명령은 모든 매개변수의 위치를 하나씩 앞으로 이동시킨다. 즉, `$1`에는 이전 `$2`의 값이, `$2`에는 이전 `$3`의 값이 들어가는 식이다.

#### if ... then ... [else ...] fi

`if` 문은 조건을 검사하고 그에 맞는 명령 목록을 실행한다. 구문은 다음과 같다.

```sh
if condition then
   list of commands
[else
   list of commands
]
fi
```

> **참고:** `else if`에 해당하는 구문으로 `elif`를 사용할 수도 있다.

예를 들어 다음 스크립트는 첫 번째 인수로 `start`를 전달하면 `Starting the application`을 출력하고, `stop`을 전달하면 `Stopping the application`을 출력한다.

```sh
if [ "$1" = "start" ]; then
    echo Starting the application
fi
if [ "$1" = "stop" ]; then
    echo Stopping the application
fi
```

> **참고:** 조건식에서는 `[` 뒤와 `]` 앞에 각각 공백이 있어야 한다.

`$1`을 큰따옴표로 감싸는 이유는 스크립트를 호출할 때 매개변수가 없더라도 인터프리터에서 예외가 발생하지 않도록 하기 위해서다.

`$0`은 스크립트를 호출할 때 사용한 명령을 가리킨다. 예를 들어 `test.sh`라는 스크립트를 다음과 같이 실행했다면,

```sh
./test.sh
```

`$0`에는 `./test.sh`가 들어간다.

조건식에는 다음과 같은 항목을 사용할 수 있다.

- `-f file`: 파일이 존재하면 참이다.
- `-r file`: 파일에 대한 읽기 권한이 있으면 참이다.
- `-z string`: 문자열이 비어 있으면 참이다.
- `-n string`: 문자열이 비어 있지 않으면 참이다.
- `string1 = string2`: 두 문자열이 같으면 참이다.
- `string1 != string2`: 두 문자열이 다르면 참이다.

### for 반복문(for Loop)

`for` 반복문은 동일한 코드 조각을 반복 실행할 때 사용한다. 구문은 다음과 같다.

```sh
for { var } in {list}
do
   list of commands
done
```

예를 들면 다음과 같다.

```sh
for i in 1 2 3
do
   echo iteration $i
done
```

출력 결과는 다음과 같다.

```text
iteration 1
iteration 2
iteration 3
```

### while 반복문(while Loop)

`while` 반복문의 구문은 다음과 같다.

```sh
while [ condition ]
do
   list of commands
done
```

예를 들면 다음과 같다.

```sh
n=1
while [ $n -lt 3 ];
do
   echo iteration $n
   n=$((n + 1))
done
```

출력 결과는 다음과 같다.

```text
iteration 1
iteration 2
```

`[ $n -lt 3 ]`의 `-lt`는 '보다 작다(less than)'는 뜻이므로, 조건은 `n`의 값이 `3`보다 작은지를 검사한다.

### case

`case`를 사용하면 프로그램 실행의 여러 대안을 기술할 수 있다. 구문은 다음과 같다. **구체적인 구문과 예제는 다음 페이지에서 이어진다.**

> **원문 표기 참고:** 이 구간의 `if` 구문 형식은 책에 제시된 설명용 표기를 그대로 옮겼다. 실제 실행 가능한 셸 코드에서는 조건과 `then`의 배치, 공백 등 셸 문법을 확인해야 한다.

## 원본 PDF 394~398페이지 번역

### `case` (계속)

`case` 구문의 형식은 다음과 같다.

```sh
case $variable-name in
pattern1)
  list of commands
  ;;
pattern2)
  list of commands
  ;;

*)
  list of commands
  ;;
esac
```

`;;`는 명령 목록의 끝을 나타낸다. 다른 어떤 패턴과도 일치하지 않으면 `*)` 패턴에 해당하는 명령을 실행한다.

예를 들어 다음 스크립트는 운영체제 이름을 확인해 화면에 출력한다. Cygwin, OS400, Linux 중 어느 것도 사용하지 않는 경우에는 `Operating system not recognized`를 출력한다.

```sh
case "`uname`" in
  CYGWIN*) echo cygwin;;
  OS400*) echo OS400;;
  Linux*) echo Linux;;
  *) echo Operating system not recognized
esac
```

### 출력 리다이렉션(Output Redirection)

`>`를 사용하면 출력을 파일로 보낼 수 있다. 예를 들어 다음 명령은

```sh
echo Hello > myFile.txt
```

`myFile.txt`라는 파일을 생성하고 그 안에 `Hello`를 기록한다. 화면에는 아무것도 출력되지 않는다.

또한 `1>&2`는 표준 출력을 표준 오류로, `2>&1`은 표준 오류를 표준 출력으로 리다이렉션한다. [원문에서는 앞의 명령을 'stdout의 오류 메시지'라고 설명하지만, 파일 디스크립터 관점에서는 위와 같이 이해하는 것이 정확하다.]

### 조건부 실행(Conditional Executions)

다른 명령의 실행 여부를 결정하는 명령 또는 조건을 작성할 수 있다. 이때 `&&`와 `||`를 사용한다.

```sh
command1 && command2
```

`command1`이 종료 상태 `0`을 반환하면 `command2`를 실행한다. `command1` 대신 조건을 사용할 수도 있다. 조건이 참이면 `command2`가 실행되고, 그렇지 않으면 실행되지 않는다.

```sh
command1 || command2
```

`command1`이 `0`이 아닌 종료 상태를 반환하면 `command2`를 실행한다.

```sh
command1 && command2 || command3
```

`command1`이 종료 상태 `0`을 반환하면 `command2`를 실행한다. 그렇지 않으면 `command3`를 실행한다.

### `catalina.sh` 파일

`catalina.sh`는 Unix/Linux에서 Tomcat을 시작하고 종료하는 데 사용한다. Tomcat을 시작하려면 첫 번째 인수로 `start`를 전달하고, 종료하려면 `stop`을 전달한다. 유효한 인수는 다음과 같다.

- `debug`: 디버거에서 Catalina를 시작한다(OS400에서는 사용할 수 없음).
- `debug -security`: 보안 관리자를 사용해 Catalina를 디버깅한다(OS400에서는 사용할 수 없음).
- `embedded`: 임베디드 모드로 Catalina를 시작한다.
- `jpda start`: JPDA 디버거에서 Catalina를 시작한다.
- `run`: 현재 창에서 Catalina를 시작한다.
- `run -security`: 보안 관리자를 사용하여 현재 창에서 시작한다.
- `start`: 별도 창에서 Catalina를 시작한다.
- `start -security`: 보안 관리자를 사용하여 별도 창에서 시작한다.
- `stop`: Catalina를 종료한다.

`catalina.sh` 파일은 **리스팅 17.12**에 제시되어 있다. 앞 절에서 소개한 내용을 바탕으로 이 스크립트의 동작을 이해할 수 있을 것이다.

### 리스팅 17.12: `catalina.sh` 파일 (시작 부분)

다음은 원문에 실린 셸 스크립트다. **PDF 추출 과정에서 긴 주석 및 명령행이 여러 줄로 분리된 부분이 있으므로, 실행 가능한 원본 스크립트와 동일하다고 간주해서는 안 된다.** 원문에 등장하는 코드의 순서와 내용을 가능한 한 유지했다.

```sh
#!/bin/sh
#
---------------------------------------------------------------------
--------
# Start/Stop Script for the CATALINA Server
#
# Environment Variable Prequisites
#
# CATALINA_HOME May point at your Catalina "build" directory.
#
# CATALINA_BASE (Optional) Base directory for resolving dynamic
portions
#               of a Catalina installation. If not present,
resolves to
#               the same directory that CATALINA_HOME points to.
#
# CATALINA_OPTS (Optional) Java runtime options used when the
"start",
#               "stop", or "run" command is executed.
#
# CATALINA_TMPDIR (Optional) Directory path location of temporary
directory
#               the JVM should use (java.io.tmpdir). Defaults to
#               $CATALINA_BASE/temp.
#
# JAVA_HOME        Must point at your Java Development Kit
installation.
#
# JAVA_OPTS       (Optional) Java runtime options used when the
"start",
#               "stop", or "run" command is executed.
#
# JPDA_TRANSPORT (Optional) JPDA transport used when the "jpda
start"
#               command is executed. The default is "dt_socket".
#
# JPDA_ADDRESS     (Optional) Java runtime options used when the "jpda
start"
#               command is executed. The default is 8000.
#
# JSSE_HOME       (Optional) May point at your Java Secure Sockets
Extension
#               (JSSE) installation, whose JAR files will be added
to the
#               system class path used to start Tomcat.
#
# CATALINA_PID    (Optional) Path of the file which should contains
the pid
#               of catalina startup Java process, when start (fork)
is used
#
# $Id: catalina.sh,v 1.8 2003/09/02 12:23:13 remm Exp $
#
---------------------------------------------------------------------
--------

# OS specific support. $var _must_ be set to either true or false.
cygwin=false
os400=false
case "`uname`" in
CYGWIN*) cygwin=true;;
OS400*) os400=true;;
esac

# resolve links - $0 may be a softlink
PRG="$0"

while [ -h "$PRG" ]; do
  ls=`ls -ld "$PRG"`
  link=`expr "$ls" : '.*-> \(.*\)$'`
  if expr "$link" : '.*/.*' > /dev/null; then
     PRG="$link"
  else
     PRG=`dirname "$PRG"`/"$link"
  fi
done

# Get standard environment variables
PRGDIR=`dirname "$PRG"`
CATALINA_HOME=`cd "$PRGDIR/.." ; pwd`
if [ -r "$CATALINA_HOME"/bin/setenv.sh ]; then
    . "$CATALINA_HOME"/bin/setenv.sh
fi

# For Cygwin, ensure paths are in UNIX format before anything is
touched
if $cygwin; then
   [ -n "$JAVA_HOME" ] && JAVA_HOME=`cygpath --unix "$JAVA_HOME"`
   [ -n "$CATALINA_HOME" ] && CATALINA_HOME=`cygpath --unix
"$CATALINA_HOME"`
   [ -n "$CATALINA_BASE" ] && CATALINA_BASE=`cygpath --unix
"$CATALINA_BASE"`
   [ -n "$CLASSPATH" ] && CLASSPATH=`cygpath --path --unix
"$CLASSPATH"`
     [ -n "$JSSE_HOME" ] && JSSE_HOME=`cygpath --path --unix "$JSSE_HOME"`
fi

# For OS400
if $os400; then
   # Set job priority to standard for interactive (interactive - 6) by
using
   # the interactive priority - 6, the helper threads that respond to
requests
   # will be running at the same priority as interactive jobs.
   COMMAND='chgjob job('$JOBNAME') runpty(6)'
   system $COMMAND

     # Enable multi threading
     export QIBM_MULTI_THREADED=Y
fi

# Get standard Java environment variables
if [ -r "$CATALINA_HOME"/bin/setclasspath.sh ]; then
    BASEDIR="$CATALINA_HOME"
    . "$CATALINA_HOME"/bin/setclasspath.sh
else
    echo "Cannot find $CATALINA_HOME/bin/setclasspath.sh"
    echo "This file is needed to run this program"
    exit 1
fi

# Add on extra jar files to CLASSPATH
if [ -n "$JSSE_HOME" ]; then

CLASSPATH="$CLASSPATH":"$JSSE_HOME"/lib/jcert.jar:"$JSSE_HOME"/lib/j
net
.jar:"$JSSE_HOME"/lib/jsse.jar
fi
CLASSPATH="$CLASSPATH":"$CATALINA_HOME"/bin/bootstrap.jar

if [ -z "$CATALINA_BASE" ] ; then
    CATALINA_BASE="$CATALINA_HOME"
fi

if [ -z "$CATALINA_TMPDIR" ] ; then
    # Define the java.io.tmpdir to use for Catalina
    CATALINA_TMPDIR="$CATALINA_BASE"/temp
fi
```

**스크립트 흐름:** 먼저 `uname`으로 Cygwin과 OS400을 구별한다. 다음으로 `$0`이 심볼릭 링크인지 확인하고 실제 스크립트 경로를 추적한다. 그 경로를 기준으로 `CATALINA_HOME`을 계산한 뒤, `setenv.sh`가 있으면 읽는다. Cygwin에서는 경로를 Unix 형식으로 바꾸고, OS400에서는 작업 우선순위와 멀티스레딩 환경을 설정한다. 이어서 `setclasspath.sh`를 읽어 Java 환경을 구성하고, 필요하다면 JSSE 라이브러리와 `bootstrap.jar`를 클래스 경로에 추가한다. 마지막으로 설정되지 않은 `CATALINA_BASE`와 `CATALINA_TMPDIR`의 기본값을 지정한다.

> **다음 구간:** 원본 PDF 399페이지부터 리스팅 17.12의 나머지 스크립트가 이어진다.


## 리스팅 17.12 — `catalina.sh`의 나머지 부분 (PDF 399~403쪽)

Cygwin에서 Java를 실행하기 전에는 `cygpath`를 이용해 `JAVA_HOME`, `CATALINA_HOME`, `CATALINA_BASE`, 임시 디렉터리 및 클래스 경로를 Windows 경로로 바꾼다. 그런 다음 실제 실행 명령을 선택한다.

`jpda` 명령을 지정하면 기본 전송 방식은 `dt_socket`, 기본 포트는 `8000`으로 설정된다. `debug`는 Java 디버거를 사용하고, `embedded`는 임베디드 모드의 `Embedded` 클래스를 실행한다. `run`은 현재 셸에서 서버를 실행하고, `start`는 백그라운드에서 실행하면서 출력을 `logs/catalina.out`에 기록한다. PID 파일 경로가 설정되어 있으면 백그라운드 프로세스의 PID도 기록한다. `stop`은 `Bootstrap`에 `stop` 인수를 전달하며, `-force`를 사용하면 PID 파일에 적힌 프로세스를 강제 종료할 수 있다. 지원하지 않는 명령에는 사용법을 출력한다.

아래는 해당 구간의 원문 셸 코드다. **PDF 텍스트 추출로 긴 줄이 잘린 곳이 있으므로 그대로 실행하지 말고 원본 PDF와 대조해야 한다.**

```bash
# For Cygwin, switch paths to Windows format before running java
if $cygwin; then
   JAVA_HOME=`cygpath --path --windows "$JAVA_HOME"`

     CATALINA_HOME=`cygpath --path --windows "$CATALINA_HOME"`
     CATALINA_BASE=`cygpath --path --windows "$CATALINA_BASE"`
     CATALINA_TMPDIR=`cygpath --path --windows "$CATALINA_TMPDIR"`
     CLASSPATH=`cygpath --path --windows "$CLASSPATH"`
     JSSE_HOME=`cygpath --path --windows "$JSSE_HOME"`
fi

# ----- Execute The Requested Command ---------------------------------
--------

echo "Using CATALINA_BASE: $CATALINA_BASE"
echo "Using CATALINA_HOME: $CATALINA_HOME"
echo "Using CATALINA_TMPDIR: $CATALINA_TMPDIR"
echo "Using JAVA_HOME:       $JAVA_HOME"

if [ "$1" = "jpda" ] ; then
    if [ -z "$JPDA_TRANSPORT" ]; then
        JPDA_TRANSPORT="dt_socket"
    fi
    if [ -z "$JPDA_ADDRESS" ]; then
        JPDA_ADDRESS="8000"
    fi
    if [ -z "$JPDA_OPTS" ]; then
        JPDA_OPTS="-Xdebug -
Xrunjdwp:transport=$JPDA_TRANSPORT,address=$JPDA_ADDRESS,server=y,susp
e
nd=n"
    fi
    CATALINA_OPTS="$CATALINA_OPTS $JPDA_OPTS"
    shift
fi

if [ "$1" = "debug" ] ; then

     if $os400; then
        echo "Debug command not available on OS400"
        exit 1
     else
        shift

    if [ "$1" = "-security" ] ; then
        echo "Using Security Manager"
        shift
        exec "$_RUNJDB" $JAVA_OPTS $CATALINA_OPTS \
          -Djava.endorsed.dirs="$JAVA_ENDORSED_DIRS" -classpath
"$CLASSPATH" \
          -sourcepath "$CATALINA_HOME"/../../jakarta-tomcat-
4.0/catalina/src/share \
          -Djava.security.manager \
          -Djava.security.policy=="$CATALINA_BASE"/conf/catalina.policy \
          -Dcatalina.base="$CATALINA_BASE" \
          -Dcatalina.home="$CATALINA_HOME" \
          -Djava.io.tmpdir="$CATALINA_TMPDIR" \
          org.apache.catalina.startup.Bootstrap "$@" start
    else
        exec "$_RUNJDB" $JAVA_OPTS $CATALINA_OPTS \
          -Djava.endorsed.dirs="$JAVA_ENDORSED_DIRS" -classpath
"$CLASSPATH" \

          -sourcepath "$CATALINA_HOME"/../../jakarta-tomcat-
4.0/catalina/src/share \
          -Dcatalina.base="$CATALINA_BASE" \
          -Dcatalina.home="$CATALINA_HOME" \
          -Djava.io.tmpdir="$CATALINA_TMPDIR" \
          org.apache.catalina.startup.Bootstrap "$@" start
     fi
  fi

elif [ "$1" = "embedded" ] ; then

    shift
    echo "Embedded Classpath: $CLASSPATH"
    exec "$_RUNJAVA" $JAVA_OPTS $CATALINA_OPTS \
      -Djava.endorsed.dirs="$JAVA_ENDORSED_DIRS" -classpath "$CLASSPATH"
\
      -Dcatalina.base="$CATALINA_BASE" \
      -Dcatalina.home="$CATALINA_HOME" \
      -Djava.io.tmpdir="$CATALINA_TMPDIR" \
      org.apache.catalina.startup.Embedded "$@"

elif [ "$1" = "run" ]; then

    shift
    if [ "$1" = "-security" ] ; then

     echo "Using Security Manager"
     shift
     exec "$_RUNJAVA" $JAVA_OPTS $CATALINA_OPTS \
       -Djava.endorsed.dirs="$JAVA_ENDORSED_DIRS" -classpath
"$CLASSPATH" \
       -Djava.security.manager \
       -Djava.security.policy=="$CATALINA_BASE"/conf/catalina.policy \
       -Dcatalina.base="$CATALINA_BASE" \
       -Dcatalina.home="$CATALINA_HOME" \
       -Djava.io.tmpdir="$CATALINA_TMPDIR" \
       org.apache.catalina.startup.Bootstrap "$@" start
  else
     exec "$_RUNJAVA" $JAVA_OPTS $CATALINA_OPTS \
       -Djava.endorsed.dirs="$JAVA_ENDORSED_DIRS" -classpath
"$CLASSPATH" \
       -Dcatalina.base="$CATALINA_BASE" \
       -Dcatalina.home="$CATALINA_HOME" \
       -Djava.io.tmpdir="$CATALINA_TMPDIR" \
       org.apache.catalina.startup.Bootstrap "$@" start
  fi

elif [ "$1" = "start" ] ; then

  shift
  touch "$CATALINA_BASE"/logs/catalina.out
  if [ "$1" = "-security" ] ; then
      echo "Using Security Manager"
      shift
      "$_RUNJAVA" $JAVA_OPTS $CATALINA_OPTS \
        -Djava.endorsed.dirs="$JAVA_ENDORSED_DIRS" -classpath
"$CLASSPATH" \

         -Djava.security.manager \
         -Djava.security.policy=="$CATALINA_BASE"/conf/catalina.policy \
         -Dcatalina.base="$CATALINA_BASE" \
         -Dcatalina.home="$CATALINA_HOME" \
         -Djava.io.tmpdir="$CATALINA_TMPDIR" \
         org.apache.catalina.startup.Bootstrap "$@" start \
         >> "$CATALINA_BASE"/logs/catalina.out 2>&1 &

         if [ ! -z "$CATALINA_PID" ]; then
             echo $! > $CATALINA_PID
         fi
  else

    "$_RUNJAVA" $JAVA_OPTS $CATALINA_OPTS \
      -Djava.endorsed.dirs="$JAVA_ENDORSED_DIRS" -classpath
"$CLASSPATH" \
      -Dcatalina.base="$CATALINA_BASE" \
      -Dcatalina.home="$CATALINA_HOME" \
      -Djava.io.tmpdir="$CATALINA_TMPDIR" \
      org.apache.catalina.startup.Bootstrap "$@" start \
      >> "$CATALINA_BASE"/logs/catalina.out 2>&1 &

           if [ ! -z "$CATALINA_PID" ]; then
               echo $! > $CATALINA_PID
           fi
    fi

elif [ "$1" = "stop" ] ; then

    shift
    "$_RUNJAVA" $JAVA_OPTS $CATALINA_OPTS \
      -Djava.endorsed.dirs="$JAVA_ENDORSED_DIRS" -clasapath "$CLASSPATH"
\
         -Dcatalina.base="$CATALINA_BASE" \
         -Dcatalina.home="$CATALINA_HOME" \
         -Djava.io.tmpdir="$CATALINA_TMPDIR" \
         org.apache.catalina.startup.Bootstrap "$@" stop

    if [ "$1" = "-force" ] ; then
        shift
        if [ ! -z "$CATALINA_PID" ]; then
             echo "Killing: `cat $CATALINA_PID`"
             kill -9 `cat $CATALINA_PID`
        fi
    fi

else

  echo "Usage: catalina.sh ( commands ... )"
  echo "commands:"
  if $os400; then
     echo " debug                Start Catalina in a debugger (not
available on OS400)"
     echo " debug -security Debug Catalina with a security manager
(not available on OS400)"
  else
     echo " debug                Start Catalina in a debugger"

     echo " debug -security             Debug Catalina with a security manager"
   fi
   echo "    embedded                   Start Catalina in embedded mode"
   echo "    jpda start              Start Catalina under JPDA debugger"
   echo "    run                       Start Catalina in the current window"
   echo "    run -security           Start in the current window with security
manager"
   echo "    start                     Start Catalina in a separate window"
   echo "    start -security         Start in a separate window with security
manager"
   echo "    stop                      Stop Catalina"
   exit 1
fi
```


## Linux/Unix에서 Tomcat 시작하기 (PDF 403~404쪽)

Tomcat을 더 간편하게 시작하려면 `startup.sh`를 사용한다. 이 스크립트는 실행 환경을 설정한 뒤 `catalina.sh`에 `start` 인수를 전달한다. 리스팅 17.13은 `startup.sh`를 보여 준다. 스크립트는 먼저 심볼릭 링크를 따라 실제 파일 위치를 찾고, 같은 디렉터리에 실행 가능한 `catalina.sh`가 있는지 검사한 다음 `exec`로 실행한다.

```bash
#!/bin/sh
#
---------------------------------------------------------------------
-
--------
# Start Script for the CATALINA Server
#
# $Id: startup.sh,v 1.3 2002/08/04 18:19:43 patrickl Exp $
#
---------------------------------------------------------------------
--------

# resolve links - $0 may be a softlink
PRG="$0"

while [ -h "$PRG" ] ; do
  ls=`ls -ld "$PRG"`
  link=`expr "$ls" : '.*-> \(.*\)$'`
  if expr "$link" : '.*/.*' > /dev/null; then
     PRG="$link"
  else
     PRG=`dirname "$PRG"`/"$link"
  fi
done

PRGDIR=`dirname "$PRG"`
EXECUTABLE=catalina.sh

# Check that target executable exists
if [ ! -x "$PRGDIR"/"$EXECUTABLE" ]; then
    echo "Cannot find $PRGDIR/$EXECUTABLE"
    echo "This file is needed to run this program"
    exit 1
fi

exec "$PRGDIR"/"$EXECUTABLE" start "$@"
```


## Linux/Unix에서 Tomcat 종료하기 (PDF 404~405쪽)

`shutdown.sh`를 실행하면 Tomcat을 종료할 수 있다. 이 스크립트는 `catalina.sh`를 호출하면서 `stop`을 인수로 전달한다. 리스팅 17.14의 종료 스크립트도 심볼릭 링크를 해석하고 대상 실행 파일을 확인하는 점에서 시작 스크립트와 거의 동일하다.

```bash
#!/bin/sh
#
---------------------------------------------------------------------
# Stop script for the CATALINA Server
#
# $Id: shutdown.sh,v 1.3 2002/08/04 18:19:43 patrickl Exp $
#
---------------------------------------------------------------------
# resolve links - $0 may be a softlink
PRG="$0"

while [ -h "$PRG" ] ; do
  ls=`ls -ld "$PRG"`
  link=`expr "$ls" : '.*-> \(.*\)$'`
  if expr "$link" : '.*/.*' > /dev/null; then
     PRG="$link"
  else
     PRG=`dirname "$PRG"`/"$link"
  fi
done
PRGDIR=`dirname "$PRG"`
EXECUTABLE=catalina.sh

# Check that target executable exists
if [ ! -x "$PRGDIR"/"$EXECUTABLE" ]; then
    echo "Cannot find $PRGDIR/$EXECUTABLE"
    echo "This file is needed to run this program"
    exit 1
fi
exec "$PRGDIR"/"$EXECUTABLE" stop "S@"
```


## 17장 요약 (PDF 405쪽)

이 장에서는 애플리케이션을 시작하는 두 클래스인 `org.apache.catalina.startup` 패키지의 `Catalina`와 `Bootstrap`을 설명했다. 또한 Windows와 Unix/Linux에서 Tomcat을 쉽게 시작하고 종료하도록 해 주는 배치 파일 및 셸 스크립트도 살펴보았다.

> 원문 주의: 리스팅 17.14의 마지막 인수는 PDF에 `"S@"`로 인쇄되어 있다. 일반적인 인수 전달 표기인 `"$@"`와 다르지만 위 코드는 원문을 유지했다.
