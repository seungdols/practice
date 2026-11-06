# 4장: Tomcat 기본 커넥터 (Tomcat Default Connector)

> 원본 PDF 76~91쪽 번역 진행 중입니다. 코드·HTTP 메시지 예시는 원문 표기를 유지합니다.

## 개요 (Overview)

3장에서 구현한 커넥터는 잘 동작했으며, 더 많은 기능을 수행하도록 개선할 수도 있었습니다. 하지만 그 커넥터는 학습용 도구, 즉 Tomcat 4의 기본 커넥터를 소개하기 위해 설계되었습니다. 3장의 커넥터를 이해하는 것은 Tomcat 4에 포함된 기본 커넥터를 이해하는 데 핵심적입니다. 이제 4장에서는 Tomcat 4 기본 커넥터의 코드를 분석하면서 실제 Tomcat 커넥터를 만드는 데 필요한 요소를 살펴봅니다.

**참고:** 이 장에서 말하는 '기본 커넥터(default connector)'는 Tomcat 4의 기본 커넥터입니다. 이 커넥터는 현재 더 빠른 Coyote 커넥터로 대체되어 더 이상 권장되지 않지만, 여전히 훌륭한 학습 자료입니다.

Tomcat 커넥터는 서블릿 컨테이너에 연결할 수 있는 독립적인 모듈입니다. 이미 Coyote, mod_jk, mod_jk2, mod_webapp 등 여러 커넥터가 존재합니다. Tomcat 커넥터는 다음 요구 사항을 충족해야 합니다.

1. `org.apache.catalina.Connector` 인터페이스를 구현해야 합니다.
2. `org.apache.catalina.Request` 인터페이스를 구현한 클래스의 요청 객체를 생성해야 합니다.
3. `org.apache.catalina.Response` 인터페이스를 구현한 클래스의 응답 객체를 생성해야 합니다.

Tomcat 4의 기본 커넥터는 3장에서 만든 간단한 커넥터와 비슷하게 동작합니다. 들어오는 HTTP 요청을 기다리고, 요청 및 응답 객체를 생성한 다음, 이 객체들을 컨테이너에 전달합니다. 커넥터는 `org.apache.catalina.Container` 인터페이스의 `invoke` 메서드를 호출하여 요청과 응답 객체를 전달합니다. 메서드 선언은 다음과 같습니다.

```java
public void invoke(
  org.apache.catalina.Request request,
  org.apache.catalina.Response response);
```

컨테이너는 `invoke` 메서드 내부에서 서블릿 클래스를 로드하고, `service` 메서드를 호출하며, 세션을 관리하고, 오류 메시지를 기록하는 등의 작업을 수행합니다.

기본 커넥터에는 3장의 커넥터에 없던 몇 가지 최적화도 적용되어 있습니다. 첫째, 객체 생성에 드는 높은 비용을 줄이기 위해 여러 종류의 객체를 풀(pool)로 관리합니다. 둘째, 여러 부분에서 문자열 대신 문자 배열(`char` 배열)을 사용합니다.

이 장의 예제 애플리케이션은 기본 커넥터와 연결되는 간단한 컨테이너입니다. 그러나 이 장의 중심 주제는 간단한 컨테이너가 아니라 기본 커넥터입니다. 컨테이너는 5장에서 설명합니다. 다만 기본 커넥터의 사용 방법을 보여주기 위해 이 장의 끝부분에 있는 '간단한 컨테이너 애플리케이션(The Simple Container Application)' 절에서 해당 컨테이너를 살펴봅니다.

또 한 가지 주목할 점은 기본 커넥터가 HTTP 1.1에서 새로 도입된 기능을 모두 구현하면서 HTTP 0.9 및 HTTP 1.0 클라이언트도 처리할 수 있다는 사실입니다. 기본 커넥터를 이해하려면 먼저 HTTP 1.1의 새로운 기능을 알아야 하므로, 첫 번째 절에서 이를 설명합니다. 그다음 `org.apache.catalina.Connector` 인터페이스와 요청·응답 객체를 만드는 방법을 살펴봅니다. 3장의 커넥터가 어떻게 동작하는지 이해했다면 기본 커넥터도 어렵지 않게 이해할 수 있을 것입니다.

이 장은 HTTP 1.1의 세 가지 새로운 기능을 소개하면서 시작합니다. 이 기능들은 기본 커넥터의 내부 동작을 이해하는 데 필수적입니다. 이어서 모든 커넥터가 구현해야 하는 `org.apache.catalina.Connector` 인터페이스를 소개합니다. 그런 다음 3장에서 접했던 `HttpConnector`, `HttpProcessor` 등의 클래스를 다시 살펴봅니다. 다만 이번 클래스들은 3장의 대응 클래스보다 더 발전된 구현입니다.

## HTTP 1.1의 새로운 기능 (HTTP 1.1 New Features)

이 절에서는 HTTP 1.1의 새로운 기능 세 가지를 설명합니다. 이 기능들을 이해하는 것은 기본 커넥터가 HTTP 요청을 처리하는 방식을 이해하는 데 매우 중요합니다.

### 지속 연결 (Persistent Connections)

HTTP 1.1 이전에는 브라우저가 웹 서버에 연결할 때마다 요청한 리소스를 전송한 직후 서버가 연결을 닫았습니다. 하지만 인터넷 페이지에는 이미지 파일이나 애플릿 같은 다른 리소스가 포함될 수 있습니다. 따라서 페이지를 요청한 브라우저는 그 페이지에서 참조하는 리소스도 다운로드해야 합니다. 페이지와 모든 참조 리소스를 각각 다른 연결로 다운로드한다면 처리 속도가 매우 느려질 것입니다.

이 때문에 HTTP 1.1은 **지속 연결(persistent connection)**을 도입했습니다. 지속 연결에서는 페이지를 다운로드한 직후 서버가 연결을 닫지 않습니다. 대신 웹 클라이언트가 페이지에서 참조하는 모든 리소스를 요청할 때까지 기다립니다. 이 방식으로 페이지와 참조 리소스를 동일한 연결을 통해 다운로드할 수 있습니다. HTTP 연결을 수립하고 종료하는 작업에는 상당한 비용이 들기 때문에, 서버와 클라이언트, 네트워크 모두에서 작업량과 시간을 크게 절약할 수 있습니다.

