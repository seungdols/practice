package main

func main() {
	sum := 0
	for i := 1; i <= 100; i++ {
		sum += i
	}
	println(sum)

	n := 1
	for n < 100 {
		n *= 2
	}
	println(n)

	idxs := []int{1, 2, 3, 4, 5}
	for value := range idxs {
		println(value)
	}

	var a = 1
	for a < 15 {
		if a == 5 {
			a += a
			continue
		}
		a++
		if a > 10 {
			break
		}
	}

	if a == 11 {
		goto END
	}
	println(a)
END:
	println("END")

	i := 0

L1:
	for {
		if i == 0 {
			break L1
		}
	}
	println("OK")
}
