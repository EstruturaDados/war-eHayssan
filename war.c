// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Substituir o vetor estático por alocação dinâmica com calloc
// - Criar uma função para simular ataques entre dois territórios
// - Utilizar números aleatórios para representar dados de batalha
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.

/* Novidades em relação ao Nível Aventureiro:
* Modularização total em funções
* Missões aleatórias atribuídas: 
i. destruir um exército aleatório
ii. conquistar certa quantidade territórios
* Menu interativo com opções
*/
// ============================================================================

// Inclusão das bibliotecas padrão necessárias para entrada/saída, alocação de memória, manipulação de strings e tempo.
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // Necessário para calloc e free
#include <time.h>   // Necessário para srand e time

// --- Constantes Globais ---
// Definem valores fixos para o número de territórios, missões e tamanho máximo de strings, facilitando a manutenção.
#define TAMANHO_NOME 30
#define TAMANHO_COR 10

// --- Estrutura de Dados ---
// Define a estrutura para um território, contendo seu nome, a cor do exército que o domina e o número de tropas.
typedef struct {
    char nome[TAMANHO_NOME];
    char corExercito[TAMANHO_COR];
    int quantidadeTropas;
} Territorio;

// --- Estruturas e Enums para Missões ---
// Define os tipos de missões possíveis no jogo para facilitar a verificação.
typedef enum {
    DESTRUIR_COR,
    CONQUISTAR_N_TERRITORIOS
} MissaoTipo;

// Estrutura para armazenar os detalhes da missão atual.
typedef struct {
    MissaoTipo tipo;
    char descricao[100];
    char corAlvo[TAMANHO_COR]; // Para missão DESTRUIR_COR
    int progressoTotal;        // Alvo para a missão (N territórios ou N exércitos a destruir)
} Missao;


// --- Protótipos das Funções ---
// Declarações antecipadas de todas as funções que serão usadas no programa, organizadas por categoria.

// Funções de lógica do jogo:
int atacar(Territorio *atacante, Territorio *defensor);
int compararDados(const void *a, const void *b);
void cadastrarTerritorio(Territorio *territorio, int numero);
void rolarDados(int *dados, int quantidade);
void liberarMemoria(Territorio *mapa);
void atribuirMissao(Missao *missao, const Territorio *territorios, int totalTerritorios);
int verificarMissaoCumprida(const Missao *missao, const Territorio *territorios, int totalTerritorios, int territoriosConquistados);
int obterProgressoMissao(const Missao *missao, const Territorio *territorios, int totalTerritorios, int territoriosConquistados);

// Funções de setup e gerenciamento de memória:
Territorio* inicializarJogo(int *totalTerritorios);
int executarLoopPrincipal(Territorio *territorios, int totalTerritorios);
void gerenciarFaseDeAtaque(Territorio *territorios, int totalTerritorios, int *territoriosConquistados);
int executarFaseDeAtaque(Territorio *territorios, int totalTerritorios, Missao *missao, int *territoriosConquistados);

// Remove caracteres residuais do buffer de entrada e evita problemas de leitura de strings
void limparBuffer();

// após scanf numério
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

/**
 * @brief Libera a memória alocada dinamicamente para o mapa de territórios.
 * @param mapa Ponteiro para o vetor de territórios a ser liberado.
 */
void liberarMemoria(Territorio *mapa) {
    free(mapa);
}


// Funções de interface com o usuário:

// Exibe o cabeçalho decorativo do sistema
void exibirCabecalho() {
    printf("\n");
    printf("==============================================================\n");
    printf("      SISTEMA DE CADASTRO DE TERRITÓRIOS - JOGO WAR           \n");
    printf("==============================================================\n");
    printf("\n");
}

// Realiza o cadastro de um território solicitando dados ao usuário
// territorio: estrutura onde os dados serão armazenados (passado por valor)
// numero: Número sequencial do território sendo cadastrado (para exibição)
void cadastrarTerritorio(Territorio *territorio, int numero) {
    printf("\n CADASTRO DO TERRITÓRIO %d\n", numero);
    printf("--------------------------------------------------------------\n");
    
    // Entrada do nome do território
    printf("Nome do território: ");
    fgets(territorio->nome, TAMANHO_NOME, stdin);
    // Remove o caractere de nova linha se presente
    territorio->nome[strcspn(territorio->nome, "\n")] = '\0';
    
    // Entrada da cor do exército
    printf("Cor do exército: ");
    fgets(territorio->corExercito, TAMANHO_COR, stdin);
    territorio->corExercito[strcspn(territorio->corExercito, "\n")] = '\0';
    
    // Entrada da quantidade de tropas com validação
    printf("Quantidade de tropas: ");
    while (scanf("%d", &territorio->quantidadeTropas) != 1 || territorio->quantidadeTropas < 0) {
        limparBuffer();
        printf("Valor inválido! Digite um número positivo: ");
    }
    limparBuffer(); // Limpa o buffer após scanf
    
    printf("Território cadastrado com sucesso!\n");
}

