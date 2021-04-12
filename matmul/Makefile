build:
	gcc main.c src/matmul.c -o bin/matmul -pthread

THREADS = 8
DIM_SIZE = 500
export THREADS
export DIM_SIZE

run-simple: build
	bin/matmul simple ${DIM_SIZE} ${THREADS} | cat
	
run-optimized: build
	bin/matmul optimized ${DIM_SIZE} ${THREADS} | cat
	
run-multithreaded: build
	bin/matmul multi-threaded ${DIM_SIZE} ${THREADS} | cat
	
run-all: build
	bin/matmul all ${DIM_SIZE} ${THREADS} | cat


cachegrind-simple: build
	valgrind --tool=cachegrind bin/matmul simple ${DIM_SIZE} ${THREADS} | cat

cachegrind-optimized: build
	valgrind --tool=cachegrind bin/matmul optimized ${DIM_SIZE} ${THREADS} | cat

cachegrind-multithreaded: build
	valgrind --tool=cachegrind bin/matmul multi-threaded ${DIM_SIZE} ${THREADS} | cat

generate-graphs: build
	python scripts/graph.py