지속 연결은 HTTP 1.1의 기본 연결 방식입니다. 브라우저는 이를 명시하기 위해 값이 `keep-alive`인 `connection` 요청 헤더를 전송할 수도 있습니다.

```http
connection: keep-alive
```

### 청크 인코딩 (Chunked Encoding)

지속 연결을 사용하면 서버는 같은 연결에서 여러 리소스의 바이트 스트림을 전송할 수 있고, 클라이언트도 여러 요청을 보낼 수 있습니다. 따라서 수신자가 바이트를 올바르게 해석할 수 있도록 송신자는 각 요청이나 응답의 콘텐츠 길이를 알려주는 헤더를 전송해야 합니다. 그러나 송신자가 전송할 바이트 수를 미리 알지 못하는 경우도 많습니다. 예를 들어 서블릿 컨테이너는 응답 전체가 준비될 때까지 기다리지 않고 처음 몇 바이트가 준비되는 즉시 응답 전송을 시작할 수 있습니다. 즉, `content-length` 헤더의 값을 미리 알 수 없을 때에도 수신자가 바이트 스트림을 해석할 수 있는 방법이 필요합니다.

여러 요청이나 응답을 전송하지 않는 상황에서도 서버나 클라이언트가 전송할 데이터의 양을 반드시 알고 있는 것은 아닙니다. HTTP 1.0에서는 서버가 `content-length` 헤더를 생략하고 연결에 데이터를 계속 기록하다가 전송이 끝나면 연결을 닫을 수 있었습니다. 이 경우 클라이언트는 파일의 끝을 의미하는 `-1`을 읽을 때까지 데이터를 계속 읽습니다.

HTTP 1.1에서는 바이트 스트림을 여러 **청크(chunk)**로 나누어 전송한다는 사실을 나타내기 위해 `transfer-encoding`이라는 특수 헤더를 사용합니다. 각 청크의 데이터 앞에는 청크 길이를 16진수로 표현한 값과 CR/LF가 전송됩니다. 길이가 0인 청크는 전송의 끝을 나타냅니다. 다음 38바이트를 길이가 각각 29바이트와 9바이트인 두 청크로 전송한다고 가정해 보겠습니다.

```text
I'm as helpless as a kitten up a tree.
```

전송 내용은 다음과 같습니다.

```text
1D\r\n
I'm as helpless as a kitten u
9\r\n
p a tree.
0\r\n
```

`1D`는 29를 16진수로 표현한 값으로, 첫 번째 청크가 29바이트로 구성됨을 나타냅니다. `0\r\n`은 전송의 끝을 나타냅니다.

### 100(Continue) 상태 코드 사용 (Use of the 100 (Continue) Status)

HTTP 1.1 클라이언트는 요청 본문을 전송하기 전에 서버에 `Expect: 100-continue` 헤더를 보내고 확인 응답을 기다릴 수 있습니다. 일반적으로 클라이언트가 긴 요청 본문을 보내려고 하지만 서버가 이를 받아들일지 확신할 수 없을 때 사용합니다. 긴 본문을 모두 전송한 뒤 서버가 요청을 거부한다는 사실을 알게 된다면 자원이 낭비되기 때문입니다.

서버가 `Expect: 100-continue` 헤더를 받았을 때 요청을 처리할 의사와 능력이 있다면, 다음 상태 줄로 응답하고 뒤이어 두 쌍의 CRLF 문자를 전송합니다.

```http
HTTP/1.1 100 Continue
```

그다음 서버는 입력 스트림을 계속 읽어야 합니다.

## Connector 인터페이스 (The Connector Interface)

Tomcat 커넥터는 `org.apache.catalina.Connector` 인터페이스를 구현해야 합니다. 이 인터페이스의 여러 메서드 가운데 가장 중요한 것은 `getContainer`, `setContainer`, `createRequest`, `createResponse`입니다.

`setContainer`는 커넥터를 컨테이너와 연결하는 데 사용합니다. `getContainer`는 연결된 컨테이너를 반환합니다. `createRequest`는 들어오는 HTTP 요청을 위한 요청 객체를 생성하고, `createResponse`는 응답 객체를 생성합니다.

`org.apache.catalina.connector.http.HttpConnector` 클래스는 `Connector` 인터페이스의 구현체이며, 다음 절인 'HttpConnector 클래스(The HttpConnector Class)'에서 설명합니다. 이제 기본 커넥터의 UML 클래스 다이어그램인 **그림 4.1**을 자세히 살펴보겠습니다. 다이어그램을 단순하게 유지하기 위해 `Request`와 `Response` 인터페이스의 구현 내용은 생략했습니다. 또한 `SimpleContainer` 클래스를 제외한 타입 이름에서는 `org.apache.catalina` 접두어도 생략했습니다.

> **그림 4.1:** 기본 커넥터의 UML 클래스 다이어그램 — 원본 PDF 다음 페이지에 이어집니다.

## 기본 커넥터 클래스 다이어그램 (그림 4.1, PDF 80쪽)

**그림 4.1: 기본 커넥터의 클래스 다이어그램**

따라서 다이어그램의 `Connector`는 `org.apache.catalina.Connector`로, `util.StringManager`는 `org.apache.catalina.util.StringManager`로 읽어야 합니다.

`Connector`와 `Container`는 일대일 관계입니다. 관계를 나타내는 화살표 방향에서 알 수 있듯이 `Connector`는 `Container`를 알고 있지만 반대 방향은 그렇지 않습니다. 또한 3장과 달리 `HttpConnector`와 `HttpProcessor`의 관계는 일대다입니다.

## HttpConnector 클래스 (The HttpConnector Class)

