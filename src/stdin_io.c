#include "stdin_io.h"
#include <stdio.h>

unsigned int get_dni(){
	unsigned int dni = 0;
	char c;
	while((c = getchar()) != '\n'){
		switch(c){
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				dni = dni * 10 + c - '0';
				break;
			default:
				while( getchar() != '\n'){}
				return 0;
		}
	}
	return dni;
}

void read_str(char *str, size_t max_len){
	char c;
	int i = 0;
	str[max_len] = 0;
	while((c = getchar()) != '\n'){
		if(i >= max_len)
			continue;
		str[i++] = c;
		str[i] = 0;
	}
}

char read_opt(){
	char c = getchar();
	if(c != '\n'){
		while( getchar() != '\n');
	}
	return c;
}
