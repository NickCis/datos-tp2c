#include <stdio.h>
#include <string.h>
#include "../src/bloque.h"

#define TEST_STRING_LENGTH 3
char* strings[] = {
	"hola como te va",
	"pepepe",
	"mas string"
};

#define BLOQUE_SIZE (35)

typedef enum MODE {
	READ=0,
	WRITE
} MODE;

int main(int argc, char* argv[]){
	MODE mode = WRITE;
	FILE* fd;
	TBloque* bloque;
	if(argc >= 2){
		if(!strcmp(argv[1], "-r"))
			mode = READ;
	}

	fd = fopen("./bloque_test.dat", (mode == READ)? "r+b" : "w+b");

	bloque = Bloque_crear(BLOQUE_SIZE);
	switch(mode){
		case READ:{
			int bNum = 0;
			while(! Bloque_leer(bloque, fd)){
				printf("Num block: %d\n", bNum++);
				int i =0;
				uint8_t *buf;
				size_t size;
				while((buf = Bloque_get_buf(bloque, i++, &size))){
					printf("%u '%s'\n", size, (char*) buf);
					free(buf);
				}
			}
			break;
		}


		default:{
			int i = 0, ret;
			for(i=0;i<TEST_STRING_LENGTH; i++){
				if(! Bloque_agregar_buf(bloque, (uint8_t*) strings[i], strlen(strings[i])+1)){
					printf("%d Pude agregar\n", i);
				}else{
					ret = Bloque_escribir(bloque, fd);
					printf("Escritura ret: %d\n", ret);
					Bloque_destruir(bloque);
					bloque = Bloque_crear(BLOQUE_SIZE);
					if(! Bloque_agregar_buf(bloque, (uint8_t*) strings[i], strlen(strings[i])+1))
						printf("%d Pude agregar en segunda vuelta\n", i);
				}
			}

			ret = Bloque_escribir(bloque, fd);
			printf("Escritura ret: %d\n", ret);

			break;
		}

	}

	Bloque_destruir(bloque);

	fclose(fd);

	return 0;
}
