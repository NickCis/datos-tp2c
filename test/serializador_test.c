#include <stdio.h>
#include <string.h>
#include "../src/serializador.h"



int main() {
	SerializadorData data;

	data.buf = (uint8_t*) "hola como te va";
	data.size = strlen("hola como te va")+1;

	size_t size = 0;

	printf("serializo informacion\n");

	// Cargo un str con prefijo de longitud, el primer parametro es NULL por qe estoy arrancando a serializar.
	// Notar que se pasa size = 0
	uint8_t* buf = Serializador_pack(NULL, SER_VAR_CHAR, &data, &size);

	// Cargo un buffer sin prefijo de longitud.
	buf = Serializador_pack(buf, SER_FIX_BUF, &data, &size);

	// Cargo un numero
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) 10, &size);

	printf("se serializo '%d'bytes\n", size);

	printf("==== desserializo informacion ====\n");
	// En data, pongo el buffer
	data.buf = buf;
	data.size = size;
	// Seteo pos en 0
	size_t pos =0;

	size = 0;
	// Extraigo el primer str con prefijo de longitud
	char* str = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	printf("Se saca str '%s' '%d' pos:'%d'\n", str, size, pos);

	// Voy a extraer un str sin prefijo de longitud, yo se cual es su longitud.
	size = strlen("hola como te va")+1;
	str = (char*) Serializador_unpack(&data, SER_FIX_BUF, &size, &pos);
	printf("Se saca str '%s' '%d' pos:'%d'\n", str, size, pos);

	// Extraigo entero
	int *i = (int*) Serializador_unpack(&data, SER_INT, &size, &pos);
	printf("Se saca int '%d' '%d' pos:'%d'\n", *i, size, pos);

	free(buf);

	return 0;
}
