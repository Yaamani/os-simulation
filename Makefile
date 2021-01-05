CCFLAGS=-g

build:
	for file in $(wildcard *.c); do \
 		$(MAKE) $${file%.c}.out; \
 	done


process_generator.out: process_generator.c headers.h IO/* DataStructures/*
	gcc $(CCFLAGS) $(filter %.c,$^) -o $@

scheduler.out: scheduler.c headers.h IO/* SchedulingAlgorithms/* DataStructures/*
	gcc $(CCFLAGS) $(filter %.c,$^) -o $@

%.out: %.c headers.h
	gcc $(CCFLAGS) $< -o $@


# build:
# 	gcc process_generator.c -o process_generator.out
# 	gcc clk.c -o clk.out
# 	gcc scheduler.c -o scheduler.out
# 	gcc process.c -o process.out
# 	gcc test_generator.c -o test_generator.out

clean:
	rm -f *.out  processes.txt

all: clean build

run:
	./process_generator.out
