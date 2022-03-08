```kt
var name: String = "seungho"
var nickName: String = "seungdols"
val fullName
	get() = "$name $nickName" // custom getter의 경우, 호출시마다 해당 상태값을 반영함.

fun main() {
    println(fullName)
    name = "seungdols"
    println(fullName)
}
```

```
output
 
seungho seungdols
seungdols seungdols
```

---
```kt
fun calculate(): Int {
    println("Calculating...")
    return 42
}

val fizz = calculate() // fizz에는 int 값이 담김. 함수 호출은 1번
val buzz
	get() = calculate()
    
fun main() {
    println(fizz)
    println(fizz)
    println(buzz)
}
```

```
output
Calculating...
42
42
Calculating...
42
```
---
