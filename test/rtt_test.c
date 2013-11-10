#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../src/rtt.h"

typedef enum MODE {
	READ=0,
	WRITE,
	DEL
} MODE;

char* text1 = "hola este es el texto uno";
char* text2 = "hola texto no casa";

int main(int argc, char* argv[]){
	MODE mode = WRITE;
	TRtt* rtt = Rtt_crear("rtt_test_ocu_apa.dat", "rtt_test_ocu_doc.dat", "rtt_test_arb.dat", "rtt_test_lista.dat", "rtt_test_lista_baja.dat", 64, 512);

	if(argc >= 2){
		if(!strcmp(argv[1], "-r"))
			mode = READ;
		/*else if(!strcmp(argv[1], "-d"))
			mode = DEL;*/
	}

	switch(mode){
		case READ:{
			char palabra[255];
			printf("Ingrese palabra a buscar: ");
			scanf("%s", palabra);
			printf("Se buscara '%s'\n", palabra);
			long* textos;
			size_t len = 0;
			textos = Rtt_buscar(rtt, palabra, &len);
			if(len){
				printf("Encontre!\n");
				size_t i;

				for(i=0; i < len; i++)
					printf("%d -> %ld\n", i, textos[i]);

				free(textos);
			}else{
				printf("No encontre\n");
			}
			break;
		}
		default:{
			printf("Agregando textos\n");
			Rtt_agregar_texto(rtt, 1, text1);
			Rtt_agregar_texto(rtt, 2, text2);
			Rtt_generar_indice(rtt);
			break;
		}
	}

	Rtt_destruir(rtt);
	return 0;
}
