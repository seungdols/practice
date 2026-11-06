# 18장 — Deployer (배포 관리자)

> 원본 PDF 406~413페이지 번역. 이 장은 이후 페이지에서 계속된다.

## 개요

웹 애플리케이션을 사용할 수 있으려면 이를 나타내는 컨텍스트를 먼저 호스트에 배포해야 한다. Tomcat에서는 WAR 파일로 배포하거나, 애플리케이션 전체를 Tomcat 설치 디렉터리 아래 `webapps` 디렉터리에 복사할 수 있다. 각 애플리케이션에는 선택적으로 컨텍스트 구성 설정을 담은 XML 형식의 디스크립터 파일을 둘 수 있다.

**참고:** Tomcat 4와 5에는 Tomcat 및 배포된 애플리케이션을 관리하는 `manager`, `admin` 애플리케이션이 포함된다. 두 애플리케이션의 클래스 파일은 `%CATALINA_HOME%/server/webapps` 아래에 있다. 디스크립터 파일은 각각 `manager.xml`, `admin.xml`이다. Tomcat 4에서는 이 디스크립터가 `%CATALINA_HOME%/webapps`에 위치하지만, Tomcat 5에서는 각 애플리케이션의 해당 디렉터리, 즉 `%CATALINA_HOME%/server/webapps/admin` 및 `%CATALINA_HOME%/server/webapps/manager`에 위치한다.

이 장에서는 `org.apache.catalina.Deployer` 인터페이스로 표현되는 배포 관리자를 사용해 웹 애플리케이션을 배포하는 방법을 설명한다. 배포 관리자는 호스트에 연결되어 자식 컨텍스트를 설치한다. 컨텍스트를 설치한다는 것은 `StandardContext` 인스턴스를 만들고 이를 호스트의 자식으로 추가한다는 뜻이다. 부모 호스트가 시작되면 자식 컨텍스트도 시작된다. 이는 `Wrapper`를 제외한 컨테이너의 `start()` 메서드가 자식 컨테이너의 `start()`를 호출하기 때문이다. 배포 관리자는 컨텍스트를 개별적으로 시작하거나 중지할 수도 있다.

먼저 Tomcat이 호스트에 웹 애플리케이션을 배포하는 방법을 살펴본 다음, `Deployer` 인터페이스와 표준 구현 클래스인 `org.apache.catalina.core.StandardHostDeployer`를 설명한다.

## 웹 컨텍스트 배포하기

15장에서는 다음과 같이 `StandardHost`를 인스턴스화하고 `Context`를 자식 컨테이너로 추가했다.


```java
context.setPath("/app1");
context.setDocBase("app1");

LifecycleListener listener = new ContextConfig();
((Lifecycle) context).addLifecycleListener(listener);

Host host = new StandardHost();
host.addChild(context);
```

이것이 앞서 사용한 애플리케이션 배포 방식이다. 하지만 실제 Tomcat에는 이런 코드가 그대로 존재하지 않는다. 그렇다면 실제 배포에서는 어떻게 컨텍스트가 호스트에 추가될까? 답은 `StandardHost` 인스턴스에 등록된 `org.apache.catalina.startup.HostConfig` 생명주기 리스너에 있다.

`StandardHost.start()`가 호출되면 `START` 이벤트가 발생한다. `HostConfig`는 자신의 `start()` 메서드를 호출해 이에 반응하며, 지정된 디렉터리에서 발견한 모든 웹 애플리케이션을 배포하고 설치한다.

15장에서 `Digester`로 XML을 파싱하는 방법을 배웠지만 모든 규칙을 살펴보지는 않았다. 특히 이 장의 주제인 배포 관리자 관련 규칙을 다루지 않았다. `org.apache.catalina.startup.Catalina`는 `Digester`를 사용해 `server.xml` 요소를 Java 객체로 변환한다. 이 클래스의 `createStartDigester()`에는 다음 코드가 있다.


```java
digester.addRuleSet(new HostRuleSet("Server/Service/Engine/"));
```

`org.apache.catalina.startup.HostRuleSet`은 15장에서 소개한 `RuleSetBase`를 상속한다. 따라서 규칙을 정의하는 `addRuleInstances()`를 구현해야 한다. 다음은 그 메서드의 일부다.


```java
  digester.addObjectCreate(prefix + "Host",

     "org.apache.catalina.core.StandardHost", "className");
  digester.addSetProperties(prefix + "Host");
  digester.addRule(prefix + "Host",
     new CopyParentClassLoaderRule(digester));
  digester.addRule(prefix + "Host",
     new LifecycleListenerRule (digester,
     "org.apache.catalina.startup.HostConfig", "hostConfigClass"));
```

이 코드는 `server.xml`에 `Server/Service/Engine/Host` 패턴이 나타나면 `org.apache.catalina.startup.HostConfig` 인스턴스를 만들고 이를 호스트의 생명주기 리스너로 추가한다는 뜻이다. 따라서 `HostConfig`는 `StandardHost`의 시작과 중지 메서드가 발생시키는 이벤트를 처리한다.

