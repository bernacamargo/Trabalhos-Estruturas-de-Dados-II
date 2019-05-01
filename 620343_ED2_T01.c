/* ==========================================================================
 * Universidade Federal de São Carlos - Campus Sorocaba
 * Disciplina: Estruturas de Dados 2
 * Prof. Tiago A. de Almeida
 *
 * Trabalho 01
 *
 * RA: 620343
 * Aluno: Bernardo Pinheiro Camargo
 * ========================================================================== */
 
/* Bibliotecas */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
 
 
/* Tamanho dos campos dos registros */
#define TAM_PRIMARY_KEY    11
#define TAM_NOME         51
#define TAM_MARCA         51
#define TAM_DATA         11
#define TAM_ANO         3
#define TAM_PRECO         8
#define TAM_DESCONTO     4
#define TAM_CATEGORIA     51
 
 
#define TAM_REGISTRO     192
#define MAX_REGISTROS     1000
#define MAX_CATEGORIAS     30
#define TAM_ARQUIVO (MAX_REGISTROS * TAM_REGISTRO + 1)
 
 
/* Saídas para o usuario */
#define OPCAO_INVALIDA                 "Opcao invalida!\n"
#define MEMORIA_INSUFICIENTE         "Memoria insuficiente!"
#define REGISTRO_N_ENCONTRADO         "Registro(s) nao encontrado!\n"
#define CAMPO_INVALIDO                 "Campo invalido! Informe novamente.\n"
#define ERRO_PK_REPETIDA            "ERRO: Ja existe um registro com a chave primaria: %s.\n"
#define ARQUIVO_VAZIO                 "Arquivo vazio!\n"
#define INICIO_BUSCA                  "**********************BUSCAR**********************\n"
#define INICIO_LISTAGEM              "**********************LISTAR**********************\n"
#define INICIO_ALTERACAO             "**********************ALTERAR*********************\n"
#define INICIO_EXCLUSAO              "**********************EXCLUIR*********************\n"
#define INICIO_ARQUIVO              "**********************ARQUIVO*********************\n"
#define INICIO_ARQUIVO_SECUNDARIO    "*****************ARQUIVO SECUNDARIO****************\n"
#define SUCESSO                       "OPERACAO REALIZADA COM SUCESSO!\n"
#define FALHA                          "FALHA AO REALIZAR OPERACAO!\n"
 
 
 
/* Registro do Produto */
typedef struct {
    char pk[TAM_PRIMARY_KEY];
    char nome[TAM_NOME];
    char marca[TAM_MARCA];
    char data[TAM_DATA];    /* DD/MM/AAAA */
    char ano[TAM_ANO];
    char preco[TAM_PRECO];
    char desconto[TAM_DESCONTO];
    char categoria[TAM_CATEGORIA];
} Produto;
 
 
/*----- Registros dos Índices -----*/
 
/* Struct para índice Primário */
typedef struct primary_index{
  char pk[TAM_PRIMARY_KEY];
  int rrn;
} Ip;
 
/* Struct para índice secundário */
typedef struct secundary_index{
  char pk[TAM_PRIMARY_KEY];
  char string[TAM_NOME];
} Is;
 
/* Struct para índice secundário de preços */
typedef struct secundary_index_of_final_price{
  float price;
  char pk[TAM_PRIMARY_KEY];
} Isf;
 
/* Lista ligada para o Índice abaixo*/
typedef struct linked_list{
  char pk[TAM_PRIMARY_KEY];
  struct linked_list *prox;
} ll;
 
/* Struct para lista invertida */
typedef struct reverse_index{
  char cat[TAM_CATEGORIA];
  ll* lista;
} Ir;
 
/*----- GLOBAL -----*/
char ARQUIVO[TAM_ARQUIVO];
 
/* ==========================================================================
 * ========================= PROTÓTIPOS DAS FUNÇÕES =========================
 * ========================================================================== */
 
/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo();
 
/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto);
 
/* Recupera do arquivo o registro com o rrn informado
 *  e retorna os dados na struct Produto */
Produto recuperar_registro(int rrn);
 
/* (Re)faz o índice respectivo */
void criar_iprimary(Ip *iprimary, int* nregistros);
  
/* Rotina para impressao de indice secundario */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat);
 