3장에서 `org.apache.catalina.connector.http.HttpConnector`의 단순화된 버전을 설명했으므로 이 클래스의 기본 동작은 이미 알고 있을 것입니다. 이 클래스는 Catalina와 함께 동작하기 위한 `org.apache.catalina.Connector`, 자체 스레드에서 실행하기 위한 `java.lang.Runnable`, 그리고 `org.apache.catalina.Lifecycle`을 구현합니다. `Lifecycle` 인터페이스는 이를 구현한 각 Catalina 컴포넌트의 생명주기를 관리하는 데 사용됩니다.

`Lifecycle`은 6장에서 설명합니다. 지금은 `HttpConnector` 인스턴스를 생성한 뒤 `initialize`와 `start` 메서드를 호출해야 한다는 사실만 알면 됩니다. 두 메서드는 컴포넌트의 생애 동안 각각 한 번만 호출해야 합니다. 이제 3장의 `HttpConnector`와 다른 부분, 즉 서버 소켓 생성, `HttpProcessor` 풀 유지, HTTP 요청 처리 방법을 살펴보겠습니다.

### 서버 소켓 생성 (Creating a Server Socket)

`HttpConnector`의 `initialize` 메서드는 `java.net.ServerSocket` 인스턴스를 반환하는 비공개 메서드 `open`을 호출하고 그 결과를 `serverSocket`에 할당합니다. 하지만 `open`은 `java.net.ServerSocket` 생성자를 직접 호출하는 대신 서버 소켓 팩토리에서 `ServerSocket` 인스턴스를 얻습니다. 이 팩토리에 관한 자세한 내용은 `org.apache.catalina.net` 패키지의 `ServerSocketFactory` 인터페이스와 `DefaultServerSocketFactory` 클래스를 참고하세요. 두 타입은 이해하기 어렵지 않습니다.

### HttpProcessor 인스턴스 유지 (Maintaining HttpProcessor Instances)

3장에서는 `HttpConnector` 인스턴스가 한 번에 하나의 `HttpProcessor` 인스턴스만 가지고 있었으므로 한 번에 하나의 HTTP 요청만 처리할 수 있었습니다. 기본 커넥터에서 `HttpConnector`는 `HttpProcessor` 객체 풀을 가지고 있으며 각 `HttpProcessor` 인스턴스에는 자체 스레드가 있습니다. 따라서 여러 HTTP 요청을 동시에 처리할 수 있습니다.

`HttpConnector`는 요청마다 `HttpProcessor` 객체를 새로 생성하지 않도록 풀을 유지합니다. 인스턴스는 `processors`라는 `java.io.Stack`에 저장됩니다.

```java
private Stack processors = new Stack();
```

생성할 `HttpProcessor` 인스턴스 수는 `minProcessors`와 `maxProcessors` 두 변수로 결정합니다. 기본값은 각각 5와 20이지만 `setMinProcessors`, `setMaxProcessors` 메서드로 변경할 수 있습니다.

```java
protected int minProcessors = 5;

private int maxProcessors = 20;
```

처음에 `HttpConnector` 객체는 `minProcessors`만큼 `HttpProcessor` 인스턴스를 생성합니다. 동시에 처리해야 할 요청이 더 많아지면 인스턴스 수가 `maxProcessors`에 도달할 때까지 추가로 생성합니다. 상한에 도달한 뒤에도 처리할 인스턴스가 부족하면 들어오는 HTTP 요청은 무시됩니다. 인스턴스를 계속 생성하도록 하려면 `maxProcessors`를 음수로 설정합니다. `curProcessors` 변수는 현재 인스턴스 수를 유지합니다.

다음은 `HttpConnector`의 `start` 메서드에서 초기 `HttpProcessor` 인스턴스를 생성하는 코드입니다.

```java
while (curProcessors < minProcessors) {
    if ((maxProcessors > 0) && (curProcessors >= maxProcessors))
        break;
    HttpProcessor processor = newProcessor();
    recycle(processor);
}
```

`newProcessor` 메서드는 새 `HttpProcessor` 객체를 생성하고 `curProcessors`를 증가시킵니다. `recycle` 메서드는 `HttpProcessor`를 스택에 다시 넣습니다.

각 `HttpProcessor` 인스턴스는 HTTP 요청 라인과 헤더를 파싱하고 요청 객체에 값을 채웁니다. 따라서 각 인스턴스에는 요청 객체와 응답 객체가 연결되어 있습니다. `HttpProcessor` 생성자에는 `HttpConnector`의 `createRequest`와 `createResponse` 메서드를 호출하는 코드가 있습니다.

### HTTP 요청 처리 (Serving HTTP Requests)

3장과 마찬가지로 `HttpConnector`의 핵심 로직은 `run` 메서드에 있습니다. `run`에는 `HttpConnector`가 중지될 때까지 서버 소켓에서 HTTP 요청을 기다리는 `while` 루프가 있습니다.

```java
while (!stopped) {
    Socket socket = null;
    try {
        socket = serverSocket.accept();
    ...
```

들어오는 각 HTTP 요청에 대해 비공개 메서드 `createProcessor`를 호출하여 `HttpProcessor` 인스턴스를 얻습니다.

```java
HttpProcessor processor = createProcessor();
```

하지만 대부분의 경우 `createProcessor`는 새 객체를 생성하지 않고 풀에서 가져옵니다. 스택에 인스턴스가 남아 있으면 하나를 꺼냅니다. 스택이 비어 있고 최대 인스턴스 수를 초과하지 않았다면 새 인스턴스를 생성합니다. 반면 최대치에 도달했다면 `null`을 반환합니다. 이 경우 소켓을 닫고 해당 HTTP 요청은 처리하지 않습니다.

```java
if (processor == null) {
    try {
        log(sm.getString("httpConnector.noProcessor"));
        socket.close();
    }
    ...
    continue;
```

`createProcessor`가 `null`을 반환하지 않으면 클라이언트 소켓을 `HttpProcessor`의 `assign` 메서드에 전달합니다.

```java
processor.assign(socket);
```

