# 16장: 종료 훅 (Shutdown Hook)

## 개요 (PDF 340페이지)

사용자가 애플리케이션을 종료할 때 정리 작업(clean-up)을 수행할 기회가 필요한 경우가 많다. 문제는 사용자가 항상 권장되는 종료 절차를 따르지는 않는다는 점이다. 예를 들어 Tomcat 배포 환경에서는 `Server` 객체를 생성하고 그 `start` 메서드를 호출하여 서블릿 컨테이너를 시작한다. 이 메서드는 다시 다른 컴포넌트들의 `start` 메서드를 호출한다. 정상적인 상황이라면 `Server` 객체가 다른 컴포넌트를 모두 중지할 기회를 얻도록 14장 「Server and Service」에서 설명한 올바른 종료 명령을 보내야 한다. 실행 중인 콘솔 창을 닫는 식으로 갑자기 프로그램을 끝내면 예상치 못한 문제가 발생할 수 있다.

다행히 Java는 종료 과정 중에 프로그래머가 코드를 실행할 수 있는 편리한 방법을 제공한다. 이를 통해 정리 코드를 실행할 기회를 확보할 수 있다. 이 장에서는 사용자가 애플리케이션을 어떤 방식으로 종료하든 정리 코드를 실행하도록 **종료 훅(shutdown hook)**을 사용하는 방법을 설명한다.

Java 가상 머신은 다음 두 가지 유형의 이벤트에 반응하여 종료된다.

- `System.exit` 메서드를 호출하거나 마지막 비데몬(non-daemon) 스레드가 종료되는 등 애플리케이션이 정상적으로 끝나는 경우
- 사용자가 `CTRL+C`를 입력하거나 실행 중인 Java 프로그램을 닫기 전에 시스템에서 로그오프하는 등 가상 머신을 갑자기 종료시키는 경우

가상 머신은 종료할 때 다음 두 단계의 순서를 따른다.

1. 등록된 종료 훅이 있다면 모두 시작한다. 종료 훅은 미리 `Runtime`에 등록해 둔 스레드다. 모든 종료 훅은 서로 동시에 실행되며 각각의 실행이 끝날 때까지 진행된다.
2. 적절한 경우 아직 호출되지 않은 종료 처리 메서드(finalizer)를 모두 호출한다.

이 장에서 관심을 두는 부분은 첫 번째 단계다. 프로그래머가 가상 머신에 프로그램의 정리 코드를 실행하도록 지시할 수 있기 때문이다. 종료 훅은 단순히 `java.lang.Thread` 클래스를 상속한 하위 클래스의 인스턴스다.

## 종료 훅 만들기 (PDF 341페이지)

종료 훅을 만드는 절차는 간단하다.

- `Thread` 클래스를 상속하는 클래스를 작성한다.
- 해당 클래스의 `run` 메서드를 구현한다. 이 메서드에는 애플리케이션이 정상적으로 종료되든 갑자기 종료되든 실행해야 할 코드를 작성한다.
- 애플리케이션에서 종료 훅 클래스의 인스턴스를 생성한다.
- 현재 `Runtime`의 `addShutdownHook` 메서드에 종료 훅을 등록한다.

여기서 일반적인 스레드와 달리 종료 훅을 직접 시작하지 않는다는 점에 주목하자. 가상 머신이 종료 절차를 수행할 때 종료 훅을 시작하고 실행한다.

목록 16.1은 `ShutdownHookDemo`라는 간단한 클래스와 `Thread`를 상속한 `ShutdownHook` 클래스를 보여 준다. `ShutdownHook` 클래스의 `run` 메서드는 콘솔에 `Shutting down`이라는 문자열을 출력할 뿐이다. 하지만 종료 전에 실행해야 하는 코드를 이 자리에 넣을 수 있다.

**목록 16.1: 종료 훅 사용하기**

```java
package ex16.pyrmont.shutdownhook;
public class ShutdownHookDemo {

    public void start() {
      System.out.println("Demo");
      ShutdownHook ShutdownHook = new ShutdownHook();
      Runtime.getRuntime().addShutdownHook(ShutdownHook);
    }

    public static void main(String[] args) {
      ShutdownHookDemo demo = new ShutdownHookDemo();
      demo.start();
      try {
         System.in.read();
      }
      catch(Exception e) {
      }
    }
}

class ShutdownHook extends Thread {
   public void run() {
     System.out.println("Shutting down");
   }
}
```

## 종료 훅의 동작 (PDF 342페이지)

`ShutdownHookDemo` 클래스를 인스턴스화한 다음 `main` 메서드는 `start` 메서드를 호출한다. `start` 메서드는 종료 훅을 만들고 현재 런타임에 등록한다.

```java
ShutdownHook shutdownHook = new ShutdownHook();
Runtime.getRuntime().addShutdownHook(shutdownHook);
```