리스팅 18.1은 이벤트 처리 메서드인 `HostConfig.lifecycleEvent()`를 보여 준다. `HostConfig`는 `StandardHost`의 리스너이므로 호스트가 시작되거나 중지될 때마다 이 메서드가 호출된다.

### 리스팅 18.1 — `HostConfig.lifecycleEvent()`


```java
  // Identify the host we are associated with
  try {
     host = (Host) event.getLifecycle();
     if (host instanceof StandardHost) {
         int hostDebug = ((StandardHost) host).getDebug();
         if (hostDebug > this.debug) {
             this.debug = hostDebug;
         }
         setDeployXML(((StandardHost) host).isDeployXML());
         setLiveDeploy(((StandardHost) host).getLiveDeploy());
         setUnpackWARs(((StandardHost) host).isUnpackWARs());
     }
  }
  catch (ClassCastException e) {
     log(sm.getString("hostConfig.cce", event.getLifecycle()), e);
     return;
  }

    // Process the event that has occurred
    if (event.getType().equals(Lifecycle.START_EVENT))
        start ();
    else if (event.getType().equals(Lifecycle.STOP_EVENT))
        stop();
}
```

호스트가 `StandardHost` 인스턴스라면 `setDeployXML()`, `setLiveDeploy()`, `setUnpackWARs()`가 호출된다.


```java
setDeployXML(((StandardHost) host).isDeployXML());
setLiveDeploy(((StandardHost) host).getLiveDeploy());
setUnpackWARs(((StandardHost) host).isUnpackWARs());
```

`StandardHost.isDeployXML()`은 컨텍스트 디스크립터 파일을 배포할지 나타낸다. `deployXML`의 기본값은 `true`다. `liveDeploy`는 새로운 배포 대상을 주기적으로 확인할지 결정하며, `unpackWARs`는 WAR로 배포된 애플리케이션의 압축을 풀지 결정한다.

`START` 이벤트를 받으면 `HostConfig.lifecycleEvent()`는 애플리케이션을 배포하기 위해 `start()`를 호출한다.

### 리스팅 18.2 — `HostConfig.start()`


```java
  if (debug >= 1)
      log(sm.getString("hostConfig.start"));
  if (host.getAutoDeploy()) {
      deployApps();
  }
  if (isLiveDeploy ()) {
      threadStart();
  }
}
```

`autoDeploy`가 `true`이면 `deployApps()`를 호출한다. 이 속성의 기본값은 `true`다. 또한 `liveDeploy`가 `true`이면 `threadStart()`로 새 스레드를 시작한다. 이 속성 역시 기본값이 `true`다. 라이브 배포는 이 절의 뒷부분에서 더 설명한다.

`deployApps()`는 호스트의 `appBase` 속성을 얻는다. 기본값은 `webapps`다(`server.xml` 참조). 배포 과정에서는 `%CATALINA_HOME%/webapps` 아래 모든 디렉터리를 배포 대상 애플리케이션 디렉터리로 간주한다. 이 디렉터리에 있는 WAR 파일과 디스크립터 파일도 배포 대상이다.

### 리스팅 18.3 — `deployApps()`


```java
  if (!(host instanceof Deployer))

        return;
    if (debug >= 1)
        log(sm.getString("hostConfig.deploying"));
    File appBase = appBase();
    if (!appBase.exists() || !appBase.isDirectory())

      return;
    String files[] = appBase.list();
    deployDescriptors(appBase, files);
    deployWARs(appBase, files);
    deployDirectories(appBase, files);
}
```

`deployApps()`는 `deployDescriptors()`, `deployWARs()`, `deployDirectories()`를 차례로 호출한다. 세 메서드에는 `appBase`를 나타내는 `File`과 `webapps` 안의 파일 이름 배열이 전달된다. 컨텍스트는 경로로 식별되며, 배포된 모든 컨텍스트의 경로는 고유해야 한다. 배포된 컨텍스트는 `HostConfig`의 `deployed`라는 `ArrayList`에 추가된다. 따라서 각 배포 메서드는 같은 경로의 컨텍스트가 이미 배포되었는지 먼저 확인한다.

이제 세 가지 배포 방법을 하나씩 살펴본다. 세 메서드의 호출 순서가 중요한가? **그렇다.**

## 디스크립터 배포하기

컨텍스트 객체를 기술하는 XML 파일을 작성할 수 있다. 예를 들어 Tomcat 4 및 5와 함께 제공되는 `admin`과 `manager` 애플리케이션에는 각각 리스팅 18.4와 18.5의 디스크립터가 있다.

### 리스팅 18.4 — `admin.xml`


```xml
<Context path="/admin" docBase="../server/webapps/admin"
  debug="0" privileged="true">
  <!-- Uncomment this Valve to limit access to the Admin app to
   localhost for obvious security reasons. Allow may be a comma-
   separated list of hosts (or even regular expressions).
  <Valve className="org.apache.catalina.valves.RemoteAddrValve"
     allow="127.0.0.1"/>
  -->

  <Logger className="org.apache.catalina.logger.FileLogger"
      prefix="localhost_admin_log." suffix=".txt"
    timestamp="true"/>
</Context>
```

### 리스팅 18.5 — `manager.xml`


