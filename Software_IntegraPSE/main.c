
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_ACOES 100

/* Comeca em 1 para que o valor 0 nunca seja confundido com uma situacao. */
typedef enum { PLANEJADA = 1, REALIZADA, CANCELADA } Situacao;

typedef struct {
    int codigo;
    char escola[60];
    char tema[60];
    char data[11];               
    char publicoAlvo[40];
    char responsavel[50];
    int participantesPrevistos;
    int participantesEfetivos;   
    Situacao situacao;
} Acao;

Acao acoes[MAX_ACOES];   /* acoes cadastradas */
int totalAcoes = 0;      /* quantas posicoes do vetor estao em uso */

int exibirMenu(void);
void cadastrarAcao(void);
void listarAcoes(void);
void pesquisarAcoes(void);
void atualizarSituacao(void);
void gerarResumo(void);
int codigoExiste(int codigo);
int contemIgnoreCase(const char *texto, const char *busca);
const char *situacaoParaTexto(Situacao s);
int lerLinha(char *buffer, int tamanho);
int lerTexto(char *destino, int tamanho, const char *mensagem);
int lerInteiro(int *destino, const char *mensagem, int min, int max);
int lerData(char *destino, const char *mensagem);
int lerSituacao(Situacao *s);
int anoBissexto(int ano);
int diasNoMes(int mes, int ano);

/* main enxuta: so repete o menu e chama a funcao escolhida. */
int main(void) {
    int opcao;

    printf("=== Gerenciador de Acoes do PSE ===\n");

    do {
        opcao = exibirMenu();

        switch (opcao) {
            case 1: cadastrarAcao(); break;
            case 2: listarAcoes(); break;
            case 3: pesquisarAcoes(); break;
            case 4: atualizarSituacao(); break;
            case 5: gerarResumo(); break;
            case 0: printf("\nSaindo...\n"); break;
        }                
    } while (opcao != 0);

    return 0;
}

int lerLinha(char *buffer, int tamanho) {
    char *fim;
    int c;

    if (fgets(buffer, tamanho, stdin) == NULL) {
        return 0;
    }

    fim = strchr(buffer, '\n');
    if (fim != NULL) {
        *fim = '\0';
    } else {
        while ((c = getchar()) != '\n' && c != EOF) {
           
        }
    }
    return 1;
}

/* Mostra o menu e devolve a opcao (0 a 5) ou -1 se a entrada for invalida. */
int exibirMenu(void) {
    char linha[20];
    char *fim;
    long opcao;

    printf("\n--- MENU ---\n"
           "1 - Cadastrar acao\n"
           "2 - Listar acoes\n"
           "3 - Pesquisar acoes\n"
           "4 - Atualizar situacao\n"
           "5 - Resumo geral\n"
           "0 - Sair\n"
           "Escolha: ");

    if (!lerLinha(linha, sizeof(linha))) {
        return 0;        
    }

    opcao = strtol(linha, &fim, 10);
    if (fim == linha || *fim != '\0' || opcao < 0 || opcao > 5) {
        printf("Opcao invalida. Digite um numero de 0 a 5.\n");
        return -1;
    }
    return (int)opcao;
}

int lerTexto(char *destino, int tamanho, const char *mensagem) {
    char linha[128];
    int i = 0;

    printf("%s", mensagem);
    if (!lerLinha(linha, sizeof(linha))) {
        return 0;
    }

    while (isspace((unsigned char)linha[i])) {
        i++;            
    }
    if (linha[i] == '\0') {
        printf("Erro: campo obrigatorio vazio ou so espacos.\n");
        return 0;
    }
    if ((int)strlen(linha) >= tamanho) {
        printf("Erro: texto excede o limite de %d caracteres.\n", tamanho - 1);
        return 0;
    }

    strcpy(destino, linha);
    return 1;
}


int lerInteiro(int *destino, const char *mensagem, int min, int max) {
    char linha[32];
    char *fim;
    long valor;

    printf("%s", mensagem);
    if (!lerLinha(linha, sizeof(linha))) {
        return 0;
    }

    valor = strtol(linha, &fim, 10);
    if (fim == linha || *fim != '\0') {
        printf("Entrada invalida. Digite apenas numeros.\n");
        return 0;
    }
    if (valor < min || valor > max) {
        printf("Valor fora do intervalo permitido (%d a %d).\n", min, max);
        return 0;
    }

    *destino = (int)valor;
    return 1;
}

int anoBissexto(int ano) {
    return (ano % 4 == 0 && ano % 100 != 0) || ano % 400 == 0;
}

