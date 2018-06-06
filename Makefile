CC = gcc


OBJ_FILES := $(patsubst src/%.c, obj/%.o, $(wildcard src/*.c))

CFLAGS = -Wall -std=c11
PFLAGS =
LIBS = 

processor: setup $(OBJ_FILES)
			$(CC) $(CFLAGS) $(PFLAGS) $(LIBS) -o notebook $(OBJ_FILES)
			@echo "To execute, run ./notebook <filenames>"

setup:
	mkdir -p obj

debug: CFLAGS := -g -O0 -std=c11
debug: processor

obj/%.o: src/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

clean: 
	-@rm -rf obj
	-@rm notebook


