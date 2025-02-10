#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_TOKENS 1000
#define TOKEN_EOF -1

// Token Types
typedef enum {
    TOKEN_INT = 0,
    TOKEN_IDENTIFIER = 1,
    TOKEN_NUMBER = 2,
    TOKEN_ASSIGN = 3,
    TOKEN_PLUS = 4,
    TOKEN_MINUS = 5,
    TOKEN_MULTIPLY = 6,
    TOKEN_DIVIDE = 7,
    TOKEN_IF = 8,
    TOKEN_EQUAL = 9,
    TOKEN_LPARENT = 10,
    TOKEN_RPARENT = 11,
    TOKEN_LBRACE = 12,
    TOKEN_RBRACE = 13,
    TOKEN_SEMICLN = 14,
    TOKEN_UNKNOWN = 15
} TokenType;

// Token Structure
typedef struct {
    int type;
    char text[100];
} TokenNew;

TokenNew tokenss[MAX_TOKENS];

// Function Prototypes
void readTokens(const char* filename);
void validateTokens();

void readTokens(const char* filename) {
    if (access(filename, F_OK) != 0) {
        perror("File does not exist");
        exit(1);
    }

    FILE* file = fopen(filename, "r");
    if (!file) {
        perror("Failed to open file");
        fprintf(stderr, "Filename: %s\n", filename);
        exit(1);
    }

    int i = 0;
    while (fscanf(file, "%d %s", &tokenss[i].type, tokenss[i].text) != EOF) {
        printf("Read token %d: type=%d, text=%s\n", i, tokenss[i].type, tokenss[i].text);
        i++;
        if (i >= MAX_TOKENS) {
            fprintf(stderr, "Error: Too many tokens in file.\n");
            break;
        }
    }

    tokenss[i].type = TOKEN_EOF; // Add EOF token at the end
    fclose(file);
}

void validateTokens() {
    for (int i = 0; tokenss[i].type != TOKEN_EOF; i++) {
        // Ensure token type is within the valid range
        if (tokenss[i].type < TOKEN_INT || tokenss[i].type > TOKEN_UNKNOWN) {
            fprintf(stderr, "Invalid token type at index %d: %d\n", i, tokenss[i].type);
            exit(1);
        }
        printf("[DEBUG] Validated token %d: type=%d, text=%s\n", i, tokenss[i].type, tokenss[i].text);
    }
}

int main() {
    printf("debug 1");
    readTokens("tokens.txt");
    validateTokens();
    printf("debug 2");
    ASTNode* program = parseProgram((Token*)tokenss, MAX_TOKENS);
    if (program) {
        printf("debug 3");
        printAST(program, 0);
        printf("debug 4");
        freeAST(program);
    } else {
        fprintf(stderr, "Parsing failed\n");
    }
    return 0;
}
