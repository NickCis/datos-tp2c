#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

const char *alfabeto = "_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.?,-";
#define L_ABC 41

int euclides(int q, int x1, int x2, int b, int y1, int y2,int a);
void _decrypt(int inv[], int code[], int result[]);
void _encrypt(int matrix[], int word_code[], int result[]);

void normalizar(const char* text, char* out){
	char c, a;
	const char* abc;
	int esta;
	while( (c = *(text++) )){
		esta = 0;
		abc = alfabeto;
		while( (a = *(abc++) ) ){
			if(a == c){
				esta = 1;
				break;
			}
		}
		if(!esta){
			if(c >= 'a' && c <= 'z'){
				*(out++) = c + ('A' - 'a');
			}
		}else{
			*(out++) = c;
		}
	}
}

void str_to_base_n(int* matrix, int* clave){
	int is_present, i, pos;
	for (i = 0; i < 9; i++) {
		is_present = 0;
		pos  = 0;
		while(!is_present && pos < L_ABC){
			if(alfabeto[pos] == clave[i]){
				matrix[i] = pos;
				is_present = 1;
			}
			pos += 1;
		}
	}
}

void _decrypt(int inv[], int code[], int result[]){
	result[0] = (inv[0]*code[0]+inv[1]*code[1]+inv[2]*code[2]) % 41;
	result[1] = (inv[3]*code[0]+inv[4]*code[1]+inv[5]*code[2]) % 41;
	result[2] = (inv[6]*code[0]+inv[7]*code[1]+inv[8]*code[2]) % 41;

	result[0] = result[0] < 0 ? result[0] + 41 : result[0];
	result[1] = result[1] < 0 ? result[1] + 41 : result[1];
	result[2] = result[2] < 0 ? result[2] + 41 : result[2];
}

void _encrypt(int matrix[], int word_code[], int result[]){

	result[0] = (matrix[0]*word_code[0]+matrix[1]*word_code[1]+matrix[2]*word_code[2]) % 41;
	result[1] = (matrix[3]*word_code[0]+matrix[4]*word_code[1]+matrix[5]*word_code[2]) % 41;
	result[2] = (matrix[6]*word_code[0]+matrix[7]*word_code[1]+matrix[8]*word_code[2]) % 41;

	result[0] = result[0] < 0 ? result[0] + 41 : result[0];
	result[1] = result[1] < 0 ? result[1] + 41 : result[1];
	result[2] = result[2] < 0 ? result[2] + 41 : result[2];

}

int euclides(int q, int x1, int x2, int b, int y1, int y2,int a){
	int x1_swp, x2_swp, b_swp;
	bool next_step =  1;//verdadero

	while(next_step){
		x1_swp = x1;
		x2_swp = x2;
		b_swp  = b;

		q = (int) b/a;
		x1 = y1;
		x2 = y2;
		b  = a;
		y1 = x1_swp - (q*x1);
		y2 = x2_swp - (q*x2);
		a  = b_swp  - (q*a);

		if((a == 1) || (a == 0 )){
			next_step = 0;//falso
		}
	}
	if(a == 1)
		if(y2<0)
			return (y2+41);
		else
			return y2;
	else
		return 0;
}

int validar_clave(char* clave){
	int det, det_1, det_2, inv_det;
	char matrix[10];

	if(strlen(clave) != 9)
		return 1;

	normalizar(clave, matrix);

	// calculamos el determinante
	det_1 = matrix[0]*matrix[4]*matrix[8] + matrix[1]*matrix[5]*matrix[6] + matrix[2]*matrix[3]*matrix[7];
	det_2 = matrix[2]*matrix[4]*matrix[6] + matrix[5]*matrix[7]*matrix[0] + matrix[8]*matrix[1]*matrix[3];

	det = det_1 - det_2;

	if(det == 0)
		return 1;

	if(det < 0)
		det = (det % 41)+41;
	else
		det = det % 41;

	inv_det = euclides(0, 1, 0, 41, 0, 1, det);
	if(inv_det == 0){
		return 1;
	}

	return 0;
}

