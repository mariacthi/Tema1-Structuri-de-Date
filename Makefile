# compiler setup
CC=gcc
CFLAGS=-Wall -Wextra -std=c99

build: vma

vma: main.c vma.c
	$(CC) $(CFLAGS) main.c vma.c -o vma

run_vma: ./vma

.PHONY: pack clean
