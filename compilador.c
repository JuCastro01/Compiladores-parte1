
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

//tentativa de resolver os warnings
#define TAM_ID 10 

const char *reservadas[] = {
    "char", "else", "if", "int", "main", "readint", "void", "while", "writeint"
};

char token[50], lexema[50];
int linha = 1;
char *buffer;
char *p;


void proximo_token();
void erro(const char *msg);
int eh_reservada(const char *s);
void programa();
void match(const char *esperado);
void compound_stmt();
void stmt();
void expr();
void decl();
void expr_simp();
void termo();
void fator();

void imprimir_token(const char *classe, const char *lexema_extra) {
    printf("# %d:%s", linha, classe);
    if (lexema_extra != NULL) {
        printf(" | %s", lexema_extra);
    }
    printf("\n");
}

void ignorar_espacos() {
    while (*p == ' ' || *p == '\t' || *p == '\r') p++;
}

void ignorar_comentario() {
    while (1) {
        if (*p == '/' && *(p + 1) == '/') {
            p += 2;
            while (*p && *p != '\n') p++;
            if (*p == '\n') {
                linha++;
                p++;
            }
        } else if (*p == '/' && *(p + 1) == '*') {
            p += 2;
            while (*p && !(*p == '*' && *(p + 1) == '/')) {
                if (*p == '\n') linha++;
                p++;
            }
            if (*p) p += 2;
        } else {
            break;
        }
    }
}

int eh_reservada(const char *s) {
    for (int i = 0; i < sizeof(reservadas)/sizeof(reservadas[0]); i++) {
        if (strcmp(s, reservadas[i]) == 0) return 1;
    }
    return 0;
}
//le o proximo token da entrada
void proximo_token() {
    ignorar_espacos();
    ignorar_comentario();
    ignorar_espacos();

    while (*p == '\n') {
        linha++;
        p++;
        ignorar_espacos();
        ignorar_comentario();
    }

    if (*p == '\0') {
        strcpy(token, "EOF");
        return;
    }

    if (isalpha(*p) || *p == '_') {
        int i = 0;
        while (isalnum(*p) || *p == '_') {
            if (i < TAM_ID - 1) {
                lexema[i++] = *p;
            }
            p++;
        }
        lexema[i] = '\0';
        if (i >= TAM_ID) {
            printf("\nErro lexico na linha %d: identificador é muito longo\n", linha);
            exit(1);
        }
        if (eh_reservada(lexema)) {
            strcpy(token, lexema);
            imprimir_token(token, NULL);
        } else {
            strcpy(token, "id");
            imprimir_token("id", lexema);
        }
        return;
    }

    // decimal ou hexadecimal
    if (isdigit(*p)) {
        int valor = 0;
        if (*p == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X')) {
            // Hexadecimal
            p += 2;
            while (isxdigit(*p)) {
                valor = valor * 16 + (isdigit(*p) ? *p - '0' : toupper(*p) - 'A' + 10);
                p++;
            }
        } else {
            // Decimal
            while (isdigit(*p)) {
                valor = valor * 10 + (*p - '0');
                p++;
            }
        }
        sprintf(lexema, "%d", valor);
        strcpy(token, "intconst");
        imprimir_token("intconst", lexema);
        return;
    }


    if (*p == '\'') {
        p++;
        char c = *p++;
        if (*p != '\'') {
            printf("\nErro lexico na linha %d: \n", linha);
            exit(1);
        }
        p++;
        sprintf(lexema, "%c", c);
        strcpy(token, "charconst");
        imprimir_token("charconst", lexema);
        return;
    }
//tokens possiveis
    switch (*p) {
        case '+': strcpy(token, "+"); imprimir_token(token, NULL); p++; break;
        case '-': strcpy(token, "-"); imprimir_token(token, NULL); p++; break;
        case '*': strcpy(token, "*"); imprimir_token(token, NULL); p++; break;
        case '/': strcpy(token, "/"); imprimir_token(token, NULL); p++; break;
        case '(': strcpy(token, "abre_par"); imprimir_token(token, NULL); p++; break;
        case ')': strcpy(token, "fecha_par"); imprimir_token(token, NULL); p++; break;
        case '{': strcpy(token, "abre_chaves"); imprimir_token(token, NULL); p++; break;
        case '}': strcpy(token, "fecha_chaves"); imprimir_token(token, NULL); p++; break;
        case ';': strcpy(token, "ponto_virgula"); imprimir_token(token, NULL); p++; break;
        case ',': strcpy(token, "virgula"); imprimir_token(token, NULL); p++; break;
        case '=':
            if (*(p + 1) == '=') {
                strcpy(token, "igual"); imprimir_token(token, NULL); p += 2;
            } else {
                strcpy(token, "atribuicao"); imprimir_token(token, NULL); p++;
            }
            break;
        case '!':
            if (*(p + 1) == '=') {
                strcpy(token, "diferente"); imprimir_token(token, NULL); p += 2;
            } else {
                printf("\nErro lexico na linha %d: caractere inesperado [%c]\n", linha, *p);
                exit(1);
            }
            break;
        case '>':
            if (*(p + 1) == '=') {
                strcpy(token, "maior_igual"); imprimir_token(token, NULL); p += 2;
            } else {
                strcpy(token, "maior"); imprimir_token(token, NULL); p++;
            }
            break;
        case '<':
            if (*(p + 1) == '=') {
                strcpy(token, "menor_igual"); imprimir_token(token, NULL); p += 2;
            } else {
                strcpy(token, "menor"); imprimir_token(token, NULL); p++;
            }
            break;
        case '&':
            if (*(p + 1) == '&') {
                strcpy(token, "&&"); imprimir_token(token, NULL); p += 2;
            } else {
                printf("\nErro lexico na linha %d: caractere inesperado [%c]\n", linha, *p);
                exit(1);
            }
            break;
        case '|':
            if (*(p + 1) == '|') {
                strcpy(token, "||"); imprimir_token(token, NULL); p += 2;
            } else {
                printf("\nErro lexico na linha %d: caractere inesperado [%c]\n", linha, *p);
                exit(1);
            }
            break;
        default:
            printf("\nErro léxico na linha %d: caractere inesperado [%c]\n", linha, *p);
            exit(1);
    }
}
//erro do sintatico
void erro(const char *msg) {
    printf("\n# %d:erro sintatico, %s\n", linha, msg);
    exit(1);
}
// verificar se o token atual combina com o esperado e ve o proximo
void match(const char *esperado) {
    if (strcmp(token, esperado) == 0) {
        proximo_token();
    } else {
        char msg[100];
        sprintf(msg, "esperado [%s] encontrado [%s]", esperado, token);
        erro(msg);
    }
}
//inicio da analise do programa principal (void main())
void programa() {
    match("void");
    match("main");
    match("abre_par");
    match("void");
    match("fecha_par");
    compound_stmt();
}

