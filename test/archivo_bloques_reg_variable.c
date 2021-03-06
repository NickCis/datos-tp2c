#include <stdio.h>
#include <string.h>
#include "../src/archivo_bloque.h"

#define TEST_PATH "archivo__test.dat"
#define TEST_PATH_ADM "archivo__test.dat.adm"
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

#define BLOQUE_SIZE (27)

typedef enum MODE {
	READ=0,
	WRITE,
	MODIF,
	LLENO
} MODE;

int main(int argc, char* argv[]){
	MODE mode = WRITE;
	TArchivo* arch;
	if(argc >= 2){
		if(!strcmp(argv[1], "-r"))
			mode = READ;
		else if(!strcmp(argv[1], "-m"))
			mode = MODIF;
		else if(!strcmp(argv[1], "-l"))
			mode = LLENO;
	}

	// Creo un archivo ( Path del archivo, tama~no del bloque) SIN mapa de bits
	//arch = Archivo_crear(TEST_PATH, BLOQUE_SIZE);
	//
	// Creo un archivo ( Path del archivo, tama~no del bloque) CON mapa de bits
	arch = Archivo_crear_adm(TEST_PATH, TEST_PATH_ADM, BLOQUE_SIZE);

	switch(mode){
		case READ:{
			// Leo el primero bloque
			Archivo_bloque_leer(arch);
			uint8_t *buf;
			size_t size;
			// get_buf, lee por registros, si llega al final del bloque, lee el sigueinte bloque y sigue leyendo registros.
			while((buf = Archivo_get_buf(arch, &size))){
				printf("%u '%s'\n", size, (char*) buf);
				free(buf);
			}
			break;
		}

		case LLENO:{
			int i = 0;
			for(i=0;i<TEST_STRING_LENGTH; i++){
				printf("Bloque n: '%d' valor mapa: '%d'\n", i, Archivo_libre(arch, i));
			}
			break;
		}

		case MODIF:{
			printf("Modifico el bloque 2\n");
			// Me paro en el bloque 3 (la numeracion es desde 0)
			Archivo_bloque_seek(arch, 1, SEEK_SET);
			uint8_t *buf;
			size_t size;

			// Cargo el bloque en memoria (leo de archivo)
			Archivo_bloque_leer(arch);

			// Leo registros solo del bloque
			printf("Leo registros solo del bloque\n");
			while((buf = Archivo_get_bloque_buf(arch, &size))){
				printf("%u '%s'\n", size, (char*) buf);
				free(buf);
			}

			// Vuelvo a correr el fd al principio del bloque
			Archivo_bloque_seek(arch, 1, SEEK_SET);

			// No me interesa el bloque en ram, asi que creo uno nuevo.
			Archivo_bloque_new(arch);

			// Se deberia controlar el error de si se pudo escribir o no!.
			Archivo_bloque_agregar_buf(arch, (uint8_t*) "esto es modificado", 19);

			// Escribe el bloque actual a archivo
			Archivo_flush(arch);

			break;
		}

		default:{
			printf("Escribiendo\n");
			int i = 0;
			for(i=0;i<TEST_STRING_LENGTH; i++){
				// Agregar_buf, escribe en el bloque actual, si llega al final del bloque, crea uno nuevo y sigue escribiendo
				if(! Archivo_agregar_buf(arch, (uint8_t*) strings[i], strlen(strings[i])+1))
					printf("%d Pude agregar\n", i);
			}
			// Escribe el bloque actual a archivo
			Archivo_flush(arch);
			break;
		}

	}

	Archivo_destruir(arch);
	return 0;
}
