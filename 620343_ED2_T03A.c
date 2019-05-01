 /* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 03A - Hashing com reespalhamento linear
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


#define POS_OCUPADA 	"[%d] Ocupado: %s\n"
#define POS_LIVRE 		"[%d] Livre\n"
#define POS_REMOVIDA 	"[%d] Removido\n"

/* Estado das posições da tabela hash */
#define LIVRE       0
#define OCUPADO     1
#define REMOVIDO    2

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
#define INICIO_EXCLUSAO 			 			"**********************EXCLUIR*********************\n"

#define SUCESSO  				 "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA 					 "FALHA AO REALIZAR OPERACAO!\n"
#define ERRO_TABELA_CHEIA 		"ERRO: Tabela Hash esta cheia!\n\n"
#define REGISTRO_INSERIDO 		"Registro %s inserido com sucesso. Numero de colisoes: %d.\n\n"


/* Registro do jogo */
typedef struct {
	char pk[TAM_PRIMARY_KEY];
	char nome[TAM_NOME];
	char marca[TAM_MARCA];
	char data[TAM_DATA];	/* DD/MM/AAAA */
	char ano[TAM_ANO];
	char preco[TAM_PRECO];
	char desconto[TAM_DESCONTO];
	char categoria[TAM_CATEGORIA];
} Jogo;

/* Registro da Tabela Hash
 * Contém o estado da posição, a chave primária e o RRN do respectivo registro */
typedef struct {
	int estado;
	char pk[TAM_PRIMARY_KEY];
	int rrn;
} Chave;

/* Estrutura da Tabela Hash */
typedef struct {
    int tam;
    Chave *v;
  } Hashtabela;

  /* Variáveis globais */
char ARQUIVO[TAM_ARQUIVO];
int nregistros;




/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */


/* Recebe do usuário uma string simulando o arquivo completo. */
void carregar_arquivo();

/* Exibe o jogo */
int exibir_registro(int rrn);

/*Função de Hash*/
short hash(const char* chave, int tam);


/*Auxiliar para a função de hash*/
short f(char x);

/*Retorna o primeiro número primo >= a*/
int  prox_primo(int a);

/*Funções do Menu*/
void carregar_tabela(Hashtabela* tabela);
void cadastrar(Hashtabela* tabela);
int  alterar(Hashtabela tabela);
void buscar(Hashtabela tabela);
int  remover(Hashtabela* tabela);
void liberar_tabela(Hashtabela* tabela);


/* <<< DECLARE AQUI OS PROTOTIPOS >>> */
void criar_tabela(Hashtabela *tabela, int tam);
void imprimir_tabela(Hashtabela tabela);
int exibir_registro(int rrn);
Jogo recuperar_registro(int rrn);
int is_prime(int n);
void gerarChave(Jogo *novo);
void recebe_dados(Jogo *novo);
int inserir(Hashtabela *tabela, Jogo novo);
void inserir_no_arquivo(Jogo novo);
int busca_pos_pk(Hashtabela tabela, char *pk);