```xml
<Context path="/manager" docBase="../server/webapps/manager"
  debug="0" privileged="true">
  <!-- Link to the user database we will get roles from -->
  <ResourceLink name="users" global="UserDatabase"
     type="org.apache.catalina.UserDatabase"/>

</Context>
```

두 디스크립터 모두 `Context` 요소를 갖는다. `docBase` 속성은 각각 `%CATALINA_HOME%/server/webapps/admin` 및 `%CATALINA_HOME%/server/webapps/manager`를 가리킨다. 즉, 이 두 애플리케이션은 일반적인 배포 위치에 놓이지 않는다.

`HostConfig`는 리스팅 18.6의 `deployDescriptors()`를 사용해 XML 파일을 배포한다. Tomcat 4에서는 `%CATALINA_HOME%/webapps`에 있는 XML 파일을, Tomcat 5에서는 `%CATALINA_HOME%/server/webapps/`의 하위 디렉터리에 있는 XML 파일을 대상으로 한다.

### 리스팅 18.6 — `HostConfig.deployDescriptors()`


```java
  if (!deployXML)
      return;

  for (int i = 0; i < files.length; i++) {
     if (files[i].equalsIgnoreCase("META-INF"))
         continue;
     if (files[i].equalsIgnoreCase("WEB-INF"))
         continue;
     if (deployed.contains(files[i]))
         continue;
     File dir = new File(appBase, files[i]);
     if (files[i].toLowerCase().endsWith(".xml")) {
         deployed.add(files[i]);

        // Calculate the context path and make sure it is unique

            String file = files[i].substring(0, files[i].length() - 4);
            String contextPath = "/" + file;
            if (file.equals("ROOT")) {
                contextPath = "";
            }
            if (host.findChild(contextPath) != null) {
                continue;
            }

            // Assume this is a configuration descriptor and deploy it
            log(sm.getString("hostConfig.deployDescriptor", files[i]));
            try {
               URL config =
                  new URL("file", null, dir.getCanonicalPath());
                  ((Deployer) host).install(config, null);
            }
            catch (Throwable t) {
               log(sm.getString("hostConfig.deployDescriptor.error",
                  files[i]), t);
            }
        }
    }
}
```

이 메서드는 `deployXML`이 거짓이면 즉시 반환한다. `META-INF`, `WEB-INF`, 이미 배포된 항목을 건너뛴다. `.xml` 파일을 발견하면 파일 이름으로 컨텍스트 경로를 계산한다. `ROOT.xml`은 빈 경로에 대응한다. 같은 경로의 자식 컨테이너가 없으면 디스크립터 URL을 생성하고 `Deployer.install()`을 호출한다. 오류가 발생하면 로그에 기록한다.

## WAR 파일 배포하기

웹 애플리케이션은 WAR 파일 형태로 배포할 수도 있다. `HostConfig`는 리스팅 18.7의 `deployWARs()`를 사용해 `%CATALINA_HOME%/webapps`의 WAR 파일을 배포한다.

### 리스팅 18.7 — `HostConfig.deployWARs()` (413페이지까지)


```java
protected void deployWARs(File appBase, String[] files) {
  for (int i = 0; i < files.length; i++) {
     if (files[i].equalsIgnoreCase("META-INF"))
         continue;
     if (files[i].equalsIgnoreCase("WEB-INF"))
         continue;
     if (deployed.contains(files [i]))
         continue;
     File dir = new File(appBase, files [i]);

    if (files[i].toLowerCase().endsWith(".war")) {
        deployed.add(files [i]);
        // Calculate the context path and make sure it is unique
        String contextPath = "/" + files[i];
        int period = contextPath.lastIndexOf(".");
        if (period >= 0)
        contextPath = contextPath.substring(0, period);
        if (contextPath.equals("/ROOT"))
            contextPath = "";
        if (host.findChild(contextPath) != null)
            continue;

        if (isUnpackWARs()) {
            // Expand and deploy this application as a directory
            log(sm.getString("hostConfig.expand", files[i]));
            try {
               URL url = new URL("jar:file:" +
                  dir.getCanonicalPath() + "!/");
               String path = expand(url);
               url = new URL("file:" + path);
               ((Deployer) host).install(contextPath, url);
            }
            catch (Throwable t) {
               log(sm.getString("hostConfig.expand.error", files[i]), t);
            }
        }
        else {
            // Deploy the application in this WAR file
            log(sm.getString("hostConfig.deployJar", files[i]));
            try {
               URL url = new URL("file", null,
                   dir.getCanonicalPath());
               url = new URL("jar:" + url.toString() + "!/");
               ((Deployer) host).install(contextPath, url);
            }
            catch (Throwable t) {
               log(sm.getString("hostConfig.deployJar.error",

                files[i]), t);
            }
        }
    }
}
```

`deployWARs()`는 이미 배포된 파일을 건너뛰고, WAR 파일 이름에서 `.war` 확장자를 제거해 컨텍스트 경로를 계산한다. `ROOT.war`의 경로는 빈 문자열이 된다. 동일한 경로의 컨텍스트가 이미 존재하면 건너뛴다. `unpackWARs`가 참이면 WAR를 풀어 디렉터리 URL로 설치하고, 거짓이면 `jar:` URL을 사용해 WAR 파일 자체를 설치한다.

