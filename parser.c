#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum {
    TOKEN_INT, TOKEN_IDENTIFIER, TOKEN_NUMBER, TOKEN_ASSIGN,
    TOKEN_PLUS, TOKEN_MINUS, TOKEN_MULTIPLY, TOKEN_DIVIDE,
    TOKEN_IF, TOKEN_EQUAL, TOKEN_LPARENT, TOKEN_RPARENT,
    TOKEN_LBRACE, TOKEN_RBRACE, TOKEN_SEMICLN, TOKEN_UNKNOWN, TOKEN_EOF
} TokenType;

typedef struct {
    TokenType type;
    char text[100];
} Token;

typedef enum {
    NODE_PROGRAM,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_BINARY_OP,
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_CONDITION,
    NODE_IF
} NodeType;

typedef struct ASTNode {
    NodeType type;
    struct ASTNode* left;
    struct ASTNode* right;
    char text[100];
} ASTNode;

ASTNode* makeNode(NodeType type, const char* text);
void nextToken();
ASTNode* parse_Declaration();
ASTNode* parse_Assignment();
ASTNode* parse_Expression();
ASTNode* parse_IF();
ASTNode* parseStatement();
ASTNode* parseProgram();
void printAST(ASTNode* node, int depth);
void freeAST(ASTNode* node);
void readTokens(const char* filename);
void validateTokens();

ASTNode* makeNode(NodeType type, const char* text) {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    if (text) {
        strcpy(node->text, text);
    }
    return node;
}

Token current_token;
Token* tokens;
int token_Index = 0;

void nextToken() {
    if (tokens[token_Index].type != TOKEN_EOF) {
        current_token = tokens[token_Index++];
    } else {
        current_token.type = TOKEN_EOF;
    }
    printf("[DEBUG] Moved to next token: %s (Type: %d)\n", current_token.text, current_token.type);
}

ASTNode* parse_Declaration() {
    printf("[DEBUG] Entering parse_Declaration with token: %s\n", current_token.text);
    nextToken();
    printf("[DEBUG] After consuming 'int', current token: %s\n", current_token.text);
    ASTNode* node = makeNode(NODE_DECLARATION, current_token.text);
    nextToken();
    printf("[DEBUG] After consuming identifier, current token: %s\n", current_token.text);
    if (current_token.type != TOKEN_SEMICLN) {
        fprintf(stderr, "[ERROR] Expected ';' but found: %s\n", current_token.text);
        return NULL;
    }
    nextToken();
    printf("[DEBUG] After consuming ';', next token: %s\n", current_token.text);
    return node;
}

ASTNode* parse_Assignment() {
    ASTNode* node = makeNode(NODE_ASSIGNMENT, current_token.text);
    nextToken();
    nextToken();
    node->left = parse_Expression();
    nextToken();
    return node;
}

ASTNode* parse_Expression() {
    ASTNode* node = NULL;
    if (current_token.type == TOKEN_NUMBER) {
        node = makeNode(NODE_NUMBER, current_token.text);
        nextToken();
    } else if (current_token.type == TOKEN_IDENTIFIER) {
        node = makeNode(NODE_VARIABLE, current_token.text);
        nextToken();
    } else if (current_token.type == TOKEN_LPARENT) {
        nextToken();
        node = parse_Expression();
        nextToken();
    }
    
    while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS || current_token.type == TOKEN_EQUAL) {
        ASTNode* opNode = makeNode(NODE_BINARY_OP, current_token.text);
        nextToken();
        opNode->left = node;
        opNode->right = parse_Expression();
        node = opNode;
    }
    return node;
}

