CC=clang
CFLAGS=-Wall -Werror

.PHONY:clean all

all: section_header_test

section_header_test: header_reader.o header_section_reader.o util.o test_read_section_header.o section_reader.o
	$(CC) $(CFLAGS) $^ -o $@
 







clean: 
	rm -f *.o