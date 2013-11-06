#include "consultas.h"
#include <string.h>
#include "hash_extensible.h"
#include "config.h"
#include "serializador.h"
#include "autoincrement.h"

THashExtensible* hash_consultas = NULL;
unsigned int consultas_last_id = 0;

struct TConsulta {
	unsigned int id;
	unsigned int id_serv;
	unsigned int dni;
	char *consulta;
	char *fecha;
	char *hora;
	char hay_rta;
	char *rta;
	char *rta_fecha;
	char *rta_hora;
	char oculta;
};

unsigned int Consultas_get_id(uint8_t* ele, size_t size){
	unsigned int id = * ( (unsigned int*) ele);
	return id;
}

TConsulta* _consultaDesdeBuf(uint8_t* buf, size_t size);

uint8_t* _consBufDesdeData(
	unsigned int id,
	unsigned int id_serv,
	unsigned int dni,
	char* consulta,
	char* fecha,
	char* hora,
	char hay_rta,
	char *rta,
	char *rta_fecha,
	char *rta_hora,
	char oculta,
	size_t *size_out
);

int Consultas_init(){
	hash_consultas = HashExtensible_crear(
		CONSULTAS_HASH_BLOQUE_PATH,
		CONSULTAS_HASH_TABLA_PATH,
		CONSULTAS_HASH_BAJA_PATH,
		CONSULTAS_HASH_BLOQUE_SIZE,
		&HashDispersionModulo,
		&Consultas_get_id
	);

	if(!hash_consultas)
		return 1;

	consultas_last_id = getLastId(CONSULTAS_LAST_ID_FILE);

	return 0;
}

int Consultas_end(){
	HashExtensible_destruir(hash_consultas);
	writeLastId(CONSULTAS_LAST_ID_FILE, consultas_last_id);
	hash_consultas = NULL;
	consultas_last_id = 0;
	return 0;
}

TConsulta* Consulta_new(
	unsigned int id_serv,
	unsigned int dni,
	char* consulta,
	char* fecha,
	char* hora
){
	unsigned int last_id = consultas_last_id;
	size_t size = 0;
	uint8_t* buf = _consBufDesdeData(
		last_id+1,
		id_serv,
		dni,
		consulta,
		fecha,
		hora,
		0,
		NULL,
		NULL,
		NULL,
		'n',
		&size
	);

	if(HashExtensible_insertar(hash_consultas, buf, size)){
		free(buf);
		return NULL;
	}

	TConsulta* cons = _consultaDesdeBuf(buf, size);
	free(buf);

	consultas_last_id++;
	return cons;
}

uint8_t* _consBufDesdeData(
	unsigned int id,
	unsigned int id_serv,
	unsigned int dni,
	char* consulta,
	char* fecha,
	char* hora,
	char hay_rta,
	char *rta,
	char *rta_fecha,
	char *rta_hora,
	char oculta,
	size_t *size_out
){
	size_t size = 0;
	SerializadorData data = {0};

	// Serializo id
	uint8_t *buf = Serializador_pack(NULL, SER_INT, (SerializadorData*) id, &size);

	// Serializo id_serv
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) id_serv, &size);

	// Serializo dni
	buf = Serializador_pack(buf, SER_INT, (SerializadorData*) dni, &size);

	// Serializo consulta
	data.buf = (uint8_t*) consulta;
	data.size = strlen(consulta) + 1;
	buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

	// Serializo fecha
	data.buf = (uint8_t*) fecha;
	data.size = 8 + 1;
	buf = Serializador_pack(buf, SER_FIX_BUF, &data, &size);

	// Serializo hora
	data.buf = (uint8_t*) hora;
	data.size = 4 + 1;
	buf = Serializador_pack(buf, SER_FIX_BUF, &data, &size);

	// hay_rta
	int hay_rta_int = hay_rta;
	buf = Serializador_pack(buf, SER_CHAR, (SerializadorData*) hay_rta_int, &size);

	if(hay_rta){
		// Serializo rta
		data.buf = (uint8_t*) rta;
		data.size = strlen(rta) + 1;
		buf = Serializador_pack(buf, SER_VAR_CHAR, &data, &size);

		// Serializo rta_fecha
		data.buf = (uint8_t*) rta_fecha;
		data.size = 8 + 1;
		buf = Serializador_pack(buf, SER_FIX_BUF, &data, &size);

		// Serializo rta_hora
		data.buf = (uint8_t*) rta_hora;
		data.size = 4 + 1;
		buf = Serializador_pack(buf, SER_FIX_BUF, &data, &size);
	}

	// oculta
	int oculta_int = oculta;
	buf = Serializador_pack(buf, SER_CHAR, (SerializadorData*) oculta_int, &size);

	*size_out = size;
	return buf;
}