/*
 * Função: exibirTerritorio
 * ------------------------
 * Exibe as informações de um território formatadas
 * 
 * territorio: vetor estático os territórios cadastrados
 * numero: Número sequencial do território (para exibição)
 */
void exibirTerritorio(const Territorio territorio, int numero) {
    printf("\n TERRITÓRIO %d\n", numero);
    printf("   Nome: %s\n", territorio.nome);
    printf("   Exército: %s\n", territorio.corExercito);
    printf("   Tropas: %d\n", territorio.quantidadeTropas);
}

/*
 * Função: exibirMapaCompleto
 * -----------------------------
 * Exibe todos os territórios cadastrados de forma organizada
 * 
 * territorios: Vetor estático contendo todos os territórios cadastrados
 */
void exibirMapaCompleto(const Territorio *territorios, int totalTerritorios) {
    printf("\n\n");
    printf("-------------------------------------------------------------\n");
    printf("            RESUMO DOS TERRITÓRIOS CADASTRADOS               \n");
    printf("-------------------------------------------------------------\n");
    
    for (int i = 0; i < totalTerritorios; i++) {
        exibirTerritorio(territorios[i], i + 1);
    }
    
    printf("\n");
    printf("-------------------------------------------------------------\n");
    printf(" Total de territórios cadastrados: %d\n", totalTerritorios);
    printf("-------------------------------------------------------------\n");
}

/*
 * Função: calcularEstatisticas
 * ----------------------------
 * Calcula e exibe estatísticas sobre os territórios cadastrados
 * 
 * territorios: Vetor estático contendo todos os territórios cadastrados
 */
void calcularEstatisticas(const Territorio *territorios, int totalTerritorios) {
    int totalTropas = 0;
    int maiorExercito = territorios[0].quantidadeTropas;
    int menorExercito = territorios[0].quantidadeTropas;
    char territorioMaior[TAMANHO_NOME];
    char territorioMenor[TAMANHO_NOME];
    
    strcpy(territorioMaior, territorios[0].nome);
    strcpy(territorioMenor, territorios[0].nome);
    
    // Calcula estatísticas
    for (int i = 0; i < totalTerritorios; i++) {
        totalTropas += territorios[i].quantidadeTropas;
        
        if (territorios[i].quantidadeTropas > maiorExercito) {
            maiorExercito = territorios[i].quantidadeTropas;
            strcpy(territorioMaior, territorios[i].nome);
        }
        
        if (territorios[i].quantidadeTropas < menorExercito) {
            menorExercito = territorios[i].quantidadeTropas;
            strcpy(territorioMenor, territorios[i].nome);
        }
    }

    // Exibe estatísticas
    printf("\n ESTATÍSTICAS DO JOGO\n");
    printf("-------------------------------------------------------------\n");
    printf(" Total de tropas no tabuleiro: %d\n", totalTropas);
    printf(" Território mais fortificado: %s (%d tropas)\n", territorioMaior, maiorExercito);
    printf(" Território mais vulnerável: %s (%d tropas)\n", territorioMenor, menorExercito);
    printf(" Média de tropas por território: %.1f\n", (float)totalTropas / totalTerritorios);
    printf("-------------------------------------------------------------\n");
}

// --- Funções de Lógica do Jogo ---

/**
 * @brief Função de comparação para qsort, ordena em ordem decrescente.
 */
