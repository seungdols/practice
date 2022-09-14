# 1장 안정성

## 아이템 1: 가변성을 제한하라

코틀린은 모듈로 프로그램을 설계 한다. 모듈은 클래스, 객체 ,함수, 타입 별칭, Top-level property 등 다양한 요소로 구성 된다. 

아래의 코드를 살펴 봅니다. 

```kotlin
class NumberSum {  
  
    suspend fun sumNumber() {  
        var num = 0  
        coroutineScope {  
            for (i in 1..1000) {  
                launch {  
                    delay(10)  
                    num += 1  
                }  
            }  
        }  
  
        println(num)  // 실행 마다 다른 값이 나온다.
    }  
}
```

코루틴은 경량 쓰레드라고 생각하면 되고, 위의 코드에서는 쓰레드 간의 동기화가 되어 있지 않기 때문에, 값을 마음 대로 변경 하게 되는 이슈가 있고, 우선 num 변수 자체가 가변적인 변수라 이를 불변하게 제어 할 필요가 있다.

### 코틀린에서 가변성 제어 

- 읽기 전용 프로퍼티
	- `val` keyword
- 가변 컬렉션과 읽기 전용 컬렉션 구분
	- List를 MutableList로 다운캐스팅을 하지 말것 
	- Mutable Collection이 필요하다면, toMutableList()로 변환하여 사용한다.
- data class의 copy

```kotlin
var name: String = "seungho"  
var nickName: String = "seungdols"  
val fullName  
    get() = "$name $nickName" // custom getter의 경우, 호출시마다 해당 상태값을 반영함.
println(fullName)  
name = "Maja"  
println(fullName)

// seungho seungdols
// Maja seungdols

class GetterExample {  
    val fizz = calculate() // fizz에는 int 값이 담김. 함수 호출은 1번  
    val buzz  
        get() = calculate()  
    private fun calculate(): Int {  
        println("Calculating...")  
        return 42  
    }  
}
```

var의 경우 get, set 모두 가능 하지만, val의 경우 get만 가능하다. val 로 선언된 프로퍼티를 var 로 오버라이드 가능하다. 

```kotlin
val name: String? = "seungho"  
val surname: String = "seungdols"  
  
val fullName: String?  
    get() = name?.let { "$it $surname"}  
val fullName2: String? = name?.let { "$it $surname"}  
  
fun test() {  
    if (fullName != null) {  
        println(fullName?.length)  
    }  
    if (fullName2 != null) {  
        println(fullName2.length)  // smart case OK
    }  
}
```

```kotlin
val list1: MutableList<Int> = mutableListOf()
var list2: List<Int> = listOf()
```

위의 두 mutable에 대하여 차이를 명확하게 이해하는 것이 좋다. 

첫 번째의 경우, mutableList 내부에서의 변경 가능성
두 번째의 경우, 프로퍼티 자체의 변경 가능성

최악의 경우는 아래의 코드이다.

```kotlin
var list: MutableList<Int> = mutableListOf()
```

결론적으로 mutableList를 쓰는 것보다 프로퍼티를 var로 표현하는 것이 안정성이 더 높고, 방어 로직을 추가하기가 쉽다.