> **계속:** 리스팅 18.7과 WAR 배포 설명은 원본 PDF 414페이지 이후에서 이어진다. PDF 추출 과정에서 코드 줄바꿈이 바뀌었을 수 있으므로 실제 실행 시 원본을 대조해야 한다.


---

## 원본 PDF 414~418페이지 번역

### 디렉터리 배포 (Deploying a Directory)

다른 방법으로, 애플리케이션 디렉터리 전체를 `%CATALINA_HOME%/webapps` 디렉터리에 복사하여 그대로 배포할 수 있다. `HostConfig`는 이러한 애플리케이션을 배포하기 위해 리스팅 18.8의 `deployDirectories` 메서드를 사용한다.

**리스팅 18.8: `HostConfig`의 `deployDirectories` 메서드**

```java
protected void deployDirectories (File appBase, string[] files){
  for (int i = 0; i < files.length; i++) {
     if (files[i].equalsIgnoreCase("META-INF"))
         continue;
     if (files[i].equalsIgnoreCase("WEB-INF"))
         continue;
     if (deployed.contains(files[i]))
         continue;
     File dir = new File(appBase, files[i]);
     if (dir.isDirectory()) {
         deployed.add(files[i]);

       // Make sure there is an application configuration directory
       // This is needed if the Context appBase is the same as the
       // web server document root to make sure only web applications
       // are deployed and not directories for web space.
       File webInf = new File(dir, "/WEB-INF");
       if (!webInf.exists() || !webInf.isDirectory() ||
           !webInf.canRead())
           continue;

       // Calculate the context path and make sure it is unique
       String contextPath = "/" + files[i];
       if (files[i].equals("ROOT"))
           contextPath = "";
       if (host.findChild(contextPath) != null)
           continue;

       // Deploy the application in this directory
       log(sm.getString("hostConfig.deployDir", files[i]));
       try {
          URL url = new URL("file", null, dir.getCanonicalPath());
              ((Deployer) host).install(contextPath, url);
            }
            catch (Throwable t) {
              log(sm.getString("hostConfig.deployDir.error", files[i]), t);
            }
        }

    }
}
```

### 실시간 배포 (Live Deploy)

앞서 설명했듯이 `StandardHost` 인스턴스는 `HostConfig` 객체를 생명주기 리스너로 사용한다. `StandardHost` 객체가 시작되면 `start` 메서드가 `START` 이벤트를 발생시킨다. 이에 대응하여 이벤트 처리기인 `HostConfig.lifecycleEvent()`가 `start()` 메서드를 호출한다. Tomcat 4에서는 `liveDeploy` 속성이 `true`이면 이 `start()` 메서드의 마지막 부분에서 `threadStart()`를 호출한다. 이 속성의 기본값은 `true`다.

```java
    if (isLiveDeploy()) {
        threadStart();
    }
```

`threadStart()`는 새로운 스레드를 생성하여 `run()` 메서드를 실행한다. `run()`은 새로운 배포 대상이 있는지, 그리고 이미 배포된 애플리케이션의 `web.xml` 파일이 수정되었는지 주기적으로 검사한다. 리스팅 18.9에 `run()` 메서드를 제시한다.

**리스팅 18.9: Tomcat 4의 `HostConfig.run()` 메서드**

```java
/**
 * The background thread that checks for web application autoDeploy
 * and changes to the web.xml config.
 */
public void run() {
  if (debug >= 1)
      log("BACKGROUND THREAD Starting");
  // Loop until the termination semaphore is set
  while (!threadDone) {
      // Wait for our check interval
      threadSleep();
      // Deploy apps if the Host allows auto deploying
      deployApps();
      // Check for web.xml modification
      checkWebXmlLastModified();
    }
    if (debug >= 1)
        log("BACKGROUND THREAD Stopping");
}
```

`threadSleep()` 메서드는 `checkInterval` 속성으로 지정된 시간만큼 스레드를 잠들게 한다. 기본값은 15이며, 따라서 검사는 15초마다 수행된다.

Tomcat 5에서는 `HostConfig` 클래스가 별도의 전용 스레드를 사용하지 않는다. 대신 `StandardHost` 클래스의 `backgroundProcess()` 메서드가 주기적으로 `"check"` 이벤트를 발생시킨다.

```java
lifecycle.fireLifecycleEvent("check", null);
}
```

**참고:** `backgroundProcess()`는 컨테이너의 모든 백그라운드 처리를 담당하는 특수 스레드에 의해 주기적으로 호출된다.

`"check"` 이벤트를 수신하면 생명주기 리스너인 `HostConfig` 객체는 검사 작업을 수행하는 `check()` 메서드를 호출한다.

```java
if (event.getType().equals("check"))
      check();
  ...
```

Tomcat 5의 `HostConfig.check()` 메서드는 리스팅 18.10과 같다.

**리스팅 18.10: Tomcat 5의 `HostConfig.check()` 메서드**

```java
protected void check() {
  if (host.getAutoDeploy()) {
      // Deploy apps if the Host allows auto deploying
      deployApps();
      // Check for web.xml modification
      checkContextLastModified();
  }
}
```

