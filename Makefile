# Makefile

PARCC=mpicc
SEQCC=gcc
INTPCC=mpiicc

FLAGS=-Wall -lm -std=c99

PARSRC=src/parallel.c src/k-means-parallel.c src/k_means.c src/read-parallel.c
SEQSRC=src/sequential.c src/k_means.c
REDSRC=src/readparallel.c

PAROUT=bin/parallel
SEQOUT=bin/sequential
REDOUT=bin/read


LATEXC=pdflatex
LATEXFLAGS=-output-directory doc/logs -halt-on-error
LATEXSRC=doc/src/report.tex
LATEXMVOUTPUT=mv doc/logs/report.pdf doc/
BIBTEX=bibtex
BIBAUX=doc/logs/report.aux

parallel:
	$(PARCC) $(PARSRC) $(FLAGS) -o $(PAROUT)

parallel-int:
	$(INTPCC) $(PARSRC) $(FLAGS) -o $(PAROUT)

sequential:
	$(SEQCC) $(SEQSRC) $(FLAGS) -o $(SEQOUT)

par-red:
	$(PARCC) $(REDSRC) $(FLAGS) -o $(REDOUT)

report:
	$(LATEXC) $(LATEXFLAGS) $(LATEXSRC)
	$(BIBTEX) $(BIBAUX)
	$(LATEXC) $(LATEXFLAGS) $(LATEXSRC)
	$(LATEXC) $(LATEXFLAGS) $(LATEXSRC)
	$(LATEXMVOUTPUT)
