package main

func main() {
	rawLiteral := `seungdols\n`

	interLiteral := "seungdols\nseungho"

	println(rawLiteral)
	println(interLiteral)

	var i int = 100
	var u uint = uint(i)
	println(u)

	str := "seungdols"
	bytes := []byte(str)
	println(bytes)
}