보는 바와 같이 `check()`는 `deployApps()`를 호출한다. Tomcat 4와 Tomcat 5의 `deployApps()`는 웹 애플리케이션을 배포하며, 앞서 리스팅 18.3에 제시했다. 설명한 것처럼 이 메서드는 `deployDescriptors()`, `deployWARs()`, `deployDirectories()`를 호출한다.

Tomcat 5의 `check()`는 `checkContextLastModified()`도 호출한다. 이 메서드는 배포된 모든 컨텍스트를 순회하면서 각 컨텍스트의 `web.xml` 파일과 `WEB-INF` 디렉터리 내용에 대한 타임스탬프를 검사한다. 검사 대상 리소스 중 하나라도 변경되면 해당 컨텍스트를 재시작한다. 또한 배포된 모든 WAR 파일의 타임스탬프를 확인하고, WAR 파일이 수정된 애플리케이션은 다시 배포한다.

Tomcat 4에서는 백그라운드 스레드의 `run()` 메서드가 `checkWebXmlLastModified()`를 호출하며, 이 메서드는 Tomcat 5의 `checkContextLastModified()`와 비슷한 작업을 수행한다.

### Deployer 인터페이스

배포기는 `org.apache.catalina.Deployer` 인터페이스로 표현된다. `StandardHost` 클래스는 `Deployer` 인터페이스를 구현한다. 따라서 `StandardHost` 인스턴스는 배포기이기도 하며, 웹 애플리케이션을 배포하거나 배포 해제할 수 있는 컨테이너이기도 하다. `Deployer` 인터페이스는 리스팅 18.11에 제시한다.

**리스팅 18.11: `Deployer` 인터페이스 (418페이지까지의 코드)**

```java
package org.apache.catalina;

import java.io.IOException;
import java.net.URL;

/**
 * A <b>Deployer</b> is a specialized Container into which web
 * applications can be deployed and undeployed. Such a Container
 * will create and install child Context instances for each deployed
 * application. The unique key for each web application will be the
 * context path to which it is attached.
 *
 * @author Craig R. McClanahan
 * @version $Revision: 1.6 $ $Date: 2002/04/09 23:48:21 $
 */
public interface Deployer {
  /**
   * The ContainerEvent event type sent when a new application is
   * being installed by <code>install()</code>, before it has been
   * started.
   */
  public static final String PRE_INSTALL_EVENT = "pre-install";

  /**
   * The ContainerEvent event type sent when a new application is
   * installed by <code>install()</code>, after it has been started.
   */
  public static final String INSTALL_EVENT = "install";

  /**
   * The ContainerEvent event type sent when an existing application is
   * removed by <code>remove()</code>.
   */
  public static final String REMOVE_EVENT = "remove";

  /**

   * Return the name of the Container with which this Deployer is
   * associated.
   */
  public String getName();
  /**
   * Install a new web application, whose web application archive is at
   * the specified URL, into this container with the specified context.
     * path. A context path of "" (the empty string) should be used for
   * the root application for this container. Otherwise, the context
   * path must start with a slash.
   * <p>
   * If this application is successfully installed, a ContainerEvent of
   * type <code>INSTALL_EVENT</code> will be sent to all registered
   * listeners,
   * with the newly created <code>Context</code> as an argument.
   *
   * @param contextPath The context path to which this application
   * should be installed (must be unique)
   * @param war A URL of type "jar:" that points to a WAR file, or type
   * "file:" that points to an unpacked directory structure containing
   * the web application to be installed
   *
```




## 원본 PDF 419~423페이지 번역

### `Deployer` 인터페이스 (계속)

앞 페이지에서 시작된 `install(String contextPath, URL war)` 메서드의 Javadoc은 다음 예외를 설명한다. 컨텍스트 경로의 형식이 잘못되었거나(빈 문자열이 아니면서 `/`로 시작하지 않는 경우) 이미 설치된 웹 애플리케이션에 연결된 경로라면 예외가 발생한다. 설치 과정에서 입출력 오류가 발생할 수도 있다.

다음 오버로드인 `install(URL config, URL war)`는 지정된 URL의 **컨텍스트 구성 파일**과 **웹 애플리케이션 아카이브**를 이용하여 새 웹 애플리케이션을 설치한다. 구성 파일에는 `<Context>` 요소가 포함된다. 설치에 성공하면 새로 생성된 `Context`를 인수로 포함하는 `INSTALL_EVENT` 유형의 `ContainerEvent`가 등록된 모든 리스너에 전달된다. `config`는 컨텍스트 구성 파일의 URL이고, `war`는 WAR 파일을 가리키는 `jar:` URL 또는 압축 해제된 애플리케이션 디렉터리를 가리키는 `file:` URL이다. URL이 `null`이거나 구성 파일에 지정된 컨텍스트 경로가 이미 사용 중이면 예외가 발생한다.

다음은 인터페이스에 남아 있는 메서드 선언이다. 원문 Javadoc 및 Java 코드는 그대로 수록했다.

