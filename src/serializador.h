#ifndef __SERIALIZADOR_H__
#define __SERIALIZADOR_H__
#include <stdlib.h>
#include <stdint.h>
typedef enum {
	SER_VAR_CHAR, // <- char* de largo variable (se usa prefijo de longitud)
	SER_FIX_BUF, // <- buffer de largo fijo especificado por el usuario en la lectura(no guarda prefijo de longitud)
	SER_INT, // <- int
	SER_CHAR // <- char
} SerializadorTipos;

typedef struct {
	uint8_t* buf;
	size_t size;
} SerializadorData;

/** Genera un buffer serializado.
 * La primera vez que se empieza a serializar buf debe ser NULL.
 * Si se usa tipo SER_INT y SER_CHAR, se esta suponiendo que en ver de pasar un SerilizadorData* como data, se esta pasando un int y char respectivamente.
 *
 * @param buf[in]: puntero a buffer serailizado anterior (se realloquea este puntero)
 * @param tipo[in]: tipo a serializar
 * @param data[in]: data a serailizar (se debe crear a mano)
 * @param size[in/out]: inicialmente debe tener el tama~no anterior (o 0), despues va a tener el nuevo tama~no del buffer
 * @return puntero de buffer (se debe liberar!).
 */
uint8_t* Serializador_pack(uint8_t* buf, SerializadorTipos tipo, SerializadorData* data, size_t* size);

/** Deserializa la informacion, no malloquea, sino utiliza el buffer de data.
 * Data debe estar inicializada con buf, el buffer donde se encuentra la informacion y size con el tama~no del buffer, pos en unppcio debe pasars ecomo 0, se usara intermanete para saber que registro buscar.
 * @param data: el buffer que tiene toda la informacion a deserializar
 * @param tipo: el tipo de lo qe se esta deserializando
 * @param size[in/out]: tama~no de lo qe se deserializo, en tipo SER_FIX_BUF se usa como tama~no de la data serializada
 * @param pos[uso interno]: se debe pasar inicialmente en 0, y dsp usar el valor que tiene.
 * @return puntero de la data serializada casteada a uint8_t*, no malloquea, usa memoria del buffer!
 */
uint8_t* Serializador_unpack(SerializadorData* data, SerializadorTipos tipo, size_t* size, size_t* pos);

#endif