이후 프로그램은 사용자가 Enter 키를 누르기를 기다린다.

```java
System.in.read();
```

사용자가 Enter 키를 누르면 프로그램이 종료된다. 그러나 가상 머신은 종료 훅을 실행하므로 결과적으로 `Shutting down`이라는 문구가 출력된다.

## 종료 훅 예제

다른 예로 `MySwingApp`이라는 간단한 Swing 애플리케이션을 살펴보자(그림 16.1). 이 애플리케이션은 시작할 때 임시 파일을 만든다. 애플리케이션이 종료되면 그 임시 파일을 삭제해야 한다.

**그림 16.1: Swing 애플리케이션**

이 클래스의 코드는 목록 16.2에 제시되어 있다.

## 목록 16.2: 간단한 Swing 애플리케이션 (PDF 343페이지)

```java
package ex16.pyrmont.shutdownhook;
import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;

public class MySwingApp extends JFrame {
  JButton exitButton = new JButton();
  JTextArea jTextArea1 = new JTextArea();
  String dir = System.getProperty("user.dir");
  String filename = "temp.txt";

  public MySwingApp() {
    exitButton.setText("Exit");
    exitButton.setBounds(new Rectangle(304, 248, 76, 37));
    exitButton.addActionListener(new java.awt.event.ActionListener() {
        public void actionPerformed(ActionEvent e) {
          exitButton_actionPerformed(e);
        }
    });
    this.getContentPane().setLayout(null);
    jTextArea1.setText("Click the Exit button to quit");
    jTextArea1.setBounds(new Rectangle(9, 7, 371, 235));
    this.getContentPane().add(exitButton, null);
    this.getContentPane().add(jTextArea1, null);
    this.setDefaultCloseOperation(EXIT_ON_CLOSE);
    this.setBounds(0,0, 400, 330);
    this.setvisible(true);
    initialize();
  }

  private void initialize() {
     // create a temp file
     File file = new File(dir, filename);
     try {
        System.out.println("Creating temporary file");
        file.createNewFile();
     }
     catch (IOException e) {
        System.out.println("Failed creating temporary file.");
     }
```

> **다음 페이지로 계속:** 목록 16.2의 `initialize()` 메서드 및 `MySwingApp` 클래스의 나머지 코드는 PDF 344페이지부터 이어진다. 원문에는 `this.setvisible(true)`라고 표기되어 있으므로 코드에서 그대로 보존했다.

## PDF 344~348페이지 번역

### 목록 16.2: `MySwingApp` 클래스의 나머지 코드 (PDF 344페이지)

앞 페이지에서 이어지는 코드이다.

```java
    }

    private void shutdown() {
       // delete the temp file
       File file = new File(dir, filename);
       if (file.exists()) {
           System.out.println("Deleting temporary file.");
           file.delete();
       }
    }

    void exitButton_actionPerformed(ActionEvent e) {
      shutdown();
      System.exit(0);
    }

    public static void main(String[] args) {
      MySwingApp mySwingApp = new MySwingApp();
    }
}
```

애플리케이션을 실행하면 `initialize` 메서드를 호출한다. 이 메서드는 다시 사용자의 디렉터리에 `temp.txt`라는 임시 파일을 생성한다.

```java
    private void initialize() {
       // create a temp file
       File file = new File(dir, filename);
       try {
          System.out.println("Creating temporary file");
          file.createNewFile();
       }
       catch (IOException e) {
          System.out.println("Failed creating temporary file.");
       }
    }
```

사용자가 애플리케이션을 닫을 때에는 임시 파일을 삭제해야 한다. 사용자가 항상 **Exit** 버튼을 클릭해 종료하기를 바라지만, 그렇게 하면 임시 파일을 삭제하는 `shutdown` 메서드가 반드시 호출되기 때문이다. 그러나 프레임의 **X** 버튼을 누르거나 다른 방식으로 애플리케이션을 종료하면 임시 파일이 삭제되지 않는다.

### 목록 16.3: Swing 애플리케이션에서 종료 훅 사용하기 (PDF 345~346페이지)

목록 16.3의 클래스는 이 문제의 해결책을 제시한다. 목록 16.2의 코드를 수정해 종료 훅을 추가했다. 종료 훅 클래스는 바깥 클래스의 모든 메서드에 접근할 수 있도록 내부 클래스로 선언한다. 목록 16.3에서 종료 훅의 `run` 메서드는 `shutdown` 메서드를 호출하므로 가상 머신이 종료될 때 이 메서드가 호출되도록 한다.

