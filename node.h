#ifndef NODE_H
#define NODE_H
struct Node {
    char *word;
    int count;
    struct Node *next;
};
typedef struct Node Node;
Node *createNode(char *word);
void addNodeToBucket(Node *newNode, Node *bucket);
#endif