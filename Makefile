CC=gcc
MPICC=mpicc
OUT_SEQ=seq
OUT_DIST=dist

all: seq dist

seq: sequential.c
	$(CC) -DN=$(N) -o $(OUT_SEQ) $^

dist: distributed.c
	$(MPICC) -DN=$(N) -o $(OUT_DIST) $^

clean:
	rm $(OUT_SEQ) $(OUT_DIST)
