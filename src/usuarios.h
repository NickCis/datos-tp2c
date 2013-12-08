#ifndef __USUARIOS_H__
#define __USUARIOS_H__

typedef struct TUsuario TUsuario;

/** Inicializa el uso de usuarios.
 */
int Usuarios_init();

/** Finaliza el uso de usuarios
 */
int Usuarios_end();


/** Crea un nuevo usuario, lo agrega al hash.
 * @param dni
 * @param nombre
 * @param apellido
 * @param c_mail cantidad de mails a agregar (puede ser 0)
 * @param mails array de char* de mails a agregar
 * @param pass contrase~na
 * @param prov provincia
 * @param t_u tipo de usuario ( 'u' -> usuario normal, 'p' -> proveedor, 'a' -> admin, 's' -> superadmin)
 * @return tda de usuario
 */
TUsuario* Usuario_new(
	unsigned int dni,
	char* nombre,
	char* apellido,
	int c_mail,
	char *mails[],
	char* pass,
	char* prov,
	char t_u
);

/** Obtiene un usuario desde su dni
 * @param dni
 * @return tda de usuario
 */
TUsuario* Usuario_from_dni(unsigned int dni);

/** Obtiene usuarios desde tipo de usuario
 * @param t_u tipo de usuario
 * @param len[out] largo de la lista
 * @return array de ids de usuario
 */
unsigned int* Usuario_from_t_u(char t_u, size_t *len);

/** Borra un usuario
 */
TUsuario* Usuario_del(unsigned int dni);

/** Metodos para obtener/modificar informacion del usuario.
 * Los sets devuelve 0 si salio todo bien.
 */
unsigned int Usuario_get_dni(TUsuario* this);
const char* Usuario_get_nombre(TUsuario* this);
int Usuario_set_nombre(TUsuario* this, char* nombre);
const char* Usuario_get_apellido(TUsuario* this);
int Usuario_set_apellido(TUsuario* this, char* nombre);
const char* Usuario_get_mail(TUsuario* this, int n);
int Usuario_set_mail(TUsuario* this, int n, char* mail);
int Usuario_get_mail_c(TUsuario* this);
int Usuario_set_mail_c(TUsuario* this, int mail_c);
const char* Usuario_get_password(TUsuario* this);
int Usuario_set_password(TUsuario* this, char* pass);
const char* Usuario_get_provincia(TUsuario* this);
int Usuario_set_provincia(TUsuario* this, char* prov);
char Usuario_get_tipo(TUsuario* this);
int Usuario_set_tipo(TUsuario* this, char t_u);

/** Graba cambios que se le hicieron al usuario.
 * Solo es necesario hacerlo si se hicieorn cambios de un usuario obtenido por Usuario_from_dni.
 * @return 0-> ok, resto error
 */
int Usuario_store(TUsuario* this);

/** Libera memroia para la entidad usuario
 */
int Usuario_free(TUsuario* this);


#endif
