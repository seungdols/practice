# What is context ?
해당 함수가 실행 되는 공간


## this 

함수 호출 관점에서 바라본다면 ???

## function의 this 

```js
function mySNS() {
    this.name = "LINE";
    //debugger; 
    return this.name;
}//this -> 전역공간을 가리킴.
```

## method의 this

```js
//mySNS : context 
//getName은 mySNS context에 속함. mySNS를 가리킴.
var mySNS = {
    name : "LINE",
    getName : function() {
        return this.name;
    }
}
```

## constructor의 this

```js

function mySNS() {
    this.name = "LINE";
    this.getName = function() {
        return this.name;
    }
}
//constructor 사용 안함
mySNS();//window 전역에 name이 생성 된다.

//constructor 사용
new mySNS();
//새로운 obj 생성
```

## call & apply의 this

```js
call();
apply();

//두 함수 모두 함수를 호출하면서 this를 넘길 수 있다.
```

### apply() 

Function.prototype.apply()

Syntax 

> fun.apply(thisArg, [argsArray])

* Params
    * thisArg : 호출 될 함수에게 지정 될 this
    * argsArray : 유사배열 객체. (arguments, null , undefined 가능)



### call()

Function.prototype.call()

Syntax

> fun.call(thisArg[, arg1[, arg2[, ...]]])

* Params
    * thisArg : func 호출에 제공 되는 this 값
    * arg1, arg2, ... : 객체를 위한 인수


```js
function sum(num1, num2) {
    return num1 + num2;
}

function sum_apply(num1, num2) {
    return sum.apply(this, arguments);
}

function sum_call(num1, num2){
    return sum.call(this, num1,num2);
}
```

call 함수와 apply 함수의 차이는 굉장히 단순한데, 인수를 넘기느냐, 배열을 넘기느냐의 차이다.


### 이쯤에서 발견한 사실

* Scope는 선언 될 때 결정 된다.
* this는 실행 될 때 결정 된다.