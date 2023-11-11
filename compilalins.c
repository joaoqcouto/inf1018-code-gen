#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Joao_Quintella_do_Couto 2010798 3WB */

/*
	Para ajudar no Assembly:
	
	Parâmetros:
	p1 = %edi -> -20(%rbp)
	p2 = %esi -> -24(%rbp)

	Variáveis locais:
	v1 = -4(%rbp) -> %eax para retorno no final
	v2 = -8(%rbp)
	v3 = -12(%rbp)
	v4 = -16(%rbp)
	
	Para operações:
	edi = operando 1
	esi = operando 2
	eax = resultado
*/

static void error (const char *msg, int line) {
	fprintf(stderr, "erro %s na linha %d\n", msg, line);
	exit(EXIT_FAILURE);
}

typedef int (*funcp) ();

void funcStart(unsigned char codigo[],int * iCodigo){
	unsigned char iniCodigo[] = {
								0x55,					// push %rbp
								0x48, 0x89, 0xe5,		// mov %rsp, %rbp	
							};
	for(int i = 0; i<4;i++) codigo[*iCodigo+i] = iniCodigo[i]; // coloca o codigo de inicio no vetor principal
	*iCodigo += 4;	// 4 casas do vetor foram preenchidas
}

void funcStartPilha(unsigned char codigo[],int * iCodigo){
	unsigned char iniCodigo[] = {
								0x55,					// push %rbp
								0x48, 0x89, 0xe5,		// mov %rsp, %rbp
								0x48, 0x83, 0xec, 0x20	// sub $32, %rsp (alocando espaco)
							};
	for(int i = 0; i<8;i++) codigo[*iCodigo+i] = iniCodigo[i]; // coloca o codigo de inicio no vetor principal
	*iCodigo += 8;	// 8 casas do vetor foram preenchidas
}


void funcRet(unsigned char codigo[],int * iCodigo){
	unsigned char retCodigo[] = {
								0xc9,		// leave
								0xc3		// ret
							};
	for(int i = 0; i<2;i++) codigo[*iCodigo+i] = retCodigo[i]; // coloca o codigo de retorno no vetor principal
	*iCodigo += 2;	// 2 casas do vetor foram preenchidas
}

void funcRetLins(unsigned char codigo[],int * iCodigo){ // coloca o retorno com o v1 no ret
	unsigned char retCodigo[5] = {
								0x8b, 0x45, 0xfc,		// mov -0x4(rbp),%eax
								0xc9,				// leave
								0xc3				// ret
							};
	for(int i = 0; i<5;i++) codigo[*iCodigo+i] = retCodigo[i]; // coloca o codigo de retorno no vetor principal
	*iCodigo += 5;	// 5 casas do vetor foram preenchidas
}

void funcoes(unsigned char codigo[],int * iCodigo){
	
	funcStart(codigo,iCodigo);
	unsigned char addCodigo[] = { // uma funcao add generica
								0x89, 0xf8,	// mov %edi, %eax
								0x01, 0xf0	// add %esi, %eax
							};
	for(int i = 0; i<4;i++) codigo[*iCodigo+i] = addCodigo[i]; // coloca o codigo da funcao add no vetor principal
	*iCodigo += 4; 
	funcRet(codigo,iCodigo);
	// add fica em codigo -> codigo + 9 (com o start e ret)

	funcStart(codigo,iCodigo);
	unsigned char subCodigo[] = { // uma funcao sub generica
								0x89, 0xf8,	// mov %edi, %eax
								0x29, 0xf0	// sub %esi, %eax
							};
	for(int i = 0; i<4;i++) codigo[*iCodigo+i] = subCodigo[i]; // coloca o codigo da funcao add no vetor principal
	*iCodigo += 4; 
	funcRet(codigo,iCodigo);
	// sub fica em codigo + 10 -> codigo + 19 (com o start e ret)

	funcStart(codigo,iCodigo);
	unsigned char imulCodigo[] = { // uma funcao imul generica
								0x89, 0xf8,			// mov %edi, %eax
								0x0f, 0xaf, 0xc6	// imul %esi, %eax
							};
	for(int i = 0; i<5;i++) codigo[*iCodigo+i] = imulCodigo[i]; // coloca o codigo da funcao add no vetor principal
	*iCodigo += 5;
	funcRet(codigo,iCodigo);
	// imul fica em codigo + 20 -> codigo + 30 (com o start e ret)

}