```java
 * @exception IllegalArgumentException if the specified context path
 * is malformed (it must be "" or start with a slash)
 * @exception IllegalStateException if the specified context path
 * is already attached to an existing web application
 * @exception IOException if an input/output error was encountered
 * during installation
 */
public void install(String contextPath, URL war) throws IOException;

/**
 * <p>Install a new web application, whose context configuration file
 * (consisting of a <code>&lt;Context&gt;</code> element) and web
 * application archive are at the specified URLs.</p>
 *
 * <p>If this application is successfully installed, a ContainerEvent
 * of type <code>INSTALL_EVENT</code> will be sent to all registered
 * listeners, with the newly created <code>Context</code> as an
 * argument.
 * </p>
 *
 * @param config A URL that points to the context configuration file
 * to be used for configuring the new Context
 * @param war A URL of type "jar:" that points to a WAR file, or type
 * "file:" that points to an unpacked directory structure containing
 * the web application to be installed
 *
 * @exception IllegalArgumentException if one of the specified URLs
 * is null
 * @exception IllegalStateException if the context path specified in
 * the context configuration file is already attached to an existing
 * web application
 * @exception IOException if an input/output error was encountered
 * during installation
 */
public void install(URL config, URL war) throws IOException;

/**
 * Return the Context for the deployed application that is associated
 * with the specified context path (if any); otherwise return
 * <code>null</code>.
 *
 * @param contextPath The context path of the requested web
 * application
 */
public Context findDeployedApp(String contextPath);

/**
 * Return the context paths of all deployed web applications in this
 * Container. If there are no deployed applications, a zero-length
 * array is returned.
 */
public String[] findDeployedApps();

/**
 * Remove an existing web application, attached to the specified
 * context path. If this application is successfully removed, a
 * ContainerEvent of type <code>REMOVE_EVENT</code> will be sent to
 * all registered listeners, with the removed <code>Context</code> as
 * an argument.
 *
 * @param contextPath The context path of the application to be
 * removed
 *
 * @exception IllegalArgumentException if the specified context path
 * is malformed (it must be "" or start with a slash)
 * @exception IllegalArgumentException if the specified context path
 * does not identify a currently installed web application
 * @exception IOException if an input/output error occurs during
 * removal
 */
public void remove(String contextPath) throws IOException;

/**
 * Start an existing web application, attached to the specified
 * context path. Only starts a web application if it is not running.
 *
 * @param contextPath The context path of the application to be
 * started
 * @exception IllegalArgumentException if the specified context path
 * is malformed (it must be "" or start with a slash)
 * @exception IllegalArgumentException if the specified context path
 * does not identify a currently installed web application
 * @exception IOException if an input/output error occurs during
 * startup
 */
public void start(String contextPath) throws IOException;

/**
 * Stop an existing web application, attached to the specified
 * context path. Only stops a web application if it is running.
 *
 * @param contextPath The context path of the application to be
 * stopped
 * @exception IllegalArgumentException if the specified context path
 * is malformed (it must be "" or start with a slash)
 * @exception IllegalArgumentException if the specified context path
 * does not identify a currently installed web application
 * @exception IOException if an input/output error occurs while
 * stopping the web application
 */
public void stop(String contextPath) throws IOException;
}
```

`findDeployedApp(String contextPath)`는 지정한 경로에 배포된 애플리케이션의 `Context`를 반환하며, 없으면 `null`을 반환한다. `findDeployedApps()`는 컨테이너에 배포된 모든 애플리케이션의 컨텍스트 경로를 반환하며, 없으면 길이가 0인 배열을 반환한다.

`remove(String contextPath)`는 해당 경로의 웹 애플리케이션을 제거한다. 제거에 성공하면 제거된 `Context`를 인수로 포함하는 `REMOVE_EVENT` 유형의 `ContainerEvent`가 등록된 리스너에 전달된다. `start(String contextPath)`는 아직 실행 중이지 않은 애플리케이션만 시작하며, `stop(String contextPath)`는 실행 중인 애플리케이션만 중지한다. 이 메서드들은 잘못된 경로나 설치되지 않은 애플리케이션에 대해 `IllegalArgumentException`을, 처리 중 입출력 오류에 대해 `IOException`을 발생시킬 수 있다.

### `StandardHost`와 배포 위임

`StandardHost` 클래스는 웹 애플리케이션의 배포와 설치에 관련된 작업을 수행하기 위해 도우미 클래스인 `org.apache.catalina.core.StandardHostDeployer`를 사용한다. 아래 `StandardHost` 코드 일부에서 볼 수 있듯이, 웹 애플리케이션 배포 및 설치 작업을 `StandardHostDeployer` 인스턴스에 위임한다.

```java
/**
 * The <code>Deployer</code> to whom we delegate application
 * deployment requests.
 */
private Deployer deployer = new StandardHostDeployer(this);

public void install(String contextPath, URL war) throws IOException {
  deployer.install(contextPath, war);
}
public synchronized void install(URL config, URL war) throws
IOException {
  deployer.install(config, war);
}
public Context findDeployedApp(String contextPath) {
  return (deployer.findDeployedApp(contextPath));
}
public String[] findDeployedApps() {
  return (deployer.findDeployedApps());
}
public void remove(String contextPath) throws IOException {
  deployer.remove(contextPath);
}
public void start(String contextPath) throws IOException {
  deployer.start(contextPath);
}
public void stop(String contextPath) throws IOException {
  deployer.stop(contextPath);
}
```

