package main

func main() {
	var a int = 1
	println(a)

	var i, j, k int = 1, 2, 3
	println(i, j, k)

	b := 2
	println(b)

	const c int = 10
	println(c)

	const (
		Name     = "seungho"
		NickName = "seungdols"
	)

	println(Name, NickName)

	const (
		Naver = iota // 0
		Line         // 1
		Coupang
		Toss
	)

	println(Naver, Line, Coupang, Toss)
}
