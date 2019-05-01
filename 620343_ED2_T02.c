/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 02 - Árvore B
 *
 * RA: 
 * Aluno: 
 * ========================================================================== */

/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51
#define TAM_STRING_INDICE (TAM_MARCA + TAM_NOME)


#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define MAX_ORDEM 150
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)


/* Saídas do usuário */
#define OPCAO_INVALIDA 				"Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE 		"Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO 		"Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO 				"Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA 			"ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO 				"Arquivo vazio!"
#define INICIO_BUSCA 				"********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM				"********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 			"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO				"********************************ARQUIVO*******************************\n"
#define INICIO_INDICE_PRIMARIO      "***************************INDICE PRIMÁRIO****************************\n"
#define INICIO_INDICE_SECUNDARIO    "***************************INDICE SECUNDÁRIO**************************\n"
#define SUCESSO  				 	"OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 	"FALHA AO REALIZAR OPERACAO!\n"
#define NOS_PERCORRIDOS_IP 			"Busca por %s. Nos percorridos:\n"
#define NOS_PERCORRIDOS_IS 			"Busca por %s.\nNos percorridos:\n"


/* Registro do Produto */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Produto;


/*Estrutura da chave de um nó do Índice Primário*/
typedef struct Chaveip
{
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave_ip;

/*Estrutura da chave de um  do Índice Secundário*/
typedef struct Chaveis
{
	char string[TAM_STRING_INDICE];
	char pk[TAM_PRIMARY_KEY];
} Chave_is;

/* Estrutura das Árvores-B */
typedef struct nodeip
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_ip *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_ip;

typedef struct nodeis
{
	int num_chaves;		/* numero de chaves armazenadas*/
	Chave_is *chave;		/* vetor das chaves e rrns [m-1]*/
	int *desc;	/* ponteiros para os descendentes, *desc[m]*/
	char folha;			/* flag folha da arvore*/
} node_Btree_is;

typedef struct {
	int raiz;
} Indice;

/* Variáveis globais */
char ARQUIVO[MAX_REGISTROS * TAM_REGISTRO + 1];
char ARQUIVO_IP[2000*sizeof(Chave_ip)];
char ARQUIVO_IS[2000*sizeof(Chave_is)];
/* Ordem das arvores */
int ordem_ip;
int ordem_is;
int nregistros;
int nregistrosip; /*Número de nós presentes no ARQUIVO_IP*/
int nregistrosis; /*Número de nós presentes no ARQUIVO_IS*/
/*Quantidade de bytes que ocupa cada nó da árvore nos arquivos de índice:*/
int tamanho_registro_ip;
int tamanho_registro_is;
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
 /********************FUNÇÕES DO MENU*********************/
 void cadastrar(Indice* iprimary, Indice* ibrand);
 
 int alterar(Indice iprimary);
 
 void buscar(Indice iprimary,Indice ibrand);
 
 void listar(Indice iprimary,Indice ibrand);
 
 /*******************************************************/
 
 /* Recebe do usuário uma string simulando o arquivo completo e retorna o número
  * de registros. */
 int carregar_arquivo();
 
/* (Re)faz o Cria iprimary*/
void criar_iprimary(Indice *iprimary);
 
/* (Re)faz o índice de jogos  */
void criar_ibrand(Indice *ibrand) ;
 
/*Escreve um nó da árvore no arquivo de índice,
* O terceiro parametro serve para informar qual indice se trata */
void write_btree(void *salvar, int rrn, char ip);
 
/*Lê um nó do arquivo de índice e retorna na estrutura*/
void *read_btree(int rrn, char ip);
 
/* Aloca um nó de árvore para ser utilizado em memória primária, o primeiro parametro serve para informar que árvore se trata
* É conveniente que essa função também inicialize os campos necessários com valores nulos*/
void *criar_no(char ip);
 
/*Libera todos os campos dinâmicos do nó, inclusive ele mesmo*/
void libera_no(void *node, char ip);
 
/*
*   Caro aluno,
*   caso não queira trabalhar com void*, é permitido dividir as funções que utilizam
* em duas, sendo uma para cada índice...
* Um exemplo, a write_btree e read_btree ficariam como:
*
*   void write_btree_ip(node_Btree_ip *salvar, int rrn),  node_Btree_ip *read_btree_ip(int rrn),
*   void write_btree_is(node_Btree_is *salvar, int rrn) e node_Btree_is *read_btree_is(int rrn).
*/

node_Btree_ip *read_btree_ip(int rrn);

node_Btree_is *read_btree_is(int rrn);
 
/* Atualiza os dois índices com o novo registro inserido */
void inserir_registro_indices(Indice *iprimary, Indice *ibrand, Jogo j);

void inserir_ip(Indice *iprimary, Chave_ip key); 

void inserir_is(Indice *ibrand, Chave_is key); 

/* Exibe o jogo */
int exibir_registro(int rrn);


/* FUNÇÕES */
void receber_dados(Produto *novo);

Produto recuperar_registro(int rrn);

int alterarDesconto(Indice *iprimary, Isf *iprice, Is *iproduct, int nregistros, char *pk);

void listagemPorPk(Indice *iprimary, int nregistros);

void criar_iprimary(Indice *iprimary, int* nregistros);


int main()
{
	char *p; /* # */
  /* Arquivo */
	int carregarArquivo = 0; nregistros = 0, nregistrosip = 0, nregistrosis = 0;
	scanf("%d\n", &carregarArquivo); /* 1 (sim) | 0 (nao) */
	if (carregarArquivo)
		nregistros = carregar_arquivo();

	scanf("%d %d%*c", &ordem_ip, &ordem_is);

	tamanho_registro_ip = ordem_ip*3 + 4 + (-1 + ordem_ip)*14;
	tamanho_registro_is = ordem_is*3 + 4 + (-1 + ordem_is)* (	TAM_STRING_INDICE +9);

	/* Índice primário */
	Indice iprimary ;
	criar_iprimary(&iprimary);

	/* Índice secundário de nomes dos Produtos */
	Indice ibrand;
	criar_ibrand(&ibrand);

	/* Execução do programa */
	int opcao = 0;
	while(1)
	{
		scanf("%d%*c", &opcao);
		switch(opcao) {
		case 1: /* Cadastrar um novo Produto */
			cadastrar(&iprimary, &ibrand);
			break;
		case 2: /* Alterar o desconto de um Produto */
			printf(INICIO_ALTERACAO);
			if (alterar(iprimary))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3: /* Buscar um Produto */
			printf(INICIO_BUSCA);
			buscar(iprimary, ibrand);
			break;
		case 4: /* Listar todos os Produtos */
			printf(INICIO_LISTAGEM);
			listar(iprimary, ibrand);
			break;
		case 5: /* Imprimir o arquivo de dados */
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;
		case 6: /* Imprime o Arquivo de Índice Primário*/
			printf(INICIO_INDICE_PRIMARIO);
			if(!*ARQUIVO_IP)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IP; *p!='\0'; p+=tamanho_registro_ip)
				{
					fwrite( p , 1 ,tamanho_registro_ip,stdout);
					puts("");
				}
			break;
		case 7: /* Imprime o Arquivo de Índice Secundário*/
			printf(INICIO_INDICE_SECUNDARIO);
			if(!*ARQUIVO_IS)
				puts(ARQUIVO_VAZIO);
			else
				for(p = ARQUIVO_IS; *p!='\0'; p+=tamanho_registro_is)
				{
					fwrite( p , 1 ,tamanho_registro_is,stdout);
					puts("");

				}
			break;
		case 8: /*Libera toda memória alocada dinâmicamente (se ainda houver) e encerra*/
			return 0;
		default: /* exibe mensagem de erro */
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return -1;
}

/* ==========================================================================
 * ================================= FUNÇÕES ================================
 * ========================================================================== */

// Aloca um nó de indice primario ou secundario
void *criar_no(char ip){
	int i;
	node_Btree_ip *novo_ip;
	node_Btree_is *novo_is;

	if(ip == 'ip'){

		novo_ip = (node_Btree_ip*) malloc (sizeof(node_Btree_ip) * 1);
		novo_ip->chave = (Chave_ip*) malloc (sizeof(Chave_ip) * (ordem_ip-1));
		novo_ip->desc = (int*) malloc(sizeof(int) * ordem_ip);
		for(i = 0; i < ordem_ip; i++)
			novo_ip->desc[i] = -1;

		return novo_ip;
	}
	if(ip == 'is'){

		novo_is = (node_Btree_is*) malloc (sizeof(node_Btree_is) * 1);
		novo_is->chave = (Chave_is*) malloc (sizeof(Chave_is) * (ordem_is-1));
		novo_is->desc = (int*) malloc(sizeof(int) * ordem_is);
		for(i = 0; i < ordem_is; i++)
			novo_is->desc[i] = -1;

		return novo_is;	
	}
}

/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
	scanf("%[^\n]\n", ARQUIVO);
	return strlen(ARQUIVO) / TAM_REGISTRO;
}


/* Exibe o Produto */
int exibir_registro(int rrn)
{
	if(rrn < 0)
		return 0;

	float preco;
	int desconto;
	Produto j = recuperar_registro(rrn);
    char *cat, categorias[TAM_CATEGORIA];
	
	printf("%s\n", j.pk);
	printf("%s\n", j.nome);
	printf("%s\n", j.marca);
	printf("%s\n", j.data);
	printf("%s\n", j.ano);
	sscanf(j.desconto,"%d",&desconto);
	sscanf(j.preco,"%f",&preco);
	preco = preco *  (100-desconto);
	preco = ((int) preco)/ (float) 100 ;
	printf("%07.2f\n",  preco);
	strcpy(categorias, j.categoria);
	
	for (cat = strtok (categorias, "|"); cat != NULL; cat = strtok (NULL, "|"))
    	printf("%s ", cat);
	printf("\n");

	return 1;
}


void receber_dados(Produto *novo){
	char str = (*char)malloc(TAM_NOME*sizeof(char));

    scanf("%[^\n]", str);
    strcpy(novo->nome, str);
    getchar();

    scanf("%[^\n]", str);
    strcpy(novo->marca, str);
    getchar();

    scanf("%[^\n]", str);
    strcpy(novo->data, str);
    getchar();

    scanf("%[^\n]", str);
    strcpy(novo->ano, str);
    getchar();

    scanf("%[^\n]", str);
    strcpy(novo->preco, str);
    getchar();

    scanf("%[^\n]", str);
    strcpy(novo->desconto, str);
    getchar();

    scanf("%[^\n]", str);
    strcpy(novo->categoria, str);
    getchar();

    gerarChave(&novo);	
}

// Altera o desconto de um produto
int alterarDesconto(Indice *iprimary, Isf *iprice, Is *iproduct, int nregistros, char *pk){
 
    Produto j;
    Isf *preco = (Isf*)malloc(sizeof(Isf));
    int i, rrn, desconto_pos;
    float novo_valor;
    char temp[193], novo_desconto[3];
 
    rrn = searchRrn(iprimary, pk, nregistros);

    if(rrn != -1){
        scanf("%[^\n]", novo_desconto);
        getchar();
        
        if(strlen(novo_desconto) == 3){
 
            j = recuperar_registro(rrn);
 
            desconto_pos = strlen(j.nome) + strlen(j.marca) + strlen(j.data) + strlen(j.ano) + strlen(j.preco) + 5 + (rrn*192);
 
            ARQUIVO[desconto_pos] = novo_desconto[0];
            ARQUIVO[desconto_pos+1] = novo_desconto[1];
            ARQUIVO[desconto_pos+2] = novo_desconto[2];
 
            // printf("VALOR ANTIGO: %.2f\n", iprice[rrn].price);
 
            novo_valor = calcPrice(atof(j.preco), atoi(novo_desconto));
  
            for(i=0; i < nregistros; i++){
                if(strcmp(iprice[i].pk, pk) == 0){
                    iprice[i].price = novo_valor;
                    qsort(iprice, nregistros, sizeof(Isf),(void*)cmpPrice);
                }
            }
         }
 
    }
    else{
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
 
    return 1;
}


// Listagem por chave primaria
void listagemPorPk(Indice *iprimary, int nregistros){
 
    int i, j;
 
    for(i=0, j=0; i < nregistros; i++){
    	if(iprimary[i].rrn != -1 && j > 0)
    		printf("\n");
        if(exibir_registro(iprimary[i].rrn, 0)){
            j++;
        }
    }
 
}


/* Recupera do arquivo o registro com o rrn
 * informado e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn)
{
    char temp[193], *p;
    strncpy(temp, ARQUIVO + ((rrn)*192), 192);
    temp[192] = '\0';
    Produto j;
    p = strtok(temp,"@");
    strcpy(j.pk,p);
    p = strtok(temp,"@");
    strcpy(j.nome,p);
    p = strtok(NULL,"@");
    strcpy(j.marca,p);
    p = strtok(NULL,"@");
    strcpy(j.data,p);
    p = strtok(NULL,"@");
    strcpy(j.ano,p);
    p = strtok(NULL,"@");
    strcpy(j.preco,p);
    p = strtok(NULL,"@");
    strcpy(j.desconto,p);
    p = strtok(NULL,"@");
    strcpy(j.categoria,p);

    return j;
}


/* (Re)faz o índice primario */
void criar_iprimary(Indice *iprimary){
 
    int rrn;
    Produto p;
    Chave_ip chave_ip;

    iprimary->raiz = -1;
 
    // Percorre o arquivo
    for(rrn=0; rrn < nregistros; rrn++){
        p = recuperar_registro(rrn);
  
        chave.rrn = rrn;
        strcpy(chave_ip.pk, p.pk);
        inserir_ip(iprimary, chave_ip);a
    }
    
}

void inserir_ip(Indice *iprimary, Chave_ip key){

	node_Btree_ip *novo;
	Chave_ip promovido;
	int filhoDir, flag=0;

	// Arvore vazia
	if(iprimary->raiz == -1){
		novo = criar_no('ip');
		novo->num_chaves = 1;
		novo->chave[0] = key;
		novo->folha = 'F';
		iprimary->raiz = 0;
		nregistrosip++;
		write_btree_ip(novo, 0);
	}
	else {

	}

}

// Insere um novo item no ARQUIVO e nos
void cadastrar(Indice* iprimary, Indice *ibrand){
 
    // Inicializa variaveis necessarias
    Produto novo;
    Chave_ip novo_ip;
    Chave_is novo_is;
    char *arquivo_aux;
    int tam, i, pos_categoria, res;

    // Recebe os dados a serem cadastrados e armazena na struct novo
    receber_dados(&novo);

    // Verifica se ja existe a PK
    res = verifica(novo.pk, iprimary->raiz);
    if(res != -1){

        // Tamanho do registro
        tam = strlen(novo.pk) + strlen(novo.nome) + strlen(novo.marca) + strlen(novo.data) + strlen(novo.ano) + strlen(novo.preco) + strlen(novo.desconto) + strlen(novo.categoria) + 8;

        // Escreve no final do arquivo
        sprintf(ARQUIVO + strlen(ARQUIVO), "%s@%s@%s@%s@%s@%s@%s@%s@", novo.pk, novo.nome, novo.marca, novo.data, novo.ano, novo.preco, novo.desconto, novo.categoria);

        // Completa o tamanho do registro caso precise
        if(tam < TAM_REGISTRO){
            arquivo_aux = ARQUIVO + strlen(ARQUIVO);
            for(i=tam; i < TAM_REGISTRO; i++){
                sprintf(arquivo_aux, "#");
                arquivo_aux++;
            }
        }

        // Cria o novo indice primario para ser inserido
	    strcpy(novo_ip.pk, novo.pk);
	    novo_ip.rrn = nregistros;

	    // Cria o novo indice secundario para ser inserido
	    strcpy(novo_is.pk, novo.pk);
	    sprintf(novo_is.string, "%s$%s", novo.brand, novo.nome);

	    // Insere nos indices
        inserir_ip(iprimary, novo_ip);
        inserir_is(ibrand, novo_is);

        nregistros++;
    }
    // Não existe PK
    else 
    	printf(ERRO_PK_REPETIDA);    
 
}

/* FUNCOES AUXILIARES */
int checkPk(char *pk, int rrn){
	int i, retorno;
	node_Btree_ip *no = read_btree_ip(rrn);

	if(rrn == -1){
		free(no);
		return -1;
	}

    i = 0;
	while((i < no->num_chaves) && (strcmp(pk, no->chave[i].pk) > 0)){
		i++;
	}
	
	if((i < no->num_chaves) && (strcmp(pk, no->chave[i].pk) == 0)){
		retorno = no->chave[i].rrn;
		free(no);
		return retorno;
	}

	if(no->folha == 'F'){
		free(no);
		return -1;
	}
	else{
		free(no);
		return checkPk(pk, no->desc[i]);
	}
}
