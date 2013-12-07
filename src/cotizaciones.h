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