이제 소켓의 입력 스트림을 읽고 HTTP 요청을 파싱하는 일은 `HttpProcessor` 인스턴스가 담당합니다. 여기서 중요한 점이 있습니다. 다음 HTTP 요청을 처리할 수 있도록 `assign` 메서드는 `HttpProcessor`의 파싱 작업이 끝날 때까지 기다리지 않고 즉시 반환해야 합니다. 각 `HttpProcessor` 인스턴스가 파싱을 수행할 자체 스레드를 가지고 있으므로 이를 구현하는 일은 어렵지 않습니다. 다음 절인 'HttpProcessor 클래스'에서 그 방법을 살펴봅니다.

## HttpProcessor 클래스 (The HttpProcessor Class)

기본 커넥터의 `HttpProcessor` 클래스는 3장에서 다룬 동명 클래스의 완전한 버전입니다. 기본 동작은 이미 배웠으므로 이 장에서는 특히 `assign` 메서드를 비동기적으로 만들어 `HttpConnector`가 여러 HTTP 요청을 동시에 처리하도록 하는 방법에 집중합니다.

> **참고:** `HttpProcessor`의 또 다른 중요한 메서드는 비공개 `process` 메서드입니다. 이 메서드는 HTTP 요청을 파싱하고 컨테이너의 `invoke` 메서드를 호출합니다. 이 장의 뒤쪽 '요청 처리(Processing Requests)' 절에서 살펴봅니다.

3장에서 `HttpConnector`는 자체 스레드에서 실행되었지만, 다음 요청을 처리하려면 현재 처리 중인 HTTP 요청이 완료될 때까지 기다려야 했습니다. 다음은 3장의 `HttpConnector.run` 메서드 일부입니다.

```java
public void run() {
    ...
    while (!stopped) {
        Socket socket = null;
        try {
            socket = serversocket.accept();
        }
```

> **이전 번역 범위:** 원본 PDF 80~83쪽. 마지막 코드 블록은 83쪽에서 다음 페이지로 이어집니다. 원문 표기 `serversocket`는 그대로 유지했습니다.

## HttpProcessor의 스레드 실행과 소켓 할당 (원본 PDF 84~87쪽)

앞 페이지의 `HttpConnector.run()` 예제는 다음과 같이 이어진다.

```java
      catch (Exception e) {
        continue;
      }
      // Hand this socket off to an Httpprocessor
      HttpProcessor processor = new Httpprocessor(this);
      processor.process(socket);
    }
  }
```

3장에서 다룬 `HttpProcessor`의 `process` 메서드는 동기식이다. 따라서 `run` 메서드는 다음 요청을 받기 전에 `process`가 끝날 때까지 기다린다.

그러나 기본 커넥터의 `HttpProcessor` 클래스는 `java.lang.Runnable`을 구현하며, 각 인스턴스는 별도의 스레드에서 실행된다. 이 스레드를 **프로세서 스레드(processor thread)**라고 부른다. `HttpConnector`가 `HttpProcessor` 인스턴스를 생성할 때마다 `start` 메서드를 호출해 해당 프로세서 스레드를 시작한다. 목록 4.1은 기본 커넥터의 `HttpProcessor.run()` 메서드를 보여준다.

**목록 4.1: HttpProcessor 클래스의 run 메서드**

```java
public void run() {
  // Process requests until we receive a shutdown signal
  while (!stopped) {
    // Wait for the next socket to be assigned
    Socket socket = await();
    if (socket == null)
      continue;
    // Process the request from this socket
    try {
      process(socket);
    }
    catch (Throwable t) {
      log("process.invoke", t);
    }
    // Finish up this request
    connector.recycle(this);
  }
  // Tell threadStop() we have shut ourselves down successfully
  synchronized (threadSync) {
    threadSync.notifyAll();
  }
}
```

`run` 메서드의 `while` 반복문은 소켓을 가져오고, 요청을 처리한 다음, 커넥터의 `recycle` 메서드를 호출해 현재 `HttpProcessor` 인스턴스를 스택에 다시 넣는 순서로 계속 실행된다. `HttpConnector`의 `recycle` 메서드는 다음과 같다.

```java
void recycle(HttpProcessor processor) {
   processors.push(processor);
}
```

`run`의 반복문은 `await` 메서드에서 대기한다. `await`는 `HttpConnector`로부터 새 소켓을 받을 때까지 프로세서 스레드의 실행을 멈춘다. 다시 말해 `HttpConnector`가 `HttpProcessor` 인스턴스의 `assign` 메서드를 호출할 때까지 기다린다. 그런데 `await`와 `assign`은 서로 다른 스레드에서 실행된다. `assign`은 `HttpConnector.run()`에서 호출된다. 여기서는 이 스레드를 **커넥터 스레드(connector thread)**라고 부른다.

그렇다면 `assign`은 자신이 호출되었다는 사실을 `await`에 어떻게 알릴까? `available`이라는 불리언 변수와 `java.lang.Object`의 `wait`, `notifyAll` 메서드를 사용한다.

> **참고:** `wait` 메서드는 다른 스레드가 같은 객체의 `notify` 또는 `notifyAll`을 호출할 때까지 현재 스레드를 기다리게 한다.

다음은 `HttpProcessor`의 `assign` 및 `await` 메서드다.

```java
synchronized void assign(Socket socket) {
  // Wait for the processor to get the previous socket
  while (available) {
    try {
      wait();
    }
    catch (InterruptedException e) {
    }
  }
  // Store the newly available Socket and notify our thread
  this.socket = socket;
  available = true;
  notifyAll();
  ...
}

private synchronized Socket await() {
  // Wait for the Connector to provide a new Socket
  while (!available) {
    try {
      wait();
    }
    catch (InterruptedException e) {
    }
  }
  // Notify the Connector that we have received this Socket
  Socket socket = this.socket;
  available = false;
  notifyAll();
  if ((debug >= 1) && (socket != null))
    log("  The incoming request has been awaited");
  return (socket);
}
```

**표 4.1: `await`와 `assign` 메서드의 실행 흐름 요약**

| 프로세서 스레드 (`await`) | 커넥터 스레드 (`assign`) |
| --- | --- |
| `while (!available) { wait(); }` | `while (available) { wait(); }` |
| `Socket socket = this.socket;` | `this.socket = socket;` |
| `available = false;` | `available = true;` |
| `notifyAll();` | `notifyAll();` |
| `return socket;` (`run`으로 반환) | 이후 실행 계속 |