//comandos

void decl() {
    if (strcmp(token, "int") == 0 || strcmp(token, "char") == 0) {
        proximo_token();
        if (strcmp(token, "id") != 0) erro("esperado [id]");
        proximo_token();
        while (strcmp(token, "virgula") == 0) {
            proximo_token();
            if (strcmp(token, "id") != 0) erro("esperado [id]");
            proximo_token();
        }
        match("ponto_virgula");
    }
}



void compound_stmt() {
    match("abre_chaves");
    while (strcmp(token, "int") == 0 || strcmp(token, "char") == 0) decl();
    while (strcmp(token, "fecha_chaves") != 0 && strcmp(token, "EOF") != 0) stmt();
    match("fecha_chaves");
}

void stmt() {
    if (strcmp(token, "writeint") == 0) {
        match("writeint"); match("abre_par"); expr(); match("fecha_par"); match("ponto_virgula");
    } else if (strcmp(token, "readint") == 0) {
        match("readint"); match("abre_par"); match("id"); match("fecha_par"); match("ponto_virgula");
    } else if (strcmp(token, "id") == 0) {
        match("id"); match("atribuicao"); expr(); match("ponto_virgula");
    } else if (strcmp(token, "if") == 0) {
        match("if"); match("abre_par"); expr(); match("fecha_par"); stmt();
        if (strcmp(token, "else") == 0) { match("else"); stmt(); }
    } else if (strcmp(token, "while") == 0) {
        match("while"); match("abre_par"); expr(); match("fecha_par"); stmt();
    } else if (strcmp(token, "abre_chaves") == 0) {
        compound_stmt();
    } else {
        erro("stmt invalido");
    }
}
// para = , > e <
void expr() {
    expr_simp();
    if (strcmp(token, "igual") == 0 || strcmp(token, "diferente") == 0 || strcmp(token, "menor") == 0 ||
        strcmp(token, "menor_igual") == 0 || strcmp(token, "maior") == 0 || strcmp(token, "maior_igual") == 0) {
        proximo_token();
        expr_simp();
    }
}
// para + e -
void expr_simp() {
    termo();
    while (strcmp(token, "+") == 0 || strcmp(token, "-") == 0 || strcmp(token, "||") == 0) {
        proximo_token();
        termo();
    }
}
//para operacoes de * e /
void termo() {
    fator();
    while (strcmp(token, "*") == 0 || strcmp(token, "/") == 0 || strcmp(token, "&&") == 0) {
        proximo_token();
        fator();
    }
}
// para constantes, ids e entre parenteses
void fator() {
    if (strcmp(token, "id") == 0 || strcmp(token, "intconst") == 0 || strcmp(token, "charconst") == 0) {
        proximo_token();
    } else if (strcmp(token, "abre_par") == 0) {
        match("abre_par");
        expr();
        match("fecha_par");
    } else {
        erro("esperado fator");
    }
}

int main() {
//para ler o arquivo
    FILE *f = fopen("entrada.txt", "r");
    if (!f) {
        perror("Erro ao abrir o arquivo");
        return 1;
    }

    fseek(f, 0, SEEK_END);
    long tamanho = ftell(f);
    rewind(f);

    buffer = (char *)malloc(tamanho + 1);
    if (!buffer) {
        perror("Erro de alocação de memória");
        fclose(f);
        return 1;
    }

    fread(buffer, 1, tamanho, f);
    buffer[tamanho] = '\0';
    fclose(f);

    p = buffer;
    proximo_token();
    programa();
    printf("%d linhas analisadas. Programa sintáticamente correto.\n", linha);
    return 0;
}
