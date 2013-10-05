#include <stdio.h>
#include <string.h>
#include "archivo_bloque.h"
#include "sort_externo.h"

#define MAX_ENTIDAD (200)
#define SIZE_BLOQUE (512)

typedef struct {
	int id;
	char entidad[MAX_ENTIDAD+1]; //Para el \0
} TEntidad;

TEntidad * leer_entidad(FILE* fd);
int insersion(int argc, char* argv[]);
int listado(int argc, char* argv[]);
int busqueda(int argc, char* argv[]);
int sort_menu(int argc, char* argv[], TSortExternFunction func);
int sort_cmp_id(void* reg1, size_t size1, void* reg2, size_t size2);
int sort_cmp_entidad(void* reg1, size_t size1, void* reg2, size_t size2);

int main(int argc, char* argv[]){
	if(argc < 3){
		printf("Argumentos invalidos\n");
		return 1;
	}

	if(!strcmp(argv[1], "-i")){ //Insersion
		return insersion(argc, argv);

	}else if(!strcmp(argv[1], "-l")){ //Listado
		return listado(argc, argv);

	}else if(!strcmp(argv[1], "-k")){ //busqueda
		return busqueda(argc, argv);

	}else if(!strcmp(argv[1], "-ok")){
		return sort_menu(argc, argv, &sort_cmp_id);

	}else if(!strcmp(argv[1], "-on")){
		printf("Sort name\n");
		return sort_menu(argc, argv, &sort_cmp_entidad);
	}else{
		printf("Argumentos invalidos\n");
		return 1;
	}

	return 0;
}

TEntidad * leer_entidad(FILE* fd){
	char c;
	TEntidad* entidad;

	if(!fd)
		return NULL;

	entidad = (TEntidad*) calloc(1, sizeof(TEntidad));

	while((c = fgetc(fd)) != '\n' && c != ','){
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
				//Corremos lo ya guardado una decena para la izq y agregamos la nueva unidad
				// Supongo que leo "25". En ppcio (*var) = 0.
				// -> '2'
				//       (*var) = (*var) * 10 + ('2' - '0')
				//       (*var) = 0 * 10 + (50 - 48) -> (*var) = 2
				// -> '5'
				//       (*var) = (*var) * 10 + ('5' - '0')
				//       (*var) = 2 * 10 + (53 - 48) -> (*var) = 25

				entidad->id = entidad->id *10 + (c-'0');
				break;
			default:
				free(entidad);
				while((c = fgetc(fd)) != '\n' && c != EOF);
				return NULL;
				break;
		}
	}

	int i = 0;
	while((c = fgetc(fd)) != '\n' && c != EOF ){
		if(i < MAX_ENTIDAD)
			entidad->entidad[i++] = c;
		else{
			free(entidad);
			while((c = fgetc(fd)) != '\n' && c != EOF);
			return NULL;
		}
	}

	return entidad;
}

int insersion(int argc, char* argv[]){
	FILE* fd;
	char path[512];
	TEntidad* entidad;
	TArchivo* arch;

	if(argc < 4){
		printf("Argumentos invalidos\n");
		return 1;
	}

	strcpy(path, argv[2]);
	strcat(path, ".dat");
	fd = fopen(argv[3], "rb");
	arch = Archivo_crear(path, SIZE_BLOQUE);

	while((entidad = leer_entidad(fd))){
		if(Archivo_agregar_buf(arch, (uint8_t*) entidad, sizeof(int)+strlen(entidad->entidad)+1)){
			printf("Error escribiendo\n");
			printf(" >> Size: %u\n", sizeof(int)+strlen(entidad->entidad)+1);
			printf(" >> Entidad: {\n");
			printf(" \t id: '%d'\n", entidad->id);
			printf(" \t entidad: '%s'\n", entidad->entidad);
			printf(" }\n");

			free(entidad);
			fclose(fd);
			Archivo_destruir(arch);
			return 1;
		}
		free(entidad);
	}
	fclose(fd);
	Archivo_flush(arch);
	Archivo_destruir(arch);
	return 0;
}


int listado(int argc, char* argv[]){
	uint8_t *buf;
	size_t size;
	TArchivo* arch;
	char path[512];
	TEntidad* entidad;

	strcpy(path, argv[2]);
	strcat(path, ".dat");

	arch = Archivo_crear(path, SIZE_BLOQUE);
	Archivo_bloque_leer(arch);

	while((buf = Archivo_get_buf(arch, &size))){
		entidad = (TEntidad*) buf;

		printf("%u '%d,%s'\n", size, entidad->id, entidad->entidad);
		free(buf);
	}

	Archivo_destruir(arch);
	return 0;
}

int busqueda(int argc, char* argv[]){
	uint8_t *buf;
	size_t size;
	TArchivo* arch;
	char path[512];
	TEntidad* entidad;
	int id;

	if(argc < 4){
		printf("Argumentos invalidos\n");
		return 1;
	}

	id = (int) atoi(argv[2]);


	strcpy(path, argv[3]);
	strcat(path, ".dat");

	arch = Archivo_crear(path, SIZE_BLOQUE);
	Archivo_bloque_leer(arch);

	while((buf = Archivo_get_buf(arch, &size))){
		entidad = (TEntidad*) buf;

		if(entidad->id == id){
			printf("%u '%d,%s'\n", size, entidad->id, entidad->entidad);
			free(buf);
			break;
		}

		free(buf);
	}
	Archivo_destruir(arch);
	return 0;
}

int sort_cmp_id(void* reg1, size_t size1, void* reg2, size_t size2){
	TEntidad *ent1 = reg1, *ent2 = reg2;
	if(ent1->id > ent2->id)
		return -1;
	else if(ent1->id < ent2->id)
		return 1;

	return 0;
}

int sort_cmp_entidad(void* reg1, size_t size1, void* reg2, size_t size2){
	TEntidad *ent1 = reg1, *ent2 = reg2;
	return strcmp(ent1->entidad, ent2->entidad) * -1;
}

int sort_menu(int argc, char* argv[], TSortExternFunction func){
	char path[512];
	char* args[3];

	strcpy(path, argv[2]);
	strcat(path, ".dat");

	remove("temp_sort_file.dat");
	sort_externo(SIZE_BLOQUE, path, "temp_sort_file.dat", func);

	strcpy(path, "temp_sort_file");
	args[2] = path;
	return listado(3, args);
}
