#include <stdio.h>
#include <string.h>
#include "../src/hash_extensible.h"

#define TEST_PATH "archivo_hash_block_test.dat"
#define TEST_PATH_TABLA "archivo_hash_tabla_test.dat"
#define TEST_PATH_BAJA "archivo_hash_baja_test.dat"
#define TEST_STRING_LENGTH 20
char* strings[] = {
	"1 - hola como te va",
	"2 - pepepe",
	"3 -mas string",
	"4 - otras cosas ",
	"5- hola como te va",
	"6 - pepepe",
	"7 -mas string",
	"9 - otras cosas ",
	"10 - hola como te va",
	"11 - pepepe",
	"12 - mas string",
	"13 - otras cosas ",
	"14- hola como te va",
	"15 - pepepe",
	"16 -mas string",
	"17 - otras cosas ",
	"18 - hola como te va",
	"19 - pepepe",
	"20 - mas string",
	"21 - otras cosas ",
	"22 - otras cosas "
};

#define TEST_DEL_LENGHT 3
unsigned int borrar[] = {
	1,
	4,
	11
};

#define BLOQUE_SIZE (50)

typedef enum MODE {
	READ=0,
	WRITE,
	DEL,
	ADD
} MODE;

unsigned int get_id(uint8_t* ele, size_t size){
	unsigned int id = 0;
	int i =0;
	char c;
	while( (i < size) && (c = (char) ( ele[i++])) ){
		switch(c){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				id = id * 10 + c - '0';
				break;
			default:
				break;
		}
	}

	return id;
}

int main(int argc, char* argv[]){
	MODE mode = WRITE;
	THashExtensible* hash;
	if(argc >= 2){
		if(!strcmp(argv[1], "-r"))
			mode = READ;
		else if(!strcmp(argv[1], "-d"))
			mode = DEL;
		else if(!strcmp(argv[1], "-a"))
			mode = ADD;
	}

	hash = HashExtensible_crear(TEST_PATH, TEST_PATH_TABLA, TEST_PATH_BAJA, BLOQUE_SIZE, &HashDispersionModulo, &get_id);

	switch(mode){
		case READ:{
			uint8_t *buf;
			size_t size;
			unsigned int i=0;


			for(i=1;i<=TEST_STRING_LENGTH; i++){
				buf = HashExtensible_get(hash, i, &size);
				printf("Lei %d) %s\n", i, (char*) buf);
				free(buf);
			}

			buf = HashExtensible_get(hash, 42, &size);
			printf("Lei %d) %s\n", 42, (char*) buf);
			free(buf);
			buf = HashExtensible_get(hash, 823, &size);
			printf("Lei %d) %s\n", 823, (char*) buf);
			free(buf);

			break;
		}
		case DEL:{
			uint8_t *buf;
			size_t size;
			int i;

			for(i=0; i < TEST_DEL_LENGHT; i++){
				printf("Se borra elemento id) '%d' \n", borrar[i]);
				if( (buf = HashExtensible_del(hash, borrar[i], &size))){
					printf("elemento borrado: '%s'\n", buf);
					free(buf);
				}
			}
			break;

		}
		case ADD:{
			printf("agregando mas cosas\n");
			if(! HashExtensible_insertar(hash, (uint8_t*) "42 - asdasdads", strlen("42 - asdasdads")+1))
				printf("%d Pude agregar\n", 1);

			if(! HashExtensible_insertar(hash, (uint8_t*) "823 - noooo", strlen("823 - noooo")+1))
				printf("%d Pude agregar\n", 2);
			break;
		}
		default:{
			printf("Escribiendo\n");
			int i = 0;
			for(i=0;i<TEST_STRING_LENGTH; i++){
				// Agregar_buf, escribe en el bloque actual, si llega al final del bloque, crea uno nuevo y sigue escribiendo
				printf("inserto %s\n", strings[i]);
				if(! HashExtensible_insertar(hash, (uint8_t*) strings[i], strlen(strings[i])+1))
					printf("%d Pude agregar\n", i);
			}
			break;
		}
	}

	HashExtensible_destruir(hash);
	return 0;
}
