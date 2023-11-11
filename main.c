#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Joao_Quintella_do_Couto 2010798 3WB */

typedef int (*funcp) ();

funcp compilaSimples(FILE *f, unsigned char * codigo);

int main (int argc, char * argv[]) {
	FILE *f;
	if ((f = fopen (argv[1], "r")) == NULL) {
		perror ("nao conseguiu abrir arquivo!");
		exit(1);
	}

	unsigned char codigo[1024];
	funcp funcao = compilaSimples(f,codigo);
	
	int ent1,ret;

	ent1=0;
	ret = funcao(ent1);
	printf("Retorno da funcao para %d: %d\n",ent1,ret);

	ent1=2;
	ret = funcao(ent1);
	printf("Retorno da funcao para %d: %d\n",ent1,ret);

	ent1=4;
	ret = funcao(ent1);
	printf("Retorno da funcao para %d: %d\n",ent1,ret);

	ent1=6;
	ret = funcao(ent1);
	printf("Retorno da funcao para %d: %d\n",ent1,ret);

	return 0;
}
