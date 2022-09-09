package main

type Rect struct {
	width, height int
}

// value receiver
func (r Rect) area() int {
	return r.width * r.height
}

// pointer receiver
func (r *Rect) area2() int {
	r.width++
	return r.width * r.height
}

func main() {
	rect := Rect{10, 20}
	area := rect.area2()
	println(rect.width, area)
}