`StandardHostDeployer`는 다음 절에서 설명한다.

### `StandardHostDeployer` 클래스

`org.apache.catalina.core.StandardHostDeployer`는 `StandardHost`에서 웹 애플리케이션을 배포하고 설치하는 일을 돕는 클래스다. 이 클래스는 `StandardHost` 객체에서 사용하도록 설계되었다. 그 사실은 `StandardHost` 인스턴스를 인수로 받는 생성자에서도 확인할 수 있다.

```java
public StandardHostDeployer(StandardHost host) {
  super();
  this.host = host;
}
```

이 클래스의 메서드는 다음 소절에서 설명한다.

### 디스크립터 설치 (Installing a Descriptor)

`StandardHostDeployer`에는 `install` 메서드가 두 개 있다. 첫 번째는 이 절에서 설명하는 **디스크립터 설치**용 메서드다. 두 번째는 다음 절에서 설명하며 WAR 파일이나 디렉터리를 설치하는 데 사용한다.

디스크립터를 설치하는 `install` 메서드는 **리스팅 18.12**에 나와 있다. `HostConfig` 객체의 `deployDescriptors` 메서드가 `StandardHost` 객체의 `install` 메서드를 호출하면, `StandardHost`는 이 메서드를 호출한다.

**리스팅 18.12: 디스크립터를 설치하는 `install` 메서드**

```java
public synchronized void install(URL config, URL war)
  throws IOException {
  // Validate the format and state of our arguments
  if (config == null)
    throw new IllegalArgumentException
      (sm.getString("StandardHost.configRequired"));
if (!host.isDeployXML())
    throw new IllegalArgumentException
        (sm.getString("StandardHost.configNotAllowed"));
// Calculate the document base for the new web application (if
// needed)
String docBase = null; // Optional override for value in config file
if (war != null) {
    String url = war.toString();
    host.log(sm.getString("StandardHost.installingWAR", url));
    // Calculate the WAR file absolute pathname
    if (url.startsWith("jar:")) {
        url = url.substring(4, url.length() - 2);
    }
    if (url.startsWith("file://"))
        docBase = url.substring(7);
    else if (url.startsWith("file:"))
       docBase = url.substring(5);
    else
       throw new IllegalArgumentException
         (sm.getString("standardHost.warURL", url));
}

// Install the new web application
this.context = null;
this.overrideDocBase = docBase;
InputStream stream = null;
try {
   stream = config.openStream();
   Digester digester = createDigester();
   digester.setDebug(host.getDebug());
   digester.clear();
   digester.push(this);
   digester.parse(stream);
   stream.close();
   stream = null;
}
catch (Exception e) {
   host.log
      (sm.getString("standardHost.installError", docBase), e);
        throw new IOException(e.toString());
}
```

먼저 `config`가 `null`인지 확인하고, 호스트가 XML 배포를 허용하는지(`host.isDeployXML()`) 검사한다. `war` URL이 전달되면 이를 문자열로 바꾸고 `jar:` 접두부와 `file:` URL 형식을 처리하여 웹 애플리케이션의 문서 기반 경로(`docBase`)를 구한다. 지원하지 않는 URL 형식은 `IllegalArgumentException`을 발생시킨다.

그다음 `context`를 `null`로 초기화하고 `overrideDocBase`를 설정한다. 구성 파일의 입력 스트림을 열어 `Digester`를 생성한 뒤, 디버그 수준을 설정하고 기존 상태를 지운다. `digester.push(this)`로 현재 `StandardHostDeployer` 인스턴스를 스택에 넣고 `digester.parse(stream)`으로 구성 파일을 파싱한다. 정상 처리 시 스트림을 닫으며, 예외가 발생하면 호스트 로그에 오류를 남기고 `IOException`으로 다시 발생시킨다.




## 원본 PDF 424~427페이지

### 리스팅 18.12: XML 디스크립터 설치 메서드의 끝 (424페이지)

앞 페이지에서 시작한 `install(URL config, URL war)` 메서드는 `finally` 블록으로 마무리된다. 파싱 과정에서 예외가 발생하더라도 입력 스트림이 남아 있다면 닫으려고 시도한다. 스트림을 닫는 과정에서 발생한 `Throwable`은 무시한다.

```java
    finally {
       if (stream != null) {
           try {
              stream.close();
           }
           catch (Throwable t) {
              ;
              }
       }
    }
}
```

### WAR 파일과 디렉터리 설치 (424~425페이지)

두 번째 `install` 메서드는 컨텍스트 경로를 나타내는 `String`과 WAR 파일을 나타내는 `URL`을 인수로 받는다. 이 메서드는 리스팅 18.13에 나와 있다.

**리스팅 18.13: WAR 파일 또는 디렉터리를 설치하는 `install` 메서드**

