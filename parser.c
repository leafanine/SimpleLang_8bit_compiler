#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Building the base AST strutcure
typedef struct ASTNode{
    enum{
        NODE_PROGRAM,
        NODE_DECLARATION,
        NODE_ASSIGNMENT,
        NODE_BINARY_OP,
        NODE_NUMBER,
        NODE_VARIABLE,
        NODE_CONDITION,
        NODE_IF
    } type;
    struct ASTNode* left;
    struct ASTNode* right;
    char text[100];
} ASTNode;

ASTNode* makeNode(int type, const char* text){
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = type;
    node->left = left;
    node->right = right;
    if(text){
        strcpy(node->text, text);
    }
}