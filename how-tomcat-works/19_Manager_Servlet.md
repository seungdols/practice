# 19장: Manager Servlet

> 원본 PDF 428~432페이지 번역. Java/XML 코드와 기술 식별자는 원문을 유지했다. 원문에 포함된 오탈자는 주석에서 구분한다.

## 개요 (428페이지)

Tomcat 4와 5에는 배포된 애플리케이션을 관리할 수 있는 Manager 애플리케이션이 포함되어 있다. 다른 애플리케이션과 달리 Manager는 기본 배포 디렉터리인 `%CATALINA_HOME%/webapps`가 아니라 `%CATALINA_HOME%/server/webapps`에 위치한다. Manager는 Tomcat 시작 시 설치되는데, Tomcat 4에서는 `%CATALINA_HOME$/webapps` 디렉터리에, Tomcat 5에서는 `%CATALINA_HOME%/server/webapps` 디렉터리에 `manager.xml`이라는 디스크립터가 있기 때문이다.

> **참고:** 컨텍스트 디스크립터는 18장에서 설명했다.

이 장에서는 Manager 애플리케이션을 설명한다. 먼저 Manager가 어떻게 동작하는지 감을 잡을 수 있도록 사용 방법을 간략하게 살펴본다. 그런 다음 `ContainerServlet` 인터페이스를 설명한다.

## Manager 애플리케이션 사용하기 (428~430페이지)

Manager 애플리케이션은 Tomcat 4와 5 모두에서 `%CATALINA_HOME%/server/webapps/manager` 디렉터리에 있다. 이 애플리케이션의 주요 서블릿은 `ManagerServlet`이다. Tomcat 4에서 이 클래스는 Catalina의 패키지 중 하나인 `org.apache.catalina.servlets`에 속한다. Tomcat 5에서는 `org.apache.catalina.manager` 패키지의 일부이며, `WEB-INF/lib` 디렉터리에 JAR 파일로 배포된다.

> **참고:** Tomcat 4의 Manager 애플리케이션은 Tomcat 5의 것보다 약간 단순하므로 학습용으로 더 적합하다. 따라서 이 장에서는 Tomcat 4를 다룬다. 이 장을 읽고 나면 Tomcat 5의 Manager 애플리케이션이 어떻게 동작하는지도 이해할 수 있을 것이다.

다음은 Tomcat 4 배포 디스크립터에 있는 서블릿 요소다.

```xml
<servlet>
  <servlet-name>Manager</servlet-name>
  <servlet-class>
     org.apache.catalina.servlets.ManagerServlet
  </servlet-class>
  <init-param>
     <param-name>debug</param-name>
     <param-value>2</param-value>
  </init-param>
</servlet>
<servlet>
  <servlet-name>HTMLManager</servlet-name>
  <servlet-class>
     org.apache,catalina.servlets.HTMLManagerServlet
  </servlet-class>
  <init-param>
     <param-name>debug</param-name>
     <param-value>2</param-value>
  </init-param>
</servlet>
```

첫 번째 서블릿은 `org.apache.catalina.servlets.ManagerServlet`이고 두 번째는 `org.apache.catalina.servlets.HTMLManagerServlet`이다. 이 장에서는 `ManagerServlet` 클래스에 초점을 맞춘다.

이 애플리케이션의 디스크립터인 `manager.xml`은 컨텍스트 경로가 `/manager`임을 나타낸다.

```xml
<Context path="/manager" docBase="../server/webapps/manager"
  debug="0" privileged="true">
  <!-- Link to the user database we will get roles from -->
  <ResourceLink name="users" global="UserDatabase"
    type="org.apache.catalina.UserDatabase"/>
</Context>
```

첫 번째 `servlet-mapping` 요소는 `ManagerServlet`을 호출하는 방법을 알려 준다.

```xml
<servlet-mapping>
  <servlet-name>Manager</servlet-name>
  <url-pattern>/*</url-pattern>
</servlet-mapping>
```

즉, 다음 패턴으로 시작하는 URL은 `ManagerServlet`을 호출한다.

```text
http://localhost:8080/manager/
```

그러나 배포 디스크립터에는 다음 `security-constraint` 요소도 있다는 점에 유의하자.

```xml
<security-constraint>
  <web-resource-collection>
    <web-resource-name>Entire Application</web-resource-name>
    <url-pattern>/*</url-pattern>
  </web-resource-collection>
  <auth-constraint>
   <!-- NOTE: This role is not present in the default users file -—>
     <role-name>manager</role-name>
  </auth-constraint>
</security-constraint>
```

