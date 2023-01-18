#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "node.h"
#include "utility.h"
#include "buckets.h"

Node *initBuckets() { // initialize buckets
    Node *buckets = malloc(HASH_SIZE * sizeof(Node));
    if(buckets == NULL) {
        printf("malloc failed");
        exit(1);
    }
    for(int i = 0; i < HASH_SIZE; i++) {
        buckets[i].next = NULL;
    }
    return buckets;
}

void loadBuckets(Node *buckets, char *fileContents) { // fill buckets with words from file
    int index = 0;
    while(fileContents[index] != '\0') {
        while(isSeparator(fileContents[index])){
            index++;
        }
        if(fileContents[index] == '\0')
            break;
        char *word = malloc( (MAX_WORD_LENGTH+1) * sizeof(char)); // allocate memory for word
        if(word == NULL) {
            printf("malloc failed");
            exit(1);
        }
        int wordIndex = 0;
        while( fileContents[index] != '\0' && !isSeparator(fileContents[index]) && wordIndex < MAX_WORD_LENGTH) {
            word[wordIndex++] = fileContents[index++];
        }
        word[wordIndex] = '\0';
        int hash = determineHashValue(word);
        Node *newNode = createNode(word);
        addNodeToBucket(newNode, &buckets[hash]);
    }
}

void freeBuckets(Node *buckets) { // free all memory allocated for buckets and words
    for(int i = 0; i < HASH_SIZE; i++) { 
        Node *current = buckets[i].next;
        while(current != NULL) {
            Node *temp = current;
            current = current->next;
            free(temp->word);
            free(temp);
        }
    }
    free(buckets);
}

int *selectBuckets(Node *buckets) { // select buckets to filter the file with
    int *selections = calloc(HASH_SIZE, sizeof(int)); // allocate memory for selection
    if(selections == NULL) {
        printf("calloc failed");
        exit(1);
    }
    printf("\nSelect Bucket indices to filter\n\n");
    while(1) { // loop until user enters -1
        printf("Selected buckets: "); // print selected buckets
        for(int i = 0; i < HASH_SIZE; i++) {
            if(selections[i]) {
                printf("%d, ", i);
            }
        }
        printf("\n");
        char *selectionStr = userInputString();
        if(strcmp(selectionStr, "-1") == 0) {
            free(selectionStr);
            return selections;
        }
        int selection = strtod(selectionStr, NULL);
        if(selection == 0 && strcmp(selectionStr, "0") != 0) {
            selection = -9;
        }
        free(selectionStr);
        if(selection >= 0 && selection < HASH_SIZE) { // if user enters invalid selection, print error message
            selections[selection] = !selections[selection];
            if(selections[selection]) {
                printf("Selected Bucket[%d]: ", selection);
                printBucket(&buckets[selection]);
            }
            printf("\n");
        }
        else { // if user enters valid selection, toggle selection
            printf("Invalid selection\n\n");
        }
    }
}

void printBucket(Node *bucket) {
    Node *current = bucket->next;
    int j = 0;
    while(current != NULL) { // print all words in bucket.
        if(j % WORDS_PER_LINE == 0 && j > 0) { // print # WORDS_PER_LINE. start new lines with tab for better readability
            printf("\n\t");
        }
        if(current->count > 1)  // if word appears more than once, print count next to word
            printf("%s(%d)", current->word, current->count);
        else  // if word appears only once, print word
            printf("%s", current->word);
        if(current->next != NULL) { // print comma after each word except last word
            printf(", ");
        }
        current = current->next; 
        j++;
    }
    printf("\n");
}

void printBuckets(Node *buckets) {
    for(int i = 0; i < HASH_SIZE; i++) { // print buckets 
        printf("Bucket[%d]: ", i); 
        printBucket(&buckets[i]); 
    }
}