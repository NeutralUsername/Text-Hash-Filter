#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if __has_include("strings.h")
#include "strings.h"
#endif

#define MAX_WORD_LENGTH 50
#define HASH_SIZE 37
#define WORDS_PER_LINE 10
#define MAX_FILE_LENGTH 100000

typedef struct Node {
    char *word;
    int count;
    struct Node *next;
} Node;

void printBucket(Node *bucket);
void printBuckets(Node *buckets);
char *userInputString();
Node *createNode(char *word);
Node *initBuckets();
void loadBuckets(Node *buckets, char *fileContents);
int *selectBuckets(Node *buckets);
void appendHashesToBinary();
void addNodeToBucket(Node *newNode, Node *bucket);
void freeBuckets(Node *buckets);
void writeSelectionToTextFile(int *selection, char *fileContents);
void writeHashToBinaryFile(char *fileContent);
int determineHashValue(char *word);
int isSeparator(char c);

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("no filename provided");
        exit(1);
    }
    FILE *fp = fopen(argv[1], "r"); // open file to read from
    if(fp == NULL) {
        printf("invalid filename");
        exit(1);
    }
    char *tempContents = malloc((MAX_FILE_LENGTH)*sizeof(char));
    size_t tempContentsSize = fread(tempContents, sizeof(char), MAX_FILE_LENGTH, fp); // read file into tempContents
    char *fileContents = malloc((tempContentsSize+1)*sizeof(char));
    int fileContentsIndex = 0;
    for(size_t i = 0; i < tempContentsSize; i++) { //filter out unwanted characters
        if(tempContents[i] != '\0' && tempContents[i] >= 0)
            fileContents[fileContentsIndex++] = tempContents[i];
    }
    fileContents[fileContentsIndex] = '\0';
    fclose(fp);
    free(tempContents);

    Node *buckets = initBuckets(); // initialize buckets
    loadBuckets(buckets, fileContents); // fill buckets with words from file
    printBuckets(buckets); // print individual buckets
    int *selection = selectBuckets(buckets); // select buckets to filter the file with and print individual buckets
    writeSelectionToTextFile(selection, fileContents); // filter the file with the selected buckets
    writeHashToBinaryFile(fileContents);
    appendHashesToBinary(); // add word to hash table

    freeBuckets(buckets); // free buckets
    free(selection); // free selection
    free(fileContents); // free fileContents
    exit(0);
}

void appendHashesToBinary() { // add word to hash table
    FILE *fp = fopen("hash.bin", "ab");
    if(fp == NULL) {
        printf("could't open hash.bin");
        exit(1);
    }
    printf("\nEnter words to append their hash to hash.bin\n");
    char *word = userInputString();
    while(strcmp(word, "-1") != 0) {
        int hash = determineHashValue(word);
        fprintf(fp, "%d ", hash);
        free(word);
        printf("\tappended %d to hash.bin\n", hash);
        word = userInputString();
    }
    free(word);
    fclose(fp);
}

void writeHashToBinaryFile(char *fileContents) { 
    FILE *fp = fopen("hash.bin", "wb");
    if(fp == NULL) {
        printf("could't open hash.bin");
        exit(1);
    }
    int index = 0;
    while(fileContents[index] != '\0') {
        while(isSeparator(fileContents[index])) {
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
        while(fileContents[index] != '\0' && !isSeparator(fileContents[index]) && wordIndex < MAX_WORD_LENGTH) {
            word[wordIndex++] = fileContents[index++];
        }
        word[wordIndex] = '\0';
        int hash = determineHashValue(word);
        fprintf(fp, "%d ", hash);
        free(word);
    }
    fclose(fp);
}

void writeSelectionToTextFile(int *selection, char *fileContents) { // filter the file with the selected buckets
    FILE *fp1 = fopen("input_text_with_selection.txt", "w"); // save input text with words filtered out that are not in selected buckets
    FILE *fp2 = fopen("input_text_without_selection.txt", "w"); // save input text with words filtered out that are in selected buckets
    if(fp1 == NULL || fp2 == NULL) {
        printf("couldn't open selection files");
        exit(1);
    }
    int index = 0;
    while(fileContents[index] != '\0') {
        while(isSeparator(fileContents[index])) {
            fprintf(fp1, "%c", fileContents[index]);
            fprintf(fp2, "%c", fileContents[index]);
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
        while(fileContents[index] != '\0' && !isSeparator(fileContents[index]) && wordIndex < MAX_WORD_LENGTH) {
            word[wordIndex++] = fileContents[index++];
        }
        word[wordIndex] = '\0';
        int hash = determineHashValue(word);
        if(selection[hash] == 1) {
            fprintf(fp1, "%s", word);
        }
        else {
            fprintf(fp2, "%s", word);
        }
        free(word);
    }
    fclose(fp1);
    fclose(fp2);
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

int *selectBuckets(Node *buckets) { // select buckets to filter the file with
    int *selections = calloc(HASH_SIZE, sizeof(int)); // allocate memory for selection
    if(selections == NULL) {
        printf("calloc failed");
        exit(1);
    }
    printf("\nSelect Bucket indices to filter\n");
    while(1) { // loop until user enters -1
        printf("Selected buckets: "); // print selected buckets
        for(int i = 0; i < HASH_SIZE; i++) {
            if(selections[i]) {
                printf("%d, ", i);
            }
        }
        printf("\n");
        char *selectionStr = userInputString();
        int selection = strtod(selectionStr, NULL);
        free(selectionStr);
        if(selection >= 0 && selection < HASH_SIZE) { // if user enters invalid selection, print error message
            selections[selection] = !selections[selection];
            if(selections[selection]) {
                printf("Selected Bucket[%d]: ", selection);
                printBucket(&buckets[selection]);
            }
            printf("\n");
        }
        else if(selection == -1) {
            return selections;
        }
        else { // if user enters valid selection, toggle selection
            printf("Invalid selection\n");
        }
    }
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

int isSeparator(char c) {
    if(c == ' ' || c == '.' || c == '\n' || c ==';' || c==':' || c == ',' || c == '?' || c == '\t') {
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