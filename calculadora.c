#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// --- CONSTANTES ---
#define RANGE_MIN_MULTIPLIER 0.80
#define RANGE_MAX_MULTIPLIER 1.20
#define STEP_MULTIPLIER 0.02

// --- ESTRUTURAS DE DADOS ---
typedef struct
{
    char type;
    double S;
    double K;
    double T;
    double r;
    double sigma;
    double premium;
} Option;

typedef struct
{
    Option opt;
    int position; // 1 (Long), -1 (Short)
    int quantity;
} Leg;

// --- PROTÓTIPOS ---
double get_intrinsic_value(Option opt, double spot_at_expiry);
double get_leg_pnl(Leg leg, double spot_at_expiry);
void print_summary(double custo_montagem);
void simulate_payoff(Leg *strategy, int num_legs, double global_S);
void clear_input_buffer();

// --- ÁLGEBRA DE PAYOFF ---
double get_intrinsic_value(Option opt, double spot_at_expiry)
{
    if (opt.type == 'C')
        return fmax(0.0, spot_at_expiry - opt.K);
    if (opt.type == 'P')
        return fmax(0.0, opt.K - spot_at_expiry);
    return 0.0;
}

double get_leg_pnl(Leg leg, double spot_at_expiry)
{
    double payoff = get_intrinsic_value(leg.opt, spot_at_expiry);
    return (payoff - leg.opt.premium) * leg.position * leg.quantity;
}

// --- UTILITÁRIOS DE TERMINAL ---
void clear_input_buffer()
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

void print_summary(double custo_montagem)
{
    printf("\n=========================================\n");
    printf("RESUMO DA ESTRUTURA\n");
    if (custo_montagem > 0)
    {
        printf("Custo Total de Montagem: DEBITO de R$ %.2f\n", custo_montagem);
    }
    else
    {
        printf("Custo Total de Montagem: CREDITO de R$ %.2f\n", -custo_montagem);
    }
    printf("=========================================\n\n");
}

void simulate_payoff(Leg *strategy, int num_legs, double global_S)
{
    double range_min = global_S * RANGE_MIN_MULTIPLIER;
    double range_max = global_S * RANGE_MAX_MULTIPLIER;
    double step = global_S * STEP_MULTIPLIER;

    printf("Simulacao de Payoff no Vencimento (T=0):\n");
    printf("Spot Final | PnL Total\n");
    printf("------------------------\n");

    for (double sim_spot = range_min; sim_spot <= range_max; sim_spot += step)
    {
        double total_pnl = 0.0;
        for (int i = 0; i < num_legs; i++)
        {
            total_pnl += get_leg_pnl(strategy[i], sim_spot);
        }
        printf("R$ %7.2f | R$ %7.2f\n", sim_spot, total_pnl);
    }
}

// --- FUNÇÃO PRINCIPAL ---
int main(void)
{
    printf("=========================================\n");
    printf("   CALCULADORA DE ESTRUTURAS DE OPCOES   \n");
    printf("=========================================\n\n");

    double global_S, global_T, global_r, global_sigma;

    // Validação básica do primeiro parâmetro crítico
    do
    {
        printf("Preco Atual do Ativo (S0 > 0): ");
        if (scanf("%lf", &global_S) != 1 || global_S <= 0)
        {
            printf("Erro: Insira um valor numerico positivo.\n");
            clear_input_buffer();
            global_S = -1; // Força a continuação do loop
        }
    }
    while (global_S <= 0);

    printf("Tempo ate vencimento (ex: 0.0833): ");
    scanf("%lf", &global_T);

    printf("Taxa Livre de Risco (ex: 0.105): ");
    scanf("%lf", &global_r);

    printf("Volatilidade Implicita (ex: 0.35): ");
    scanf("%lf", &global_sigma);

    int num_legs;
    do
    {
        printf("\nQuantas 'pernas' tem a operacao? (1 a 10): ");
        if (scanf("%d", &num_legs) != 1 || num_legs < 1 || num_legs > 10)
        {
            printf("Erro: Insira um numero valido entre 1 e 10.\n");
            clear_input_buffer();
            num_legs = 0;
        }
    }
    while (num_legs < 1);

    Leg *strategy = malloc(num_legs * sizeof(Leg));
    if (strategy == NULL)
    {
        printf("Erro Critico: Falha ao alocar memoria.\n");
        return 1;
    }

    double custo_montagem = 0.0;

    for (int i = 0; i < num_legs; i++)
    {
        printf("\n--- Inserindo Perna %d ---\n", i + 1);

        strategy[i].opt.S = global_S;
        strategy[i].opt.T = global_T;
        strategy[i].opt.r = global_r;
        strategy[i].opt.sigma = global_sigma;

        do
        {
            printf("Tipo (C para Call, P para Put): ");
            scanf(" %c", &strategy[i].opt.type);
            strategy[i].opt.type = toupper((unsigned char) strategy[i].opt.type);
        }
        while (strategy[i].opt.type != 'C' && strategy[i].opt.type != 'P');

        printf("Strike (K): ");
        scanf("%lf", &strategy[i].opt.K);

        printf("Premio Pago/Recebido: ");
        scanf("%lf", &strategy[i].opt.premium);

        printf("Posicao (1 p/ Long, -1 p/ Short): ");
        scanf("%d", &strategy[i].position);

        printf("Quantidade: ");
        scanf("%d", &strategy[i].quantity);

        custo_montagem += (strategy[i].opt.premium * strategy[i].position * strategy[i].quantity);
    }

    print_summary(custo_montagem);
    simulate_payoff(strategy, num_legs, global_S);

    free(strategy);
    return 0;
}
