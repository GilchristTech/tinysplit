package main

import (
	"bufio"
	"fmt"
	"os"
	"strings"
)

func tinysplitLine(
	stack *[]string,
	line string,
) (string, []string) {
	var sigil = strings.TrimSpace(line)

	if sigil == "" {
		return "", nil
	}

	var pop []string
	var pop_until int = -1

	var push string

	switch sigil[0] {
	case ')':
		for i := range *stack {
			pop_until = len(*stack) - i - 1
			if (*stack)[pop_until][0] == '(' {
				break
			}
		}

	case '@':
		for i := range *stack {
			i = len(*stack) - i - 1
			switch (*stack)[i][0] {
			case '(':
				pop_until = i + 1
			case '@':
				pop_until = i
			default:
				continue
			}
			break
		}

		if len(sigil) > 1 {
			push = sigil
		}

	case '(', ':':
		push = sigil
	}

	if push != "" {
		if pop_until >= 0 {
			pop = (*stack)[pop_until:]
			*stack = (*stack)[0 : pop_until+1]
			(*stack)[pop_until] = push
		} else {
			*stack = append(*stack, sigil)
		}
	} else if pop_until >= 0 {
		pop = (*stack)[pop_until:]
		*stack = (*stack)[:pop_until]
	}

	return sigil, pop
}

func main() {
	if length := len(os.Args); length != 2 {
		fmt.Printf("%s expects 1 argument, got %d\n", os.Args[0], length)
		os.Exit(1)
	}

	var file_path string = os.Args[1]

	fd, err := os.Open(file_path)
	if err != nil {
		fmt.Printf("Error opening file: %v\n")
		os.Exit(1)
	}
	defer fd.Close()

	var scanner = bufio.NewScanner(fd)
	fmt.Println(scanner)

	var lines = make(chan string, 8)
	var done = make(chan bool)

	go func() {
		var stack = make([]string, 0, 128)

		for line := range lines {
			sigil, pop := tinysplitLine(&stack, line)
			fmt.Printf("[%d] %s; %s\n", len(stack), sigil, pop)
		}
		done <- true
	}()

	for scanner.Scan() {
		var line string = scanner.Text()
		lines <- line
	}
	close(lines)

	if err := scanner.Err(); err != nil {
		fmt.Println(err)
		os.Exit(1)
	}

	<-done
}
