package main

import "fmt"

func main() {
	var k int = 1
	if k == 1 {
		println("One")
	} else if k == 2 {
		println("Two")
	} else {
		println("Other")
	}

	var i int = 1
	var max int = 2
	if val := i * 2; val <= max {
		println(val)
	}

	var name string
	var category int = 1

	switch category {
	case 1:
		name = "seungdols1"
	case 2:
		name = "seungdols2"
	case 3, 4:
		name = "seungdols3"
	default:
		name = "Other"
	}
	println(name)

	grade(90)

	var value interface{} = "TypeTest"

	switch value.(type) {
	case int:
		println("int")
	case bool:
		println("bool")
	case string:
		println("string")
	default:
		println("unkown")
	}

	check(2)
}

func grade(score int) {
	switch {
	case score >= 90:
		println("A")
	case score >= 80:
		println("B")
	case score >= 70:
		println("C")
	case score >= 60:
		println("D")
	default:
		println("No Hope")
	}
}

func check(val int) {
	switch val {
	case 1:
		fmt.Println("1 이하")
		fallthrough
	case 2:
		fmt.Println("2 이하")
		fallthrough
	case 3:
		fmt.Println("3 이하")
		fallthrough
	default:
		fmt.Println("default 도달")
	}
}