void cadastrarItem(Produto novo, Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int *nregistros, int *ncat);
 
 // Insere na lista invertida a pk ordenada
void inserePKOrdenado(Ir *icategory, char *chave);
 
// Altera o desconto de um produto
int alterarDesconto(Ip *iprimary, Isf *iprice, Is *iproduct, int nregistros, char *pk);
  
// Busca pelos indices secundarios de acordo com uma string
// Retorna o vetor found com os resultados encontrados
Is *searchSecundaryKeyPK(Ip *iprimary, Is *isecundary, char *name, int nregistros, int *qtd_pks);
 
// Calcula o preço final de um produto
float calcPrice(float preco, int desconto);
 
// Funções auxiliares para qsort e bsearch
int searchPK(char *elem, Ip *key);
int cmpPK(Ip *chave1, Ip *chave2);
int cmpSK(Is *chave1, Is *chave2);
int cmpPrice(Isf *chave1, Isf *chave2);
int cmpCategory(Ir* chave1, Ir* chave2);
int comparatorCategory(char *elem, Ir *key);

// Busca e retorna o rrn de um registro atraves de uma chave primaria
int searchRrn(Ip *iprimary, char *pk, int nregistros);
 
// Verifica se uma PK já esta cadastrada no indíce.
// Retorna 1 se encontrar
// Retorna 0 se nao encontrar
// Retorna -1 se a chave estiver marcada 
int checkPk(Ip* iprimary, char *pk, int nregistros);
 
// Realiza busca de produto
int search(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat, int searchType);

// Cria uma chave primaria para um produto
void gerarChave(Produto *novo);
 
// Cria os indices secundarios 
void criar_isecundary(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int *nregistros, int *ncat);
 
// Listagem por chave primaria
void listagemPorPk(Ip *iprimary, int nregistros);

// Listagem por indice secundario iprice
void listagemPorPreco(Ip *iprimary, Isf *iprice, int nregistros);

// Listagem por indice secundario icategory
void listagemPorCategoria(Ip *iprimary, Ir *icategory, char *categoria, int nregistros, int ncat);

// Listagem por indice secundario ibrand
void listagemPorMarca(Ip *iprimary, Is *ibrand, int nregistros);

// Remover um registro
int removerRegistro(Ip *iprimary, int nregistros, char *pk);
 
// Remover do arquivo de dados os registros marcados para exclusão
void limparArquivoDados(Ip *iprimary, int nregistros);
 
/* ==========================================================================
 * ============================ FUNÇÃO PRINCIPAL ============================
 * =============================== NÃO ALTERAR ============================== */