void paramSave(unsigned char codigo[],int * iCodigo){ // codigo para botar os parametros na pilha
	unsigned char paramCodigo[] = {
								0x89, 0x7d, 0xec,	// mov %edi,-0x14(%rbp)
								0x89, 0x75, 0xe8	// mov %esi,-0x18(%rbp)
							};
	for(int i = 0; i<6;i++) codigo[*iCodigo+i] = paramCodigo[i]; // coloca o codigo de retorno no vetor principal
	*iCodigo += 6;	// 6 casas do vetor foram preenchidas
}

void atribuicao(unsigned char codigo[],int * iCodigo,char var0,int idx0,char var1,int idx1,char op,char var2,int idx2){
	switch (var1) { // preparando parametro 1 da operacao
		case '$': {
			char codigo1[5] = {0xbf,(char)(idx1),(char)(idx1 >> 8),(char)(idx1 >> 16),(char)(idx1 >> 24)};// mov $(valor),%edi
			for(int i = 0; i<5;i++) codigo[*iCodigo+i] = codigo1[i];
			*iCodigo += 5;
			break;
		}
		case 'p': {
			char codigo1[3]= {0x8b,0x7d,0};
			switch (idx1) {
				case 1:; codigo1[2] = 0xec; break;	// mov -0x14(rbp),%edi (coloca param1 da pilha no param1)
				case 2:; codigo1[2] = 0xe8; break;	// mov -0x18(rbp),%edi (coloca param2 da pilha no param1)
			}

			for(int i = 0; i<3;i++) codigo[*iCodigo+i] = codigo1[i];
			*iCodigo += 3;
			break;
		}

		case 'v': {
			char codigo1[3] = {0x8b,0x7d,0};
			switch (idx1) {
				case 1:; codigo1[2] = 0xfc; break;	// mov -0x4(rbp),%edi (coloca var1 da pilha no param1)
				case 2:; codigo1[2] = 0xf8; break;	// mov -0x8(rbp),%edi (coloca var2 da pilha no param1)
				case 3:; codigo1[2] = 0xf4; break;	// mov -0xc(rbp),%edi (coloca var3 da pilha no param1)
				case 4:; codigo1[2] = 0xf0; break;	// mov -0x10(rbp),%edi (coloca var4 da pilha no param1)
			}
			for(int i = 0; i<3;i++) codigo[*iCodigo+i] = codigo1[i];
			*iCodigo += 3;
			break;
		}
	}
	switch (var2) { // preparando parametro 2 da operacao
		case '$': {
			char codigo2[5] = {0xbe,(char)(idx2),(char)(idx2 >> 8),(char)(idx2 >> 16),(char)(idx2 >> 24)};// mov $(valor),%esi
			for(int i = 0; i<5;i++) codigo[*iCodigo+i] = codigo2[i];
			*iCodigo += 5;
			break;
		}

		case 'p': {
			char codigo2[3] = {0x8b,0x75,0};
			switch (idx2) {
				case 1:; codigo2[2] = 0xec; break;	// mov -0x14(rbp),%esi (coloca param1 da pilha no param2)
				case 2:; codigo2[2] = 0xe8; break;	// mov -0x18(rbp),%esi (coloca param2 da pilha no param2)
			}
			for(int i = 0; i<3;i++) codigo[*iCodigo+i] = codigo2[i];
			*iCodigo += 3;
			break;
		}
		case 'v': {
			char codigo2[3] = {0x8b,0x75,0};
			switch (idx2) {
				case 1:; codigo2[2] = 0xfc; break;	// mov -0x4(rbp),%esi (coloca var1 da pilha no param2)
				case 2:; codigo2[2] = 0xf8; break;	// mov -0x8(rbp),%esi (coloca var2 da pilha no param2)
				case 3:; codigo2[2] = 0xf4; break;	// mov -0xc(rbp),%esi (coloca var3 da pilha no param2)
				case 4:; codigo2[2] = 0xf0; break;	// mov -0x10(rbp),%esi (coloca var4 da pilha no param2)
			}
			for(int i = 0; i<3;i++) codigo[*iCodigo+i] = codigo2[i];
			*iCodigo += 3;
			break;
		}
	}
	long end;
	switch (op) { // calls de cada operacao
		case '+': {
			end = (long)(codigo) - (long)(codigo + *iCodigo + 5);		// calculo do offset p/ call da funcao add
			break;
		}

		case '-': {
			end = (long)(codigo+10) - (long)(codigo + *iCodigo + 5);	// calculo do offset  p/ call da funcao sub
			break;
		}

		case '*': {
			end = (long)(codigo+20) - (long)(codigo + *iCodigo + 5);	// calculo do offset p/ call da funcao imul
			break;
		}
	}
	char codigoop[5] = {0xe8,(char)(end),(char)(end >> 8),(char)(end >> 16),(char)(end >> 24)}; // funcao call

	for(int i = 0; i<5;i++) codigo[*iCodigo+i] = codigoop[i];
	*iCodigo += 5;

	char codigoat[3] = {0x89,0x45,0};
	switch (var0) { // a quem atribuir a resposta da operacao
		case 'p': {
			switch (idx0) {
				case 1:; codigoat[2] = 0xec; break;	// mov %eax,-0x14(rbp) (coloca resp no param1 da pilha)
				case 2:; codigoat[2] = 0xe8; break;	// mov %eax,-0x18(rbp) (coloca resp no param2 da pilha)
			}
			break;
		}
		case 'v': {
			switch (idx0) {
				case 1:; codigoat[2] = 0xfc; break;	// mov %eax,-0x4(rbp) (coloca resp na var1 da pilha)
				case 2:; codigoat[2] = 0xf8; break;	// mov %eax,-0x8(rbp) (coloca resp na var2 da pilha)
				case 3:; codigoat[2] = 0xf4; break;	// mov %eax,-0xc(rbp) (coloca resp na var3 da pilha)
				case 4:; codigoat[2] = 0xf0; break;	// mov %eax,-0x10(rbp) (coloca resp na var4 da pilha)
			}
			break;
		}
	}
	for(int i = 0; i<3;i++) codigo[*iCodigo+i] = codigoat[i];
	*iCodigo += 3;
	
}

