CC = gcc
LIBFILE = collections.a
CFLAGS =
SRC_DIR = ./src
OBJ_DIR = ./obj
LIB_DIR = ./lib
BIN_DIR = ./bin
TEST_LIBS = -lcheck -lsubunit -lm

.PHONY: dirs clean

$(LIBFILE): $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
	ar -r -o $(LIB_DIR)/$@ $^

$(OBJ_DIR)/%.o: src/%.c dirs
	echo $@
	$(CC) -c $(CFLAGS) -o $@ $<

dirs:
	mkdir -p $(LIB_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)

test: build-test
	./bin/check_hashmap

build-test: $(LIBFILE)
	gcc ./tests/check_hashmap.c ./lib/$(LIBFILE) $(TEST_LIBS) -o $(BIN_DIR)/check_hashmap

clean:
	rm -f $(OBJ_DIR)/*.o $(LIB_DIR)/$(LIBFILE)