int main(){
  /* Arquivo */
    int carregarArquivo = 0, nregistros = 0, ncat = 0, i, res, tam;
    scanf("%d%*c", &carregarArquivo); /* 1 (sim) | 0 (nao) */
    if (carregarArquivo)
        nregistros = carregar_arquivo();
 
    /* Índice primário */
    Ip *iprimary = (Ip *) malloc (MAX_REGISTROS * sizeof(Ip));
    Ip *iprimary_aux = (Ip*)malloc(sizeof(Ip));
      if (!iprimary) {
        perror(MEMORIA_INSUFICIENTE);
        exit(1);
    }
    criar_iprimary(iprimary, &nregistros);
 
    /*Alocar e criar índices secundários*/
    Is *ibrand = (Is*)malloc(MAX_REGISTROS * sizeof(Is));
    Is *iproduct = (Is*)malloc(MAX_REGISTROS * sizeof(Is));
    Ir *icategory = (Ir*)malloc(MAX_CATEGORIAS * sizeof(Ir));
    Isf *iprice = (Isf*)malloc(MAX_REGISTROS * sizeof(Isf));
 
    criar_isecundary(iproduct, ibrand, icategory, iprice, &nregistros, &ncat);
 
    Produto novo;
    char string_aux[51], string_aux2[51], *arquivo_aux;
 
    /* Execução do programa */
    int opcao = 0, opcao_sec = 0;
    while(1)
    {
        scanf("%d%*c", &opcao);
        switch(opcao)
        {
            case 1:
                /*cadastro*/
                // Recebe os dados a serem cadastrados
                scanf("%[^\n]", string_aux);
                strcpy(novo.nome, string_aux);
                getchar();
 
                scanf("%[^\n]", string_aux);
                strcpy(novo.marca, string_aux);
                getchar();
 
                scanf("%[^\n]", string_aux);
                strcpy(novo.data, string_aux);
                getchar();
 
                scanf("%[^\n]", string_aux);
                strcpy(novo.ano, string_aux);
                getchar();
 
                scanf("%[^\n]", string_aux);
                strcpy(novo.preco, string_aux);
                getchar();
 
                scanf("%[^\n]", string_aux);
                strcpy(novo.desconto, string_aux);
                getchar();
 
                scanf("%[^\n]", string_aux);
                strcpy(novo.categoria, string_aux);
                getchar();
 
                gerarChave(&novo);
 
                res = checkPk(iprimary, novo.pk, nregistros);
 
                // Existe a PK
                if(res == 1)
                    printf(ERRO_PK_REPETIDA, novo.pk);
                // Existe a PK e esta marcada para remoção
                else if(res == -1){
 
                    iprimary_aux = bsearch(novo.pk, iprimary, nregistros, sizeof(Ip), (void*)searchPK);
 
                    iprimary_aux->rrn = nregistros;
 
                    // Tamanho do registro
                    tam = strlen(novo.nome) + strlen(novo.marca) + strlen(novo.data) + strlen(novo.ano) + strlen(novo.preco) + strlen(novo.desconto) + strlen(novo.categoria) + 7; 
                    // Escreve no final do arquivo
                    sprintf(ARQUIVO + strlen(ARQUIVO), "%s@%s@%s@%s@%s@%s@%s@", novo.nome, novo.marca, novo.data, novo.ano, novo.preco, novo.desconto, novo.categoria);
 
                    // Completa o tamanho do registro caso precise
                    if(tam < TAM_REGISTRO){
                        arquivo_aux = ARQUIVO + strlen(ARQUIVO);
                        for(i=tam; i < TAM_REGISTRO; i++){
                            sprintf(arquivo_aux, "#");
                            arquivo_aux++;
                        }
                    }
 
                    nregistros = nregistros + 1;
                }
                // Não existe PK
                else if(res == 0)
                    cadastrarItem(novo, iprimary, iproduct, ibrand, icategory, iprice, &nregistros, &ncat);
 
            break;
            case 2:
                /*alterar desconto*/
                printf(INICIO_ALTERACAO);
 
                scanf("%[^\n]", string_aux);
                getchar();
 
                if(alterarDesconto(iprimary, iprice, iproduct, nregistros, string_aux)){
                    printf(SUCESSO);
                }
                else
                    printf(FALHA);
                
            break;
            case 3:
                /*excluir produto*/
                printf(INICIO_EXCLUSAO);
 
                scanf("%[^\n]", string_aux);
                getchar();                
 
                if(checkPk(iprimary, string_aux, nregistros) != 1)
                    printf(REGISTRO_N_ENCONTRADO);
                
                if(removerRegistro(iprimary, nregistros, string_aux))
                    printf(SUCESSO);
                else
                    printf(FALHA);
                
            break;
            case 4:
                /*busca*/
                printf(INICIO_BUSCA);
                scanf("%d", &opcao_sec);
                getchar();
 
                search(iprimary, iproduct, ibrand, icategory, iprice, nregistros, ncat, opcao_sec);
            break;
            case 5:
                /*listagens*/
                printf(INICIO_LISTAGEM);
                scanf("%d", &opcao_sec);
                getchar();
                switch(opcao_sec){
                    // por pk
                    case 1:
                        listagemPorPk(iprimary, nregistros);
                    break;
 
                    // por categoria
                    case 2:
                        scanf("%[^\n]", string_aux);
                        getchar();
 
                        listagemPorCategoria(iprimary, icategory, string_aux, nregistros, ncat);
                    break;
 
                    // por marca
                    case 3:
                        listagemPorMarca(iprimary, ibrand, nregistros);
                    break;
 
                    // por preco final
                    case 4:
                        listagemPorPreco(iprimary, iprice, nregistros);
                    break;
 
                    default:
                        printf(OPCAO_INVALIDA);
                    break;
                }
 
            break;
            case 6:
                /*libera espaço*/
                limparArquivoDados(iprimary, nregistros);
                
                // Seta nregistros
                nregistros = strlen(ARQUIVO) / TAM_REGISTRO;
                // Reseta ncat
                ncat = 0;
                
                criar_iprimary(iprimary, &nregistros);
                
                criar_isecundary(iproduct, ibrand, icategory, iprice, &nregistros, &ncat);
 
            break;
            case 7:
                /*imprime o arquivo de dados*/
                printf(INICIO_ARQUIVO);
                if(strlen(ARQUIVO) > 0)
                	printf("%s\n", ARQUIVO);
                else
                	printf(ARQUIVO_VAZIO);
            break;
            case 8:
                /*imprime os índices secundários*/
                imprimirSecundario(iproduct, ibrand, icategory, iprice, nregistros, ncat);
            break;
            case 9:
                  /*Liberar memória e finalizar o programa */
				free(iprimary);
				free(iproduct);
				free(icategory);
				free(ibrand);
				free(iprice);            
                return 0;
            break;
            default:
                printf(OPCAO_INVALIDA);
            break;
        }
    }
    return 0;
}
 
 
/* Exibe o Produto */
int exibir_registro(int rrn, char com_desconto)
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
    if(!com_desconto)
    {
        printf("%s\n", j.preco);
        printf("%s\n", j.desconto);
    }
    else
    {
        sscanf(j.desconto,"%d",&desconto);
        sscanf(j.preco,"%f",&preco);
        preco = preco *  (100-desconto);
        preco = ((int) preco)/ (float) 100 ;
        printf("%07.2f\n",  preco);
 
    }
    strcpy(categorias, j.categoria);
 
    cat = strtok (categorias, "|");
 
    while(cat != NULL){
        printf("%s", cat);
        cat = strtok (NULL, "|");
        if(cat != NULL){
            printf(", ");
        }
    }
 
    printf("\n");
 
    return 1;
}
 
