print: print.c uart.c
	gcc $^ -o print

loader: loader.c uart.c
	gcc $^ -o loader

front : front.c uart.c
	gcc $^ -o front
