CFLAGS=-g -O0

build:
	for file in $(wildcard *.c); do \
 		$(MAKE) $${file%.c}.out; \
 	done


process_generator.out: process_generator.c headers.h IO/* DataStructures/*
	gcc $(CFLAGS) $(filter %.c,$^) -lm -o $@

scheduler.out: scheduler.c headers.h IO/* DataStructures/*
	gcc $(CFLAGS) $(filter %.c,$^) -lm -o $@

%.out: %.c headers.h
	gcc $(CFLAGS) $< -lm -o $@


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
