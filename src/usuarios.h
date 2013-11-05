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

TUsuario* Usuario_from_dni(unsigned int dni);

const char* Usuario_get_password(TUsuario* this);

/** Libera ram alocada para la entidad usuario
 */
int Usuario_free(TUsuario* this);


#endif
