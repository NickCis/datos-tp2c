#ifndef __STDIN_IO_H__
#define __STDIN_IO_H__
#include <stdlib.h>
/** Lee dni por stdin, si es un dni invalido devuelve 0;
 */
unsigned int get_dni();

/** Lee str desde stdin hasta max_len. str ya debe tener malloqueado max_len +1!
 */
void read_str(char* str, size_t max_len);

/** Lee char de stdin y lo devuelve (saca sobrante de stdin)
 */
char read_opt();

#endif
