#ifndef __COTIZACIONES_H__
#define __COTIZACIONES_H__
typedef struct TCotizacion TCotizacion;

/** Inicializa el uso de Cotizaciones.
 */
int Cotizaciones_init();

/** Finaliza el uso de cotizaciones
 */
int Cotizaciones_end();

/**Crea una nueva cotizacion.
 * @param id_serv id del servicio
 * @param dni del usuario
 * @param pedido
 * @param fecha
 * @param hora
 * @return tda de cotizacion
 */
TCotizacion* Cotizacion_new(
	unsigned int id_serv,
	unsigned int dni,
	char* pedido,
	char* fecha,
	char* hora
);

/** Trae una cotizaci&oacute;n buscandola desde la id.
 * @param id de la cotizacion
 * @return tda de cotizacion
 */
TCotizacion* Cotizacion_from_id(unsigned int id);

/** Obtiene cotizaciones desde id servicio
 * @param id_serv id servicio
 * @param len[out] largo de la lista
 * @return array de ids de usuario
 */
unsigned int* Cotizacion_from_id_serv(unsigned int id_serv, size_t *len);

/** Obtiene cotizaciones desde id usuario
 * @param id_usu id del usuario
 * @param len[out] largo de la lista
 * @return array de ids de usuario
 */
unsigned int* Cotizacion_from_id_usu(unsigned int id_usu, size_t *len);

/** Metodos para obtener informacion de la cotizacion.
 */
unsigned int Cotizacion_get_id(TCotizacion* this);
unsigned int Cotizacion_get_id_serv(TCotizacion* this);
unsigned int Cotizacion_get_dni(TCotizacion* this);
const char* Cotizacion_get_pedido(TCotizacion* this);
const char* Cotizacion_get_fecha(TCotizacion* this);
const char* Cotizacion_get_hora(TCotizacion* this);

/** Libera memroia para la entidad cotizacion
 */
int Cotizacion_free(TCotizacion* this);

/** Borra cotizaci&oacute;n.
 * @param id de la cotizacion
 * @return tda de cotizacion
 */
TCotizacion* Cotizacion_del(unsigned int id);

#endif
