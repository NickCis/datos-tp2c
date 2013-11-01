#include <stdio.h>
#include <string.h>
#include "../src/archivo_bloque.h"

#define TEST_PATH "archivo__test.dat"
#define TEST_STRING_LENGTH 20
char* strings[] = {
	"1- hola como te va",
	"2 - pepepe",
	"3 -mas string",
	"4 - otras cosas ",
	"5- hola como te va",
	"6 - pepepe",
	"7 -mas string",
	"8 - otras cosas ",
	"5- hola como te va",
	"6 - pepepe",
	"7 -mas string",
	"8 - otras cosas ",
	"5- hola como te va",
	"6 - pepepe",
	"7 -mas string",
	"8 - otras cosas ",
	"5- hola como te va",
	"6 - pepepe",
	"7 -mas string",
	"8 - otras cosas ",
	"8 - otras cosas "
};

#define BLOQUE_SIZE (512)

typedef enum MODE {
	READ=0,
	WRITE
} MODE;

int main(int argc, char* argv[]){
	MODE mode = WRITE;
	TArchivo* arch;
	if(argc >= 2){
		if(!strcmp(argv[1], "-r"))
			mode = READ;
	}

	arch = Archivo_crear(TEST_PATH, BLOQUE_SIZE);

	switch(mode){
		case READ:{
			Archivo_bloque_leer(arch);
			uint8_t *buf;
			size_t size;
			while((buf = Archivo_get_buf(arch, &size))){
				printf("%u '%s'\n", size, (char*) buf);
				free(buf);
			}
			break;
		}


		default:{
			printf("Escribiendo\n");
			int i = 0;
			for(i=0;i<TEST_STRING_LENGTH; i++){
				if(! Archivo_agregar_buf(arch, (uint8_t*) strings[i], strlen(strings[i])+1))
					printf("%d Pude agregar\n", i);
			}
			Archivo_flush(arch);
			break;
		}

	}

	Archivo_destruir(arch);
	return 0;
}
