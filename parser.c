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

//AST nodes list
typedef enum{
    NODE_PROGRAM,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_BINARY_OP,
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_CONDITION,
    NODE_IF
} NodeType;

//Building the base AST strutcure
typedef struct ASTNode{
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

//to make New node, remember to add debug statements - LEAFA
ASTNode* makeNode(NodeType type, const char* text){
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->left = NULL;
    node->right = NULL;
    if(text){
        strcpy(node->text, text);
    }
    /*debug
    printf("\nNode is made.\n"); 
    */
    return node;
}

Token current_token;
Token* tokens;
int token_Index =  0;

//DO NOT CHANGE THESE - LEAFANINE
void nextToken() {
    if (tokens[token_Index].type != TOKEN_EOF){
        current_token = tokens[token_Index++];
    } else {
        current_token.type = TOKEN_EOF;
    }
    printf("[DEBUG] Moved to next token: %s (Type: %d)\n", current_token.text, current_token.type);
}

//parse declarations - to parse decalrations for example "int a;"
ASTNode* parse_Declaration() {
    printf("[DEBUG] Entering parse_Declaration with token: %s\n", current_token.text);
    nextToken();
    printf("[DEBUG] After consuming 'int', current token: %s\n", current_token.text);

    ASTNode* node = makeNode(NODE_DECLARATION, current_token.text);
    nextToken();
    printf("[DEBUG] After consuming identifier, current token: %s\n", current_token.text);

    if (current_token.type != TOKEN_SEMICLN) {
        fprintf(stderr, "[ERROR] Expected ';' but found: %s\n", current_token.text);
        exit(1);
    }
    nextToken();
    printf("[DEBUG] After consuming ';', next token: %s\n", current_token.text);

    return node;
}


//Parse assignments - To parse syntax like "c = 6;"
ASTNode* parse_Assignment() {
    ASTNode* node = makeNode(NODE_ASSIGNMENT, current_token.text);
    nextToken();
    nextToken();
    node->left = parse_Expression();
    nextToken();
    return node;
}

//Parse expression - To parse expression like a + 6
ASTNode* parse_Expression() {
    ASTNode* node = NULL;

    if (current_token.type ==  TOKEN_NUMBER){
        node = makeNode(NODE_NUMBER, current_token.text);
        nextToken();
    } else if (current_token.type == TOKEN_IDENTIFIER){
        node = makeNode(NODE_VARIABLE, current_token.text);
        nextToken();
    } else if (current_token.type == TOKEN_LPARENT){
        nextToken();
        node = parse_Expression();
        nextToken();
    }

    // Handle binary operators (e.g., +, -)
    while (current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS || current_token.type == TOKEN_EQUAL) {
        ASTNode* opNode = makeNode(NODE_BINARY_OP, current_token.text);
        nextToken();
        opNode->left = node;
        opNode->right = parse_Expression();
        node = opNode;
    }

    return node;
}

//PARSING if and other causes! - leafanine
ASTNode* parse_IF(){
    nextToken();
    if (current_token.type != TOKEN_LPARENT) {
        fprintf(stderr, "Syntax Error: Expected '(', found '%s'\n", current_token.text);
        exit(1);
    }
    nextToken();

    ASTNode* conditionNode = parse_Expression();
    if (current_token.type != TOKEN_RPARENT) {
        fprintf(stderr, "Syntax Error: Expected ')', found '%s'\n", current_token.text);
        exit(1);
    }
    nextToken();
    if (current_token.type != TOKEN_LBRACE) {
        fprintf(stderr, "Syntax Error: Expected '{', found '%s'\n", current_token.text);
        exit(1);
    }
    nextToken();

    ASTNode* body_Node = parseStatement();
    if (current_token.type != TOKEN_RBRACE) {
        fprintf(stderr, "Syntax Error: Expected '}', found '%s'\n", current_token.text);
        exit(1);
    }
    nextToken();
    
    ASTNode* ifNode = makeNode(NODE_IF, NULL);
    ifNode->left = conditionNode;
    ifNode->right = body_Node;
    return ifNode;
}

//Parsing statements - if statement. I am sleepoy. -3AM
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
        exit(1);
    }

    nextToken();  // **Ensure the statement parsing consumes tokens**
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
            exit(1);
        }

        if (lastNode == programNode) {
            programNode->left = statementNode;
        } else {
            lastNode->right = statementNode;
        }
        lastNode = statementNode;

        nextToken();  // **Move to the next token after parsing a statement**
        printf("\n[DEBUG] Next token after parseStatement: %s", current_token.text);
    }

    printf("\n[DEBUG] Exiting parseProgram()\n");
    return programNode;
}



void printAST(ASTNode* node, int depth) {
    if (!node) return;

    // Print indentation
    for (int i = 0; i < depth; i++) printf("  ");

    // Print node type and text
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

    // Recursively print children
    printAST(node->left, depth + 1);
    printAST(node->right, depth + 1);
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

        // Prevent buffer overflow
        if (i >= MAX_TOKENS) {
            fprintf(stderr, "Error: Too many tokens in file.\n");
            break;
        }
    }

    // Mark end of tokens
    tokenss[i].type = TOKEN_EOF;
    fclose(file);
}


// Main function
int main() {
    printf("debug 1");
    readTokens("tokens.txt");

    printf("debug 2");
   
    ASTNode* program = parseProgram((Token*)tokenss, MAX_TOKENS);

    printf("debug 3");
  
    printAST(program, 0);

    printf("debug 4");
    return 0;
}
