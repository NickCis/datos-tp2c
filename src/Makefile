CC:=gcc
CFLAGS:= -std=c99 -Wall -Werror -pedantic -pedantic-errors -O3 -DNDEBUG -ggdb -DDEBUG -fno-inline -O0
RM:= rm -fr

.PHONY: clean all

all: cargador_masivo

heapsort_test: heapsort.o heapsort_test.o
	$(CC) $(CFLAGS) $^ -o $@

bloque_test: bloque.o bloque_test.o
	$(CC) $(CFLAGS) $^ -o $@

archivo_test: archivo_bloque.o bloque.o archivo_test.o
	$(CC) $(CFLAGS) $^ -o $@

archivo_reg_test: archivo_registro.o archivo_reg_test.o
	$(CC) $(CFLAGS) $^ -o $@

#ordenador_test: ordenador.o ordenador_test.o
#	$(CC) $(CFLAGS) $^ -o $@

#cargador_masivo: main.o bloque.o archivo_bloque.o sort_externo.o ordenador.o archivo_registro.o
cargador_masivo: main.o bloque.o archivo_bloque.o sort_externo.o heapsort.o
	$(CC) $(CFLAGS) $^ -o $@

clean:
	$(RM) *.o cargador_masivo archivo_test archivo_reg_test ordenador_test bloque_test *.dat
