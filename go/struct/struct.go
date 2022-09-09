package main

import "fmt"

type person struct {
	name string
	age  int
}

type dict struct {
	data map[int]string
}

// Constructor function
func newDict() *dict {
	d := dict{}
	d.data = map[int]string{}
	return &d
}

func main() {
	p := person{}
	p.name = "seungdols"
	p.age = 32

	fmt.Println(p)

	p1 := person{"company", 33}
	fmt.Println(p1)

	p2 := new(person)
	p2.name = "seungho"
	p2.age = 22

	fmt.Println(*p2)

	dic := newDict()
	dic.data[0] = "A"

	first := dic.data[0]
	fmt.Println(first)

}