TConsulta* _consultaDesdeBuf(uint8_t* buf, size_t buf_size){
	TConsulta* this = (TConsulta*) calloc(1, sizeof(TConsulta));
	SerializadorData data;
	size_t size = 0;
	size_t pos = 0;
	data.buf = buf;
	data.size = buf_size;

	//id
	this->id = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));

	//id_serv
	this->id_serv = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));

	//dni
	this->dni = * ( (unsigned int*) Serializador_unpack(&data, SER_INT, &size, &pos));

	//consulta
	char * str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
	this->consulta = strcpy(malloc(size), str_aux);

	//fecha
	size = 9;
	str_aux = (char*) Serializador_unpack(&data, SER_FIX_BUF, &size, &pos);
	this->fecha = strcpy(malloc(size), str_aux);

	//hora
	size = 5;
	str_aux = (char*) Serializador_unpack(&data, SER_FIX_BUF, &size, &pos);
	this->hora = strcpy(malloc(size), str_aux);

	// hay_rta
	this->hay_rta = * ( (char*) Serializador_unpack(&data, SER_CHAR, &size, &pos));
	if(this->hay_rta){
		//rta
		str_aux = (char*) Serializador_unpack(&data, SER_VAR_CHAR, &size, &pos);
		this->rta = strcpy(malloc(size), str_aux);

		//rta_fecha
		size = 9;
		str_aux = (char*) Serializador_unpack(&data, SER_FIX_BUF, &size, &pos);
		this->rta_fecha = strcpy(malloc(size), str_aux);

		//rta_hora
		size = 5;
		str_aux = (char*) Serializador_unpack(&data, SER_FIX_BUF, &size, &pos);
		this->rta_hora = strcpy(malloc(size), str_aux);
	}

	// oculta
	this->oculta = * ( (char*) Serializador_unpack(&data, SER_CHAR, &size, &pos));

	return this;
}

int Consulta_free(TConsulta* this){
	if(!this)
		return 1;

	free(this->consulta);
	free(this->fecha);
	free(this->hora);
	free(this->rta);
	free(this->rta_fecha);
	free(this->rta_hora);
	free(this);
	return 0;
}

TConsulta* Consulta_from_id(unsigned int id){
	uint8_t* buf;
	size_t size;

	if( ! (buf = HashExtensible_get(hash_consultas, id, &size))){
		return NULL;
	}

	TConsulta* cons = _consultaDesdeBuf(buf, size);
	free(buf);

	return cons;
}

unsigned int Consulta_get_id(TConsulta* this){
	if(!this)
		return 0;
	return this->id;
}
unsigned int Consulta_get_id_serv(TConsulta* this){
	if(!this)
		return 0;
	return this->id_serv;
}
unsigned int Consulta_get_dni(TConsulta* this){
	if(!this)
		return 0;
	return this->dni;
}
const char* Consulta_get_consulta(TConsulta* this){
	if(!this)
		return NULL;
	return this->consulta;
}
const char* Consulta_get_fecha(TConsulta* this){
	if(!this)
		return NULL;
	return this->fecha;
}
const char* Consulta_get_hora(TConsulta* this){
	if(!this)
		return NULL;
	return this->hora;
}
char Consulta_get_hay_rta(TConsulta* this){
	if(!this)
		return 0;
	return this->hay_rta;
}
int Consulta_set_hay_rta(TConsulta* this, char hay_rta){
	if(!this)
		return 1;
	this->hay_rta = hay_rta;
	return 0;
}
const char* Consulta_get_rta(TConsulta* this){
	if(!this)
		return NULL;
	return this->rta;
}
int Consulta_set_rta(TConsulta* this, char* rta){
	if(!this)
		return 1;

	free(this->rta);
	this->rta = strcpy(malloc(strlen(rta)+1), rta);
	return 0;
}
const char* Consulta_get_rta_fecha(TConsulta* this){
	if(!this)
		return NULL;
	return this->rta_fecha;
}
int Consulta_set_rta_fecha(TConsulta* this, char* fecha){
	if(!this)
		return 1;

	free(this->rta_fecha);
	this->rta_fecha = strcpy(malloc(strlen(fecha)+1), fecha);
	return 0;
}
const char* Consulta_get_rta_hora(TConsulta* this){
	if(!this)
		return NULL;
	return this->rta_hora;
}
int Consulta_set_rta_hora(TConsulta* this, char* hora){
	if(!this)
		return 1;
	free(this->rta_hora);
	this->rta_hora = strcpy(malloc(strlen(hora)+1), hora);
	return 0;
}
char Consulta_get_oculta(TConsulta* this){
	if(!this)
		return 0;
	return this->oculta;
}
int Consulta_set_oculta(TConsulta* this, char o){
	if(!this)
		return 1;
	this->oculta = o;
	return 0;
}

int Consultas_store(TConsulta* this){
	if(!this)
		return 1;

	size_t size = 0;
	uint8_t* buf =  HashExtensible_del(hash_consultas, this->id, &size);
	if(!buf)
		return 1;

	free(buf);

	buf = _consBufDesdeData(
		this->id,
		this->id_serv,
		this->dni,
		this->consulta,
		this->fecha,
		this->hora,
		this->hay_rta,
		this->rta,
		this->rta_fecha,
		this->rta_hora,
		this->oculta,
		&size
	);

	if(HashExtensible_insertar(hash_consultas, buf, size)){
		free(buf);
		return 1;
	}

	free(buf);
	return 0;
}