```java
package ex16.pyrmont.shutdownhook;

import java.awt.*;
import javax.swing.*;
import java.awt.event.*;
import java.io.File;
import java.io.IOException;

public class MySwingAppWithShutdownHook extends JFrame {
  JButton exitButton = new JButton();
  JTextArea jTextArea1 = new JTextArea();

  String dir = System.getProperty("user.dir");
  String filename = "temp.txt";

  public MySwingAppWithShutdownHook() {
    exitButton.setText("Exit");
    exitButton.setBounds(new Rectangle(304, 248, 76, 37));
    exitButton.addActionListener(new java.awt.event.ActionListener() {
        public void actionPerformed(ActionEvent e) {
          exitButton_actionPerformed(e);
        }
    });
    this.getContentPane().setLayout(null);
    jTextArea1.setText("Click the Exit button to quit");
    jTextArea1.setBounds(new Rectangle(9, 7, 371, 235));
    this.getContentPane().add(exitButton, null);
    this.getContentPane().add(jTextArea1, null);
    this.setDefaultCloseOperation(EXIT_ON_CLOSE);
    this.setBounds(0,0, 400, 330);
    this.setVisible(true);
    initialize();
  }

  private void initialize() {
     // add shutdown hook
        MyShutdownHook shutdownHook = new MyShutdownHook();
        Runtime.getRuntime().addShutdownHook(shutdownHook);

        // create a temp file
        File file = new File(dir, filename);
        try {
           System.out.println("Creating temporary file");
           file.createNewFile();
        }
        catch (IOException e) {
           System.out.println("Failed creating temporary file.");
        }
    }

    private void shutdown() {
       // delete the temp file
       File file = new File(dir, filename);
       if (file.exists()) {
           System.out.println("Deleting temporary file.");
           file.delete();
       }
    }

    void exitButton_actionPerformed(ActionEvent e) {
      shutdown();
      System.exit(0);
    }

 public static void main(String[] args) {
   MySwingAppWithShutdownHook mySwingApp = new
MySwingAppWithShutdownHook();
 }

    private class MyShutdownHook extends Thread {
       public void run() {
         shutdown();
       }
    }
}
```

목록 16.3의 클래스에서 `initialize` 메서드에 특히 주목하자. 이 메서드는 먼저 `java.lang.Thread`를 상속하는 내부 클래스 `MyShutdownHook`의 인스턴스를 생성한다.

```java
      // add shutdown hook
      MyShutdownHook shutdownHook = new MyShutdownHook();
```

`MyShutdownHook` 인스턴스를 얻은 뒤에는 다음 코드처럼 `Runtime`의 `addShutdownHook` 메서드에 전달한다.

```java
      Runtime.getRuntime().addShutdownHook(shutdownHook);
```

`initialize` 메서드의 나머지 부분은 목록 16.2의 `initialize` 메서드와 비슷하다. 임시 파일을 만들고 `Creating temporary file`이라는 문자열을 출력한다.

```java
        // create a temp file
        File file = new File(dir, filename);
        try {
           System.out.println("Creating temporary file");
           file.createNewFile();
        }
        catch (IOException e) {
           System.out.println("Failed creating temporary file.");
        }
  }
```

이제 목록 16.3의 작은 애플리케이션을 실행해 보자. 애플리케이션을 갑자기 종료하더라도 임시 파일이 항상 삭제되는지 확인한다.

### Tomcat의 종료 훅 (PDF 347~348페이지)

예상할 수 있듯이 Tomcat에도 종료 훅이 있다. 이 종료 훅은 다른 컴포넌트를 관리하는 `Server` 객체를 시작하는 역할을 맡은 `org.apache.catalina.startup.Catalina` 클래스에서 찾을 수 있다. `CatalinaShutdownHook`이라는 내부 클래스(목록 16.4 참조)는 `java.lang.Thread`를 상속하고, `Server` 객체의 `stop` 메서드를 호출하는 `run` 메서드를 구현한다.

**목록 16.4: Catalina 종료 훅**

```java
protected class CatalinaShutdownHook extends Thread {
  public void run() {
     if (server != null) {
         try {
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
}
```

이 종료 훅은 `Catalina` 인스턴스가 시작되는 과정의 한 단계에서 생성되어 `Runtime`에 추가된다. `Catalina` 클래스에 대해서는 17장에서 더 자세히 배운다.

### 요약

때로는 애플리케이션을 종료하기 전에 정리 코드를 실행해야 한다. 그러나 사용자가 항상 올바른 방식으로 프로그램을 종료할 것이라고 기대할 수는 없다. 이 장에서 설명한 종료 훅은 사용자가 어떤 방식으로 애플리케이션을 닫든 종료 시 정리 코드를 실행할 수 있게 하는 해결책이다.

> **번역 주의:** 원문은 종료 훅이 어떤 종료 방식에서도 정리 코드를 실행한다고 설명하지만, JVM이 강제로 종료되거나 프로세스가 비정상적으로 중단되는 일부 상황에서는 실행되지 않을 수 있다. 이는 원문의 주장과 구분되는 기술적 참고 사항이다.
