
.PHONY: leaks

wish: wish.c
	gcc -o wish -Wall -Werror wish.c

leaks:
	leaks -atExit -- ./wish
