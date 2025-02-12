#ifndef PARSER_H
#define PARSER_H

#include"lexer.h"

typedef enum {
    NODE_PROGRAM,
    NODE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_BINARY_OP,
    NODE_NUMBER,
    NODE_VARIABLE,
    NODE_CONDITION,
    NODE_IF,
    NODE_JUMP,
    NODE_LABEL
} NodeType;

//The struct for the nodes of Abstract Syntax Tree
typedef struct ASTNode {
    int visit;
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