이는 애플리케이션 전체에 대한 접근이 `manager` 역할에 속한 사용자로 제한된다는 뜻이다. `auth-login` 요소는 사용자가 BASIC 인증 방식으로 올바른 사용자 이름과 암호를 제공하면 제한된 콘텐츠에 접근할 수 있다고 명시한다.

```xml
<login-config>
   <auth-method>BASIC</auth-method>
   <realm-name>Tomcat Manager Application</realm-name>
</login-config>
```

Tomcat에서는 역할과 사용자가 `%CATALINA_HOME%/conf` 디렉터리의 `tomcat-users.xml` 파일에 나열된다. 따라서 Manager 애플리케이션에 접근하려면 `manager` 역할과 그 역할에 속한 사용자를 추가해야 한다. 다음은 그 예다.

```xml
<?xml version='1.0' encoding='utf-8'?>
<tomcat-users>
  <role rolename="manager"/>
  <user username="tomcat" password="tomcat" roles="manager "/>
</tomcat-users>
```

이 `tomcat-users.xml` 파일을 사용하면 사용자 이름 `tomcat`, 암호 `tomcat`으로 Manager 애플리케이션에 접근할 수 있다.

`ManagerServlet`에서는 다음 기능을 사용할 수 있다.

- `list`
- `start`
- `stop`
- `reload`
- `remove`
- `resources`
- `roles`
- `sessions`
- `undeploy`

각 기능을 어떻게 호출하는지 알아보려면 서블릿의 `doGet` 메서드를 확인하라.

> **원문 표기 주의:** 위 XML에는 `org.apache,catalina`처럼 쉼표가 들어간 클래스 이름, `auth-login`이라는 표현, `roles="manager "`의 후행 공백, 그리고 비정상적인 XML 주석 종료 표기가 보인다. 원문을 그대로 보존했다. 또한 `tomcat/tomcat` 자격 증명은 책의 예제일 뿐 실제 운영 환경에 사용해서는 안 된다.

## ContainerServlet 인터페이스 (431페이지)

`org.apache.catalina.ContainerServlet` 인터페이스를 구현하는 서블릿은 자신을 나타내는 `StandardWrapper` 객체에 접근할 수 있다. 래퍼에 접근할 수 있으면 웹 애플리케이션을 나타내는 컨텍스트 객체, 그 컨텍스트가 속한 배포기(`StandardHost` 인스턴스), 그 밖의 객체에도 접근할 수 있다.

`ContainerServlet` 인터페이스는 리스팅 19.1에 나와 있다.

**리스팅 19.1: ContainerServlet 인터페이스**

```java
package org.apache.catalina;

public interface ContainerServlet {
  public Wrapper getWrapper();
  public void setWrapper(Wrapper wrapper);
}
```

Catalina는 `ContainerServlet`을 구현하는 서블릿의 `setWrapper` 메서드를 호출하여, 해당 서블릿을 나타내는 `StandardWrapper`의 참조를 전달한다.

## ManagerServlet 초기화 (431~432페이지)

다른 서블릿과 마찬가지로 서블릿은 `org.apache.catalina.core.StandardWrapper` 인스턴스로 표현된다. 서블릿이 처음 호출되면 `StandardWrapper` 객체의 `loadServlet` 메서드가 호출되고, 이 메서드는 다시 서블릿의 `init` 메서드를 호출한다(11장 참조). `ManagerServlet`의 경우 `loadServlet` 메서드에 있는 다음 코드 조각을 살펴보자.

```java
...
// Special handling for ContainerServlet instances
if ((servlet instanceof ContainerServlet) &&
    isContainerProvidedServlet(actualClass)) {
    ((ContainerServlet) servlet).setWrapper(this);
}

// Call the initialization method of this servlet
try {
   instanceSupport.fireInstanceEvent(InstanceEvent.BEFORE_INIT_EVENT,
      servlet);
   servlet.init(facade);
...
```

여기서 `servlet`은 로드할 서블릿(이 경우 `ManagerServlet`)을 나타낸다.

위 코드의 `if` 문은 `servlet`이 `org.apache.catalina.ContainerServlet`의 인스턴스이고 `isContainerProvidedServlet` 메서드가 `true`를 반환하면, `ContainerServlet` 인터페이스의 `setWrapper` 메서드를 호출한다는 뜻이다.

`ManagerServlet` 클래스는 `ContainerServlet`을 구현하므로 `servlet`은 `ContainerServlet`의 인스턴스다. `StandardWrapper`에 있는 `isContainerProvidedServlet` 메서드는 리스팅 19.2에 제시되어 있다.

**리스팅 19.2: StandardWrapper 클래스의 isContainerProvidedServlet 메서드**

