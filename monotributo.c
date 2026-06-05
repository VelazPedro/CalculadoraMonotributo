/*
 * ============================================================
 *  CALCULADORA DE MONOTRIBUTO ARGENTINA
 *  Valores vigentes desde febrero 2026
 *  Fuente: ARCA (ex AFIP) - Actualización IPC sem 2/2025 (14.28%)
 * ============================================================
 *
 *  Compilar:  gcc -o monotributo monotributo.c -lm
 *  Ejecutar:  ./monotributo
 * ============================================================
 */
#define _WIN32_WINNT 0x0600
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* ── Colores ANSI (funcionan en Linux/Mac; en Windows usar terminal moderno) ── */
#define COLOR_RESET   "\033[0m"
#define COLOR_BOLD    "\033[1m"
#define COLOR_VERDE   "\033[1;32m"
#define COLOR_AMARILLO "\033[1;33m"
#define COLOR_CYAN    "\033[1;36m"
#define COLOR_ROJO    "\033[1;31m"

/* ── Tabla de categorías ─────────────────────────────────────────────────── */

#define NUM_CATEGORIAS 11

typedef struct {
    char   letra;
    double tope_anual;        /* ingreso bruto anual máximo ($) */
    double imp_integrado_srv; /* impuesto integrado - servicios */
    double imp_integrado_bien;/* impuesto integrado - venta bienes */
    double aporte_sipa;       /* jubilación (SIPA) */
    double obra_social;       /* obra social */
} Categoria;

/*
 *  Valores de febrero 2026
 *  (actualización +14,28% sobre tabla agosto 2025)
 *
 *  Categorías A-B: impuesto integrado igual para servicios y bienes.
 *  Categorías C-K: servicios tributan más que bienes en impuesto integrado.
 *
 *  Fuente: ARCA / indicadores.ar / infobae / ambito feb 2026
 */
static const Categoria TABLA[NUM_CATEGORIAS] = {
/*  L    Tope anual           Srv imp       Bien imp     SIPA        Obra social  */
  {'A', 10277988.13,           4780.46,      4780.46,   15616.17,   21990.11},
  {'B', 15058447.71,           9082.88,      9082.88,   17177.79,   21990.11},
  {'C', 21113696.52,          15616.17,     14341.38,   18895.57,   21990.11},
  {'D', 26212853.42,          25495.79,     23742.95,   20785.12,   26133.18},
  {'E', 30833964.37,          47804.60,     37924.98,   22863.63,   31869.73},
  {'F', 38642048.36,          67245.13,     49398.08,   25149.99,   36650.19},
  {'G', 46211109.37,         122379.76,     61189.87,   35209.99,   39518.47},
  {'H', 70113407.33,         350567.04,    175283.51,   49293.99,   47485.89},
  {'I', 78479211.62,         697150.35,    278860.14,   69011.58,   58640.31},
  {'J', 89872640.30,         836580.42,    334632.18,   96616.21,   65810.99},
  {'K',108357084.05,        1171212.59,    390404.20,  135262.69,   75212.57}
};
/* ── Precio máximo unitario (venta bienes) ──────────────────────────────── */
#define PRECIO_MAX_UNIDAD  613492.54

/* ── Funciones auxiliares ─────────────────────────────────────────────────── */

