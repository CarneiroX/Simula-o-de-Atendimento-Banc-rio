#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Definições para tipos de operações
#define SAQUE 0
#define DEPOSITO 1
#define PAGAMENTO 2

// Tempos de duração das operações em segundos
#define TEMPO_SAQUE 60
#define TEMPO_DEPOSITO 90
#define TEMPO_PAGAMENTO 120

#define EXPEDIENTE 21600

typedef struct Cliente {
    int tempo_chegada;
    int tipo_operacao; // Tipo de operação: SAQUE, DEPOSITO ou PAGAMENTO
    int tempo_atendimento;
    struct Cliente* proximo;
} Cliente;

// Estrutura dos guichês
typedef struct Guiche {
    int tempo_disponivel;   // Tempo restante de atendimento no guichê
    int atendendo_cliente;
} Guiche;

// Estrutura para representar uma fila de clientes
typedef struct Fila {
    Cliente* inicio;
    Cliente* fim;
    int tamanho;
} Fila;

void inicializaFila(Fila* fila) {
    fila->inicio = NULL;
    fila->fim = NULL;
    fila->tamanho = 0;
}

int filaVazia(Fila* fila) {
    return fila->tamanho == 0;
}

// Adiciona um novo cliente na fila
void enfileira(Fila* fila, int tempo_chegada, int tipo_operacao) {
    Cliente* novo = (Cliente*)malloc(sizeof(Cliente));
    novo->tempo_chegada = tempo_chegada;
    novo->tipo_operacao = tipo_operacao;
    novo->tempo_atendimento = 0;
    novo->proximo = NULL;

    if (filaVazia(fila)) {
        fila->inicio = novo;
    } else {
        fila->fim->proximo = novo;
    }
    fila->fim = novo;
    fila->tamanho++;
}

// Remove e retorna o primeiro cliente da fila
Cliente* desenfileira(Fila* fila) {
    if (filaVazia(fila)) {
        return NULL;
    }

    Cliente* temp = fila->inicio;
    fila->inicio = fila->inicio->proximo;
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }
    fila->tamanho--;

    return temp;
}

// Limpa toda a fila
void limpaFila(Fila* fila) {
    while (!filaVazia(fila)) {
        Cliente* temp = fila->inicio;
        fila->inicio = fila->inicio->proximo;
        free(temp);
    }
    fila->fim = NULL;
    fila->tamanho = 0;
}

int main() {
    srand(time(NULL));  // Inicializa a geração de números aleatórios
    Fila fila;
    inicializaFila(&fila);  // Inicializa a fila de clientes vazia

    Guiche guiches[3] = {{0, 0}, {0, 0}, {0, 0}}; // Inicializa os guichês com tempo disponível zero e sem cliente

    int tempo;
    int tempo_total_espera = 0;
    int clientes_atendidos = 0;
    int num_saques = 0, num_depositos = 0, num_pagamentos = 0;

    // Loop que simula o expediente de atendimento
    for (tempo = 0; tempo < EXPEDIENTE; tempo++) {
        if (rand() % 30 == 0) {
        int tipo_operacao = rand() % 3;
        enfileira(&fila, tempo, tipo_operacao);
        }

        // Atendimento dos clientes nos guichês
        for (int i = 0; i < 3; i++) {
            if (guiches[i].tempo_disponivel == 0 && !filaVazia(&fila) && !guiches[i].atendendo_cliente) {
                guiches[i].atendendo_cliente = 1;
                Cliente* cliente = desenfileira(&fila);
                int tipo_transacao = cliente->tipo_operacao;

                // Inicia o atendimento de acordo com o tipo de operação
                switch (tipo_transacao) {
                    case SAQUE:
                        guiches[i].tempo_disponivel = TEMPO_SAQUE;
                        num_saques++;
                        break;
                    case DEPOSITO:
                        guiches[i].tempo_disponivel = TEMPO_DEPOSITO;
                        num_depositos++;
                        break;
                    case PAGAMENTO:
                        guiches[i].tempo_disponivel = TEMPO_PAGAMENTO;
                        num_pagamentos++;
                        break;
                }

                // Calcula o tempo de espera do cliente na fila
                tempo_total_espera += (tempo - cliente->tempo_chegada);
                clientes_atendidos++; 

                guiches[i].atendendo_cliente = 0;  // Marca o guichê como disponível
                free(cliente);  // Libera a memória do cliente
            }
        }

        // Atualiza o tempo disponível nos guichês
        for (int i = 0; i < 3; i++) {
            if (guiches[i].tempo_disponivel > 0) {
                guiches[i].tempo_disponivel--;
            }
        }
    }

    // Atendimento dos clientes restantes após o expediente
    float tempo_extra = 0;
    while (!filaVazia(&fila)) {
        for (int i = 0; i < 3; i++) {
            if (guiches[i].tempo_disponivel == 0 && !filaVazia(&fila) && !guiches[i].atendendo_cliente) {
                guiches[i].atendendo_cliente = 1;
                Cliente* cliente = desenfileira(&fila);
                int tipo_transacao = cliente->tipo_operacao;

                switch (tipo_transacao) {
                    case SAQUE:
                        guiches[i].tempo_disponivel = TEMPO_SAQUE;
                        num_saques++;
                        break;
                    case DEPOSITO:
                        guiches[i].tempo_disponivel = TEMPO_DEPOSITO;
                        num_depositos++;
                        break;
                    case PAGAMENTO:
                        guiches[i].tempo_disponivel = TEMPO_PAGAMENTO;
                        num_pagamentos++;
                        break;
                }

                tempo_total_espera += (EXPEDIENTE + tempo_extra - cliente->tempo_chegada);
                clientes_atendidos++;

                guiches[i].atendendo_cliente = 0;
                free(cliente);
            }
        }

        for (int i = 0; i < 3; i++) {
            if (guiches[i].tempo_disponivel > 0) {
                guiches[i].tempo_disponivel--;
            }
        }

        tempo_extra++;
    }

    // Cálculo do tempo médio de espera na fila
    float tempo_medio_espera = ((clientes_atendidos == 0) ? 0 : (float)tempo_total_espera / clientes_atendidos);

    // Conversão do tempo médio de espera e tempo extra para minutos e segundos
    int minutos_espera = (int)tempo_medio_espera / 60;
    int segundos_espera = (int)tempo_medio_espera % 60;
    int minutos_extra = (int)tempo_extra / 60;
    int segundos_extra = (int)tempo_extra % 60;

    // Impressão das estatísticas
    printf("\nEstatísticas:\n");
    printf("Número total de clientes atendidos: %d\n", clientes_atendidos);
    printf("Número de saques: %d\n", num_saques);
    printf("Número de depósitos: %d\n", num_depositos);
    printf("Número de pagamentos: %d\n", num_pagamentos);
    printf("Tempo médio de espera na fila: %d.%d minutos\n", minutos_espera, segundos_espera);
    printf("Tempo extra de expediente: %d.%d minutos\n", minutos_extra, segundos_extra);

    limpaFila(&fila); // Limpa a fila antes de terminar o programa

    return 0;
}
