#ifndef __SERVICIOS_H__
#define __SERVICIOS_H__
typedef struct TServicio TServicio;

/** Inicializa el uso de Servicios.
 */
int Servicios_init();

/** Finaliza el uso de usuarios
 */
int Servicios_end();


/** Crea un nuevo usuario, lo agrega al hash.
 */
TServicio* Servicio_new(
	unsigned int dni_prov,
	char* nombre,
	char* desc,
	char tipo
);

/** Obtiene un servicio desde su id
 */
TServicio* Servicio_from_id(unsigned int id);

/**
 */
TServicio* Servicio_from_dni_prov(unsigned int dni_prov, unsigned int *id_p);

/** Borra un servicio
 */
TServicio* Servicio_del(unsigned int id);

/** Metodos para obtener/modificar informacion del usuario.
 * Los sets devuelve 0 si salio todo bien.
 */
unsigned int Servicio_get_id(TServicio* this);
unsigned int Servicio_get_dni_p(TServicio* this);
const char* Servicio_get_nombre(TServicio* this);
const char* Servicio_get_descripcion(TServicio* this);
char Servicio_get_tipo(TServicio* this);



/** Libera memroia para la entidad usuario
 */
int Servicios_free(TServicio* this);


#endif