void limpiar_pantalla(void) {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void imprimir_encabezado(void) {
    printf(COLOR_CYAN COLOR_BOLD);
    printf("╔═══════════════════════════════════════════════════════╗\n");
    printf("║       CALCULADORA DE MONOTRIBUTO - ARGENTINA          ║\n");
    printf("║           Valores vigentes desde febrero 2026         ║\n");
    printf("║           Autor: Velaz Pedro Ezequiel                 ║\n");
    printf("╚═══════════════════════════════════════════════════════╝\n");
    printf(COLOR_RESET "\n");
}

/* Formatea un double como moneda argentina: $ 1.234.567,89 */
void formato_pesos(double monto, char *buf, int len) {
    long long entera = (long long)monto;
    int cent = (int)round((monto - (double)entera) * 100);

    char tmp[32];
    snprintf(tmp, sizeof(tmp), "%lld", entera);
    int largo = strlen(tmp);

    int j = 0;
    char con_puntos[40];
    for (int i = 0; i < largo; i++) {
        con_puntos[j++] = tmp[i];
        int resto = largo - i - 1;
        if (resto > 0 && resto % 3 == 0) con_puntos[j++] = '.';
    }
    con_puntos[j] = '\0';

    snprintf(buf, len, "$ %s,%02d", con_puntos, cent);
}

/* Busca la categoría correspondiente al ingreso bruto anual */
int buscar_categoria(double ingreso_anual) {
    for (int i = 0; i < NUM_CATEGORIAS; i++) {
        if (ingreso_anual <= TABLA[i].tope_anual) return i;
    }
    return -1; /* supera cat K → debe inscribirse como RI */
}

/* ── Menú principal ──────────────────────────────────────────────────────── */

void mostrar_tabla_completa(void) {
    printf(COLOR_BOLD COLOR_AMARILLO
           "\n  TABLA COMPLETA DE CATEGORÍAS (feb 2026)\n" COLOR_RESET);
    printf("  %-4s %-22s %-18s %-15s %-14s %-14s\n",
           "Cat", "Tope anual", "Imp.integrado(srv)", "SIPA", "Obra social","Total(srv)");
    printf("  %s\n", "──────────────────────────────────────────────────────────────────────────────────────────────");

    char buf[32];
    for (int i = 0; i < NUM_CATEGORIAS; i++) {
        double total_srv = TABLA[i].imp_integrado_srv +
                           TABLA[i].aporte_sipa +
                           TABLA[i].obra_social;
        char tope[32], imp[32], sipa[32], os[32], tot[32];
        formato_pesos(TABLA[i].tope_anual,        tope, sizeof(tope));
        formato_pesos(TABLA[i].imp_integrado_srv, imp,  sizeof(imp));
        formato_pesos(TABLA[i].aporte_sipa,       sipa, sizeof(sipa));
        formato_pesos(TABLA[i].obra_social,       os,   sizeof(os));
        formato_pesos(total_srv,                  tot,  sizeof(tot));

        printf("    %c    %-22s %-18s %-15s %-14s %-14s\n",
               TABLA[i].letra, tope, imp, sipa, os, tot);
    }
    printf("\n  * Categorías H–I–J–K son exclusivas de servicios/locaciones.\n");
    printf("  * El precio máximo por unidad en venta de bienes es ");
    formato_pesos(PRECIO_MAX_UNIDAD, buf, sizeof(buf));
    printf("%s\n\n", buf);
}

void calcular_categoria(void) {
    double ingreso_mensual, ingreso_anual;
    int actividad;  /* 1=servicios, 2=venta de bienes */

    printf(COLOR_BOLD "\n  ─── CALCULÁ TU CATEGORÍA ───\n" COLOR_RESET);
    printf("  ¿Cuánto facturás por mes en promedio? $ ");
    if (scanf("%lf", &ingreso_mensual) != 1 || ingreso_mensual < 0) {
        printf(COLOR_ROJO "  Valor inválido.\n" COLOR_RESET);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    ingreso_anual = ingreso_mensual * 12.0;

    printf("  ¿Cuál es tu actividad principal?\n");
    printf("    1. Prestación de servicios / locaciones\n");
    printf("    2. Venta de bienes / cosas muebles\n");
    printf("  Elegí una opción: ");
    if (scanf("%d", &actividad) != 1 || (actividad != 1 && actividad != 2)) {
        printf(COLOR_ROJO "  Opción inválida.\n" COLOR_RESET);
        while (getchar() != '\n');
        return;
    }
    while (getchar() != '\n');

    int idx = buscar_categoria(ingreso_anual);

    char buf_anual[32], buf_mensual[32];
    formato_pesos(ingreso_anual,   buf_anual,   sizeof(buf_anual));
    formato_pesos(ingreso_mensual, buf_mensual, sizeof(buf_mensual));

    printf(COLOR_BOLD COLOR_CYAN "\n  ┌────────────────────────────────────────┐\n");

    if (idx == -1) {
        printf("  │  ⚠  Superás el tope máximo (Cat. K)     │\n");
        printf("  │  Deberías inscribirte como               │\n");
        printf("  │  RESPONSABLE INSCRIPTO (IVA + Ganancias) │\n");
        printf("  └────────────────────────────────────────┘\n" COLOR_RESET);
        printf("\n  Consultá con un contador para la transición.\n\n");
        return;
    }

    const Categoria *cat = &TABLA[idx];
    double imp   = (actividad == 1) ? cat->imp_integrado_srv : cat->imp_integrado_bien;
    double total = imp + cat->aporte_sipa + cat->obra_social;

    char buf_tope[32], buf_imp[32], buf_sipa[32], buf_os[32], buf_total[32];
    formato_pesos(cat->tope_anual, buf_tope,  sizeof(buf_tope));
    formato_pesos(imp,             buf_imp,   sizeof(buf_imp));
    formato_pesos(cat->aporte_sipa,buf_sipa,  sizeof(buf_sipa));
    formato_pesos(cat->obra_social,buf_os,    sizeof(buf_os));
    formato_pesos(total,           buf_total, sizeof(buf_total));

    printf("  │  Facturación mensual :  %-17s│\n", buf_mensual);
    printf("  │  Facturación anual   :  %-17s│\n", buf_anual);
    printf("  │                                        │\n");
    printf("  │  " COLOR_VERDE "➤  CATEGORÍA %c" COLOR_RESET COLOR_CYAN "                          │\n", cat->letra);
    printf("  │                                        │\n");
    printf("  │  Tope anual de la cat:  %-17s│\n", buf_tope);
    printf("  │  Impuesto integrado  :  %-17s│\n", buf_imp);
    printf("  │  Aporte SIPA (jubil) :  %-17s│\n", buf_sipa);
    printf("  │  Obra social         :  %-17s│\n", buf_os);
    printf("  │  ─────────────────────────────────   │\n");
    printf("  │  " COLOR_VERDE "CUOTA MENSUAL TOTAL  :  %-17s" COLOR_CYAN "│\n" COLOR_RESET, buf_total);
    printf(COLOR_CYAN "  └────────────────────────────────────────┘\n" COLOR_RESET);

    /* Aviso si está cerca de subir de categoría */
    double porcentaje_uso = (ingreso_anual / cat->tope_anual) * 100.0;
    if (porcentaje_uso >= 85.0 && idx < NUM_CATEGORIAS - 1) {
        const Categoria *next = &TABLA[idx + 1];
        double margen = next->tope_anual - ingreso_anual;
        char buf_margen[32];
        formato_pesos(margen, buf_margen, sizeof(buf_margen));
        printf(COLOR_AMARILLO
               "\n  ⚠  Estás usando el %.1f%% del tope de tu categoría.\n"
               "     Te separan %s anuales de subir a Cat. %c.\n"
               "     ¡Atención con la recategorización!\n"
               COLOR_RESET, porcentaje_uso, buf_margen, next->letra);
    }
    printf("\n");
}

void calcular_recategorizacion(void) {
    printf(COLOR_BOLD "\n  ─── SIMULADOR DE RECATEGORIZACIÓN ───\n" COLOR_RESET);
    printf("  Ingresá tu facturación de los últimos 12 meses:\n");

    double meses[12];
    double total_anual = 0.0;

    for (int i = 0; i < 12; i++) {
        printf("    Mes %2d: $ ", i + 1);
        if (scanf("%lf", &meses[i]) != 1 || meses[i] < 0) {
            printf(COLOR_ROJO "  Valor inválido. Volvé al menú.\n" COLOR_RESET);
            while (getchar() != '\n');
            return;
        }
        total_anual += meses[i];
    }
    while (getchar() != '\n');

    char letra_actual;
    printf("\n  ¿En qué categoría estás actualmente? (A-K): ");
    scanf(" %c", &letra_actual);
    while (getchar() != '\n');

    /* normalizar a mayúscula */
    if (letra_actual >= 'a' && letra_actual <= 'k')
        letra_actual -= 32;

    int idx_actual = -1;
    for (int i = 0; i < NUM_CATEGORIAS; i++) {
        if (TABLA[i].letra == letra_actual) { idx_actual = i; break; }
    }
    if (idx_actual == -1) {
        printf(COLOR_ROJO "  Categoría inválida.\n" COLOR_RESET);
        return;
    }

    int idx_nueva = buscar_categoria(total_anual);
    char buf_total[32];
    formato_pesos(total_anual, buf_total, sizeof(buf_total));

    printf(COLOR_BOLD COLOR_CYAN "\n  Facturación total (12 meses): %s\n" COLOR_RESET, buf_total);
    printf("  Categoría actual          : %c\n", letra_actual);

    if (idx_nueva == -1) {
        printf(COLOR_ROJO
               "  ⚠  Superás Cat. K → debés inscribirte como Responsable Inscripto.\n"
               COLOR_RESET);
        return;
    }

    printf(COLOR_VERDE "  Categoría que te corresponde: %c\n" COLOR_RESET, TABLA[idx_nueva].letra);

    if (idx_nueva == idx_actual) {
        printf(COLOR_VERDE "  ✔  Permanecés en la misma categoría. Sin cambios.\n" COLOR_RESET);
    } else if (idx_nueva > idx_actual) {
        printf(COLOR_AMARILLO
               "  ↑  Debés SUBIR a categoría %c (mayor cuota mensual).\n"
               COLOR_RESET, TABLA[idx_nueva].letra);
    } else {
        printf(COLOR_CYAN
               "  ↓  Podés BAJAR a categoría %c (menor cuota mensual).\n"
               COLOR_RESET, TABLA[idx_nueva].letra);
    }

    /* Fechas de recategorización */
    printf("\n  Próximas fechas de recategorización:\n");
    printf("    • Julio  2026  (vence el 5 de julio)\n");
    printf("    • Febrero 2027 (vence el 5 de febrero)\n\n");
}

void info_recategorizacion(void) {
    printf(COLOR_BOLD "\n  ─── ¿CUÁNDO Y CÓMO RECATEGORIZARME? ───\n\n" COLOR_RESET);
    printf("  • La recategorización es OBLIGATORIA dos veces al año:\n");
    printf("    → En FEBRERO (hasta el día 5)\n");
    printf("    → En JULIO   (hasta el día 5)\n\n");
    printf("  • Revisás tu facturación de los ÚLTIMOS 12 MESES.\n");
    printf("  • Si superaste el tope de tu categoría, debés subir.\n");
    printf("  • Si facturaste menos, podés bajar (y pagar menos).\n\n");
    printf("  ¿Cómo hacerlo?\n");
    printf("  1. Entrá a https://serviciosweb.afip.gob.ar/monotributo\n");
    printf("  2. Iniciá sesión con CUIL/CUIT + Clave Fiscal.\n");
    printf("  3. Seleccioná \"Recategorización\".\n");
    printf("  4. Informá el monto facturado en los últimos 12 meses.\n");
    printf("  5. El sistema calcula la categoría → Confirmás.\n\n");
    printf("  ⚠  Si no te recategorizás, ARCA puede hacerlo de oficio\n");
    printf("     y aplicar multas o excluirte del régimen.\n\n");
    printf("  Los valores de la tabla se actualizan en FEBRERO y AGOSTO\n");
    printf("  de cada año según la variación del IPC (INDEC).\n\n");
}

/* ── Bucle principal ─────────────────────────────────────────────────────── */

int main(void) {
    int opcion;
    SetConsoleOutputCP(65001);  // UTF-8
    SetConsoleCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode;
    GetConsoleMode(hOut, &mode);
    SetConsoleMode(hOut, mode | 0x0004);
    limpiar_pantalla();
    imprimir_encabezado();

    do {
        printf(COLOR_BOLD "  ¿Qué querés hacer?\n\n" COLOR_RESET);
        printf("    1.  Calcular mi categoría y cuota mensual\n");
        printf("    2.  Simular recategorización (últimos 12 meses)\n");
        printf("    3.  Ver tabla completa de categorías\n");
        printf("    4.  ¿Cuándo y cómo recategorizarme?\n");
        printf("    0.  Salir\n\n");
        printf("  Opción: ");

        if (scanf("%d", &opcion) != 1) {
            while (getchar() != '\n');
            opcion = -1;
        } else {
            while (getchar() != '\n');
        }

        printf("\n");

        switch (opcion) {
            case 1: calcular_categoria();       break;
            case 2: calcular_recategorizacion(); break;
            case 3: mostrar_tabla_completa();    break;
            case 4: info_recategorizacion();     break;
            case 0:
                printf(COLOR_VERDE "  ¡Hasta luego! Recordá pagar el 20 de cada mes.\n\n" COLOR_RESET);
                break;
            default:
                printf(COLOR_ROJO "  Opción no válida. Intentá de nuevo.\n\n" COLOR_RESET);
        }

    } while (opcion != 0);

    return 0;
}