/* Recebe do usuário uma string simulando o arquivo completo e retorna o número
 * de registros. */
int carregar_arquivo()
{
    scanf("%[^\n]\n", ARQUIVO);
    return strlen(ARQUIVO) / TAM_REGISTRO;
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
    gerarChave(&j);
    return j;
}
 
 
/* Imprimir indices secundarios */
void imprimirSecundario(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat){
    int opPrint = 0;
    ll *aux;
    printf(INICIO_ARQUIVO_SECUNDARIO);
    scanf("%d", &opPrint);
    if(!nregistros)
        printf(ARQUIVO_VAZIO);
    switch (opPrint) {
        case 1:
            for(int i = 0; i < nregistros; i++){
                printf("%s %s\n",iproduct[i].pk, iproduct[i].string);
            }
        break;
        case 2:
            for(int i = 0; i < nregistros; i++){
                printf("%s %s\n",ibrand[i].pk, ibrand[i].string);
            }
        break;
        case 3:
            for(int i = 0; i < ncat; i++){
                printf("%s", icategory[i].cat);
                aux =  icategory[i].lista;
                while(aux != NULL){
                    printf(" %s", aux->pk);
                    aux = aux->prox;
                }
                printf("\n");
            }
        break;
 
        case 4:
        for(int i = 0; i < nregistros; i++){
            printf("%s %.2f\n",iprice[i].pk, iprice[i].price);
        }
        break;
    }
}
 
