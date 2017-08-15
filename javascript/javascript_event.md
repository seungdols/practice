### 자바스크립트 이벤트 

요소에 이벤트를 바인딩하는 세 가지 방법

* HTML 이벤트 핸들러
* 전통적인 DOM 이벤트 핸들러 
* DOM 레벨 2 이벤트 리스너 

#### HTML 이벤트 핸들러 

그다지 권장하는 방법은 아니다. 

이유 : HTML과 자바스크립트 코드를 분리 시키는 것이 더 좋고, 현재 이 방식을 전혀 사용하지 않는다. 

```javascript
<a onclick="hide()"> </a>
```

위 코드처럼 사용자가 a 태그를 누를 때, hide() 함수를 호출한다. 

#### 전통적인 DOM 이벤트 핸들러 

DOM의 최초 명세에서 소개 된 방법이다. 
장점으로는 주요 모든 브라우저에서 지원을 한다는 점이다. 그러나, 단점으로는 이벤트 별로 단 **하나의 함수**만 바인딩 할 수 있다는 점이다.

```javascript
element.onevent = functionName; //이러한 형식으로 작성한다. 

function checkUserPass() {
	//사용자 비밀번호 체크
}

var el = document.getElementById('username');
el.onblur = checkUserPass;	//이벤트 이름에 'on'을 붙여야 한다.
```
이벤트 핸들러를 사용할 때는 이벤트 이름 앞에 'on'을 사용했다.

* onsubmit
* onchange
* onfocus
* onblur
* onmouseover
* onmouseout


#### DOM 레벨 2 이벤트 리스너 

이벤트 리스너는 DOM 명세의 개정판에서 소개 되었다. 하나의 이벤트로 여러 개의 함수를 실행 할 수 있다. 단점으로는 IE8 및 그 이하 버전에서는 지원을 하지 않는다. (하지만, 우회 방법이 존재하고, jQuery를 사용하면 해결 가능하다. )

```javascript
element.addEventListner('event', functionName [, 불리언]);

function checkUserPass() {
	// 사용자 비밀번호 체크
}

var el = document.getElementById('userpass');
el.addEventListner('blur', checkUserPass, false); //마지막 값은 false를 주로 준다.
```

이벤트 리스너를 추가하는데 첫번째 매개변수는 이벤트 이름을 쓰는데 문자열 따옴표 안에 기입한다. 

이벤트가 발생시 동작할 코드는 함수명만 기입하는데, 이 때 `()`를 입력하면, 페이지가 로드 될 때 함수를 실행하라는 의미가 되어 괄호는 생략한다.

고로, 매개변수가 있는 함수를 사용하고자 할 때 **다른 방법**이 필요하다.

```javascript
el.addEventListner('blur', function () {
	checkUserPass(6);
}, false);
```

위 코드처럼 익명함수를 도입해서 그 안에서 이벤트가 발생 했을 때 호출 할 함수를 익명 함수 안에서 호출 해주면 된다. 

### 구 버전의 IE 지원하기

IE5 ~ IE8까지 다른 이벤트 모델을 채택해 `addEventListner` 메서드를 지원 하지 않는다. 하지만, 구 버전 IE에서도 이벤트 리스너가 동작하게 만들 수는 있다.

```javascript
if (el.addEventListner) { //해당 이벤트를 지원 한다면, 아래 구문을  실행
	el.addEventListner('blur', function () { 
    	checkUserPass(6);
    }, false);
} else {				// 그게 아니라면 아래 구문을 실행
	el.attachEvent('onblur', function () {	//이벤트에 'on'을 붙여야 한다.
    	checkUserPass(6);
    }, false);
}
```

위 처럼 작성하게 되면 구 버전도 지원하게 된다. 하지만, 이렇게 하는 방식 보다는 `jQuery`를 사용하는 것이 더 간편한 방법이긴 하다.

`el.addEventListner('blur', checkUserPass, false);` 여기에서 마지막 인수로 false를 주는데 이에 대해 알아 보자. 

### 이벤트의 흐름 

HTML요소는 다른 요소의 내부에 중첩 된다. 그래서 자식 요소에 이벤트가 발생하면, 부모요소에도 동일한 이벤트가 발생한 것으로 볼 수 있다.

이벤트 흐름이 중요한 이유는 이벤트의 흐름은 어느 한 요소와 그의 부모요소 혹은 후손 요소 중 하나에 이벤트 핸들러가 바인딩 되어 있을 때만 의미가 있다. 

**이벤트 버블링**

이벤트가 사용자의 동작에 직접적 영향을 받은 특정 요소로부터 부모요소로 전파 되어 가는 방식.

**이벤트 캡처링** 

이벤트가 가장 바깥에서부터 안쪽으로 전파 되어 들어오는 방식이다. (IE8 및 그 이하는 지원 하지 않는다.)

주로 `addEventListner( , , fasle)`를 주게 되는데 이는 버블링 방식을 사용하는 것이다. 

`true`일 경우 이벤트 캡쳐링 방식을 사용하는 것이다. 

### 이벤트 객체 

event 객체는 이벤트가 발생했을 때, 이벤트를 발생시킨 요소와 발생한 이벤트에 대한 정보를 제공해준다. 

| 속성 | IE5 ~ 8 속성 | 목적 |
|-----||-----||-----|
| target | srcElement | 이벤트가 발생한 요소 |
| type | type | 발생한 이벤트의 종류 |
| cancelable | 지원 안함 | 요소의 기본 동작을 취소 할 수 있는지 여부 |

| 메서드 | IE5 ~ 8 | 목적 |
|-----||-----||-----|
| preventDefault() | returnValue | 이벤트의 기본 동작을 취소한다. |
| stopPropagation() | cancelBubble | 이벤트의 캡처링 혹은 버블링을 중단 한다. |


참고로, IE5 ~ 8 이하까지 지원하려면, 아래처럼 작성해야 event 객체를 가져올 수 있다. 

```javascript
var event_target;
event_target = e.target || e.srcElement;
```

위 코드는 단축 평가가 적용 된다. 

** 단축 평가란 ?**

> 논리 연산자들은 왼쪽에서 오른쪽으로 실행 되지만, 이 연산자들의 결과를 얻는 순간 단축 평가를 실행한다. 단축 평가는 평가의 중단을 뜻한다. 그러나 그 결과 값은 (반드시 true 또는 false가 아니라 평가가 중단하게 된 계기가 된 값을 반환한다.)

ex) 

```javascript
var name = 'seungdols'; //내용을 가진 문자열은 참 값으로 평가 될 수 있다.
var decision = (name || 'seungdols company'); // seungdols

var name = '';
var decision = (name || 'seungdols company'); // seungdols company

//name 변수가 값을 가지고 있지 않다면, 빈 객체를 생성할 수도 있다.
```

### Tip

단축평가를 고려하면, true로 취급할 수 있는 값이 발견 되는 순간 나머지 값들에 대해 평가를 진행하지 않는다. 

고로, 개발 능력이 뛰어난 사람들은 아래의 내용을 고려한다. 

* OR연산자를 사용시 true를 리턴 할 가능성이 높은 조건을 첫 번째로 둔다. 
* AND연산자 사용시 false를 리턴 할 가능성이 높은 조건을 첫 번째로 둔다. 
* 평가를 수행 할 때, 비용이 큰 코드는 나중에 배치하여, 앞에서 true가 나올 경우 비용이 큰 코드를 실행 하지 않도록 한다.


참고 : 제이펍 - 인터랙티브 프론트엔드 웹 개발 교과서 자바스크립트 & 제이쿼리




