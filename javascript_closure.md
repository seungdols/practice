# Closure를 알아보자.

클로저를 모른다고, 개발을 못하는 것은 아니다.

Scope를 이해한 상태에서 분명 난 지역변수라고 생각했으나, 실제로는 그렇게 동작하지 않을 때가 있다. 
그 때, 클로저를 의심해야 한다.

 
함수호출이 종료되면, 함수내 지역변수는 유효하지 않음

 ```js
function mySNS() {
    var name = "LINE";
    return name;
}

//실행
mySNS();
console.log(name);
```

name은 언제까지 유효할까?

```js
function mySNS() {
    var name = "LINE";
    return function() {
        return name;
    };
}

var myWrapSNS = mySNS();
//이 순간에도 mySNS는 종료 되지 않는다. 자신을 누군가가 참조하고 있다.
myWrapSNS();
```

예컨대 누군가가 우리집에 왔는데 내가 저녁 10시 넘었으니 가라고 등떠밀면 안 된다. (비유 같지 않은 비유..)

## 클로저란!!

자신의 scope 밖에 있는 변수들에 접근 할 수 있는 함수를 의미한다.
즉, 함수 외부에 선언 된 변수를 참조하는 함수이다.

위의 예제 코드에서 클로저는 바로 요놈이다. 

```js
        function() {
        return name;
    };
```

name은 언제까지 유효할까? 고민해보자.

```js
function mySNS() {
    var name = "LINE";
    return {
        getName : function() {
            return name;
        },
        setName : function(newName) {
            name = newName;
        }
    }
}
var myWrapSNS = mySNS();
myWrapSNS.getName();
```


