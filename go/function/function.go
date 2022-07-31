package main

func main() {
	msg := "Hello, World"
	say(&msg)
	name := "seungdols"
	say(&name)
	println(msg, name)
	variadicSay("This", "is", "a", "book")
	total := sum(1, 7, 3, 5, 9)
	println(total)

	count, total := sum2(1, 7, 3, 5, 9)
	println(count, total)

	println(sum3(1, 7, 3, 5, 9))
}

func say(msg *string) {
	println(*msg)
	*msg = "Changed"
}

func variadicSay(msg ...string) {
	for _, s := range msg {
		println(s)
	}
}

func sum(nums ...int) int {
	s := 0
	for _, n := range nums {
		s += n
	}

	return s
}

func sum2(nums ...int) (count int, total int) {
	for _, n := range nums {
		total += n
	}
	count = len(nums)
	return
}

func sum3(nums ...int) (int, int) {
	total := 0
	for _, n := range nums {
		total += n
	}
	count := len(nums)
	return count, total
}
