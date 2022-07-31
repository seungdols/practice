package main

func main() {
	add := func(a int, b int) int {
		return a + b
	}

	r1 := calc(add, 10, 20)
	println(r1)

	r2 := calc2(add, 20, 30)
	println(r2)
}

func calc(f func(int, int) int, a int, b int) int {
	result := f(a, b)
	return result
}

type calculator func(int, int) int

func calc2(f calculator, a int, b int) int {
	result := f(a, b)
	return result
}