ASTNode* parse_IF() {
    nextToken();
    if (current_token.type != TOKEN_LPARENT) {
        fprintf(stderr, "Syntax Error: Expected '(', found '%s'\n", current_token.text);
        return NULL;
    }
    nextToken();
    ASTNode* conditionNode = parse_Expression();
    if (current_token.type != TOKEN_RPARENT) {
        fprintf(stderr, "Syntax Error: Expected ')', found '%s'\n", current_token.text);
        return NULL;
    }
    nextToken();
    if (current_token.type != TOKEN_LBRACE) {
        fprintf(stderr, "Syntax Error: Expected '{', found '%s'\n", current_token.text);
        return NULL;
    }
    nextToken();
    ASTNode* body_Node = parseStatement();
    if (current_token.type != TOKEN_RBRACE) {
        fprintf(stderr, "Syntax Error: Expected '}', found '%s'\n", current_token.text);
        return NULL;
    }
    nextToken();
    ASTNode* ifNode = makeNode(NODE_IF, NULL);
    ifNode->left = conditionNode;
    ifNode->right = body_Node;
    return ifNode;
}

ASTNode* parseStatement() {
    printf("\n[DEBUG] Parsing statement for token: %s", current_token.text);
    ASTNode* tempNode = NULL;
    if (current_token.type == TOKEN_INT) {
        tempNode = parse_Declaration();
    } else if (current_token.type == TOKEN_IDENTIFIER) {
        tempNode = parse_Assignment();
    } else if (current_token.type == TOKEN_IF) {
        tempNode = parse_IF();
    } else {
        fprintf(stderr, "Unexpected token: %s\n", current_token.text);
        return NULL;
    }
    return tempNode;
}

ASTNode* parseProgram() {
    ASTNode* programNode = makeNode(NODE_PROGRAM, "Program");
    ASTNode* lastNode = programNode;
    printf("\n[DEBUG] Entering parseProgram()\n");
    while (current_token.type != TOKEN_EOF) {
        printf("\n[DEBUG] Current token before parseStatement: %s", current_token.text);
        ASTNode* statementNode = parseStatement();
        if (!statementNode) {
            fprintf(stderr, "[ERROR] parseStatement() returned NULL\n");
            break;
        }
        if (lastNode == programNode) {
            programNode->left = statementNode;
        } else {
            lastNode->right = statementNode;
        }
        lastNode = statementNode;
        nextToken();
        printf("\n[DEBUG] Next token after parseStatement: %s", current_token.text);
        if (current_token.type == TOKEN_EOF) break;
    }
    printf("\n[DEBUG] Exiting parseProgram()\n");
    return programNode;
}

void printAST(ASTNode* node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf(" ");
    switch (node->type) {
        case NODE_PROGRAM: printf("Program\n"); break;
        case NODE_DECLARATION: printf("Declaration: %s\n", node->text); break;
        case NODE_ASSIGNMENT: printf("Assignment: %s\n", node->text); break;
        case NODE_BINARY_OP: printf("BinaryOp: %s\n", node->text); break;
        case NODE_NUMBER: printf("Number: %s\n", node->text); break;
        case NODE_VARIABLE: printf("Variable: %s\n", node->text); break;
        case NODE_CONDITION: printf("Condition: %s\n", node->text); break;
        case NODE_IF: printf("If\n"); break;
        default: printf("Unknown Node\n"); break;
    }
    printAST(node->left, depth + 1);
    printAST(node->right, depth + 1);
}

void freeAST(ASTNode* node) {
    if (node == NULL) return;
    freeAST(node->left);
    freeAST(node->right);
    free(node);
}

#define MAX_TOKENS 1000
#define TOKEN_EOF -1

typedef struct {
    int type;
    char text[100];
} TokenNew;

TokenNew tokenss[MAX_TOKENS];

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
    while (fscanf(file, "%d %s", (int*)&tokenss[i].type, tokenss[i].text) != EOF) {
        printf("Read token %d: type=%d, text=%s\n", i, tokenss[i].type, tokenss[i].text);
        i++;
        if (i >= MAX_TOKENS) {
            fprintf(stderr, "Error: Too many tokens in file.\n");
            break;
        }
    }
    tokenss[i].type = TOKEN_EOF;
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
