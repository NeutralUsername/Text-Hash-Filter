#include "node.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Node *createNode(char *word) {
    Node *node = malloc(sizeof(Node));
    if(node == NULL) {
        printf("malloc failed");
        exit(1);
    }
    node->next = NULL;
    node->count = 1;
    node->word = word;
    return node;
}

void addNodeToBucket(Node *newNode, Node *bucket) { // insert Node into bucket
    Node *prev = bucket;
    Node *current = bucket->next;
    while(current != NULL) { 
        if(strcmp(current->word, newNode->word) == 0) { // if word is already in bucket increment count
            current->count++;
            free(newNode->word);
            free(newNode);
            return;
        }
        else if(strcasecmp(current->word, newNode->word) > 0) { // if word is less than current word(alphabetically), insert word before current word
            prev->next = newNode;
            newNode->next = current;
            return;
        }
        else { // if word is greater than current word, move to next word in bucket
            prev = current;
            current = current->next;
        }
    }
    prev->next = newNode; // if word is greater than all words in bucket (or bucket is empty), insert word at end of bucket
}