#include "usuarios.h"
#include "hash_extensible.h"
#include "config.h"
#include "serializador.h"

// Hash usado para guardar en disco a los usuarios
THashExtensible* hash_usuarios = NULL;

struct TUsuario {
	unsigned int dni;
	char *nombre;
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
	int c_mail,
	char *mails[],
	char* pass,
	char* prov,
	char t_u
){
	//uint8_t buf = NULL;
	//size_t size = sizeof(unsigned int) + sizeof(int) + sizeof(char);
	return NULL;
}