void desvio(unsigned char * codigo,int * iCodigo,int line,int * linhaCodigo,char var0,int idx0,int num) {
	unsigned char codigod[8]= {	0x8b,0x7d,0, // mov -?(rbp),%edi (coloca varp no edi pra comparar)
								0x83,0xff,0, // cmp $0,%edi (compara varp com 0)
								0x75,0		 // jne ? (se não for 0 pula com o offset ainda desconhecido)
							};		
	
	if(num<line) { // se a linha a desviar for anterior a atual
		int desvio = linhaCodigo[num];			// pegar a posição do início da linha no vetor
		int offset = desvio - (*iCodigo + 8);	// calculo do offset (primeira posição de num - prox posição livre)
		codigod[7] = (unsigned char)offset;		// coloca o offset no jne (posição ocupada por 0 no momento)
	}
	else linhaCodigo[num] = (*iCodigo + 8); // marca a linha futura com a posição livre após a função jne
	switch (var0) { // preparando o elemento a comparar
		case 'p': {
			switch (idx0) {
				case 1:; codigod[2] = 0xec; break;	// mov -0x14(rbp),%edi (coloca param1 da pilha no edi)
				case 2:; codigod[2] = 0xe8; break;	// mov -0x18(rbp),%edi (coloca param2 da pilha no edi)
			}
			break;
		}

		case 'v': {
			switch (idx0) {
				case 1:; codigod[2] = 0xfc; break;	// mov -0x4(rbp),%edi  (coloca var1 da pilha no edi)
				case 2:; codigod[2] = 0xf8; break;	// mov -0x8(rbp),%edi  (coloca var2 da pilha no edi)
				case 3:; codigod[2] = 0xf4; break;	// mov -0xc(rbp),%edi  (coloca var3 da pilha no edi)
				case 4:; codigod[2] = 0xf0; break;	// mov -0x10(rbp),%edi (coloca var4 da pilha no edi)
			}
		}
	}
	for(int i = 0; i<8;i++) codigo[*iCodigo+i] = codigod[i];
	*iCodigo += 8;
}

