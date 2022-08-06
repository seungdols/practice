package main

import "fmt"

func main() {

	tickers := map[string]string{
		"GOOGLE": "Google Inc",
		"MSFT":   "Microsoft",
		"FB":     "Facebook",
	}

	_, exists := tickers["MSFT"]
	if !exists {
		println("No MSFT ticker")
	}

	var m map[int]string
	m = make(map[int]string)

	m[111] = "Apple"
	m[222] = "Grape"
	m[333] = "Tomato"

	value := m[111]
	println(value)

	for key, value := range tickers {
		fmt.Println(key, value)
	}
}
