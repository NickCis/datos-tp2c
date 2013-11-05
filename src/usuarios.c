#include <string.h>
#include "usuarios.h"
#include "hash_extensible.h"
#include "config.h"
#include "serializador.h"

// Hash usado para guardar en disco a los usuarios
THashExtensible* hash_usuarios = NULL;

/** Devuelve un usuario sacando la info desde buf
 */
TUsuario* _usuarioDesdeBuf(uint8_t* buf, size_t size);

struct TUsuario {
	unsigned int dni;
	char *nombre;
	char *apellido;
	int c_mail;
	char *mails[3];
	char *pass;
	char *prov;
	char t_u;
};

unsigned int Usuarios_get_id(uint8_t* ele, size_t size){
	unsigned int id = * ( (unsigned int*) ele);
	return id;
}

int Usuarios_init(){
	hash_usuarios = HashExtensible_crear(
		USUARIOS_HASH_BLOQUE_PATH,
		USUARIOS_HASH_TABLA_PATH,
		USUARIOS_HASH_BAJA_PATH,
		USUARIOS_HASH_BLOQUE_SIZE,
		&HashDispersionModulo,
		&Usuarios_get_id
	);

	if(!hash_usuarios)
		return 1;

	return 0;
}

int Usuarios_end(){
	HashExtensible_destruir(hash_usuarios);
	hash_usuarios = NULL;
	return 0;
}

TUsuario* Usuario_new(
	unsigned int dni,
	char* nombre,
	char* apellido,
	int c_mail,
	char *mails[],
	char* pass,
	char* prov,
	char t_u
){
	size_t size = 0;
	SerializadorData data = {0};

	// Serializo DNI
	uint8_t *buf = Serializador_pack(NULL, SER_INT, (SerializadorData*) dni, &size);

	// Serializo nombre
	data.buf = (uint8_t*) nombre;
	data.size = strlen(nombre) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	// Serializo apellido
	data.buf = (uint8_t*) apellido;
	data.size = strlen(apellido) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	//Serializo cantidad de mails
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) c_mail, &size);
	// mails
	int i;
	for(i=0; i < c_mail ; i++){
		data.buf = (uint8_t*) mails[i];
		data.size = strlen(mails[i])+1;
		buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);
	}

	// Serializo pass
	data.buf = (uint8_t*) pass;
	data.size = strlen(pass) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	// Serializo prov
	data.buf = (uint8_t*) prov;
	data.size = strlen(prov) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	// t_u
	int t_u_int = t_u;
	buf = Serializador_pack(buf, SER_CHAR, (SerializadorData*) t_u_int, &size);

	if(HashExtensible_insertar(hash_usuarios, buf, size)){
		free(buf);
		return NULL;
	}

	TUsuario* user = _usuarioDesdeBuf(buf, size);
	free(buf);

	return user;
}

TUsuario* _usuarioDesdeBuf(uint8_t* buf, size_t buf_size){
	TUsuario* this = (TUsuario*) calloc(1, sizeof(TUsuario));
	SerializadorData data;
	size_t size = 0;
	size_t pos = 0;
	data.buf = buf;
	data.size = buf_size;

	//Dni
	this->dni = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));

	//Nombre
	char * str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->nombre = strcpy(malloc(size), str_aux);

	//Apellido
	str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->apellido = strcpy(malloc(size), str_aux);

	//Cantidad de emails
	this->c_mail = * ( (int*) Serializador_unpack(&data, SER_INT, &size, &pos));

	//Emails
	int i;
	for(i=0; i < this->c_mail; i++){
		str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
		this->mails[i] = strcpy(malloc(size), str_aux);
	}

	//Pass
	str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->pass = strcpy(malloc(size), str_aux);

	//Prov
	str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->prov = strcpy(malloc(size), str_aux);

	//t_u
	this->t_u = * ( (char*) Serializador_unpack(&data, SER_CHAR, &size, &pos));

	return this;
}

int Usuario_free(TUsuario* this){
	if(!this)
		return 1;

	free(this->nombre);
	free(this->apellido);
	int i;
	for(i=0; i < this->c_mail ; i++){
		free(this->mails[i]);
	}
	free(this->pass);
	free(this->prov);
	free(this);
	return 0;
}

TUsuario* Usuario_from_dni(unsigned int dni){
	uint8_t* buf;
	size_t size;

	if( ! (buf = HashExtensible_get(hash_usuarios, dni, &size))){
		return NULL;
	}

	TUsuario* user = _usuarioDesdeBuf(buf, size);
	free(buf);

	return user;
}

const char* Usuario_get_password(TUsuario* this){
	if(!this)
		return NULL;
	return this->pass;
}
