#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0


void decrypt(int inv[], int code[], int result[]){
  result[0] = (inv[0]*code[0]+inv[1]*code[1]+inv[2]*code[2]) % 41;
  result[1] = (inv[3]*code[0]+inv[4]*code[1]+inv[5]*code[2]) % 41;
  result[2] = (inv[6]*code[0]+inv[7]*code[1]+inv[8]*code[2]) % 41;

  result[0] = result[0] < 0 ? result[0] + 41 : result[0];
  result[1] = result[1] < 0 ? result[1] + 41 : result[1];
  result[2] = result[2] < 0 ? result[2] + 41 : result[2];
}

void encrypt(int matrix[], int word_code[], int result[]){

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

int main (int argc, char const* argv[])
{
  char *symbols = "_ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789.?,-";
  char *masterKey = "E_1FG5;-?";
  char *my_word   ="GOL";

  printf("Clave maestra: %s\n", masterKey);

  bool is_present = 0;//falso
  int pos = 0;

  // matrix 3x3
  int adj[9];
  int inv[9] = {0};
  int idn[9], det, det_1, det_2, inv_det;
  int result1[3], result2[3];
  int word_code[6];
  int i, diff;
 //char *string_encoded = "", string_decoded = "";
  int matrix[9] = {0};

  for (i = 0; i < 9; i++) {
    is_present = 0;
    pos  = 0;
    while(!is_present && pos < 41){
      if(symbols[pos] == masterKey[i]){
        matrix[i] = pos;
	//printf("\n %i",matrix[i]);
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

  if(det == 0)
    printf("\n La matriz no tiene inversa");
  else{
    if(det < 0)
      det = (det % 41)+41;
    else
      det = det % 41;

    inv_det = euclides(0, 1, 0, 41, 0, 1, det);
    if(inv_det == 0){
      printf("\n La clave no tiene inversa, debe escoger otra");
      return -1;
    }

    printf( "Matriz clave: \n" );
    for (i = 1; i < 10; i++) {
      printf( " | %i " , matrix[i-1]);
      if(i%3==0)
        printf( "\n");
    }
    printf("\n El determinante modulo 41 es = %i \n",inv_det);

    printf( "\n Inversa:\n ");
    for (i = 0; i < 9; i++) {
      if(i%2 == 0)
        inv[i] = ( adj[i] * inv_det ) % 41;
      else{
        inv[i] = ( (-1) * adj[i] * inv_det ) % 41;
      }

    }

    for (i = 1; i < 10; i++) {
      printf(" |%i ",inv[i-1]);
      if(i%3==0)
        printf( " \n" );
    }
    
  printf( "\n Identidad: \n");
    idn[0] = (matrix[0]*inv[0]+matrix[1]*inv[3]+matrix[2]*inv[6])%41;
    idn[1] = (matrix[0]*inv[1]+matrix[1]*inv[4]+matrix[2]*inv[7])%41;
    idn[2] = (matrix[0]*inv[2]+matrix[1]*inv[5]+matrix[2]*inv[8])%41;

    idn[3] = (matrix[3]*inv[0]+matrix[4]*inv[3]+matrix[5]*inv[6])%41;
    idn[4] = (matrix[3]*inv[1]+matrix[4]*inv[4]+matrix[5]*inv[7])%41;
    idn[5] = (matrix[3]*inv[2]+matrix[4]*inv[5]+matrix[5]*inv[8])%41;

    idn[6] = (matrix[6]*inv[0]+matrix[7]*inv[3]+matrix[8]*inv[6])%41;
    idn[7] = (matrix[6]*inv[1]+matrix[7]*inv[4]+matrix[8]*inv[7])%41;
    idn[8] = (matrix[6]*inv[2]+matrix[7]*inv[5]+matrix[8]*inv[8])%41;

    for (i = 1; i < 10; i++) {

      printf( " | %i" , (idn[i-1] + 41)%41);
      if(i%3==0)
       printf( " \n ");
    }


printf( "Como tenemos inversa, codificamos un texto...");

    for (i = 0; i < 3; i++) {
      is_present = 0;
      pos  = 0;
      while(!is_present && pos < 41){
        if(symbols[pos] == my_word[i]){
          word_code[i] = pos;
          is_present = 1;
        }
        pos += 1;
      }
    }
    printf( "\n Se codificará: %s " ,my_word);
printf(" ( ",word_code[0]);
printf(",", word_code[1]);
printf(",", word_code[2],")");


encrypt(matrix, word_code, result1);
printf( " \n Resultado de la codificacion (" ,result1[0]);
printf(",",result1[1]);
printf(",",result1[2],")");
printf(" \n Palabra: ");
    for (i = 0; i < 3; i++) {
      printf("%c",symbols[result1[i]]);

    }
   
    decrypt(inv, result1, result2);
    printf("\n Resultado de la decodificacion (",result2[0]);
printf( ",",result2[1]);
printf( ",",result2[2]);
printf( ")");
printf( "\nPalabra: ");

    for (i = 0; i < 3; i++) {

     printf("%c", symbols[result2[i]]);
    }
//printf("%s",string_decoded);
  }


  return 0;
}
