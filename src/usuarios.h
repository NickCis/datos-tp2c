#ifndef __USUARIOS_H__
#define __USUARIOS_H__

typedef struct TUsuario TUsuario;

/** Inicializa el uso de usuarios.
 */
int Usuarios_init();

/** Finaliza el uso de usuarios
 */
int Usuarios_end();


TUsuario* Usuario_new(
	unsigned int dni,
	char* nombre,
	int c_mail,
	char *mails[],
	char* pass,
	char* prov,
	char t_u
);


#endif