/* (Re)faz o índice primario */
void criar_iprimary(Ip *iprimary, int* nregistros){
 
    int rrn;
    Produto p;
 
    // Percorre o arquivo
    for(rrn=0; rrn < *nregistros; rrn++){
 
        p = recuperar_registro(rrn);
 
        gerarChave(&p);
 
        iprimary[rrn].rrn = rrn;
        strcpy(iprimary[rrn].pk, p.pk);
    }
    
    qsort(iprimary, (*nregistros), sizeof(Ip), (void*)cmpPK);
}
/* (Re)faz os indices secundarios */
void criar_isecundary(Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int *nregistros, int *ncat){
 
    Ir *categoria;
    ll *lista = (ll*)malloc(sizeof(ll));
    int i, pos_categoria;
    char *cat;
    float preco_final;
    Produto p;
 
    // Percorre o arquivo
    for(i=0; i < *nregistros; i++){
 
        // Recupera o registro
        p = recuperar_registro(i);
 
        // Salva os indices
        // SK produto
        strcpy(iproduct[i].pk, p.pk);
        strcpy(iproduct[i].string, p.nome);
        qsort(iproduct, (i+1), sizeof(Is), (void*)cmpSK);
 
        // SK marca
        strcpy(ibrand[i].pk, p.pk);
        strcpy(ibrand[i].string, p.marca);
        qsort(ibrand, (i+1), sizeof(Is), (void*)cmpSK);
 
        // SK preco final
        preco_final = calcPrice(atof(p.preco), atoi(p.desconto));
        iprice[i].price = preco_final;
        strcpy(iprice[i].pk, p.pk);
        qsort(iprice, (i+1), sizeof(Isf), (void*)cmpPrice);
 
        // SK categorias
        cat = strtok(p.categoria, "|");
        while(cat != NULL){
            // Verifica se ja existe a categoria        
            categoria = bsearch(cat, icategory, *ncat, sizeof(Ir), (void*)comparatorCategory);
 
            // Caso não exista
            if (!categoria){
 
                icategory[*ncat].lista = (ll*)malloc(sizeof(ll));
 
                // Cria a categoria
                strcpy(icategory[*ncat].cat, cat);
 
                // Cria o nó lista
                lista = (ll*)malloc(sizeof(ll));
                strcpy(lista->pk, p.pk);
                lista->prox = NULL;
                // icategory[*ncat].lista->prox = NULL;
 
                icategory[*ncat].lista = lista;
 
                *ncat = *ncat + 1;
 
        		qsort(icategory, *ncat, sizeof(Ir), (void*)cmpCategory);
            }
            // Caso exista a categoria
            else {
                inserePKOrdenado(categoria, p.pk);
            }
 
            cat = strtok (NULL, "|");
        }
        
 
    }
 
}

// Insere um novo item no ARQUIVO e nos respectivos indíces
void cadastrarItem(Produto novo, Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int *nregistros, int *ncat){
 
    // Inicializa variaveis necessarias
    Ir *categoria = (Ir*)malloc(sizeof(Ir));
    ll *lista;
    char *string_aux = malloc(TAM_NOME*sizeof(char)), *arquivo_aux, *cat;
    float preco_final;
    int tam, i, pos_categoria;
    
    // Tamanho do registro
    tam = strlen(novo.nome) + strlen(novo.marca) + strlen(novo.data) + strlen(novo.ano) + strlen(novo.preco) + strlen(novo.desconto) + strlen(novo.categoria) + 7; 
 
    // Escreve no final do arquivo
    sprintf(ARQUIVO + strlen(ARQUIVO), "%s@%s@%s@%s@%s@%s@%s@", novo.nome, novo.marca, novo.data, novo.ano, novo.preco, novo.desconto, novo.categoria);
 
    // Completa o tamanho do registro caso precise
    if(tam < TAM_REGISTRO){
        arquivo_aux = ARQUIVO + strlen(ARQUIVO);
        for(i=tam; i < TAM_REGISTRO; i++){
            sprintf(arquivo_aux, "#");
            arquivo_aux++;
        }
    }
 
    // Salva e ordena os indices
 
    // PK
    strcpy(iprimary[(*nregistros)].pk, novo.pk);
    iprimary[(*nregistros)].rrn = (*nregistros);
    qsort(iprimary, (*nregistros)+1, sizeof(Ip), (void*)cmpPK);
 
    // SK produto
    strcpy(iproduct[(*nregistros)].pk, novo.pk);
    strcpy(iproduct[(*nregistros)].string, novo.nome);
    qsort(iproduct, (*nregistros)+1, sizeof(Is), (void*)cmpSK);
 
    // SK marca
    strcpy(ibrand[(*nregistros)].pk, novo.pk);
    strcpy(ibrand[(*nregistros)].string, novo.marca);
    qsort(ibrand, (*nregistros)+1, sizeof(Is), (void*)cmpSK);
 
    // SK preco final
    preco_final = calcPrice(atof(novo.preco), atoi(novo.desconto));
    iprice[(*nregistros)].price = preco_final;
    strcpy(iprice[(*nregistros)].pk, novo.pk);
    qsort(iprice, (*nregistros)+1, sizeof(Isf), (void*)cmpPrice);
 
    // SK categorias
    cat = strtok(novo.categoria, "|");
    while(cat != NULL){
        // Verifica se ja existe a categoria        
        categoria = (Ir*)bsearch(cat, icategory, *ncat, sizeof(Ir), (void*)comparatorCategory);
 
        // Caso não exista
        if (!categoria){
 
            icategory[*ncat].lista = (ll*)malloc(sizeof(ll));
            lista = (ll*)malloc(sizeof(ll));
 
            // Cria a categoria
            strcpy(icategory[*ncat].cat, cat);
 
            // Cria o nó lista
            strcpy(lista->pk, novo.pk);
            lista->prox = NULL;
            // icategory[*ncat].lista->prox = NULL;
 
            icategory[*ncat].lista = lista;
 
            // icategory[*ncat].lista = lista;
            *ncat = *ncat + 1;
 
    		qsort(icategory, *ncat, sizeof(Ir), (void*)cmpCategory);
        }
        // Caso exista a categoria
        else {
            inserePKOrdenado(categoria, novo.pk);
        }
 
        cat = strtok (NULL, "|");
    }
    
 
    // incrementa a quantidade de registros
    *nregistros = *nregistros + 1;
    
 
}

