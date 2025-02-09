#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAX_TOKEN_LENGTH 100

//Defining all the token types
typedef enum {
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,
    TOKEN_IF, TOKEN_EQUAL, TOKEN_LPARENT, TOKEN_RPARENT,
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMICLN, TOKEN_UNKNOWN, TOKEN_EOF
} TokenType;

//Token structure 
typedef struct {
    TokenType type;
    char text[MAX_TOKEN_LENGTH];
} Token;

void getNxtToken(FILE *file, Token *token) {
    int c;
    while((c = fgetc(file)) != EOF) {
        if(isspace(c)) {
            continue;
        }

        if (isalpha(c)) {
            int length = 0;
            token->text[length++] = c;
            while (isalnum(c = fgetc(file))) {
                if (length < MAX_TOKEN_LENGTH - 1) token->text[length++] = c;
            }
            ungetc(c, file);
            token->text[length] = '\0';

            if(strcmp(token->text, "int") == 0){
                token->type = TOKEN_INT;
            }
            else if (strcmp(token->text, "if") == 0){
                token->type =TOKEN_IF;
            }
            else {
                token->type = TOKEN_IDENTIFIER;
            }
            return;
        }

        //Dealing with numbers
        if (isdigit(c)) {
            int length = 0;
            token->text[length++] = c;
            while (isdigit(c = fgetc(file))) {
                if(length<MAX_TOKEN_LENGTH-1){
                    token->text[length++] = c;
                }
            }
            ungetc(c, file);
            token->text[length] = '\0';
            token->type =TOKEN_NUMBER;
            return;
        }

        //Dealing with symbols/operators
        switch(c) {
            case '=':
            if ((c = fgetc(file)) == '='){
                token->type = TOKEN_EQUAL;
                strcpy(token->text, "==");
            }
            else{
                ungetc(c, file);
                token->type = TOKEN_ASSIGN;
                token->text[0] = '=';
                token->text[1] = '\0';
            }
            return;

            //addition operator
            case '+':
            token->type = TOKEN_PLUS;
            token->text[0] = '+';
            token->text[1] = '\0';
            return;

            //subtraction operator
            case '-':
            token->type = TOKEN_MINUS;
            token->text[0] = '-';
            token->text[1] = '\0';
            return;

            //multiplication operator
            case '*':
            token->type = TOKEN_MULTIPLY;
            token->text[0] = '*';
            token->text[1] = '\0';
            return;

            //divison operator
            case '/':
            token->type = TOKEN_DIVIDE;
            token->text[0] = '/';
            token->text[1] = '\0';
            return;

            //left-parentheis
            case '(':
            token->type = TOKEN_LPARENT;
            token->text[0] = '(';
            token->text[1] = '\0';
            return;

            //right-parenthesis
            case ')':
            token->type = TOKEN_RPARENT;
            token->text[0] = ')';
            token->text[1] = '\0';
            return;
            
            //left-braces
            case '{':
            token->type = TOKEN_LBRACE;
            token->text[0] = '{';
            token->text[1] = '\0';
            return;

            //right-braces
            case '}':
            token->type = TOKEN_LBRACE;
            token->text[0] = '}';
            token->text[1] = '\0';
            return;

            //Semicolon
            case ';':
            token->type = TOKEN_SEMICLN;
            token->text[0] = ';';
            token->text[1] = '\0';
            return;

            //default switch
            default: token->type = TOKEN_UNKNOWN;
            token->text[0] = c;
            token->text[1] = '\0';
            return;
        }
    }
    token->type = TOKEN_EOF;
    token->text[0]='\0';
}

int main() {
    FILE *file = fopen("input.txt", "r");
    if (!file) {
        perror("Unable to open the file!");
        return 1;
    }

    Token token;
    do {
        getNxtToken(file, &token);
        printf("Token: %d, Text: %s\n", token.type, token.text);
    } while (token.type != TOKEN_EOF);

    fclose(file);
    return 0;
}
