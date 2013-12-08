#include <stdio.h>
#include <string.h>
#include "../src/cifrador_hill.h"
#include <stdlib.h>
typedef int bool;

int main (int argc, char const* argv[]){
	const char *clave /*= "E_1FG5;-?"*/;
	const char *palabra /*="GOL"*/;
	char *out, *out2;

	if(argc != 3){
		printf("%s <clave> <palabra a cifrar>\n", argv[0]);
		return 1;
	}

	clave = argv[1];
	palabra = argv[2];
	out = (char*) calloc(1, strlen(palabra)+1);
	out2 = (char*) calloc(1, strlen(palabra)+1);

	normalizar(palabra, out2);

	if(validar_clave(clave)){
		printf("clave: '%s' no valida\n", clave);
		return 1;
	}

	printf("Se usara la clave '%s'\n", clave);

	encrypt(clave, out2, out);

	printf("Se encripto palabra '%s' -> '%s'\n", out2, out);

	decrypt(clave, out, out2);

	printf("Se encripto palabra '%s' -> '%s'\n", out, out2);

	free(out);
	free(out2);

	return 0;
}