// Cria uma chave primaria para um produto
void gerarChave(Produto *novo){
    char pk[TAM_PRIMARY_KEY] = {novo->nome[0], novo->nome[1], novo->marca[0], novo->marca[1], novo->data[0], novo->data[1], novo->data[3], novo->data[4], novo->ano[0], novo->ano[1]};
    strcpy(novo->pk, pk);
}
 
// Insere na lista invertida a pk ordenada
void inserePKOrdenado(Ir *icategory, char *chave){
 
    ll *atual, *ant, *novo;
 
     atual = icategory->lista;
     ant = NULL;
 
    // Percorre a lista de pk para inserir ordenado
    while(atual != NULL && strcmp(chave, atual->pk) > 0){
        ant = atual;
        atual = atual->prox;
    }
 
    // Aloca o novo nó
    novo = (ll*)malloc(sizeof(ll));
    // Copia o valor da pk para novo->pk
    strcpy(novo->pk, chave);
 
    // Insere no inicio
    if(ant == NULL){
        novo->prox = icategory->lista;
        icategory->lista = novo;
    }
    // Insere no meio/fim
    else {
        ant->prox = novo;
        novo->prox = atual;
    }
}

// Busca pelos indices secundarios de acordo com uma string
// Retorna o vetor found com os resultados encontrados
Is *searchSecundaryKeyPK(Ip *iprimary, Is *isecundary, char *name, int nregistros, int *qtd_pks){
 
    int i;
    Is *found;
 
    found = (Is*)malloc(nregistros*sizeof(Is));
 
    // Percorre o indice secundario atras da chave
    for(i=0; i < nregistros; i++){
 
        // Caso encontre-a adiciona ela ao vetor found
        if(strcmp(isecundary[i].string, name) == 0){
 
            // Verifica se a chave não está marcada para remoção
            // if(checkPk(iprimary, isecundary[i].pk, nregistros) == 1){
                strcpy(found[*qtd_pks].pk, isecundary[i].pk);
                strcpy(found[*qtd_pks].string, isecundary[i].string);
                // incrementa a quantidade de registros encontrados na busca
                *qtd_pks = *qtd_pks + 1;
            // }
        }
    }
 
    if(*qtd_pks > 0){
        return found;
    }
    else
        return NULL;
 
}    

// Busca e retorna o rrn de um registro atraves de uma chave primaria
int searchRrn(Ip *iprimary, char *pk, int nregistros){
 
    Ip *found;
 
    found = bsearch(pk, iprimary, nregistros, sizeof(Ip),(void*)searchPK);
 
    if(found)
        return found->rrn;
    else
        return -1;
 
}

// Calcula o preço final de um produto
float calcPrice(float preco, int desconto){
    float preco_final;
 
    preco_final = (preco * (100-desconto))/100;
    preco_final = preco_final * 100;
    preco_final = ((int)preco_final/(float)100);
 
    return preco_final;
}

int cmpPK(Ip *chave1, Ip *chave2){
    return strcmp(chave1->pk, chave2->pk);    
}
 
int searchPK(char *elem, Ip *key){
    return strcmp(elem, key->pk);
}
 
int cmpSK(Is *chave1, Is *chave2){
    if(strcmp(chave1->string, chave2->string) == 0){
        return strcmp(chave1->pk, chave2->pk);
    }
    return strcmp(chave1->string, chave2->string);
}
 
