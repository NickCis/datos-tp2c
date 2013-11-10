#include <stdio.h>
#include <string.h>
#include "../src/lista_invertida.h"

typedef enum MODE {
	READ=0,
	WRITE,
	DEL
} MODE;

int main(int argc, char* argv[]){
	MODE mode = WRITE;
	TListaInvertida* lista;

	if(argc >= 2){
		if(!strcmp(argv[1], "-r"))
			mode = READ;
		else if(!strcmp(argv[1], "-d"))
			mode = DEL;
	}
	lista = ListaInvertida_crear("lista_reg.dat", "lista_baja.dat", 64);

	switch(mode){
		case READ:{
			int id = 0;
			printf("Ingrese id lista: ");
			scanf("%d", &id);
			uint8_t* buf;
			size_t size;
			ListaInvertida_set(lista, id);
			while( (buf = ListaInvertida_get(lista, &size))){
				printf(" '%u' ", * ((unsigned int*) buf));
				free(buf);
			}
			printf("\n");
			break;
		}

		case DEL:{
			int id = 0;
			printf("Ingrese id lista: ");
			scanf("%d", &id);
			ListaInvertida_set(lista, id);

			if(ListaInvertida_erase(lista))
				printf("Error borrando id %d\n", id);
			else
				printf("OK borrando id %d\n", id);

			break;
		}

		default:{
			int id = 0;
			char c;
			printf("Ingrese id lista (enter para nueva): ");
			c = getchar();
			if(c != '\n'){
				while(( c != '\n')){
					if(c >= '0' && c <= '9')
						id = id * 10 + c - '0';
					c = getchar();
				}
				ListaInvertida_set(lista, id);
			} else
				printf("Ref nueva: %d\n", ListaInvertida_new(lista));

			printf("Cantidad de elements y dsp elementos: ");
			int cant = 0;
			scanf("%d", &cant);
			while(cant-- > 0){
				scanf("%d", &id);
				ListaInvertida_agregar(lista, (uint8_t*) &id, sizeof(id));
			}
			ListaInvertida_escribir(lista);
			break;
		}
	}

	ListaInvertida_destruir(lista);
	return 0;
}
