#ifndef __CONSULTAS_H__
#define __CONSULTAS_H__
#include <stdlib.h>

typedef struct TConsulta TConsulta;

/** Inicializa el uso de Consultas.
 */
int Consultas_init();

/** Finaliza el uso de usuarios
 */
int Consultas_end();


/** Crea un nuevo usuario, lo agrega al hash.
 */
TConsulta* Consulta_new(
	unsigned int id_serv,
	unsigned int dni,
	char* consulta,
	char* fecha,
	char* hora
);

/** Obtiene un servicio desde su id
 */
TConsulta* Consulta_from_id(unsigned int id);

/** Borra un servicio
 */
TConsulta* Consulta_del(unsigned int id);

unsigned int* Consulta_buscar(char* t, size_t* len);

/** Metodos para obtener/modificar informacion del usuario.
 * Los sets devuelve 0 si salio todo bien.
 */
unsigned int Consulta_get_id(TConsulta* this);
unsigned int Consulta_get_id_serv(TConsulta* this);
unsigned int Consulta_get_dni(TConsulta* this);
const char* Consulta_get_consulta(TConsulta* this);
const char* Consulta_get_fecha(TConsulta* this);
const char* Consulta_get_hora(TConsulta* this);
char Consulta_get_hay_rta(TConsulta* this);
int Consulta_set_hay_rta(TConsulta* this, char hay_rta);
const char* Consulta_get_rta(TConsulta* this);
int Consulta_set_rta(TConsulta* this, char* rta);
const char* Consulta_get_rta_fecha(TConsulta* this);
int Consulta_set_rta_fecha(TConsulta* this, char* fecha);
const char* Consulta_get_rta_hora(TConsulta* this);
int Consulta_set_rta_hora(TConsulta* this, char* hora);
char Consulta_get_oculta(TConsulta* this);
int Consulta_set_oculta(TConsulta* this, char o);


int Consulta_store(TConsulta* this);
/** Libera memroia para la entidad usuario
 */
int Consulta_free(TConsulta* this);

#endif
