#ifndef __SERVICIOS_H__
#define __SERVICIOS_H__
#include <stdlib.h>
typedef struct TServicio TServicio;

/** Inicializa el uso de Servicios.
 */
int Servicios_init();

/** Finaliza el uso de usuarios
 */
int Servicios_end();


/** Crea un nuevo servicio, lo agrega al hash.
 * @param dni_prov dni del proveedor
 * @param nombre
 * @param desc
 * @param tipo
 * @return tda de servicio
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

/** Itera trayendo todos los servicios de un proveedor.
 * @param dni_prov de la cotizacion
 * @param id_p inicialmente se debe setear en 0
 * @return tda de cotizacion o NULL si no hay mas.
 */
TServicio* Servicio_from_dni_prov(unsigned int dni_prov, unsigned int *id_p);

/** Devuelve una lista de ids de servicios de la categoria.
 * @param id_cat  id de la categoria
 * @param len[out] largo de la categoria
 * @return array de ids de servicios de la categoria.
 */
unsigned int* Servicio_from_categoria(unsigned int id_cat, size_t *len);

/** Agrega una categoria al servicio.
 * @param id del servicio
 * @param id_cat id de la categoria
 * @return 0 ok, resto error
 */
int Servicio_agregar_categoria(unsigned int id, unsigned int id_cat);

/** Busca utilizando RTT el servicio sobre el campo descripci&oacute;n
 * @param t palabra a buscar
 * @param len[out] largo de la lista devuelta
 * @return array de ids de servicios.
 */
unsigned int* Servicio_buscar(char* t, size_t* len);

/** Borra un servicio
 * @param id
 * @return tda del servicio borrado.
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
int Servicio_free(TServicio* this);


#endif
