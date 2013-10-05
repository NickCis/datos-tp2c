#include <stdio.h>
#include <string.h>
#include "archivo_registro.h"

#define TEST_PATH "archivo_registro_test.dat"
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

typedef enum MODE {
	READ=0,
	WRITE
} MODE;

int main(int argc, char* argv[]){
	MODE mode = WRITE;
	TArchivoReg* arch;
	if(argc >= 2){
		if(!strcmp(argv[1], "-r"))
			mode = READ;
	}

	arch = ArchivoReg_crear(TEST_PATH);

	switch(mode){
		case READ:{
			uint8_t *buf;
			size_t size;
			while((buf = ArchivoReg_leer(arch, &size))){
				printf("%u '%s'\n", size, (char*) buf);
				free(buf);
			}
			break;
		}

		default:{
			printf("Escribiendo\n");
			int i = 0;
			for(i=0;i<TEST_STRING_LENGTH; i++){
				if( ArchivoReg_escribir(arch, (uint8_t*) strings[i], strlen(strings[i])+1) == 0)
					printf("%d Pude agregar\n", i);
			}
			break;
		}
	}

	ArchivoReg_destruir(arch);
	return 0;
}