int compararDados(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

/**
 * @brief Rola uma quantidade de dados e os ordena em ordem decrescente.
 * @param dados Ponteiro para o array onde os resultados serão armazenados.
 * @param quantidade Número de dados a serem rolados.
 */
void rolarDados(int *dados, int quantidade) {
    for (int i = 0; i < quantidade; i++) {
        dados[i] = (rand() % 6) + 1;
    }
    qsort(dados, quantidade, sizeof(int), compararDados);
}

/**
 * @brief Simula um ataque de um território para outro.
 * @param atacante Ponteiro para o território que está atacando.
 * @param defensor Ponteiro para o território que está defendendo.
 * @return int Retorna 1 se o território foi conquistado, 0 caso contrário.
 */
int atacar(Territorio *atacante, Territorio *defensor) {
    int conquistado = 0; // Variável para rastrear se a conquista ocorreu
    if (atacante->quantidadeTropas <= 1) {
        printf("\nATAQUE FALHOU: O território atacante deve ter mais de 1 tropa para atacar.\n");
        return 0;
    }

    printf("\n--- BATALHA: %s (A) vs %s (D) ---\n", atacante->nome, defensor->nome);

    int perdasAtaque = 0;
    int perdasDefesa = 0;

    // Rola um dado para o atacante e um para o defensor
    int dadoAtaque = (rand() % 6) + 1;
    int dadoDefesa = (rand() % 6) + 1;

    printf("Rolando os dados...\n");
    printf(" -> Ataque (%s): %d\n", atacante->corExercito, dadoAtaque);
    printf(" -> Defesa (%s): %d\n", defensor->corExercito, dadoDefesa);

    // Se o dado de ataque for maior, o defensor perde. Caso contrário (menor ou igual), o atacante perde.
    if (dadoAtaque > dadoDefesa) {
        perdasDefesa = 1;
        printf("Vitória do atacante! O defensor perde 1 tropa.\n");
    } else {
        perdasAtaque = 1;
        printf("Vitória do defensor! O atacante perde 1 tropa.\n");
    }
    
    // Atualiza a quantidade de tropas
    atacante->quantidadeTropas -= perdasAtaque;
    defensor->quantidadeTropas -= perdasDefesa;

    printf("----------------------------------------\n");
    printf("Resultado da Batalha:\n");
    printf(" -> %s perdeu %d tropa(s) e agora tem %d.\n", atacante->nome, perdasAtaque, atacante->quantidadeTropas);
    printf(" -> %s perdeu %d tropa(s) e agora tem %d.\n", defensor->nome, perdasDefesa, defensor->quantidadeTropas);

    // Verifica se o território foi conquistado
    if (defensor->quantidadeTropas <= 0) {
        conquistado = 1; // Marca que o território foi conquistado
        printf("----------------------------------------\n");
        printf("!!! TERRITÓRIO %s CONQUISTADO PELO EXÉRCITO %s !!!\n", defensor->nome, atacante->corExercito);
        
        // Muda a cor do exército do território conquistado
        strcpy(defensor->corExercito, atacante->corExercito);

        // Verifica se o atacante tem tropas para mover (deve sobrar pelo menos 1)
        if (atacante->quantidadeTropas > 1) {
            // Pergunta ao jogador quantas tropas mover
            int tropasParaMover = 0;
            const int minTropas = 1; // Mínimo de 1 tropa para mover
            int maxTropas = atacante->quantidadeTropas - 1;

            printf("Você deve mover tropas para o novo território.\n");
            printf("Tropas disponíveis em %s: %d\n", atacante->nome, atacante->quantidadeTropas);
            do {
                printf("Quantas tropas deseja mover? (Mín: %d, Máx: %d): ", minTropas, maxTropas);
                scanf("%d", &tropasParaMover);
                limparBuffer();
            } while (tropasParaMover < minTropas || tropasParaMover > maxTropas);

            // Atualiza as tropas
            defensor->quantidadeTropas = tropasParaMover;
            atacante->quantidadeTropas -= tropasParaMover;
            printf("%d tropas movidas para %s. Tropas restantes em %s: %d\n", tropasParaMover, defensor->nome, atacante->nome, atacante->quantidadeTropas);
        } else {
            printf("O atacante não possui tropas suficientes para mover. O território conquistado ficará com 1 tropa.\n");
            defensor->quantidadeTropas = 1;
            atacante->quantidadeTropas -= 1;
        }
    }
    printf("----------------------------------------\n\n");
    return conquistado;
}

/**
 * @brief Atribui uma missão aleatória e dinâmica ao jogador.
 * @param missao Ponteiro para a estrutura da missão a ser preenchida.
 * @param territorios Ponteiro para o vetor de territórios para análise.
 * @param totalTerritorios Número total de territórios.
 */
void atribuirMissao(Missao *missao, const Territorio *territorios, int totalTerritorios) {
    int tipoMissaoSorteada = rand() % 2;

    if (tipoMissaoSorteada == 0) { // Missão: Destruir uma cor
        missao->tipo = DESTRUIR_COR;
        
        // Alocação dinâmica otimizada para coletar cores inimigas únicas
        char (*coresInimigas)[TAMANHO_COR] = NULL;
        int numCoresInimigas = 0;
        const char* corJogador = territorios[0].corExercito; // Assume que o jogador é o primeiro

        for (int i = 0; i < totalTerritorios; i++) {
            // Pula os territórios do próprio jogador
            if (strcmp(territorios[i].corExercito, corJogador) != 0) {
                int corJaExiste = 0;
                for (int j = 0; j < numCoresInimigas; j++) {
                    if (strcmp(coresInimigas[j], territorios[i].corExercito) == 0) {
                        corJaExiste = 1;
                        break;
                    }
                }
                // Se a cor for nova, realoca o array para adicionar mais um espaço
                if (!corJaExiste) {
                    char (*temp)[TAMANHO_COR] = realloc(coresInimigas, (numCoresInimigas + 1) * sizeof(char[TAMANHO_COR]));
                    if (temp == NULL) {
                        printf("Erro de realocação de memória. A missão pode não ser atribuída corretamente.\n");
                        // Libera a memória que já foi alocada antes de sair
                        free(coresInimigas);
                        // Fallback para missão padrão
                        missao->tipo = CONQUISTAR_N_TERRITORIOS;
                        missao->progressoTotal = 2;
                        sprintf(missao->descricao, "Conquistar %d territórios.", missao->progressoTotal);
                        return;
                    }
                    coresInimigas = temp;
                    strcpy(coresInimigas[numCoresInimigas++], territorios[i].corExercito);
                }
            }
        }

        if (numCoresInimigas > 0) {
            int corSorteada = rand() % numCoresInimigas;
            strcpy(missao->corAlvo, coresInimigas[corSorteada]);
            sprintf(missao->descricao, "Destruir completamente o exército %s.", missao->corAlvo);

            // Calcula o progresso total (quantos territórios da cor alvo existem)
            missao->progressoTotal = 0;
            for (int i = 0; i < totalTerritorios; i++) {
                if (strcmp(territorios[i].corExercito, missao->corAlvo) == 0) {
                    missao->progressoTotal++;
                }
            }
        } else {
            // Fallback se não houver inimigos (pouco provável, mas seguro)
            missao->tipo = CONQUISTAR_N_TERRITORIOS;
            missao->progressoTotal = 2;
            sprintf(missao->descricao, "Conquistar %d territórios.", missao->progressoTotal);
        }
        // Libera a memória alocada para a lista de cores, se houver alguma
        if (coresInimigas != NULL) {
            free(coresInimigas);
        }
    } else {
        missao->tipo = CONQUISTAR_N_TERRITORIOS;
        // Quantidade aleatória entre 2 e metade dos territórios
        int minConquistas = 2;
        int maxConquistas = totalTerritorios / 2;
        if (maxConquistas < minConquistas) maxConquistas = minConquistas;
        missao->progressoTotal = minConquistas + (rand() % (maxConquistas - minConquistas + 1));
        sprintf(missao->descricao, "Conquistar %d territórios.", missao->progressoTotal);
    }

    printf("\n--- SUA MISSÃO ---\n");
    printf("Objetivo: %s\n", missao->descricao);
    printf("--------------------\n");
}

/**
 * @brief Verifica se a missão atual do jogador foi cumprida.
 * @param missao Ponteiro para a missão atual.
 * @param territorios Ponteiro para o vetor de territórios.
 * @param totalTerritorios Número total de territórios.
 * @param territoriosConquistados Contagem de territórios conquistados pelo jogador.
 * @return int Retorna 1 se a missão foi cumprida, 0 caso contrário.
 */
int verificarMissaoCumprida(const Missao *missao, const Territorio *territorios, int totalTerritorios, int territoriosConquistados) {
    switch (missao->tipo) {
        case CONQUISTAR_N_TERRITORIOS:
            return territoriosConquistados >= missao->progressoTotal;
        
        case DESTRUIR_COR:
            for (int i = 0; i < totalTerritorios; i++) {
                if (strcmp(territorios[i].corExercito, missao->corAlvo) == 0) {
                    return 0; // Encontrou um território da cor alvo, missão não cumprida.
                }
            }
            return 1; // Não encontrou territórios da cor alvo.

        default:
            return 0;
    }
}

/**
 * @brief Calcula o progresso atual da missão para exibição.
 * @param missao Ponteiro para a missão atual.
 * @param territorios Ponteiro para o vetor de territórios.
 * @param totalTerritorios Número total de territórios.
 * @param territoriosConquistados Contagem de territórios conquistados pelo jogador.
 * @return int O valor do progresso atual.
 */
int obterProgressoMissao(const Missao *missao, const Territorio *territorios, int totalTerritorios, int territoriosConquistados) {
    switch (missao->tipo) {
        case CONQUISTAR_N_TERRITORIOS:
            return territoriosConquistados;
        
        case DESTRUIR_COR:
            int territoriosRestantes = 0;
            for (int i = 0; i < totalTerritorios; i++) {
                if (strcmp(territorios[i].corExercito, missao->corAlvo) == 0) {
                    territoriosRestantes++;
                }
            }
            return missao->progressoTotal - territoriosRestantes;
    }
    return 0;
}
/**
 * @brief Gerencia a fase de ataque, solicitando ao jogador os territórios e validando a ação.
 * @param territorios Ponteiro para o vetor de territórios.
 * @param totalTerritorios Número total de territórios.
 * @param territoriosConquistados Ponteiro para a contagem de territórios conquistados.
 */
void gerenciarFaseDeAtaque(Territorio *territorios, int totalTerritorios, int *territoriosConquistados) {
    int idxAtacante, idxDefensor;

    printf("\n--- FASE DE ATAQUE ---\n");
    exibirMapaCompleto(territorios, totalTerritorios);
    printf("Escolha o número do território ATACANTE (ou 0 para voltar): ");
    scanf("%d", &idxAtacante);
    if (idxAtacante == 0) return;

    printf("Escolha o número do território DEFENSOR: ");
    scanf("%d", &idxDefensor);
    limparBuffer();

    // Validação dos índices
    if (idxAtacante > 0 && idxAtacante <= totalTerritorios &&
        idxDefensor > 0 && idxDefensor <= totalTerritorios &&
        idxAtacante != idxDefensor) {
        
        Territorio *atacante = &territorios[idxAtacante - 1];
        Territorio *defensor = &territorios[idxDefensor - 1];

        // Validação de propriedade
        if (strcmp(atacante->corExercito, defensor->corExercito) == 0) {
            printf("\nAção inválida: Não é possível atacar um território que já pertence ao seu exército.\n");
            return;
        }

        // Chama a função de ataque e atualiza contador se território foi conquistado
        if (atacar(atacante, defensor)) {
            (*territoriosConquistados)++;
        }

        // Exibe o status atualizado dos territórios envolvidos
        printf("\n--- SITUAÇÃO PÓS-BATALHA ---\n");
        exibirTerritorio(*atacante, idxAtacante);
        exibirTerritorio(*defensor, idxDefensor);
    } else {
        printf("\nSeleção inválida! Verifique os números dos territórios e tente novamente.\n");
    }
}

/**
 * @brief Executa o menu e o loop da fase de ataque.
 * @param territorios Ponteiro para o vetor de territórios.
 * @param totalTerritorios Número total de territórios.
 * @param missao Ponteiro para a missão atual.
 * @param territoriosConquistados Ponteiro para a contagem de territórios conquistados.
 * @return int Retorna 1 se a missão foi cumprida, 0 caso contrário.
 */
int executarFaseDeAtaque(Territorio *territorios, int totalTerritorios, Missao *missao, int *territoriosConquistados) {
    int opcao;
    do {
        int progressoAtual = obterProgressoMissao(missao, territorios, totalTerritorios, *territoriosConquistados);

        printf("\n--- FASE DE ATAQUE ---\n");
        printf("MISSÃO: %s (Progresso: %d/%d)\n", missao->descricao, progressoAtual, missao->progressoTotal);
        printf("1. Atacar\n");
        printf("2. Verificar missão e mapa\n");
        printf("0. Encerrar fase de ataque\n");
        printf("Escolha uma ação: ");

        if (scanf("%d", &opcao) != 1) {
            opcao = -1;
        }
        limparBuffer();

        switch (opcao) {
            case 1:
                gerenciarFaseDeAtaque(territorios, totalTerritorios, territoriosConquistados);
                // Verifica se a missão foi cumprida após o ataque
                if (verificarMissaoCumprida(missao, territorios, totalTerritorios, *territoriosConquistados)) {
                    return 1; // Sinaliza que a missão foi cumprida
                }
                break;
            case 2:
                exibirMapaCompleto(territorios, totalTerritorios);
                printf("\nLEMBRETE DA MISSÃO: %s (Progresso: %d/%d)\n", missao->descricao, progressoAtual, missao->progressoTotal);
                break;
            case 0:
                break;
            default:
                printf("\nOpção inválida! Tente novamente.\n");
        }
    } while (opcao != 0);
    return 0; // Missão não cumprida nesta fase
}

/**
 * @brief Executa o loop principal do jogo, exibindo o menu e processando as opções do jogador.
 * @param territorios Ponteiro para o vetor de territórios.
 * @param totalTerritorios Número total de territórios.
 */
int executarLoopPrincipal(Territorio *territorios, int totalTerritorios) {
    int opcao;
    Missao missaoAtual;
    int territoriosConquistados = 0; // Contador para a missão de conquista

    atribuirMissao(&missaoAtual, territorios, totalTerritorios);

    do {
        int progressoAtual = obterProgressoMissao(&missaoAtual, territorios, totalTerritorios, territoriosConquistados);

        printf("\n--- MENU PRINCIPAL ---\n");
        printf("SUA MISSÃO: %s (Progresso: %d/%d)\n", missaoAtual.descricao, progressoAtual, missaoAtual.progressoTotal);
        printf("1. Exibir mapa completo\n");
        printf("2. Iniciar fase de ataque\n");
        printf("3. Exibir estatísticas\n");
        printf("0. Sair\n");
        printf("Escolha uma opção: ");
        
        if (scanf("%d", &opcao) != 1) {
            opcao = -1; // Força a opção inválida
        }
        limparBuffer();

        switch (opcao) {
            case 1:
                exibirMapaCompleto(territorios, totalTerritorios);
                break;
            case 2:
                // Entra no loop da fase de ataque e verifica se a missão foi cumprida lá
                if (executarFaseDeAtaque(territorios, totalTerritorios, &missaoAtual, &territoriosConquistados)) {
                    printf("\n\n====================================================\n");
                    printf("!!! PARABÉNS, VOCÊ CUMPRIU SUA MISSÃO: %s !!!\n", missaoAtual.descricao);
                    printf("====================================================\n\n");
                    return 1; // Retorna 1 para indicar que a missão foi cumprida
                }
                break;
            case 3:
                calcularEstatisticas(territorios, totalTerritorios);
                break;
            case 0:
                break;
            default:
                printf("\nOpção inválida! Tente novamente.\n");
        }
    } while (opcao != 0);
    return 0; // Retorna 0 para indicar saída normal pelo menu
}

/**
 * @brief Prepara o estado inicial do jogo, alocando memória e cadastrando os territórios.
 * @param totalTerritorios Ponteiro para armazenar o número total de territórios.
 * @return Ponteiro para o vetor de territórios alocado dinamicamente, ou NULL em caso de falha.
 */
Territorio* inicializarJogo(int *totalTerritorios) {
    exibirCabecalho();
    printf("Bem-vindo ao Sistema de Cadastro de Territórios!\n");

    printf("Quantos territórios você deseja cadastrar para o jogo (mínimo 3)? ");
    while (scanf("%d", totalTerritorios) != 1 || *totalTerritorios < 3) {
        limparBuffer();
        printf("Valor inválido! Digite um número inteiro maior ou igual a 3: ");
    }
    limparBuffer();

    Territorio *territorios = (Territorio *) calloc(*totalTerritorios, sizeof(Territorio));
    if (territorios == NULL) {
        printf("Erro: Falha na alocação de memória. O programa será encerrado.\n");
        return NULL;
    }

    printf("Você irá cadastrar %d territórios para iniciar o jogo.\n", *totalTerritorios);
    printf("-------------------------------------------------------------\n");

    for (int i = 0; i < *totalTerritorios; i++) {
        cadastrarTerritorio(&territorios[i], i + 1);
    }
    return territorios;
}

// --- Função Principal (main) ---
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.
int main() {
    int totalTerritorios = 0;
    // Inicializa o gerador de números aleatórios
    srand(time(NULL));

    // Prepara o tabuleiro e os territórios
    Territorio *territorios = inicializarJogo(&totalTerritorios);
    if (territorios == NULL) {
        return 1; // Retorna um código de erro
    }

    // Inicia o loop principal do jogo
    int missaoCumprida = executarLoopPrincipal(territorios, totalTerritorios);

    liberarMemoria(territorios); // Libera a memória alocada dinamicamente
    if (!missaoCumprida) {
        printf("\n Sistema finalizado com sucesso!\n");
        printf(" Que comecem os jogos!\n\n");
    }
    
    return 0;
}