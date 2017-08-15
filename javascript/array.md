### Array란 
배열으로 번역을 하고, 데이터 구조 중 하나입니다. 가장 흔하게 사용하는 구조이며, 빠른 접근성의 특성을 가지고 있습니다. 하지만, 자바스크립트에서는 배열은 그렇게 빠른 데이터 구조는 아닙니다. 

대신, 배열과 유사한 특성을 갖는 객체를 제공합니다. 물론, 속도적인 측면에서는 굉장히 느린 점이 단점입니다. 
자바스크립트는 배열 첨자를 문자열로 변환하여 속성을 만들게 됩니다. 

```javascript
var empty = [];
var numbers = ['zero','one','seungdols','creatijin'];

numbers[2]//seungdols
numbers.length; // 4
empty.length; // 0

var number_obj = {
  '0':'zero',
  '1':'one'
};
//number_obj는 Object.prototype를 상속 받고, numbers는 Array.prototype을 상속 받아 둘 사이에 차이가 존재합니다. 
```

우선 number_obj는 length라는 특별한 속성을 갖지 못합니다. 이는 배열의 길이를 반환하는 특별한 속성이죠. 
하지만, 배열의 첨자 하한은 0부터 시작하므로, length는 배열의 길이에 +1 된 값을 반환하게 됩니다. 
또한, upper bound 기반이 아니여서 배열의 길이가 자동으로 늘어나게 됩니다. (C 언어 같은 경우 미리 길이를 선언 해야 함)

#### 배열 요소 삭제 

요소 삭제는 delete로 가능합니다. 

```javascript
var numbers = ['zero','one','seungdols','creatijin'];
delete numbers[2];//seungdols 삭제 
```

하지만, 위에서 seungdols라는 문자열이 삭제 된 뒤, 해당 공간은 비어있으나, 다른 배열의 첨자는 동일합니다. 
즉, [0,0,x, 0] 이와 같이 0은 첨자 인덱스가 있으나, 삭제한 공간의 인덱스는 비어 나중에 작업시 불편합니다. 그래서 배열을 정돈해주어야 합니다. 그래서 지원하는 메소드가 있습니다.

`slice`라는 메소드를 이용하면, 요소 일부를 삭제 후, 이를 나머지 요소로 대체 할 수 있습니다. 

```javascript
var numbers = ['seungdols','company'];
numbers.slice(0,1);
["seungdols"]
```
slice는 첫 인자가 시작 위치, 두번째가 삭제할 요소 수입니다. 고로, `company`가 삭제 되었습니다. 고로, 성능상으로는 속도가 느립니다. 

### 배열의 순회 

자바스크립트에서 속성의 순회는 `for in` 문법으로 해도 되지만, 배열의 경우 순서를 보장해주는 것이 좋습니다. 하지만, `for in`문은 순서를 보장하지 않으므로 `for`문을 사용하면 됩니다. 

```javascript
for (var i = 0; i < numbers.length; i++){
console.log(numbers[i]);
}
```
이렇게 사용하시면 됩니다. 

#### 객체와 배열의 구분법

**이 내용은 더글라스 크락포드의 책의 내용을 참조하시면 됩니다.**

```javascript
var is_arrays = function(value){
    return value && 
           typeof value === 'object' && //Object인지 확인
           typeof value.length === 'number' &&//객체인 경우 거짓이 됨.
           typeof value.slice === 'function' &&//배열의 경우에만 참 값이 됨.
           !(value.propertyIsEnumerable('length'));//for in문으로 열거가 가능한지 검사함.
};
```
위 처럼 하게 되면, 배열인지 객체인지 판별이 가능하다. 

**배열은 초기화 되지 않습니다. (만약, 0으로 초기화 된 값으로 배열을 얻고 싶다면, 직접 값을 넣어주는 작업을 해야 합니다.)**

