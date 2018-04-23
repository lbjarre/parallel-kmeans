# Makefile

PARCC=mpicc
SEQCC=gcc
FLAGS=-Wall -lm -std=c99

PARSRC=src/parallel.c
SEQSRC=src/sequential.c

PAROUT=bin/parallel
SEQOUT=bin/sequential

parallel:
	$(PARCC) $(PARSRC) $(FLAGS) -o $(PAROUT)

sequential:
	$(SEQCC) $(SEQSRC) $(FLAGS) -o $(SEQOUT)

