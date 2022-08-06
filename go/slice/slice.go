package main

import (
	"fmt"
)

func main() {
	var a []int
	a = []int{1, 2, 3}
	a[1] = 10
	fmt.Println(a)

	s := make([]int, 5, 10)
	println(len(s), cap(s))

	var s1 []int
	if s == nil {
		println("Nil slice")
	}
	println(len(s1), cap(s1))

	s2 := []int{0, 1, 2, 3, 4, 5}
	s2 = s2[2:5]
	fmt.Println(s2)

	s3 := []int{0, 1}
	s3 = append(s3, 2)
	fmt.Println(s3)

	sliceA := []int{1, 2, 3}
	sliceB := []int{4, 5, 6}

	sliceA = append(sliceA, sliceB...)
	fmt.Println(sliceA)
}
