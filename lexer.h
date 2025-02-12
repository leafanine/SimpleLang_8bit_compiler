#ifndef LEXER_H
#define LEXER_H

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

// Token structure - this defines how a token is like.
typedef struct {
    char text[MAX_TOKEN_LENGTH]; //stores the token text like for int its "int" for if its "if"
    TokenType type; //this will contain the actual TokenType that we defined earlier using enumerate
} Token;

void getNextToken(FILE *file, Token *token); //this is just here to be included, it will be described later in lexer.c

#endif