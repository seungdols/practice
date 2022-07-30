package main

func main() {
	var a, b int = 1, 2
	c := a + b/3
	println(c)

	println(a == b)
	println(a != b)
	println(a >= c)
	d := (a & b) << 2
	println(d)

	var e int
	e *= 10
	println(e)

	var k int = 10
	var p = &k
	println(*p)
}