프로세서 스레드가 막 시작되었을 때 `available`은 `false`이므로, 스레드는 반복문 안에서 기다린다(표 4.1의 첫 번째 열). 다른 스레드가 `notify` 또는 `notifyAll`을 호출할 때까지 대기한다. 즉, `wait`를 호출하면 커넥터 스레드가 해당 `HttpProcessor` 인스턴스에서 `notifyAll`을 호출할 때까지 프로세서 스레드가 멈춘다.

이제 두 번째 열을 보자. 새 소켓이 할당되면 커넥터 스레드가 `HttpProcessor.assign()`을 호출한다. 이때 `available`은 `false`이므로 반복문을 건너뛰고 다음과 같이 소켓을 인스턴스 변수에 저장한다.

```java
this.socket = socket;
```

그런 다음 커넥터 스레드는 `available`을 `true`로 설정하고 `notifyAll`을 호출한다. 프로세서 스레드가 깨어나면 `available`이 `true`이므로 대기 반복문을 빠져나온다. 이어서 인스턴스의 소켓을 지역 변수에 저장하고, `available`을 `false`로 바꾸고, `notifyAll`을 호출한 뒤 소켓을 반환한다. 이 소켓은 최종적으로 처리된다.

왜 `await`는 인스턴스 변수 `this.socket`을 바로 반환하지 않고 지역 변수 `socket`을 사용할까? 현재 소켓의 처리가 완전히 끝나기 전에도 인스턴스 변수에 다음 요청의 소켓을 할당할 수 있도록 하기 위해서다.

왜 `await`에서도 `notifyAll`을 호출할까? `available`이 `true`인 상태에서 다른 소켓이 도착할 수 있기 때문이다. 이때 커넥터 스레드는 `assign`의 반복문에서 기다리다가 프로세서 스레드의 `notifyAll` 호출을 받고 다시 진행한다.

## Request 객체

기본 커넥터의 HTTP 요청 객체는 `org.apache.catalina.Request` 인터페이스로 표현된다. 이 인터페이스는 `RequestBase` 클래스가 직접 구현하며, `RequestBase`는 `HttpRequest`의 부모 클래스다. 최종 구현 클래스는 `HttpRequest`를 상속하는 `HttpRequestImpl`이다. 3장과 마찬가지로 `RequestFacade`와 `HttpRequestFacade`라는 퍼사드 클래스도 존재한다.

그림 4.2는 `Request` 인터페이스와 구현 클래스의 UML 다이어그램이다. `javax.servlet` 및 `javax.servlet.http` 패키지에 속한 타입을 제외하고, 그림에서는 `org.apache.catalina` 접두어를 생략했다.

> **그림 4.2:** Request 인터페이스 및 구현 클래스의 UML 다이어그램은 원본 PDF 87쪽에서 소개되며, 실제 그림은 다음 페이지에 이어진다.

> **번역 진행:** 원본 PDF 84~87쪽 추가 완료. 다음 범위는 88쪽부터.

---

## Request 인터페이스와 관련 타입 (원본 PDF 88쪽)

**그림 4.2: Request 인터페이스와 관련 타입**

3장에서 요청 객체를 이해했다면 이 다이어그램도 어렵지 않게 이해할 수 있을 것이다.

## Response 객체 (Response Objects)

`Response` 인터페이스와 이를 구현하는 클래스의 UML 다이어그램은 그림 4.3에 제시되어 있다.

**그림 4.3: Response 인터페이스와 구현 클래스**

> 그림 4.2와 그림 4.3의 UML 도식은 원본 PDF 88쪽을 참조한다. Markdown에서는 그림의 제목과 참조 위치를 표시한다.

## 요청 처리 (Processing Requests)

이제 요청 객체와 응답 객체, 그리고 `HttpConnector` 객체가 이들을 어떻게 생성하는지 이해했을 것이다. 마지막으로 살펴볼 것은 실제 처리 과정이다. 이 절에서는 소켓이 할당된 뒤 `HttpProcessor` 클래스의 `run` 메서드가 호출하는 `process` 메서드에 집중한다. `process` 메서드는 다음 작업을 수행한다.

- 연결을 파싱한다.
- 요청을 파싱한다.
- 헤더를 파싱한다.

`process` 메서드를 설명한 다음, 각 작업을 이 절의 하위 절에서 자세히 살펴본다.

`process` 메서드는 처리 중 오류가 없음을 나타내는 불리언 변수 `ok`와, `Response` 인터페이스의 `finishResponse` 메서드를 호출해야 하는지를 나타내는 불리언 변수 `finishResponse`를 사용한다.

```java
boolean ok = true;
boolean finishResponse = true;
```

또한 `process` 메서드는 인스턴스 불리언 변수 `keepAlive`, `stopped`, `http11`을 사용한다. `keepAlive`는 연결이 지속 연결임을 나타낸다. `stopped`는 커넥터가 `HttpProcessor` 인스턴스를 중지시켰으므로 `process` 메서드 역시 중지해야 함을 나타낸다. `http11`은 HTTP 요청이 HTTP 1.1을 지원하는 웹 클라이언트에서 왔음을 나타낸다.

3장과 마찬가지로 소켓의 입력 스트림을 감싸기 위해 `SocketInputStream` 인스턴스를 사용한다. 여기서 주목할 점은 `SocketInputStream` 생성자에 전달하는 버퍼 크기가 `HttpProcessor` 클래스의 지역 변수가 아니라 커넥터에서 가져온 값이라는 것이다. 기본 커넥터 사용자는 `HttpProcessor`에 직접 접근할 수 없기 때문이다. 버퍼 크기를 `Connector` 인터페이스에 두면 커넥터를 사용하는 누구나 그 크기를 설정할 수 있다.

```java
SocketInputStream input = null;
OutputStream output = null;
// Construct and initialize the objects we will need
try {
   input = new SocketInputStream(socket.getInputstream(),
      connector.getBufferSize());
}
catch (Exception e) {
   ok = false;
}
```

