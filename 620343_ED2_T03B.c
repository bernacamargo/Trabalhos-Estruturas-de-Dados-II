/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com encadeamento
 *
 * RA: 620343
 * Aluno: Bernardo Pinheiro Camargo
 * ========================================================================== */

/* Bibliotecas */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY 11
#define TAM_NOME 51
#define TAM_MARCA 51
#define TAM_DATA 11
#define TAM_ANO 3
#define TAM_PRECO 8
#define TAM_DESCONTO 4
#define TAM_CATEGORIA 51

#define TAM_REGISTRO 192
#define MAX_REGISTROS 1000
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)

/* Saídas do usuário */
#define OPCAO_INVALIDA "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE "Memoria insuficiente!\n"
#define REGISTRO_N_ENCONTRADO "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA "ERRO: Ja existe um registro com a chave primaria: %s.\n\n"
#define ARQUIVO_VAZIO "Arquivo vazio!"
#define INICIO_BUSCA 							 "********************************BUSCAR********************************\n"
#define INICIO_LISTAGEM						  "********************************LISTAR********************************\n"
#define INICIO_ALTERACAO 						"********************************ALTERAR*******************************\n"
#define INICIO_ARQUIVO					    "********************************ARQUIVO*******************************\n"
#define INICIO_EXCLUSAO  "**********************EXCLUIR*********************\n"
#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define REGISTRO_INSERIDO "Registro %s inserido com sucesso.\n\n"



/* Registro do produto */
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

/* Registro da Tabela Hash
 * Contém a chave primária, o RRN do registro atual e o ponteiro para o próximo
 * registro. */
typedef struct chave {
	char pk[TAM_PRIMARY_KEY];
	int rrn;
	struct chave *prox;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
  int tam;
  Chave **v;
} Hashtable;

/* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;

/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();


/* Exibe o produto */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);


/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtable* tabela);
void cadastrar(Hashtable* tabela);
int  alterar(Hashtable tabela);
void buscar(Hashtable tabela);
int  remover(Hashtable* tabela);
void liberar_tabela(Hashtable* tabela);

/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
void criar_tabela(Hashtable *tabela, int tam);
void imprimir_tabela(Hashtable tabela);
int exibir_registro(int rrn);
Produto recuperar_registro(int rrn);
int is_prime(int n);
void gerarChave(Produto *novo);
void recebe_dados(Produto *novo);
int inserir(Hashtable *tabela, Produto novo);
void inserir_no_arquivo(char *ARQUIVO, Produto novo);
int busca_pos_pk(Hashtable tabela, char *pk);


/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */

int main() 
{
	
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d%*c", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d%*c", &tam);
	tam = prox_primo(tam);

	Hashtable tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) 
		carregar_tabela(&tabela);
	


	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
		scanf("%d%*c", &opcao);
		switch(opcao) {

		case 1:
			cadastrar(&tabela);
			break;
		case 2:
			printf(INICIO_ALTERACAO);
			if(alterar(tabela))
				printf(SUCESSO);
			else
				printf(FALHA);
			break;
		case 3:
			printf(INICIO_BUSCA);
			buscar(tabela);
			break;
		case 4:
			printf(INICIO_EXCLUSAO);
			printf("%s", (remover(&tabela)) ? SUCESSO : FALHA );
			break;
		case 5:
			printf(INICIO_LISTAGEM);
			imprimir_tabela(tabela);
			break;
		case 6:
			liberar_tabela(&tabela);
			break;

		case 10:
			printf(INICIO_ARQUIVO);
			printf("%s\n", (*ARQUIVO!='\0') ? ARQUIVO : ARQUIVO_VAZIO);
			break;

		default:
			printf(OPCAO_INVALIDA);
			break;
		}
	}
	return 0;
}


/* <<< IMPLEMENTE AQUI AS FUNCOES >>> */

/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo() {
	scanf("%[^\n]\n", ARQUIVO);
}

/*Auxiliar para a função de hash*/
short f(char x)
{
	return (x < 59) ? x - 48 : x - 54; 
}

