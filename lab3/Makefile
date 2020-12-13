
NAME ?= lab3
NP ?= 2
RANG_M ?= 4

all: run

build:
	mpicc -lm '$(NAME).c' -o '$(NAME)'

run: build
	mpirun -np $(NP) './$(NAME)' $(RANG_M)