```java
public synchronized void install(String contextPath, URL war)
  throws IOException {
  // Validate the format and state of our arguments
  if (contextPath == null)
      throw new IllegalArgumentException
        (sm.getString("standardHost.pathRequired"));
  if (!contextPath.equals("") && !contextPath.startsWith("/"))
      throw new IllegalArgumentException
           (sm.getString("standardHost.pathFormat", contextPath));
    if (findDeployedApp(contextPath) != null)
        throw new IllegalStateException
           (sm.getString("standardHost.pathUsed", contextPath));
    if (war == null)
        throw new IllegalArgumentException
           (sm.getString("standardHost.warRequired"));

    // Calculate the document base for the new web application
    host.log(sm.getString("standardHost.installing",
    contextPath, war.toString()));
    String url = war.toString();
    String docBase = null;
    if (url.startsWith("jar:")) {
        url = url.substring(4, url.length() - 2);
    }
    if (url.startsWith("file://"))
        docBase = url.substring(7);
    else if (url.startsWith("file:"))
        docBase = url.substring(5);
    else
        throw new IllegalArgumentException
           (sm.getString("standardHost.warURL", url));

    // Install the new web application
    try {
       Class clazz = Class.forName(host.getContextClass());
       Context context = (Context) clazz.newInstance();
       context.setPath(contextPath);

      context.setDocBase(docBase);
      if (context instanceof Lifecycle) {
          clazz = Class.forName(host.getConfigClass());
          LifecycleListener listener =
             (LifecycleListener) clazz.newInstance();
          ((Lifecycle) context).addLifecycleListener(listener);
      }
      host.fireContainerEvent(PRE_INSTALL_EVENT, context);
      host.addChild(context);
      host.fireContainerEvent(INSTALL_EVENT, context);
    }
    catch (Exception e) {
      host.log(sm.getString("standardHost.installError", contextPath),
         e);
      throw new IOException(e.toString());
    }
}
```

컨텍스트가 설치되면 `StandardHost`에 추가된다는 점에 유의하자.

### 컨텍스트 시작 (426페이지)

`StandardHostDeployer`의 `start` 메서드는 컨텍스트를 시작하는 데 사용한다. 리스팅 18.14에 제시되어 있다.

**리스팅 18.14: `StandardHostDeployer` 클래스의 `start` 메서드**

```java
public void start(String contextPath) throws IOException {
  // Validate the format and state of our arguments
  if (contextPath == null)
      throw new IllegalArgumentException
         (sm.getString("standardHost.pathRequired"));
  if (!contextPath.equals("") && !contextPath.startsWith("/"))
      throw new IllegalArgumentException
         (sm.getString("standardHost.pathFormat", contextPath));
  Context context = findDeployedApp(contextPath);
  if (context == null)
        throw new IllegalArgumentException
         (sm.getstring("standardHost.pathMissing", contextPath));
  host.log("standardHost.start " + contextPath);
  try {
      ((Lifecycle) context).start();
  }
  catch (LifecycleException e) {
      host.log("standardHost.start " + contextPath + ": ", e);
         throw new IllegalStateException
      ("standardHost.start " + contextPath + ": " + e);
  }
}
```

### 컨텍스트 중지 (426~427페이지)

컨텍스트를 중지하려면 리스팅 18.15에 나온 `StandardHostDeployer`의 `stop` 메서드를 호출한다.

**리스팅 18.15: `StandardHostDeployer` 클래스의 `stop` 메서드**

```java
public void stop(String contextPath) throws IOException {
  // Validate the format and state of our arguments
  if (contextPath == null)
      throw new IllegalArgumentException
        (sm.getstring("standardHost.pathRequired"));
  if (!contextPath.equals("") && !contextPath.startsWith("/"))
      throw new IllegalArgumentException
        (sm.getstring("standardHost.pathFormat", contextPath));
  Context context = findDeployedApp(contextPath);
  if (context == null)
      throw new IllegalArgumentException
          (sm.getstring("standardHost.pathMissing", contextPath));
    host.log("standardHost.stop " + contextPath);
    try {

      ((Lifecycle) context).stop();
    }
    catch (LifecycleException e) {
      host.log("standardHost.stop " + contextPath + ": ", e);
      throw new IllegalStateException
         ("standardHost.stop " + contextPath + ": " + e);
    }
}
```

> **원문 코드 주의:** 리스팅 18.14와 18.15에 `sm.getstring`이라는 표기가 등장한다. Java는 대소문자를 구분하므로 실제 컴파일 시 `getString`인지 확인해야 한다. 여기서는 원문 표기를 보존했다.

### 요약 (427페이지)

배포기(Deployer)는 웹 애플리케이션을 배포하고 설치하는 컴포넌트이다. 배포기는 `org.apache.catalina.Deployer` 인터페이스로 표현된다. `StandardHost` 클래스는 `Deployer`를 구현하므로 웹 애플리케이션을 배포할 수 있는 특별한 컨테이너가 된다. `StandardHost`는 애플리케이션 배포 및 설치 작업을 도우미 클래스인 `org.apache.catalina.core.StandardHostDeployer`에 위임한다. `StandardHostDeployer`는 애플리케이션 배포와 설치뿐 아니라 컨텍스트 시작과 중지 작업을 수행하는 코드를 제공한다.
