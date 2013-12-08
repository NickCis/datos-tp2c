#ifndef __CIFRADOR_HILL_
#define __CIFRADOR_HILL_

/** Comprueba si un texto esta en el abecedario.
 * @param text, texto
 * @param out[out] texto normalizado (se debe pasar un puntero malloqueado)
 */
void normalizar(const char* text, char* out);

void str_to_base_n(int* matrix, int* clave);

/** Dencripta
 */
int decrypt(const char* clave, const char* palabra, char* out);

/** Encripta
 */
int encrypt(const char* clave, const char* palabra, char* out);

/** Valida que una clave cumpla con los requisitos.
 * @param clave
 * @return 0 si es valida, resto si no lo es.
 */
int validar_clave(const char* clave);
#endif
