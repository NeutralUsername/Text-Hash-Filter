#include "main.h"
#include "node.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int isSeparator(char c) {
    if(c == ' ' || c == '.' || c == '\n' || c ==';' || c==':' || c == ',' || c == '?' || c == '\t' || c == '\r') {
        return 1;
    }
    return 0;
}

int determineHashValue(char *word) {
    int hash = 0;
    int i = 0;
    while(word[i] != '\0') {
        hash += word[i];
        i++;
    }
    return hash % HASH_SIZE;
}

char *userInputString() { // get word from user
    char *word = malloc(MAX_WORD_LENGTH * sizeof(char));
    if(word == NULL) {
        printf("malloc failed");
        exit(1);
    }
    printf("user input (-1 to stop): ");
    scanf("%s", word);
    while(getchar() != '\n'); // clear input buffer (to prevent infinite loop)
    return word;
}