int cmpPrice(Isf *chave1, Isf *chave2){
    if(chave1->price == chave2->price)
        return strcmp(chave1->pk, chave2->pk);
    else if(chave1->price < chave2->price)
        return -1;    
    else if(chave1->price > chave2->price)
        return 1;
}
 
int cmpCategory(Ir* chave1, Ir* chave2){
    return strcmp(chave1->cat, chave2->cat);
}
 
int comparatorCategory(char *elem, Ir *key){
    return strcmp(elem, key->cat);
}

// Verifica se uma PK já esta cadastrada no indíce.
// Retorna 1 se encontrar
// Retorna 0 se nao encontrar
// Retorna -1 se a chave estiver marcada 
int checkPk(Ip* iprimary, char *pk, int nregistros){
 
    int i;
 
    for(i=0; i < nregistros; i++){
 
        if (strcmp(iprimary[i].pk, pk) == 0 && iprimary[i].rrn > 0){
            return 1;
        }
        else if(iprimary[i].rrn < -1)
            return -1;
 
    }
 
    return 0;
 
}
 
// Realiza busca de produto
int search(Ip* iprimary, Is* iproduct, Is* ibrand, Ir* icategory, Isf *iprice, int nregistros, int ncat, int searchType){
 
    Is *pks = (Is*)malloc(MAX_REGISTROS*sizeof(Is));
    Ir *pks2 = (Ir*)malloc(sizeof(Ir));
    ll *aux = (ll*)malloc(sizeof(ll));
    char pk[TAM_PRIMARY_KEY], prod_nome[TAM_NOME], marca[TAM_MARCA], categoria[TAM_CATEGORIA] ;
    int i, rrn, qtd_pks, j, flags[MAX_REGISTROS];
 
    switch(searchType){
 
        // busca por pk
        case 1:
            scanf("%[^\n]", pk);
            getchar();
 
            rrn = searchRrn(iprimary, pk, nregistros);
 
            if(!exibir_registro(rrn, 0)){
                printf(REGISTRO_N_ENCONTRADO);
            }
 
        break;
 
        // busca por nome ou modelo
        case 2:
            qtd_pks = 0;
            j = 0;
            scanf("%[^\n]", prod_nome);
            getchar();
 
            pks = searchSecundaryKeyPK(iprimary, iproduct, prod_nome, nregistros, &qtd_pks);
 
            if(pks != NULL){
 
                for(i=0; i < qtd_pks; i++){
 
                    rrn = searchRrn(iprimary, pks[i].pk, nregistros);
 					
 					if(rrn != -1 && j > 0)
 						printf("\n");
                    if(exibir_registro(rrn, 0)){
                    	j++;
                    }
 
                }
            }
            
            if(j == 0){
                printf(REGISTRO_N_ENCONTRADO);
            }
 
 
        break;
 
        // busca por marca e categoria
        case 3:    
            qtd_pks = 0;
            j=0;
 
            scanf("%[^\n]", marca);
            getchar();
 
            scanf("%[^\n]", categoria);
            getchar();
 
            // Busca as pks da marca
            pks = NULL;
            pks = searchSecundaryKeyPK(iprimary, ibrand, marca, nregistros, &qtd_pks);
 
            // Marca as posições do vetor para "não visitado"
            for(i=0; i < qtd_pks; i++){
                flags[i] = -1;
            }
 
            // Verifica se encontrou alguma marca
            if(qtd_pks > 0){
 				
                // Busca as pks da categoria informada
                pks2 = (Ir*)bsearch(categoria, icategory, ncat, sizeof(Ir), (void*)comparatorCategory);
 
                // Verifica se encontoru alguma pk na categoria
                if(pks2 != NULL && pks2->lista != NULL){
                    aux = pks2->lista;
                    
                    // Percorre a lista de pks da categoria
                    while(aux != NULL){
 
                        // Percorre a lista de pks da marca
                        for(i=0; i < qtd_pks; i++){   

                            // Caso a pk esteja nas duas listas e não tenha sido exibida ainda exibe o registro 
                            if(strcmp(pks[i].pk, aux->pk) == 0 && flags[i] == -1){
 
                                // Marca a posição no vetor já exibida
                                flags[i] = 1;
 
                                // Busca o rrn
                                rrn = searchRrn(iprimary, pks[i].pk, nregistros);
			  					if(rrn != -1 && j > 0)
			 						printf("\n");
                                // Exibe o registro
                                if(exibir_registro(rrn, 0)){
                                	j++;
                                }
                            
                            }
 
                            
                        }
 
                        aux = aux->prox;
                    }
                }
            }

            if(j == 0)
            	printf(REGISTRO_N_ENCONTRADO);

        break;
 
        default:
            printf(OPCAO_INVALIDA);
        break;
    }
 
}