// Exibe o produto
int exibir_registro(int rrn)
{
	if(rrn<0)
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

// Recupera um registro do arquivo de dados através do rrn
Produto recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Produto j;

	p = strtok(temp,"@");
	strcpy(j.pk,p);
	p = strtok(NULL,"@");
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

// Gera o hash
short hash(const char* chave, int tam){

	int i, h = 0;

	for(i = 1; i < 9; i++){
		h = h + (i * f(chave[i-1]));
	}

	return h % tam;
}

// Inicializa a tabela hash 
void criar_tabela(Hashtable *tabela, int tam){
	int rrn;

	tabela->v = (Chave**) malloc(sizeof(Chave*) * tam);
	tabela->tam = tam;

	// Inicializa os nós da tabela hash com nó cabeça
	for(rrn = 0; rrn < tam; rrn++){
		tabela->v[rrn] = (Chave*)malloc(sizeof(Chave));
		tabela->v[rrn]->prox = NULL;
	}
}

// Preenche a tabela hash com o arquivo de dados recebido
void carregar_tabela(Hashtable *tabela){
	int rrn, tam;
	Produto produto;
	char temp[TAM_ARQUIVO];

	tam = strlen(ARQUIVO) / TAM_REGISTRO;
	nregistros = 0;

	for(rrn=0; rrn < tam; rrn++){

		// Recupera o registro do arquivo de dados
		produto = recuperar_registro(rrn);
		
		// Verifica se o registro ja existe
		if(busca_pos_pk(*tabela, produto.pk) == -1){
			// Insere na tabel hash
			inserir(tabela, produto);
			// Insere no arquivo auxiliar
			inserir_no_arquivo(temp, produto);
			// Incrementa o numero de registros
			nregistros++;
		}

	}

	// Limpa o arquivo de dados
	strcpy(ARQUIVO, "");
	// Copia do arquivo de dados auxiliar para o original
	strcpy(ARQUIVO, temp);
}


// Libera a tabela hash
void liberar_tabela(Hashtable* tabela){
	free(tabela->v);
}

// Imprime a tabela hash
void imprimir_tabela(Hashtable tabela){

	int i;
	Chave *aux;

	for (i = 0; i < tabela.tam; ++i){
		printf("[%d]", i);

		aux = tabela.v[i]->prox;
		while(aux != NULL){
			printf(" %s", aux->pk);
			aux = aux->prox;
		}
		printf("\n");
	}
}

// Verifica se o numero é primo
int is_prime(int n){	
	int i, flag=0;

    for (i = 2; i <= n / 2; i++) {
        if ((n % i) == 0) {
            flag = 1;
            break;
        }
    }

    if (flag == 0)
        return 1;
     else
        return 0;

}

// Busca o proximo numero primo >= a
int prox_primo(int a){
	int i;

	for(i=a; i < a+71; i++){
		if(is_prime(i))
			break;
	}

	return i;

}

// Cria uma chave primaria para um produto
void gerarChave(Produto *novo){
    char pk[TAM_PRIMARY_KEY] = {novo->nome[0], novo->nome[1], novo->marca[0], novo->marca[1], novo->data[0], novo->data[1], novo->data[3], novo->data[4], novo->ano[0], novo->ano[1]};
    strcpy(novo->pk, pk);
}

// Recebe os dados do produto
void recebe_dados(Produto *novo){

	char string_aux[TAM_NOME];

    scanf("%[^\n]", string_aux);
    getchar();
    strcpy(novo->nome, string_aux);

    scanf("%[^\n]", string_aux);
    getchar();
    strcpy(novo->marca, string_aux);

    scanf("%[^\n]", string_aux);
    getchar();
    strcpy(novo->data, string_aux);

    scanf("%[^\n]", string_aux);
    getchar();
    strcpy(novo->ano, string_aux);

    scanf("%[^\n]", string_aux);
    getchar();
    strcpy(novo->preco, string_aux);

    scanf("%[^\n]", string_aux);
    getchar();
    strcpy(novo->desconto, string_aux);

    scanf("%[^\n]", string_aux);
    getchar();
    strcpy(novo->categoria, string_aux);

    gerarChave(novo);	
}

// Insere um produto na tabela hash e no arquivo de dados
void cadastrar(Hashtable *tabela){

	Produto novo;

	// Armazena os dados recebidos do usuario no produto novo
	recebe_dados(&novo);

	// Verifica se o dado ja existe
	if(busca_pos_pk(*tabela, novo.pk) != -1){
		// PK repetida
		printf(ERRO_PK_REPETIDA, novo.pk);
	}
	else {

		// Insere na tabela hash
		if(inserir(tabela, novo)){

			// Escreve no arquivo de dados
			inserir_no_arquivo(ARQUIVO, novo);		
		
			// Incrementa a quantidade de registros
			nregistros++;	

			// Mensagem de sucesso
			printf(REGISTRO_INSERIDO, novo.pk);

		}

	}

}

// Função auxiliar para inserção do produtp
int inserir(Hashtable *tabela, Produto produto){

	int h;
	Chave *novo;
	Chave *aux;

	// Gera a hash
	h = hash(produto.pk, tabela->tam);
	
	// Aloca o novo nó
	novo = (Chave*)malloc(sizeof(Chave));
	strcpy(novo->pk, produto.pk);
	novo->rrn = nregistros;

	// Aux aponto para o no cabeca da lista
	aux = tabela->v[h];

	// Lista vazia
	if(aux->prox == NULL){
		novo->prox = NULL;
		tabela->v[h]->prox = novo;
	}
	else {
		// Procura a posição para inserir
		while(aux->prox != NULL && strcmp(aux->prox->pk, produto.pk) < 0)
			aux = aux->prox;

		novo->prox = aux->prox;
		aux->prox = novo;
	}

	return 1;
}

// Escreve no arquivo de dados
void inserir_no_arquivo(char *ARQUIVO, Produto novo){

	int tam, i;
	char *arquivo_aux;

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

}

// Busca na tabela
void buscar(Hashtable tabela){
	char pk[TAM_PRIMARY_KEY];
	int pos;
	Chave *aux;

	scanf("%[^\n]", pk);
	getchar();

	// Busca posicao da pk
	pos = busca_pos_pk(tabela, pk);

	if(pos == -1){
		printf(REGISTRO_N_ENCONTRADO);
	}
	else{
		aux = tabela.v[pos]->prox;

		while(aux != NULL){
			if(strcmp(aux->pk, pk) == 0){
				exibir_registro(aux->rrn);
				break;
			}

			aux = aux->prox;
		}

	}
	
}

// Retorna posicao da PK na tabela hash
// Retorna -1 se não encontrar
int busca_pos_pk(Hashtable tabela, char *pk){

	int pos;
	Chave *aux;	

	pos = hash(pk, tabela.tam);

	aux = tabela.v[pos]->prox;

	while(aux != NULL){

		if(strcmp(aux->pk, pk) == 0)
			return pos;
		
		aux = aux->prox;
	}
	
	return -1;
}

// Altera o desconto do produto
int alterar(Hashtable tabela){
    Produto j;
    Chave *aux;
    int pos, desconto_pos;
    char novo_desconto[TAM_DESCONTO], pk[TAM_PRIMARY_KEY];

	scanf("%[^\n]", pk);
	getchar();
 
    pos = busca_pos_pk(tabela, pk);

    if(pos != -1){

		scanf("%[^\n]", novo_desconto);
    	getchar();

		while(strlen(novo_desconto) != 3 || (strcmp(novo_desconto, "000") < 0) || strcmp(novo_desconto, "100") > 0){
    		printf(CAMPO_INVALIDO);
        	scanf("%[^\n]", novo_desconto);
        	getchar();
    	}

		aux = tabela.v[pos]->prox;

		while(aux != NULL){
			if(strcmp(aux->pk, pk) == 0){
		        j = recuperar_registro(aux->rrn);

		        desconto_pos = strlen(j.pk) + strlen(j.nome) + strlen(j.marca) + strlen(j.data) + strlen(j.ano) + strlen(j.preco) + 6 + (aux->rrn * TAM_REGISTRO);

		        ARQUIVO[desconto_pos] = novo_desconto[0];
		        ARQUIVO[desconto_pos+1] = novo_desconto[1];
		        ARQUIVO[desconto_pos+2] = novo_desconto[2];

		        break;
			}

			aux = aux->prox;
		}

 
    }
    else{
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
 
    return 1;
}

// Remove um produto da tabela hash e marca para exclusao no arquivo
int  remover(Hashtable* tabela){

	Chave *aux, *ant;
	char pk[TAM_PRIMARY_KEY];
	int pos, pos2, rrn;

	scanf("%[^\n]", pk);
	getchar();

	// Procura a posição na tabela hash
	pos = busca_pos_pk(*tabela, pk);

	// Encontrou a pk
	if(pos != -1){

		ant = NULL;
		aux = tabela->v[pos]->prox;

		while(aux != NULL){

			if(strcmp(aux->pk, pk) == 0){

				// Arruma os ponteiros
				if(ant != NULL)
					ant->prox = aux->prox;
				else
					tabela->v[pos]->prox = aux->prox;
				
				// Armazena o rrn
				rrn = aux->rrn;

				// Libera o nó
				free(aux);	

				// Marca para remoção
				pos2 = rrn * TAM_REGISTRO;
				ARQUIVO[pos2] = '*';
				ARQUIVO[pos2+1] = '|';

				// nregistros--;

				return 1;
			}

			ant = aux;
			aux = aux->prox;
		}
	}

	// Não encontrou a pk
	printf(REGISTRO_N_ENCONTRADO);
	return 0;
}
