# JavaScript 좀 더 나은 패턴 


JavaScript 코드량의 증가 이유?

UI에서 하는 일이 점점 늘어나고 있다.

서버에서 했던 비즈니스로직이 UI로 옮겨가고 있다. 

SPA(Single Page Application)로 구성을 주로 하는 추세이다. 

**결론** : 하는 일이 많아졌다.

전역변수 없애는 방법은 무엇이 있을까?

`JavaScript`는 namespace가 일반적인 언어처럼 지원이 안되므로 다른 방법으로 namespace를 만들어야 한다.

어떻게 `모듈화`를 할 것인가? 

참고 - [Javascript Namespaces and Modules](https://www.kenneth-truyers.net/2013/04/27/javascript-namespaces-and-modules/)

```js
var Utility = {
    getID: function(el) {
        //do
    },
    getQuery: function(el) {
        //do
    },
    getClassList: function(el) {

    },
    trim: function() {}
}
```

```js
var MAYAPPLICATION = MYAPPLICATION || {};
 
var ns = MYAPPLICATION.createNS("MYAPPLICATION.MODEL.PRODUCTS");
 
ns.product = function(price){
   this.price = price;
   this.getPrice = function(){
    return this.price;
   }
};
```

위 코드는 참고 링크에 있는 코드로써 NameSpace를 만드는데 거의 정형화 되어 있는 표준 방법이다. 

서비스가 중첩되어 있고, 큰 규모라면 명칭이 계속 길어진다. (자신이 만드는 것보다 만들어진 라이브러리를 쓰자..)

그럼에도 불구하고 길다면, Aliasing을 해서 지역변수에 참조를 가지고 계속 사용하면 된다. 

```js
MYAPPLICATION.createNS("MYAPPLICATION.MODEL.PRODUCTS");
MYAPPLICATION.createNS("MYAPPLICATION.LOGIC.BUSINESS");
 
MYAPPLICATION.MODEL.PRODUCTS.product = function(price){                                           
    this.price = price;    
    this.isVatApplicable = true;
    this.getPrice = function(){                                               
        return this.price;                                            
    }                                       
};
 
MYAPPLICATION.MODEL.PRODUCTS.currentvatrate = 1.21;
 
MYAPPLICATION.LOGIC.BUSINESS.getSelectedProductTotal = function () {
    var p = new MYAPPLICATION.MODEL.PRODUCTS.product(100);//Alias 사용
    if(p.isVatApplicable){
        return p.getPrice() * MYAPPLICATION.MODEL.PRODUCTS.currentvatrate;
    }
    else {
        return p.getPrice();
    }
}
```

```js
var myProduct = function(price){
    var price = price;    
     
    return {
        isVatApplicable: true,
        getPrice: function(){                                               
            return price;                                            
        }
    };
};
```


```js
var myProduct = function(width, height){
    var price = price;    
    var dimensions = {
        width: width,
        height: height
    };
    return {
        dimensions: dimensions
    };
};
```

만약 위 코드처럼 배열, obj를 리턴하면 리턴 된 dimensions이 계속 참조를 유지하므로, 실행시에 값이 달라질 수 있다. 

고로, 각 요소에 접근하는 함수를 따로 만들어주는 것이 private한 속성을 유지하면서 값을 제공 할 수 있다.

### 모듈 패턴!!!???

**1번**

```js
var myProduct = function(width, height){
    var dimensions = {
        width: width,
        height: height
    };

    var getWidth = function() {
        return dimensions.width;
    };

    var getHeight = function() {
        return dimensions.height;
    };
    return {
        getWidth: getWidth,
        getHeight: getHeight
    };
};

var obj = myProduct(100,100);
obj.getWidth();
```

위 코드처럼 리턴을 해주는 것이 좋은 방법이며, 오브젝트를 그대로 넘기면 참조를 유지하고 있어서 
내부 값이 변경 될 수 있다.

**2번**

```js
var myProduct = function(_width, _height){
    //private
    var dimensions = {
        width: _width,
        height: _height
    };

    //public
    var Width = function(width) {
        this.width = width + dimensions.width;
    };

    Width.prototype.getWidth = function() {
        return this.width;
    };
    //
    return Width;//생성자를 리턴하면, prototype chain도 가지게 된다.
};


var myWidth = myProduct(100,200);//return value : function

var obj = new myWidth(100,200);//Width 생성자

obj.getWidth();//prototype chain에 getWidth가 존재함.
```

1번과 2번은 둘 다 모듈 패턴이긴 하지만, 약간의 차이가 존재한다. 
이 차이에 대해서는 계속 써보면서 차이를 구별해서 쓰는 것이 좋다.


### 결론!

* `private`한 변수의 값이 obj라면, 그대로 리턴하지 말 것.
* namespace를 사용해 전역 공간의 변수를 적게 사용하도록 설계할 것.
* 모듈 패턴을 이용해 `private 변수`로 변수 접근을 막을 수 있다.