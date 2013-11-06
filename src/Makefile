CC=gcc
CFLAGS= -std=c99 -Wall -Werror -pedantic -pedantic-errors -O3 -DNDEBUG -ggdb -DDEBUG -fno-inline -O0
LDFLAGS=
CSTD= c99

fuentes ?= $(wildcard *.c)
o_files = $(patsubst %.c,%.o,$(fuentes))
tests ?= $(wildcard ../test/*.c)
mains ?= $(wildcard main*) 
mains_o = $(patsubst %.c,%.o,$(mains)) 
o_files_no_main =  $(filter-out $(mains_o), $(o_files))

RM:= rm -fr

.PHONY: clean all test main

all: main

test: $(o_files);
	- mkdir test
	for test in $(tests); \
	do \
		name=`basename "$$test" | sed 's/\.[c]//g'`; \
		$(CC) $(LDFLAGS) $(CFLAGS) $(o_files_no_main) $$test -o ./test/$$name; \
	done 

#cargador_masivo: main.o bloque.o archivo_bloque.o sort_externo.o ordenador.o archivo_registro.o
cargador_masivo: $(o_files)
	$(CC) $(LDFLAGS) $(CFLAGS) main_cargador_masivo.o $(o_files_no_main) -o $@

creador_admin: $(o_files_no_main) main_creador_admin.o
	$(CC) $(LDFLAGS) $(CFLAGS) main_creador_admin.o $(o_files_no_main) -o $@

main: $(o_files_no_main) main.o
	$(CC) $(LDFLAGS) $(CFLAGS) main.o $(o_files_no_main) -o $@

clean:
	$(RM) *.o test/ main