// Listagem por chave primaria
void listagemPorPk(Ip *iprimary, int nregistros){
 
    int i, j;
 
    for(i=0, j=0; i < nregistros; i++){
    	if(iprimary[i].rrn != -1 && j > 0)
    		printf("\n");
        if(exibir_registro(iprimary[i].rrn, 0)){
            j++;
        }
    }
 
}

// Listagem por indice secundario iprice
void listagemPorPreco(Ip *iprimary, Isf *iprice, int nregistros){
 
    int i, rrn, j;
 
    for(i=0, j=0; i < nregistros; i++){
 
        rrn = searchRrn(iprimary, iprice[i].pk, nregistros);
  		if(rrn != -1 && j > 0)
 			printf("\n");
        if(exibir_registro(rrn, 1))
        	j++;
    }
 
}

// Listagem por indice secundario icategory
void listagemPorCategoria(Ip *iprimary, Ir *icategory, char *categoria, int nregistros, int ncat){
 
    int i, rrn, j=0;
    char *pk;
    Ir *node_cat = bsearch(categoria, icategory, ncat, sizeof(Ir), (void*)comparatorCategory);
    ll *lista = (ll*)malloc(sizeof(ll));
 
    // Verifica se a categoria existe
    if(node_cat){
 
        lista = node_cat->lista;
 
        while(lista != NULL){
            rrn = searchRrn(iprimary, lista->pk, nregistros);
	  		if(rrn != -1 && j > 0)
	 			printf("\n");

            if(exibir_registro(rrn, 0))
				j++;            
            lista = lista->prox;
        }
    }
    
    if(j == 0)
        printf(REGISTRO_N_ENCONTRADO);
 
}

// Listagem por indice secundario ibrand
void listagemPorMarca(Ip *iprimary, Is *ibrand, int nregistros){
 
    int i, rrn, j;
 
    for(i=0, j=0; i < nregistros; i++){
 
        rrn = searchRrn(iprimary, ibrand[i].pk, nregistros);
 		if(rrn != -1 && j > 0)
 			printf("\n");
        if(exibir_registro(rrn, 0))
			j++;    
    }
}

// Altera o desconto de um produto
int alterarDesconto(Ip *iprimary, Isf *iprice, Is *iproduct, int nregistros, char *pk){
 
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

// Remover um registro
int removerRegistro(Ip *iprimary, int nregistros, char *pk){
	
    Produto j;
    int i, rrn, pos;
    Ip *found = (Ip*)malloc(sizeof(Ip));
 
    found = bsearch(pk, iprimary, nregistros, sizeof(Ip),(void*)searchPK);
 
    if(found != NULL && found->rrn != -1){
        pos = found->rrn*192;
 
        ARQUIVO[pos] = '*';
        ARQUIVO[pos+1] = '|';

        found->rrn = -1;
    }
    else
        return 0;
 
    return 1;
}

// Remove do arquivo de dados os registros marcados para exclusão e refaz os indices
void limparArquivoDados(Ip *iprimary, int nregistros){
 
    Ip *reg;
    Produto p;
    int i, j, k, rrn, pos;
    char novo_arq[TAM_ARQUIVO], temp_arq[TAM_ARQUIVO], pk[TAM_PRIMARY_KEY];
 
    strcpy(temp_arq, ARQUIVO);
  
    for(i=0, j=0, k=0; i < TAM_ARQUIVO; i++){
 
        // Verifica se ta marcado para excluir
        if(temp_arq[i] == '*' && temp_arq[i+1] == '|'){
            i += 191;
        }
        else{
            novo_arq[j] = temp_arq[i];
            j++;
        }
 
    }
 
    novo_arq[i] = '\0';
 
    strcpy(ARQUIVO, novo_arq);
}