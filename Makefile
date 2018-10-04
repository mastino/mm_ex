# matrix multiplication with openmp intended as a Caliper test

CC=icpc
CC=g++
INC=-I${CALIPER_DIR}/include
LIB=-L${CALIPER_DIR}/lib64 -lcaliper

all: mm mm_foo

mm: mm.c
	${CC} -g -o mm ${INC} mm.c -DORDER=3000 -openmp ${LIB}

mm_foo: mm_foo.c
	${CC} -O0 -g -o mm_foo ${INC} mm_foo.c -DORDER=3000 ${LIB} -fopenmp

clean:
	rm -f mm mm_foo *.o *.cali *.json
	# rm -rf MULTI__*

