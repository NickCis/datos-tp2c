#include "serializador.h"
#include <string.h>
#include <stdio.h>

uint8_t* Serializador_pack(uint8_t* buf, SerializadorTipos tipo, SerializadorData* data, size_t* size){
	size_t size_i = *size;
	switch(tipo){
		case SER_VAR_CHAR:
			*size += sizeof(size_t);
		case SER_FIX_BUF:
			*size += data->size;
			break;

		case SER_INT:
			*size += sizeof(int);
			break;

		case SER_CHAR:
			*size += sizeof(char);
			break;

		default:
			return buf;
			break;
	}

	buf = (uint8_t*) realloc(buf, *size);

	switch(tipo){
		case SER_VAR_CHAR:
			memcpy(buf+size_i, &(data->size), sizeof(size_t));
			size_i += sizeof(size_t);
		case SER_FIX_BUF:
			memcpy(buf+size_i, data->buf, data->size);
			break;

		case SER_INT:
			memcpy(buf+size_i, &data, sizeof(int));
			break;

		case SER_CHAR:
			memcpy(buf+size_i, &data, sizeof(char));
			break;

		default:
			return buf;
			break;
	}

	return buf;
}

uint8_t* Serializador_unpack(SerializadorData* data, SerializadorTipos tipo, size_t* size, size_t* pos){
	uint8_t* ret_ptr = NULL;
	printf("pos %d\n", *pos);
	switch(tipo){
		case SER_VAR_CHAR:{
			size_t* tam = (size_t*) (data->buf + *pos);
			*size = *tam;
			*pos += sizeof(size_t);
		}
		case SER_FIX_BUF:
			ret_ptr = data->buf + *pos;
			*pos += *size;
			break;

		case SER_INT:
			*size = sizeof(int);
			ret_ptr = data->buf + *pos;
			*pos += *size;
			break;

		case SER_CHAR:
			*size = sizeof(char);
			ret_ptr = data->buf + *pos;
			*pos += *size;
			break;

		default:
			break;
	}

	return ret_ptr;
}
