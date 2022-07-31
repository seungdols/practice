package main

func main() {
	var a [3]int
	a[0] = 1
	a[1] = 2
	a[2] = 3
	println(a[1])

	var b = [3]int{1, 2, 3}
	var c = [...]int{1, 2, 3}

	println(b[2], c[2])

	var d = [2][2]int{
		{1, 2},
		{2, 3},
	}

	println(d[0][1])
}
