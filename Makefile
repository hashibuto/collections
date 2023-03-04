CC = gcc
LIBFILE = collections.a
CFLAGS =
SRC_DIR = ./src
OBJ_DIR = ./obj
LIB_DIR = ./lib

.PHONY: dirs clean

$(LIBFILE): $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(wildcard $(SRC_DIR)/*.c))
	ar -r -o $(LIB_DIR)/$@ $^

$(OBJ_DIR)/%.o: src/%.c dirs
	echo $@
	$(CC) -c $(CFLAGS) -o $@ $<

dirs:
	mkdir -p lib
	mkdir -p obj

clean:
	rm -f $(OBJ_DIR)/*.o $(LIB_DIR)/$(LIBFILE)