C_FILES := shell(cat *.c)
PHONEY: build

build: collections.o
    ar rcs bin/static/collections.a bin/static/collections.o

collections.o: $(C_FILES)
    gcc -c collections.o $(C_FILES)