int diasNoMes(int mes, int ano) {
    int dias[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (mes == 2 && anoBissexto(ano)) {
        return 29;
    }
    return dias[mes - 1];
}

int lerData(char *destino, const char *mensagem) {
    char linha[32];
    int dia, mes, ano, i;

    printf("%s", mensagem);
    if (!lerLinha(linha, sizeof(linha))) {
        return 0;
    }

    if (strlen(linha) != 10 || linha[2] != '/' || linha[5] != '/') {
        printf("Data invalida. Use DD/MM/AAAA.\n");
        return 0;
    }
    for (i = 0; i < 10; i++) {
        if (i != 2 && i != 5 && !isdigit((unsigned char)linha[i])) {
            printf("Data invalida. Use apenas numeros em dia, mes e ano.\n");
            return 0;
        }
    }

    sscanf(linha, "%d/%d/%d", &dia, &mes, &ano);

    if (ano < 1000 || mes < 1 || mes > 12 || dia < 1 || dia > diasNoMes(mes, ano)) {
        printf("Data invalida. Confira dia, mes e ano.\n");
        return 0;
    }

    strcpy(destino, linha);
    return 1;
}

int lerSituacao(Situacao *s) {
    int valor;

    if (!lerInteiro(&valor, "Situacao (1=Planejada, 2=Realizada, 3=Cancelada): ", 1, 3)) {
        return 0;
    }
    *s = (Situacao)valor;
    return 1;
}

int codigoExiste(int codigo) {
    int i;

    for (i = 0; i < totalAcoes; i++) {
        if (acoes[i].codigo == codigo) {
            return i;
        }
    }
    return -1;
}

/*
 * Busca parcial sem diferenciar maiusculas de minusculas.
 * Tenta casar 'busca' a partir de cada posicao de 'texto', comparando letra
 * a letra com tolower. Nao usa buffers auxiliares, entao funciona para
 * qualquer tamanho de texto. Ao chegar no '\0' de 'texto' a comparacao para
 * sozinha, pois '\0' nunca e igual a uma letra de 'busca'.
 */
int contemIgnoreCase(const char *texto, const char *busca) {
    int i, j;

    for (i = 0; texto[i] != '\0'; i++) {
        j = 0;
        while (busca[j] != '\0' &&
               tolower((unsigned char)texto[i + j]) == tolower((unsigned char)busca[j])) {
            j++;
        }
        if (busca[j] == '\0') {
            return 1;    
        }
    }
    return 0;
}

const char *situacaoParaTexto(Situacao s) {
    if (s == PLANEJADA) {
        return "Planejada";
    }
    if (s == REALIZADA) {
        return "Realizada";
    }
    return "Cancelada";
}

void cadastrarAcao(void) {
    Acao *nova;

    printf("\n--- Cadastrar Acao ---\n");

    if (totalAcoes >= MAX_ACOES) {
        printf("Erro: vetor cheio (maximo %d acoes).\n", MAX_ACOES);
        return;
    }

    nova = &acoes[totalAcoes];
    nova->participantesEfetivos = 0;

    if (!lerInteiro(&nova->codigo, "Codigo: ", 1, 999999)) {
        printf("Cadastro cancelado.\n");
        return;
    }
    if (codigoExiste(nova->codigo) != -1) {
        printf("Erro: codigo %d ja existe.\n", nova->codigo);
        return;
    }

    if (!lerTexto(nova->escola, sizeof(nova->escola), "Escola: ") ||
        !lerTexto(nova->tema, sizeof(nova->tema), "Tema: ") ||
        !lerData(nova->data, "Data (DD/MM/AAAA): ") ||
        !lerTexto(nova->publicoAlvo, sizeof(nova->publicoAlvo), "Publico alvo: ") ||
        !lerTexto(nova->responsavel, sizeof(nova->responsavel), "Responsavel: ") ||
        !lerInteiro(&nova->participantesPrevistos, "Participantes previstos: ", 0, 999999) ||
        !lerSituacao(&nova->situacao) ||
        (nova->situacao == REALIZADA &&
         !lerInteiro(&nova->participantesEfetivos, "Participantes efetivos: ", 0, 999999))) {
        printf("Cadastro cancelado.\n");
        return;
    }

    totalAcoes++;
    printf("Acao cadastrada com sucesso!\n");
}
/* Tabela compacta (cerca de 100 colunas); textos longos sao cortados na exibicao. */
void listarAcoes(void) {
    int i;

    printf("\n--- Listagem de Acoes ---\n");

    if (totalAcoes == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }

    printf("%-6s | %-20s | %-20s | %-10s | %-9s | %-9s | %-8s\n",
           "Codigo", "Escola", "Tema", "Data", "Situacao", "Previstos", "Efetivos");

    for (i = 0; i < totalAcoes; i++) {
        printf("%-6d | %-20.20s | %-20.20s | %-10s | %-9s | %-9d | %-8d\n",
               acoes[i].codigo, acoes[i].escola, acoes[i].tema, acoes[i].data,
               situacaoParaTexto(acoes[i].situacao),
               acoes[i].participantesPrevistos, acoes[i].participantesEfetivos);
    }

    printf("Total: %d acoes.\n", totalAcoes);
}

/* Pesquisa por codigo (exato) ou por escola/tema (parcial). Mesmo formato de saida nos tres casos. */
void pesquisarAcoes(void) {
    int opcao, codigoBusca = 0, i, achou, encontrados = 0;
    char busca[80] = "";

    printf("\n--- Pesquisar Acoes ---\n");

    if (totalAcoes == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }

    printf("1 - Por codigo\n2 - Por escola\n3 - Por tema\n");

    if (!lerInteiro(&opcao, "Escolha: ", 1, 3) ||
        (opcao == 1 && !lerInteiro(&codigoBusca, "Digite o codigo: ", 1, 999999)) ||
        (opcao != 1 && !lerTexto(busca, sizeof(busca), "Digite o trecho a buscar: "))) {
        printf("Pesquisa cancelada.\n");
        return;
    }

    printf("\nResultados:\n");

    for (i = 0; i < totalAcoes; i++) {
        if (opcao == 1) {
            achou = (acoes[i].codigo == codigoBusca);
        } else if (opcao == 2) {
            achou = contemIgnoreCase(acoes[i].escola, busca);
        } else {
            achou = contemIgnoreCase(acoes[i].tema, busca);
        }

        if (achou) {
            printf("Codigo: %d | Escola: %s | Tema: %s\n"
                   "  Data: %s | Publico alvo: %s | Responsavel: %s\n"
                   "  Previstos: %d | Efetivos: %d | Situacao: %s\n",
                   acoes[i].codigo, acoes[i].escola, acoes[i].tema,
                   acoes[i].data, acoes[i].publicoAlvo, acoes[i].responsavel,
                   acoes[i].participantesPrevistos, acoes[i].participantesEfetivos,
                   situacaoParaTexto(acoes[i].situacao));
            encontrados++;
        }
    }

    if (encontrados == 0) {
        printf("Nenhuma acao encontrada.\n");
    }
}

/*
 * Altera a situacao de uma acao. Regras: cancelada e realizada nao se
 * convertem uma na outra; ao virar REALIZADA pede os participantes efetivos;
 * em qualquer outra situacao os efetivos voltam a 0 (sem dado antigo).
 */
void atualizarSituacao(void) {
    int codigo, indice;
    Situacao nova;

    printf("\n--- Atualizar Situacao ---\n");

    if (totalAcoes == 0) {
        printf("Nenhuma acao cadastrada.\n");
        return;
    }

    if (!lerInteiro(&codigo, "Codigo da acao: ", 1, 999999)) {
        printf("Atualizacao cancelada.\n");
        return;
    }

    indice = codigoExiste(codigo);
    if (indice == -1) {
        printf("Erro: acao com codigo %d nao encontrada.\n", codigo);
        return;
    }

    printf("Acao: %s - %s\nSituacao atual: %s\n",
           acoes[indice].escola, acoes[indice].tema,
           situacaoParaTexto(acoes[indice].situacao));

    if (!lerSituacao(&nova)) {
        printf("Atualizacao cancelada.\n");
        return;
    }

    if ((acoes[indice].situacao == CANCELADA && nova == REALIZADA) ||
        (acoes[indice].situacao == REALIZADA && nova == CANCELADA)) {
        printf("Erro: nao e possivel mudar de %s para %s.\n",
               situacaoParaTexto(acoes[indice].situacao), situacaoParaTexto(nova));
        return;
    }

    if (nova != REALIZADA) {
        acoes[indice].participantesEfetivos = 0;
    } else if (!lerInteiro(&acoes[indice].participantesEfetivos,
                           "Participantes efetivos: ", 0, 999999)) {
        printf("Atualizacao cancelada.\n");
        return;
    }

    acoes[indice].situacao = nova;
    printf("Situacao atualizada com sucesso!\n");
}

/*
 * Resumo geral. qtd[] conta por situacao (usa o proprio valor do enum como
 * posicao, por isso ele comeca em 1). Percentual = efetivos / previstos das
 * acoes REALIZADAS; sem previstos, fica 0% (evita divisao por zero).
 */
void gerarResumo(void) {
    int i, qtd[4] = {0, 0, 0, 0};
    int totalEfetivos = 0, totalPrevistos = 0;
    double percentual = 0.0;

    printf("\n--- Resumo Geral ---\n");

    for (i = 0; i < totalAcoes; i++) {
        qtd[acoes[i].situacao]++;
        if (acoes[i].situacao == REALIZADA) {
            totalEfetivos += acoes[i].participantesEfetivos;
            totalPrevistos += acoes[i].participantesPrevistos;
        }
    }

    if (totalPrevistos > 0) {
        percentual = (double)totalEfetivos / totalPrevistos * 100.0;
    }

    printf("Planejadas: %d\nRealizadas: %d\nCanceladas: %d\n",
           qtd[PLANEJADA], qtd[REALIZADA], qtd[CANCELADA]);
    printf("Total de participantes efetivos: %d\n", totalEfetivos);
    printf("Percentual (efetivos/previstos das realizadas): %.2f%%\n", percentual);
}