```java
private boolean isContainerProvidedServlet(String classname) {
   if (classname.startsWith("org.apache.catalina.")) {
       return (true);
   }
   try {
       Class clazz =
         this.getClass().getClassLoader().loadClass(classname);
       return (ContainerServlet.class.isAssignableFrom(clazz));
   }
   catch (Throwable t) {
       return (false);
   }
}
```

`isContainerProvidedServlet`에 전달되는 `classname` 인수는 `ManagerServlet`의 완전한 클래스 이름인 `org.apache.catalina.servlets.ManagerServlet`이다. 따라서 `isContainerProvidedServlet` 메서드는 `true`를 반환한다.

이 메서드는 `classname`으로 지정된 서블릿 클래스가 `ContainerServlet`의 하위 타입인 경우에도 `true`를 반환한다. 즉, `classname`이 `ContainerServlet`을 확장하는 인터페이스이거나 `ContainerServlet`을 구현하는 클래스인 경우다.

> **참고:** `java.lang.Class` 클래스의 `isAssignableFrom(Class clazz)` 메서드는 현재 `Class` 객체가 나타내는 클래스 또는 인터페이스가, 인수 `clazz`가 나타내는 클래스 또는 인터페이스와 같거나 그 상위 클래스 또는 상위 인터페이스일 때 `true`를 반환한다.

> **원문 표기 주의:** 마지막 단락의 `inidicated`는 원문의 오탈자다.

---



## 원본 PDF 433~437페이지 번역

따라서 `ManagerServlet`을 나타내는 `StandardWrapper` 인스턴스의 `loadServlet` 메서드는 `ManagerServlet`의 `setWrapper` 메서드를 호출한다. 다음은 `ManagerServlet`에 구현된 `setWrapper` 메서드다.

```java
public void setWrapper(Wrapper wrapper) {
  this.wrapper = wrapper;
  if (wrapper == null) {
      context = null;
      deployer = null;
  }
  else {
      context = (Context) wrapper.getParent();
      deployer = (Deployer) context.getParent();
  }
}
```

`wrapper` 매개변수는 `null`이 아니므로 `else` 블록이 실행된다. 그 결과 `context`에는 Manager 애플리케이션을 나타내는 컨텍스트 객체가, `deployer`에는 해당 컨텍스트를 호스팅하는 `StandardHost` 인스턴스가 할당된다. 특히 `deployer`는 `ManagerServlet`의 여러 메서드에서 다양한 기능을 수행하는 데 사용되므로 중요하다.

`StandardWrapper` 클래스의 `loadServlet` 메서드가 `ManagerServlet`의 `setWrapper`를 호출한 다음에는 `ManagerServlet`의 `init` 메서드를 호출한다.

### 배포된 웹 애플리케이션 목록 표시

배포된 모든 웹 애플리케이션을 나열하려면 다음 URL을 사용한다.

```text
http://localhost:8080/manager/list
```

출력 예시는 다음과 같다.

```text
OK - Listed applications for virtual host localhost
/admin:stopped:0:../server/webapps/admin
/app1:running:0:C:\123data\JavaProjects\Pyrmont\webapps\app1
/manager:running:0:../server/webapps/manager
```

위 URL은 리스팅 19.3에 제시된 `ManagerServlet`의 `list` 메서드를 호출한다.

**리스팅 19.3: `ManagerServlet`의 `list` 메서드**

```java
protected void list(PrintWriter writer) {
  if (debug >= 1)
      log("list: Listing contexts for virtual host '" +
         deployer.getName() + "'");
  writer.println(sm.getString("managerServlet.listed",
      deployer.getName()));
  String contextPaths[] = deployer.findDeployedApps();

    for (int i = 0; i < contextPaths.length; i++) {
       Context context = deployer.findDeployedApp(contextPaths[i]);
       String displayPath = contextPaths[i];
       if( displayPath.equals("") )
           displayPath = "/";
       if (context != null ) {
           if (context.getAvailable()) {
               writer.println(sm.getString("managerServlet.listitem",
                 displayPath, "running", ""
                    + context.getManager().findSessions().length,
                    context.getDocBase()));
           }
           else {
               writer.println(sm.getString("managerServlet.listitem",
                 displayPath, "stopped", "0", context.getDocBase()));
           }
       }
    }
}
```

`list` 메서드는 `deployer`의 `findDeployedApps` 메서드를 호출해 Catalina에 배포된 모든 컨텍스트의 경로를 가져온다. 그런 다음 경로 배열을 순회하며 각 컨텍스트를 구하고, 해당 컨텍스트를 사용할 수 있는지 확인한다. 사용 가능한 컨텍스트마다 경로, 문자열 `running`, 사용자 세션 수, 문서 기본 경로(document base)를 출력한다. 사용할 수 없는 컨텍스트에 대해서는 경로, 문자열 `stopped`, 숫자 `0`, 문서 기본 경로를 출력한다.