그다음 `HttpProcessor`가 중지되거나 예외가 발생하거나 연결이 닫힐 때까지 입력 스트림을 계속 읽는 `while` 반복문이 있다.

```java
keepAlive = true;
while (!stopped && ok && keepAlive) {
  ...
}
```

반복문 안에서 `process` 메서드는 먼저 `finishResponse`를 `true`로 설정하고 출력 스트림을 얻은 뒤 요청 객체와 응답 객체를 초기화한다.

```java
finishResponse = true;
try {
   request.setStream(input);
   request.setResponse(response);
   output = socket.getOutputStream();
   response.setStream(output);
   response.setRequest(request);
   ((HttpServletResponse) response.getResponse()).setHeader
      ("Server", SERVER_INFO);
}
catch (Exception e) {
   log("process.create", e); //logging is discussed in Chapter 7
   ok = false;
}
```

그 후 `process` 메서드는 `parseConnection`, `parseRequest`, `parseHeaders` 메서드를 호출하여 들어온 HTTP 요청을 파싱하기 시작한다. 이 세 메서드는 모두 이 절의 하위 절에서 설명한다.

```java
try {
   if (ok) {
       parseConnection(socket);
       parseRequest(input, output);
       if (!request.getRequest().getProtocol()
           .startsWith("HTTP/0"))
           parseHeaders(input);
```

`parseConnection` 메서드는 프로토콜 값을 얻는다. 프로토콜은 HTTP 0.9, HTTP 1.0 또는 HTTP 1.1일 수 있다. HTTP 1.0은 지속 연결을 지원하지 않으므로 프로토콜이 HTTP 1.0이면 `keepAlive` 불리언 변수를 `false`로 설정한다. `parseHeaders` 메서드는 HTTP 요청에서 `Expect: 100-continue` 헤더를 발견하면 `sendAck` 불리언 변수를 `true`로 설정한다.

프로토콜이 HTTP 1.1이면, 웹 클라이언트가 `Expect: 100-continue` 헤더를 보낸 경우 `ackRequest` 메서드를 호출하여 응답한다. 또한 청크 전송이 허용되는지도 확인한다.

```java
if (http11) {
    // Sending a request acknowledge back to the client if
    // requested.
    ackRequest(output);
    // If the protocol is HTTP/1.1, chunking is allowed.
    if (connector.isChunkingAllowed())
        response.setAllowChunking(true);
}
```

`ackRequest` 메서드는 `sendAck` 값을 확인하고, 값이 `true`이면 다음 문자열을 전송한다.

```text
HTTP/1.1 100 Continue\r\n\r\n
```

HTTP 요청을 파싱하는 동안 여러 종류의 예외가 발생할 수 있다. 예외가 발생하면 `ok` 또는 `finishResponse`가 `false`로 설정된다. 파싱이 끝나면 `process` 메서드는 요청 객체와 응답 객체를 컨테이너의 `invoke` 메서드에 전달한다.

```java
try {
   ((HttpServletResponse) response).setHeader
       ("Date", FastHttpDateFormat.getCurrentDate());
   if (ok) {
       connector.getContainer().invoke(request, response);
   }
}
```

그다음 `finishResponse`가 여전히 `true`이면 응답 객체의 `finishResponse` 메서드와 요청 객체의 `finishRequest` 메서드를 호출하고, 출력을 플러시한다.

```java
if (finishResponse) {
    ...
    response.finishResponse();
    ...
    request.finishRequest();
    ...
    output.flush();
```

> **참고:** 위 코드 조각은 원본 PDF 91쪽에서 다음 페이지로 이어진다.

## `process()` 메서드의 마무리 (원본 PDF 92쪽)

`while` 반복문의 마지막 부분에서는 서블릿 내부에서 응답의 `Connection` 헤더를 `close`로 설정했는지, 또는 프로토콜이 HTTP/1.0인지 확인한다. 해당하는 경우 `keepAlive`를 `false`로 설정한다. 그런 다음 요청 및 응답 객체를 재사용할 수 있도록 초기화한다.

```java
if ( "close".equals(response.getHeader("Connection")) ) {
    keepAlive = false;
}
// End of request processing
status = Constants.PROCESSOR_IDLE;
// Recycling the request and the response objects
request.recycle();
response.recycle();
```

이 단계에서 `keepAlive`가 `true`이고, 앞선 파싱 과정과 컨테이너의 `invoke` 메서드에서 오류가 발생하지 않았으며, `HttpProcessor` 인스턴스가 중지되지 않았다면 `while` 반복문은 처음부터 다시 실행된다. 그렇지 않으면 `shutdownInput` 메서드를 호출하고 소켓을 닫는다.

```java
try {
    shutdownInput(input);
    socket.close();
}
...
```

`shutdownInput` 메서드는 아직 읽지 않은 바이트가 있는지 확인하고, 남아 있다면 해당 바이트들을 건너뛴다.

## 연결 파싱 (Parsing the Connection)

`parseConnection` 메서드는 소켓에서 인터넷 주소를 가져와 `HttpRequestImpl` 객체에 할당한다. 또한 프록시 사용 여부를 확인하고 소켓을 요청 객체에 설정한다. 코드 목록 4.2는 `parseConnection` 메서드를 보여준다.

**코드 목록 4.2: `parseConnection` 메서드**

```java
private void parseConnection(Socket socket)
   throws IOException, ServletException {
   if (debug >= 2)
       log(" parseConnection: address=" + socket.getInetAddress() +
          ", port=" + connector.getPort());
   ((HttpRequestImpl) request).setInet(socket.getInetAddress());
   if (proxyPort != 0)
       request.setServerPort(proxyPort);
   else
       request.setServerPort(serverPort);
   request.setSocket(socket);
}
```

## 요청 파싱 (Parsing the Request)

`parseRequest` 메서드는 3장에서 소개한 같은 이름의 메서드를 완전하게 구현한 버전이다. 3장을 충분히 이해했다면 메서드 코드를 읽는 것만으로도 동작 방식을 파악할 수 있을 것이다.

## 헤더 파싱 (Parsing Headers)

