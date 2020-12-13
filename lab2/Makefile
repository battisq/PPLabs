
NAME ?= matrix
NP ?= 6

ROWS ?= 3
COLS ?= 4

all: run

build:
	mpicc -lm '$(NAME).c' -o '$(NAME)'

run: build
	mpirun -np $(NP) './$(NAME)' $(ROWS) $(COLS)
