# Makefile

PARCC=mpicc
SEQCC=gcc
FLAGS=-O -Wall -lm

PARSRC=src/parallel.c
SEQSRC=src/sequential.c

PAROUT=bin/main
SEQOUT=bin/seqmain

parallel:
	$(PARCC) $(PARSRC) $(FLAGS) -o $(PAROUT)

sequential:
	$(SEQCC) $(SEQSRC) $(FLAGS) -o $(SEQOUT)