### 웹 애플리케이션 시작

웹 애플리케이션을 시작하려면 다음 URL을 사용한다.

```text
http://localhost:8080/manager/start?path=/contextPath
```

여기서 `contextPath`는 시작하려는 애플리케이션의 컨텍스트 경로다. 예를 들어 `admin` 애플리케이션을 시작하려면 다음 URL을 사용한다.

```text
http://localhost:8080/manager/start?path=/admin
```

애플리케이션이 이미 시작된 상태라면 오류 알림을 받는다.

이 URL을 수신하면 `ManagerServlet`은 리스팅 19.4에 제시된 `start` 메서드를 호출한다.

**리스팅 19.4: `ManagerServlet` 클래스의 `start` 메서드**

```java
protected void start(PrintWriter writer, String path) {
  if (debug >= 1)
      log("start: Starting web application at '" + path + "'");

  if ((path == null) || (!path.startsWith("/") && path.equals(""))) {
      writer.println(sm.getString("managerServlet.invalidPath", path));
      return;
  }
  String displayPath = path;
  if( path.equals("/") )
      path = "";

  try {
     Context context = deployer.findDeployedApp(path);
     if (context == null) {
         writer.println(sm.getString("managerServlet.noContext",
           displayPath));
         return;
     }
     deployer.start(path);
     if (context.getAvailable())
         writer.println
           (sm.getString("managerServlet.started", displayPath));
     else
         writer.println
           (sm.getString("managerServlet.startFailed", displayPath));
  }
  catch (Throwable t) {
     getServletContext().log
     (sm.getString("managerServlet.startFailed", displayPath), t);
     writer.println
         (sm.getString("managerServlet.startFailed", displayPath));
     writer.println(sm.getString("managerServlet.exception",
        t.toString()));
    }
}
```

몇 가지 검사를 마친 뒤 `start` 메서드는 `try` 블록에서 `deployer`의 `findDeployedApp` 메서드를 호출한다. 이 메서드는 전달받은 경로에 해당하는 컨텍스트 객체를 반환한다. 컨텍스트가 `null`이 아니면 `start` 메서드는 `deployer`의 `start` 메서드를 호출해 애플리케이션을 시작한다.

### 웹 애플리케이션 중지

애플리케이션을 중지하려면 다음 URL을 사용한다.

```text
http://localhost:8080/manager/stop?path=/contextPath
```

여기서 `contextPath`는 중지하려는 애플리케이션의 컨텍스트 경로다. 애플리케이션이 실행 중이지 않으면 오류 메시지가 표시된다.

`ManagerServlet`이 요청을 수신하면 `stop` 메서드를 호출한다. 이 메서드는 리스팅 19.5에 나와 있다.

**리스팅 19.5: `ManagerServlet` 클래스의 `stop` 메서드**

```java
protected void stop(PrintWriter writer, String path) {
  if (debug >= 1)
      log("stop: Stopping web application at '" + path + "'");
  if ((path == null) || (!path.startsWith("/") && path.equals(""))) {
      writer.println(sm.getString("managerServlet.invalidPath", path));
      return;
  }
  String displayPath = path;
  if( path.equals("/") )
      path = "";

    try {
       Context context = deployer.findDeployedApp(path);
       if (context == null) {
           writer.println(sm.getString("managerServlet.noContext",
             displayPath));
           return;
       }
       // It isn't possible for the manager to stop itself
      if (context.getPath().equals(this.context.getPath())) {
          writer.println(sm.getString("managerServlet.noSelf"));
          return;
      }
      deployer.stop(path);
      writer.println(sm.getString("managerServlet.stopped",
          displayPath));
    }
    catch (Throwable t) {
      log("ManagerServlet.stop[" + displayPath + "]", t);
      writer.println(sm.getString("managerServlet.exception",
         t.toString()));
    }
}
```

`stop` 메서드가 어떻게 동작하는지는 이제 이해할 수 있을 것이다. `ManagerServlet` 클래스의 다른 메서드들도 어렵지 않게 파악할 수 있을 것이다.

### 요약

이 장에서는 특별한 인터페이스인 `ContainerServlet`을 사용해 Catalina 내부 클래스에 접근할 수 있는 서블릿을 만드는 방법을 살펴보았다. 배포된 애플리케이션을 관리하는 데 사용할 수 있는 Manager 애플리케이션을 통해, 래퍼 객체에서 다른 객체들을 가져오는 방법도 확인했다. Tomcat을 관리하기 위해 더욱 정교한 기능을 갖춘 서블릿을 설계하는 것도 충분히 가능하다.

---

*19장 끝. 다음 번역 시작: 원본 PDF 438페이지.*