funcp compilaSimples(FILE *f, unsigned char * codigo){
	int iCodigo = 0;	//para guardar o proximo indice livre do vetor de codigo
	int line = 1;
	int linhaCodigo[50] = {0}; // para guardar a relacao linha x comando binario (inicializado com 0, limite de 50 linhas)
	linhaCodigo[0] = -1; // a linha 0 não tem nada (contagem de linhas em LinS começa na linha 1)
	int  c;
	funcoes(codigo,&iCodigo); 		// coloca as funcoes padrao
	funcStartPilha(codigo,&iCodigo);	// coloca o inicio da nova funcao
	paramSave(codigo,&iCodigo);		// salva os parametros iniciais na pilha
	while ((c = fgetc(f)) != EOF) {
		if((linhaCodigo[line])!=0) { // pra tratar possiveis jumps para a linha atual (ver funcao desvio)
			int origem = linhaCodigo[line];		// a origem do desvio é a colocada no vetor
			int offset = iCodigo - origem;		// offset = posicao para desviar (atual) - posicao apos desvio
			codigo[origem-1] = (char)offset;	// coloca offset no endereco do jump (fica 1 antes da origem)
		}		
		linhaCodigo[line]=iCodigo; // a próxima linha começa na próxima posição livre do vetor
		switch (c) {
			case 'r': { /* retorno */
        		char c0;
        		if (fscanf(f, "et%c", &c0) != 1)
          			error("comando invalido", line);
				funcRetLins(codigo,&iCodigo);	// coloca finalização e retorno
        		break;
      		}
		  	case 'v': 
		  	case 'p': {  /* atribuicao */
		    	int idx0, idx1, idx2;
		    	char var0 = c, var1, var2, op;
		    	if (fscanf(f, "%d = %c%d %c %c%d", &idx0, &var1, &idx1, &op, &var2, &idx2) != 6)
		        	error("comando invalido", line);
				atribuicao(codigo,&iCodigo,var0, idx0, var1, idx1, op, var2, idx2);
		    	break;
		  		}
		  	case 'i': { /* desvio */
			  	char var0;
			  	int idx0, num;
			  	if (fscanf(f, "f %c%d %d", &var0, &idx0, &num) != 3)
			  	    error("comando invalido", line);
				desvio(codigo, &iCodigo, line, linhaCodigo, var0, idx0, num);
			  	break;
		  	}
      		default: error("comando desconhecido", line);
    	}
    	line ++;
    	fscanf(f, " ");
  	}
	
	//for(int i=0;i<iCodigo;i++) printf("%d: %x\n",i,codigo[i]); // para checar se o vetor ta certo
	//for(int i=0;i<line;i++) printf("Linha %d: %d\n",i,linhaCodigo[i]); // para checar as linhas
	

	return (funcp)(codigo+31); //o nosso codigo comeca na posicao 31, 0->30 sao as funcoes auxiliares das operacoes
}