기본 커넥터의 `parseHeaders` 메서드는 `org.apache.catalina.connector.http` 패키지의 `HttpHeader` 클래스와 `DefaultHeaders` 클래스를 사용한다. `HttpHeader` 클래스는 HTTP 요청 헤더를 나타낸다. 3장에서처럼 문자열을 사용하는 대신, 비용이 많이 드는 문자열 연산을 피하기 위해 문자 배열을 사용한다. `DefaultHeaders`는 표준 HTTP 요청 헤더를 문자 배열로 보관하는 `final` 클래스다.

```java
static final char[] AUTHORIZATION_NAME =
    "authorization".toCharArray();
static final char[] ACCEPT_LANGUAGE_NAME =
    "accept-language".toCharArray();
static final char[] COOKIE_NAME = "cookie".toCharArray();
...
```

`parseHeaders` 메서드에는 더 이상 읽을 헤더가 없을 때까지 HTTP 요청을 계속 읽는 `while` 반복문이 있다. 반복문은 요청 객체의 `allocateHeader` 메서드를 호출하여 비어 있는 `HttpHeader` 인스턴스를 얻는 것으로 시작한다. 이어서 이 인스턴스를 `SocketInputStream`의 `readHeader` 메서드에 전달한다.

```java
HttpHeader header = request.allocateHeader();

// Read the next header
input.readHeader(header);
```

모든 헤더를 읽었다면 `readHeader` 메서드는 `HttpHeader` 인스턴스에 이름을 지정하지 않는다. 이때 `parseHeaders` 메서드는 반환해야 한다.

```java
if (header.nameEnd == 0) {
    if (header.valueEnd == 0) {
        return;
    }
    else {
        throw new ServletException
          (sm.getString("httpProcessor.parseHeaders.colon"));
    }
}
```

헤더 이름이 있다면 헤더 값도 있어야 한다.

```java
String value = new String(header.value, 0, header.valueEnd);
```

다음으로 3장에서와 마찬가지로 `parseHeaders` 메서드는 헤더 이름을 `DefaultHeaders`에 정의된 표준 이름과 비교한다. 이때 비교 대상은 문자열 두 개가 아니라 **문자 배열 두 개**라는 점에 유의하자.

```java
if (header.equals(DefaultHeaders.AUTHORIZATION_NAME)) {
    request.setAuthorization(value);
}
else if (header.equals(DefaultHeaders.ACCEPT_LANGUAGE_NAME)) {
    parseAcceptLanguage(value);
}
else if (header.equals(DefaultHeaders.COOKIE_NAME)) {
    // parse cookie
}
else if (header.equals(DefaultHeaders.CONTENT_LENGTH_NAME)) {
    // get content length
}
else if (header.equals(DefaultHeaders.CONTENT_TYPE_NAME)) {
    request.setContentType(value);
}
else if (header.equals(DefaultHeaders.HOST_NAME)) {
    // get host name
}
else if (header.equals(DefaultHeaders.CONNECTION_NAME)) {
    if (header.valueEquals(DefaultHeaders.CONNECTION_CLOSE_VALUE)) {
        keepAlive = false;
        response.setHeader("Connection", "close");
    }
}
else if (header.equals(DefaultHeaders.EXPECT_NAME)) {
    if (header.valueEquals(DefaultHeaders.EXPECT_100_VALUE))
        sendAck = true;
    else
        throw new ServletException(sm.getstring
          ("httpProcessor.parseHeaders.unknownExpectation"));
}
else if (header.equals(DefaultHeaders.TRANSFER_ENCODING_NAME)) {
    //request.setTransferEncoding(header);
}

request.nextHeader();
```

> **원문 주의:** 위 코드의 `sm.getstring` 표기는 원문 그대로 보존했다.

## 간단한 컨테이너 애플리케이션 (The Simple Container Application)

이 장의 애플리케이션은 기본 커넥터를 사용하는 방법을 보여주는 것이 주목적이다. 다음 두 클래스로 구성된다.

- `ex04.pyrmont.core.SimpleContainer`
- `ex04.pyrmont.startup.Bootstrap`

`SimpleContainer` 클래스는 커넥터와 연결될 수 있도록 `org.apache.catalina.Container` 인터페이스를 구현한다. `Bootstrap` 클래스는 애플리케이션을 시작하는 데 사용된다. 3장에 딸린 애플리케이션에서 커넥터 모듈과 `ServletProcessor`, `StaticResourceProcessor` 클래스를 제거했으므로 정적 페이지는 요청할 수 없다.

`SimpleContainer` 클래스는 다음 코드 목록 4.3에 제시된다. **코드 목록 4.3의 전체 내용은 다음 PDF 페이지로 이어진다.**

**코드 목록 4.3: `SimpleContainer` 클래스 (시작)**

```java
package ex04.pyrmont.core;

import java.beans.PropertyChangeListener;
import java.net.URL;
import java.net.URLClassLoader;
import java.net.URLStreamHandler;
import java.io.File;
import java.io.IOException;
import javax.naming.directory.DirContext;
import javax.servlet.Servlet;
import javax.servlet.ServletException;
import javax.servlet.http.HttpServletRequest;
import javax.servlet.http.HttpServletResponse;
import org.apache.catalina.Cluster;
```


## 코드 목록 4.3: `SimpleContainer` 클래스 (계속, PDF 96~98쪽)

다음은 앞 페이지에서 시작한 `SimpleContainer` 클래스의 나머지 코드다. 원본 PDF에 나타나는 대소문자 표기와 오탈자는 수정하지 않았다. PDF에서 페이지가 바뀌며 분리된 줄도 가능한 한 그대로 보존했다.

