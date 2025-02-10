#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_TOKEN_LENGTH 100

// Defining all the token types
typedef enum {
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,
    TOKEN_IF, TOKEN_EQUAL, TOKEN_LPARENT, TOKEN_RPARENT,
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMICLN,
    TOKEN_UNKNOWN, // For unrecognized characters
    TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LENGTH];
} Token;

// Function to get the next token from the input file
void getNextToken(FILE *file, Token *token) {
    memset(token, 0, sizeof(Token)); // Clear token memory to avoid garbage values
    int c;

    // Skip whitespace
    while ((c = fgetc(file)) != EOF) {
        if (isspace(c)) {
            continue;
        }

        // Identifiers & Keywords
        if (isalpha(c)) {
            int length = 0;
            token->text[length++] = c;
            while (isalnum(c = fgetc(file))) {
                if (length < MAX_TOKEN_LENGTH - 1) {
                    token->text[length++] = c;
                }
            }
            ungetc(c, file); // Push back the last character
            token->text[length] = '\0';
            if (strcmp(token->text, "int") == 0) {
                token->type = TOKEN_INT;
            } else if (strcmp(token->text, "if") == 0) {
                token->type = TOKEN_IF;
            } else {
                token->type = TOKEN_IDENTIFIER;
            }
            return;
        }

        // Numbers
        if (isdigit(c)) {
            int length = 0;
            token->text[length++] = c;
            while (isdigit(c = fgetc(file))) {
                if (length < MAX_TOKEN_LENGTH - 1) {
                    token->text[length++] = c;
                }
            }
            ungetc(c, file); // Push back the last character
            token->text[length] = '\0';
            token->type = TOKEN_NUMBER;
            return;
        }

        // Symbols/Operators
        switch (c) {
            case '=':
                if ((c = fgetc(file)) == '=') {
                    token->type = TOKEN_EQUAL;
                    strcpy(token->text, "==");
                } else {
                    ungetc(c, file);
                    token->type = TOKEN_ASSIGN;
                    strcpy(token->text, "=");
                }
                return;
            case '+': token->type = TOKEN_PLUS; strcpy(token->text, "+"); return;
            case '-': token->type = TOKEN_MINUS; strcpy(token->text, "-"); return;
            case '*': token->type = TOKEN_MULTIPLY; strcpy(token->text, "*"); return;
            case '/': token->type = TOKEN_DIVIDE; strcpy(token->text, "/"); return;
            case '(': token->type = TOKEN_LPARENT; strcpy(token->text, "("); return;
            case ')': token->type = TOKEN_RPARENT; strcpy(token->text, ")"); return;
            case '{': token->type = TOKEN_LBRACE; strcpy(token->text, "{"); return;
            case '}': token->type = TOKEN_RBRACE; strcpy(token->text, "}"); return;
            case ';': token->type = TOKEN_SEMICLN; strcpy(token->text, ";"); return;
            default:
                // Handle unknown characters
                fprintf(stderr, "[ERROR] Unknown character: '%c'\n", c);
                token->type = TOKEN_UNKNOWN;
                snprintf(token->text, MAX_TOKEN_LENGTH, "%c", c);
                return;
        }
    }

    // Properly mark EOF
    token->type = TOKEN_EOF;
    strcpy(token->text, "");
}

int main() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Unable to open the input file!");
        return 1;
    }

    FILE *outputFile = fopen("tokens.txt", "w");
    if (!outputFile) {
        perror("Failed to open output file");
        fclose(file);
        return 1;
    }

    Token token;

    // Read and write tokens until EOF
    getNextToken(file, &token);
    while (token.type != TOKEN_EOF) {
        printf("Token: %d, Text: %s\n", token.type, token.text);
        fprintf(outputFile, "%d %s\n", token.type, token.text);
        getNextToken(file, &token);
    }

    fclose(file);
    fclose(outputFile);

    printf("Tokens written to tokens.txt\n");
    return 0;
}
