// Desafio: Cartas Super Trunfo (C)
// Autor: você 😉
// Compilação (Linux/macOS): gcc -O2 -std=c11 src/super_trunfo.c -o super_trunfo
// Compilação (Windows MinGW): gcc -O2 -std=c11 src\super_trunfo.c -o super_trunfo.exe

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

// ----------------------------------------------
// Modelo de Carta
// ----------------------------------------------
typedef struct {
    char estado;           // 'A'..'H'
    char codigo[8];        // ex: "A01"
    char nome[64];         // nome da cidade
    int  populacao;        // habitantes
    float area_km2;        // km²
    float pib_bilhoes;     // em bilhões
} Carta;

// ----------------------------------------------
// Funções utilitárias de entrada/limpeza
// ----------------------------------------------
static void limpar_entrada(void) {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {}
}

static void ler_linha(char *buf, size_t n) {
    if (fgets(buf, (int)n, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    // remove \n
    size_t len = strlen(buf);
    if (len && buf[len-1] == '\n') buf[len-1] = '\0';
}

static int ler_int(const char *prompt, int minv, int maxv, int *out) {
    while (1) {
        printf("%s", prompt);
        char tmp[64];
        ler_linha(tmp, sizeof(tmp));
        if (tmp[0] == '\0') continue;
        char *end;
        long v = strtol(tmp, &end, 10);
        if (*end == '\0' && v >= minv && v <= maxv) {
            *out = (int)v;
            return 1;
        }
        printf("  Valor inválido. Digite um inteiro entre %d e %d.\n", minv, maxv);
    }
}

static int ler_float(const char *prompt, float minv, float maxv, float *out) {
    while (1) {
        printf("%s", prompt);
        char tmp[64];
        ler_linha(tmp, sizeof(tmp));
        if (tmp[0] == '\0') continue;
        char *end;
        float v = strtof(tmp, &end);
        if (*end == '\0' && v >= minv && v <= maxv) {
            *out = v;
            return 1;
        }
        printf("  Valor inválido. Digite um número entre %.2f e %.2f.\n", minv, maxv);
    }
}

// ----------------------------------------------
// Coleta e validação dos campos
// ----------------------------------------------
static void ler_estado(Carta *c) {
    while (1) {
        printf("Estado (A..H): ");
        char tmp[16];
        ler_linha(tmp, sizeof(tmp));
        if (strlen(tmp) == 1) {
            char ch = (char)toupper((unsigned char)tmp[0]);
            if (ch >= 'A' && ch <= 'H') {
                c->estado = ch;
                return;
            }
        }
        printf("  Inválido. Digite uma letra entre A e H.\n");
    }
}

static void ler_codigo(Carta *c) {
    while (1) {
        printf("Código da carta (ex.: A01, B03): ");
        char tmp[16];
        ler_linha(tmp, sizeof(tmp));
        // normaliza
        for (size_t i = 0; tmp[i]; ++i) tmp[i] = (char)toupper((unsigned char)tmp[i]);
        // regra simples: letra A..H + 2 dígitos
        if (strlen(tmp) == 3 &&
            tmp[0] >= 'A' && tmp[0] <= 'H' &&
            isdigit((unsigned char)tmp[1]) &&
            isdigit((unsigned char)tmp[2])) {
            strncpy(c->codigo, tmp, sizeof(c->codigo));
            c->codigo[sizeof(c->codigo)-1] = '\0';
            return;
        }
        printf("  Inválido. Use formato Letra(A..H)+Dois dígitos. Ex.: C02\n");
    }
}

static void ler_nome(Carta *c) {
    while (1) {
        printf("Nome da cidade: ");
        ler_linha(c->nome, sizeof(c->nome));
        if (strlen(c->nome) >= 2) return;
        printf("  Digite pelo menos 2 caracteres.\n");
    }
}

static void ler_populacao(Carta *c) {
    (void)ler_int("População (habitantes): ", 1, 2000000000, &c->populacao);
}

static void ler_area(Carta *c) {
    (void)ler_float("Área (km²): ", 0.01f, 30000000.f, &c->area_km2);
}

static void ler_pib(Carta *c) {
    (void)ler_float("PIB (em bilhões): ", 0.001f, 1000000.f, &c->pib_bilhoes);
}

// ----------------------------------------------
// Impressão e métricas derivadas
// ----------------------------------------------
static float densidade(const Carta *c) {       // hab/km²
    return (c->area_km2 > 0.f) ? (c->populacao / c->area_km2) : 0.f;
}

static float pib_per_capita(const Carta *c) {  // em milhares (bilhões / população)
    // PIB em bilhões -> converte pra unidades: bilhões * 1e9 / população
    if (c->populacao <= 0) return 0.f;
    double pib_reais = (double)c->pib_bilhoes * 1e9;
    return (float)(pib_reais / (double)c->populacao);
}

static void imprimir_carta(const Carta *c) {
    printf("\n--- CARTA %s (%c) ---\n", c->codigo, c->estado);
    printf("Cidade:         %s\n", c->nome);
    printf("População:      %d hab\n", c->populacao);
    printf("Área:           %.2f km²\n", c->area_km2);
    printf("PIB:            %.3f bilhões\n", c->pib_bilhoes);
    printf("Densidade:      %.2f hab/km²\n", densidade(c));
    printf("PIB per capita: R$ %.2f\n", pib_per_capita(c));
}

// ----------------------------------------------
// Comparação (retorna: -1 = c1 perde, 0 = empate, 1 = c1 vence)
// ----------------------------------------------
typedef enum {
    ATR_POPULACAO = 1,
    ATR_AREA = 2,
    ATR_PIB = 3,
    ATR_DENSIDADE = 4,     // aqui, MENOR densidade vence (ex.: “melhor qualidade de vida”) — critério ilustrativo
    ATR_PIB_PER_CAPITA = 5 // aqui, MAIOR vence
} Atributo;

static int comparar(const Carta *c1, const Carta *c2, Atributo atr) {
    switch (atr) {
        case ATR_POPULACAO:
            return (c1->populacao > c2->populacao) - (c1->populacao < c2->populacao);
        case ATR_AREA:
            return (c1->area_km2 > c2->area_km2) - (c1->area_km2 < c2->area_km2);
        case ATR_PIB:
            return (c1->pib_bilhoes > c2->pib_bilhoes) - (c1->pib_bilhoes < c2->pib_bilhoes);
        case ATR_DENSIDADE: {
            float d1 = densidade(c1), d2 = densidade(c2);
            // MENOR vence
            return (d2 > d1) - (d2 < d1);
        }
        case ATR_PIB_PER_CAPITA: {
            float p1 = pib_per_capita(c1), p2 = pib_per_capita(c2);
            return (p1 > p2) - (p1 < p2);
        }
        default:
            return 0;
    }
}

static void mostrar_menu(void) {
    printf("\n=== Escolha o atributo para comparar ===\n");
    printf("1) População (MAIOR vence)\n");
    printf("2) Área (MAIOR vence)\n");
    printf("3) PIB em bilhões (MAIOR vence)\n");
    printf("4) Densidade (MENOR vence)\n");
    printf("5) PIB per capita (MAIOR vence)\n");
    printf("Seleção: ");
}

// ----------------------------------------------
// Principal
// ----------------------------------------------
int main(void) {
    Carta c1 = {0}, c2 = {0};

    printf("===== Cadastro da Carta 1 =====\n");
    ler_estado(&c1);
    ler_codigo(&c1);
    ler_nome(&c1);
    ler_populacao(&c1);
    ler_area(&c1);
    ler_pib(&c1);

    printf("\n===== Cadastro da Carta 2 =====\n");
    ler_estado(&c2);
    ler_codigo(&c2);
    ler_nome(&c2);
    ler_populacao(&c2);
    ler_area(&c2);
    ler_pib(&c2);

    printf("\n===== Cartas Cadastradas =====\n");
    imprimir_carta(&c1);
    imprimir_carta(&c2);

    while (1) {
        mostrar_menu();
        int op = 0;
        if (!ler_int("", 1, 5, &op)) continue;

        int res = comparar(&c1, &c2, (Atributo)op);
        printf("\n>>> Resultado: ");
        if (res > 0) {
            printf("CARTA 1 (%s) VENCEU no critério escolhido!\n", c1.codigo);
        } else if (res < 0) {
            printf("CARTA 2 (%s) VENCEU no critério escolhido!\n", c2.codigo);
        } else {
            printf("EMPATE no critério escolhido!\n");
        }

        printf("\nDeseja comparar outro atributo? (S/N): ");
        char ch = 0; scanf(" %c", &ch); limpar_entrada();
        if (toupper((unsigned char)ch) != 'S') break;
    }

    printf("\nObrigado! Encerrando.\n");
    return 0;
}