```java
import org.apache.catalina.Container;
import org.apache.catalina.ContainerListener;
import org.apache.catalina.Loader;
import org.apache.catalina.Logger;
import org.apache.catalina.Manager;
import org.apache.catalina.Mapper;
import org.apache.catalina.Realm;
import org.apache.catalina.Request;
import org.apache.catalina.Response;

public class SimpleContainer implements Container {

  public static final String WEB_ROOT =
    System.getProperty("user.dir") + File.separator + "webroot";

  public SimpleContainer() { }
  public String getInfo() {
    return null;
  }
  public Loader getLoader() {
    return null;
  }
  public void setLoader(Loader loader) { }
  public Logger getLogger() {
    return null;
  }
  public void setLogger(Logger logger) { }
  public Manager getManager() {
    return null;
  }
  public void setManager(Manager manager) { }
  public Cluster getCluster() {
    return null;
  }
  public void setCluster(Cluster cluster) { }

  public String getName() {
    return null;
  }
  public void setName(String name) { }
  public Container getParent() {
    return null;
  }
  public void setParent(Container container) { }
  public ClassLoader getParentClassLoader() {
    return null;
  }
  public void setParentClassLoader(ClassLoader parent) { }
  public Realm getRealm() {
    return null;
  }
  public void setRealm(Realm realm) { }
  public DirContext getResources() {
    return null;
  }
  public void setResources(DirContext resources) { }
  public void addChild(Container child) { }
  public void addContainerListener(ContainerListener listener) { }
  public void addMapper(Mapper mapper) { }
  public void addPropertyChangeListener(
    PropertyChangeListener listener) { }
  public Container findchild(String name) {
    return null;
  }
  public Container[] findChildren() {
    return null;
  }
  public ContainerListener[] findContainerListeners() {
    return null;
  }
  public Mapper findMapper(String protocol) {
    return null;
  }
  public Mapper[] findMappers() {
    return null;
  }
  public void invoke(Request request, Response response)
    throws IoException, ServletException {

    string servletName = ( (Httpservletrequest)
request).getRequestURI();
    servletName = servletName.substring(servletName.lastIndexof("/") +
1);
    URLClassLoader loader = null;
    try {
       URL[] urls = new URL[1];
       URLStreamHandler streamHandler = null;
       File classpath = new File(WEB_ROOT);
       string repository = (new URL("file",null,
classpath.getCanonicalpath() + File.separator)).toString();
       urls[0] = new URL(null, repository, streamHandler);
       loader = new URLClassLoader(urls);

      }
      catch (IOException e) {
         System.out.println(e.toString() );
      }
      Class myClass = null;
      try {
         myClass = loader.loadclass(servletName);
      }
      catch (classNotFoundException e) {
         System.out.println(e.toString());
      }

      servlet servlet = null;

     try {
        servlet = (Servlet) myClass.newInstance();
        servlet.service((HttpServletRequest) request,
(HttpServletResponse) response);
     }
     catch (Exception e) {
        System.out.println(e.toString());
     }
     catch (Throwable e) {
        System.out.println(e.toString());
     }
  }

    public Container map(Request request, boolean update) {
      return null;
    }
    public void removeChild(Container child) { }
    public void removeContainerListener(ContainerListener listener) { }
    public void removeMapper(Mapper mapper) { }
    public void removoPropertyChangeListener(
      PropertyChangeListener listener) {
    }
}
```

`SimpleContainer` 클래스에서는 `invoke` 메서드의 구현만 제공한다. 기본 커넥터가 바로 이 메서드를 호출하기 때문이다. `invoke` 메서드는 클래스 로더를 생성하고, 서블릿 클래스를 로드한 다음, 서블릿의 `service` 메서드를 호출한다. 이 메서드는 3장의 `ServletProcessor` 클래스에 있는 `process` 메서드와 매우 비슷하다.

## 코드 목록 4.4: `ex04.pyrmont.startup.Bootstrap` 클래스 (PDF 99쪽)

```java
package ex04.pyrmont.startup;
import ex04.pyrmont.core.simplecontainer;
import org.apache.catalina.connector.http.HttpConnector;

public final class Bootstrap {
  public static void main(string[] args) {

        HttpConnector connector = new HttpConnector();
        SimpleContainer container = new SimpleContainer();
        connector.setContainer(container);
        try {
           connector.initialize();
           connector.start();

          // make the application wait until we press any key.
          System in.read();
        }
        catch (Exception e) {
          e.printStackTrace();
        }
    }
}
```

`Bootstrap` 클래스의 `main` 메서드는 `org.apache.catalina.connector.http.HttpConnector` 인스턴스와 `SimpleContainer` 인스턴스를 생성한다. 그런 다음 컨테이너를 인자로 전달하여 커넥터의 `setContainer` 메서드를 호출함으로써 두 객체를 연결한다. 이어서 커넥터의 `initialize` 메서드와 `start` 메서드를 호출한다. 이렇게 하면 커넥터가 8080 포트에서 들어오는 모든 HTTP 요청을 처리할 준비를 마친다.

콘솔에서 아무 키나 누르면 애플리케이션을 종료할 수 있다.

> **번역 진행:** 원본 PDF 96~99쪽을 반영했다. `SimpleContainer` 코드 목록과 `Bootstrap` 코드 목록에 보이는 원문 표기상의 오류는 임의로 교정하지 않았다.

## 애플리케이션 실행 (PDF 100쪽)

Windows에서 애플리케이션을 실행하려면 작업 디렉터리에서 다음 명령을 입력한다.

```bash
java -classpath ./lib/servlet.jar;./ ex04.pyrmont.startup.Bootstrap
```

Linux에서는 두 라이브러리 경로를 구분할 때 세미콜론 대신 콜론을 사용한다.

```bash
java -classpath ./lib/servlet.jar:./ ex04.pyrmont.startup.Bootstrap
```

3장에서 했던 것과 같은 방식으로 `PrimitiveServlet`과 `ModernServlet`을 호출할 수 있다. 다만 정적 리소스를 처리하는 프로세서가 없으므로 `index.html` 파일을 요청할 수는 없다.

## 요약 (PDF 100쪽)

이 장에서는 Catalina와 함께 작동하는 Tomcat 커넥터를 구축하는 데 필요한 요소를 살펴보았다. Tomcat 4 기본 커넥터의 코드를 분석하고, 해당 커넥터를 사용하는 작은 애플리케이션을 작성했다. 이후 장에 등장하는 모든 애플리케이션은 기본 커넥터를 사용한다.

---

**4장 번역 완료:** 원본 PDF 76~100쪽. 다음 장은 **5장 Container**이며 PDF 101쪽부터 시작한다.