int encrypt(char* clave, char* palabra, char* out){
	bool is_present = 0;//falso
	int pos = 0;

	// matrix 3x3
	int result1[3];
	int word_code[6];
	int i;
	int matrix[9] = {0};

	for (i = 0; i < 9; i++) {
		is_present = 0;
		pos  = 0;
		while(!is_present && pos < 41){
			if(alfabeto[pos] == clave[i]){
				matrix[i] = pos;
				is_present = 1;
			}
			pos += 1;
		}
	}

	int size = strlen(palabra);
	out[size] = 0;
	while(size > 2){
		for (i = 0; i < 3; i++) {
			is_present = 0;
			pos  = 0;
			while(!is_present && pos < 41){
				if(alfabeto[pos] == palabra[i]){
					word_code[i] = pos;
					is_present = 1;
				}
				pos += 1;
			}
		}

		_encrypt(matrix, word_code, result1);
		for (i = 0; i < 3; i++)
			*(out++) =  alfabeto[result1[i]];

		palabra += 3;
		size -= 3;
	}

	if(size){
		word_code[0] = 0;
		word_code[1] = 0;
		word_code[2] = 0;
		for (i = 0; i < size; i++) {
			is_present = 0;
			pos  = 0;
			while(!is_present && pos < 41){
				if(alfabeto[pos] == palabra[i]){
					word_code[i] = pos;
					is_present = 1;
				}
				pos += 1;
			}
		}

		_encrypt(matrix, word_code, result1);
		for (i = 0; i < size; i++) {
			*(out++) =  alfabeto[result1[i]];
		}
	}

	return 0;
}

int decrypt(char* clave, char* palabra, char* out){
	bool is_present = 0;//falso
	int pos = 0;

	// matrix 3x3
	int adj[9];
	int inv[9] = {0};
	int det, det_1, det_2, inv_det;
	int result2[3];
	int word_code[6];
	int i;
	int matrix[9] = {0};


	for (i = 0; i < 9; i++) {
		is_present = 0;
		pos  = 0;
		while(!is_present && pos < 41){
			if(alfabeto[pos] == clave[i]){
				matrix[i] = pos;
				is_present = 1;
			}
			pos += 1;
		}
	}

	// adj(matrix)
	adj[0] = (matrix[4]*matrix[8] - matrix[5]*matrix[7]) % 41; // 0
	adj[1] = (matrix[1]*matrix[8] - matrix[2]*matrix[7]) % 41; // 3
	adj[2] = (matrix[1]*matrix[5] - matrix[2]*matrix[4]) % 41; // 6

	adj[3] = (matrix[3]*matrix[8] - matrix[5]*matrix[6]) % 41; // 1
	adj[4] = (matrix[0]*matrix[8] - matrix[2]*matrix[6]) % 41; // 4
	adj[5] = (matrix[0]*matrix[5] - matrix[2]*matrix[3]) % 41; // 7

	adj[6] = (matrix[3]*matrix[7] - matrix[4]*matrix[6]) % 41; // 2
	adj[7] = (matrix[0]*matrix[7] - matrix[1]*matrix[6]) % 41; // 5
	adj[8] = (matrix[0]*matrix[4] - matrix[1]*matrix[3]) % 41; // 8

	// calculamos el determinante
	det_1 = matrix[0]*matrix[4]*matrix[8] + matrix[1]*matrix[5]*matrix[6] + matrix[2]*matrix[3]*matrix[7];
	det_2 = matrix[2]*matrix[4]*matrix[6] + matrix[5]*matrix[7]*matrix[0] + matrix[8]*matrix[1]*matrix[3];

	det = det_1 - det_2;

	if(det < 0)
		det = (det % 41)+41;
	else
		det = det % 41;

	inv_det = euclides(0, 1, 0, 41, 0, 1, det);
	if(inv_det == 0){
		return -1;
	}

	for (i = 0; i < 9; i++) {
		if(i%2 == 0)
			inv[i] = ( adj[i] * inv_det ) % 41;
		else{
			inv[i] = ( (-1) * adj[i] * inv_det ) % 41;
		}

	}

	int size = strlen(palabra);
	out[size] = 0;
	while(size > 2){
		for (i = 0; i < 3; i++) {
			is_present = 0;
			pos  = 0;
			while(!is_present && pos < 41){
				if(alfabeto[pos] == palabra[i]){
					word_code[i] = pos;
					is_present = 1;
				}
				pos += 1;
			}
		}

		_decrypt(inv, word_code, result2);

		for (i = 0; i < 3; i++)
			*(out++) =  alfabeto[result2[i]];

		palabra += 3;
		size -= 3;
	}

	if(size){
		word_code[0] = 0;
		word_code[1] = 0;
		word_code[2] = 0;
		for (i = 0; i < size; i++) {
			is_present = 0;
			pos  = 0;
			while(!is_present && pos < 41){
				if(alfabeto[pos] == palabra[i]){
					word_code[i] = pos;
					is_present = 1;
				}
				pos += 1;
			}
		}

		if(size == 2){
			result2[1] = (word_code[1] * matrix[0] - matrix[3] * word_code[0]) / (matrix[0] * matrix[4] - matrix[1] * matrix[3]);
			result2[0] =  (word_code[0] - matrix[1] * result2[1]) / matrix[0];
		}else{
			result2[0] = word_code[0] / matrix[0];
		}
		//_decrypt(inv, word_code, result2);
		for (i = 0; i < size; i++)
			*(out++) =  alfabeto[result2[i]];
	}

	return 0;
}