/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main() 
{    
	/* Arquivo */
	int carregarArquivo = 0;
	scanf("%d\n", &carregarArquivo); // 1 (sim) | 0 (nao)
	if (carregarArquivo) 
		carregar_arquivo();

	/* Tabela Hash */
	int tam;
	scanf("%d", &tam);
	tam = prox_primo(tam);

	Hashtabela tabela;
	criar_tabela(&tabela, tam);
	if (carregarArquivo) 
		carregar_tabela(&tabela);

	/* Execução do programa */
	int opcao = 0;
	while(opcao != 6) {
			scanf("%d%*c", &opcao);
			switch(opcao) 
			{
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
					if(	remover(&tabela))
						printf(SUCESSO);
					else
						printf(FALHA);
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

/* Exibe o jogo */
int exibir_registro(int rrn)
{
	if(rrn<0)
		return 0;
	float preco;
	int desconto;
	Jogo j = recuperar_registro(rrn);
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
Jogo recuperar_registro(int rrn)
{
	char temp[193], *p;
	strncpy(temp, ARQUIVO + ((rrn)*192), 192);
	temp[192] = '\0';
	Jogo j;

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

	for(i = 0; i < 8; i++){
		h = h + ((i+1) * f(chave[i]));
	}

	return h % tam;
}

// Inicializa a tabela hash 
void criar_tabela(Hashtabela *tabela, int tam){
	int i;

	tabela->v = (Chave*) malloc(sizeof(Chave) * tam);
	tabela->tam = tam;

	// Libera todos os espaços da tabela hash
	for(i = 0; i < tam; i++)
		tabela->v[i].estado = LIVRE;
}

// Preenche a tabela hash com o arquivo de dados recebido
void carregar_tabela(Hashtabela *tabela){
	char *string_aux, pk[TAM_PRIMARY_KEY];
	int i, h;

	nregistros = strlen(ARQUIVO) / TAM_REGISTRO;

	for(i=0; i < nregistros; i++){
		string_aux = ARQUIVO + (i*192);

		sscanf(string_aux, "%[^@]", pk);

		h = hash(pk, tabela->tam);

		while(tabela->v[h].estado == OCUPADO)
			h = (h+1)%tabela->tam;

		tabela->v[h].estado = OCUPADO;
		tabela->v[h].rrn = i;
		strcpy(tabela->v[h].pk, pk);
	}
}

// Libera a tabela hash
void liberar_tabela(Hashtabela* tabela){
	// free(tabela);
}

// Imprime a tabela hash
void imprimir_tabela(Hashtabela tabela){

	int i;

	for (i = 0; i < tabela.tam; ++i)
	{
		if(tabela.v[i].estado == LIVRE){
			printf(POS_LIVRE, i);
		}
		if(tabela.v[i].estado == OCUPADO){
			printf(POS_OCUPADA, i, tabela.v[i].pk);
		}
		if(tabela.v[i].estado == REMOVIDO){
			printf(POS_REMOVIDA, i);
		}
	}
}

// Verifica se o numero é primo
int is_prime(int n){	
	int j, flag=0;

    for (j = 2; j <= n / 2; j++) {
        if ((n % j) == 0) {
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
	int i, j;

	for(i=a; i < a*100; i++){
		if(is_prime(i))
			return i;
	}

}

// Cria uma chave primaria para um produto
void gerarChave(Jogo *novo){
    char pk[TAM_PRIMARY_KEY] = {novo->nome[0], novo->nome[1], novo->marca[0], novo->marca[1], novo->data[0], novo->data[1], novo->data[3], novo->data[4], novo->ano[0], novo->ano[1]};
    strcpy(novo->pk, pk);
}

// Recebe os dados do produto
void recebe_dados(Jogo *novo){

	char string_aux[TAM_NOME];

    scanf("%[^\n]", string_aux);
    strcpy(novo->nome, string_aux);
    getchar();

    scanf("%[^\n]", string_aux);
    strcpy(novo->marca, string_aux);
    getchar();

    scanf("%[^\n]", string_aux);
    strcpy(novo->data, string_aux);
    getchar();

    scanf("%[^\n]", string_aux);
    strcpy(novo->ano, string_aux);
    getchar();

    scanf("%[^\n]", string_aux);
    strcpy(novo->preco, string_aux);
    getchar();

    scanf("%[^\n]", string_aux);
    strcpy(novo->desconto, string_aux);
    getchar();

    scanf("%[^\n]", string_aux);
    strcpy(novo->categoria, string_aux);
    getchar();

    gerarChave(novo);	
}

// Insere um produto na tabela hash e no arquivo de dados
void cadastrar(Hashtabela *tabela){

	Jogo novo;

	recebe_dados(&novo);

	// Verifica se o dado ja existe
	if(busca_pos_pk(*tabela, novo.pk) != -1){
		printf(ERRO_PK_REPETIDA, novo.pk);
	}
	else {

		if(!inserir(tabela, novo)){
			printf(ERRO_TABELA_CHEIA);
		}

	}

}

// Função auxiliar para inserção do produtp
int inserir(Hashtabela *tabela, Jogo novo){

	int h, h2, colisoes=0;

	// Calcula o hash
	h = hash(novo.pk, tabela->tam);

	// Verifica se a posicao ta ocupada
	if(tabela->v[h].estado == OCUPADO){
		// Colidiu
		colisoes++;
		
		// Novo hash		
		h2 = ((h+1)%tabela->tam);

		// Calcula o novo hash até que seja valido ou a tabela esteja cheia
		do{
			// Se a nova posição não estiver ocupada sai do laço
			if(tabela->v[h2].estado != OCUPADO)
				break;
			
			// Novo hash
			h2 = ((h2+1) % tabela->tam);

			// Colidiu
			colisoes++;

		} while(h2 != h);

		if(h == h2)
			return 0;

		h = h2;

	}

	// Adiciona o registro a tabela hash
	tabela->v[h].estado = OCUPADO;
	tabela->v[h].rrn = nregistros;
	strcpy(tabela->v[h].pk, novo.pk);
	
	// Escreve no arquivo de texto
	inserir_no_arquivo(novo);
	
	// Incrementa a quantidade de registros
	nregistros++;

	printf(REGISTRO_INSERIDO, novo.pk, colisoes);

	return 1;
}

// Escreve no arquivo de dados
void inserir_no_arquivo(Jogo novo){

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
void buscar(Hashtabela tabela){
	char pk[TAM_PRIMARY_KEY];
	int pos;

	scanf("%[^\n]", pk);
	getchar();

	// Busca posicao da pk
	pos = busca_pos_pk(tabela, pk);

	// Caso encontre exibe o registro
	if(pos != -1)
		exibir_registro(tabela.v[pos].rrn);
	// Caso nao encontre
	else
		printf(REGISTRO_N_ENCONTRADO);
}

// Retorna posicao da PK na tabela hash
int busca_pos_pk(Hashtabela tabela, char *pk){

	int i;

	for(i=0; i < tabela.tam; i++){
		if(tabela.v[i].estado == OCUPADO && strcmp(tabela.v[i].pk, pk) == 0){
			return i;
		}
	}
	
	return -1;

}

// Altera o desconto do produto
int  alterar(Hashtabela tabela){
    Jogo j;
    int i, pos, desconto_pos;
    float novo_valor;
    char temp[193], novo_desconto[3], pk[TAM_PRIMARY_KEY];

	scanf("\n%[^\n]", pk);
	getchar();
 
    pos = busca_pos_pk(tabela, pk);

    if(pos != -1){

		scanf("%[^\n]", novo_desconto);
    	getchar();

    	while(strlen(novo_desconto) != 3){
    		printf(CAMPO_INVALIDO);
        	scanf("%[^\n]", novo_desconto);
        	getchar();
    	}
        
        j = recuperar_registro(tabela.v[pos].rrn);

        desconto_pos = strlen(j.pk) + strlen(j.nome) + strlen(j.marca) + strlen(j.data) + strlen(j.ano) + strlen(j.preco) + 6 + (tabela.v[pos].rrn *192);

        ARQUIVO[desconto_pos] = novo_desconto[0];
        ARQUIVO[desconto_pos+1] = novo_desconto[1];
        ARQUIVO[desconto_pos+2] = novo_desconto[2];
 
    }
    else{
        printf(REGISTRO_N_ENCONTRADO);
        return 0;
    }
 
    return 1;
}

// Remove um produto da tabela hash e marca para exclusao no arquivo
int  remover(Hashtabela* tabela){

	char pk[TAM_PRIMARY_KEY];
	int pos, pos2;

	scanf("%[^\n]", pk);
	getchar();


	pos = busca_pos_pk(*tabela, pk);

	// Encontrou a pk
	if(pos != -1){

		pos2 = tabela->v[pos].rrn * 192;

		ARQUIVO[pos2] = '*';
		ARQUIVO[pos2+1] = '|';

		tabela->v[pos].estado = REMOVIDO;

		return 1;
	}
	// Não encontrou a pk
	else {
		printf(REGISTRO_N_ENCONTRADO);
		return 0;
	}

}