#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if __has_include("strings.h")
#include "strings.h"
#endif

#define MAX_WORD_LENGTH 50
#define HASH_SIZE 37
#define WORDS_PER_LINE 10

typedef struct Node {
    char *word;
    int count;
    struct Node *next;
} Node;

void printBucket(Node *bucket);
void printBuckets(Node *buckets);
char *userInputWord();
int userInputNumber();
Node *createNode(char *word);
Node *initBuckets();
void loadBuckets(Node *buckets, FILE *fp);
int *selectBuckets(Node *buckets);
void appendHashesToBinary();
void addNodeToBucket(Node *newNode, Node *bucket);
void freeBuckets(Node *buckets);
void writeSelectionToTextFile(int *selection, FILE *fp);
void writeHashToBinaryFile(FILE *fp);
int determineHashValue(char *word);
int isSeparator(char c);
char *parseFileStrean(FILE *fp);

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("no filename provided");
        exit(1);
    }
    Node *buckets = initBuckets(); // initialize buckets
    FILE *fp;
    fp = fopen(argv[1], "r"); // open file to read from
    if(fp == NULL) {
        printf("invalid filename");
        freeBuckets(buckets);
        exit(1);
    }
    loadBuckets(buckets, fp); // fill buckets with words from file
    printBuckets(buckets); // print individual buckets
    int *selection = selectBuckets(buckets); // select buckets to filter the file with and print individual buckets
    writeSelectionToTextFile(selection, fp); // filter the file with the selected buckets
    writeHashToBinaryFile(fp); // write filtered words to binary file
    appendHashesToBinary(); // add word to hash table
    fclose(fp); // close file 
    freeBuckets(buckets); // free buckets
    free(selection); // free selection
    exit(0);
}

char *parseFileStrean(FILE *fp) {
    char *word = malloc( MAX_WORD_LENGTH * sizeof(char)); // allocate memory for word
    if(word == NULL) {
        printf("malloc failed");
        exit(1);
    }
    int wordIndex = 0;
    int c = fgetc(fp);
    while(c != EOF) {
        if(c < -1 || c > 127) { // if c is not a valid character, skip to next character
            c = fgetc(fp);
            continue;
        }
        if( isSeparator(c) || wordIndex == MAX_WORD_LENGTH) { // if c is a delimiter or word is full, return word
            if(wordIndex > 0) { 
                word[wordIndex] = '\0';
                if(wordIndex != MAX_WORD_LENGTH-1)
                    fseek(fp, -1L, SEEK_CUR); // move file pointer back one character
                return word;
            }
            else { 
                word[wordIndex++] = c;
                word[wordIndex] = '\0';
                return word;
            }
        }
        else { // if c is not a delimiter, add c to word and increment wordIndex
            word[wordIndex++] = c;
        }
        c = fgetc(fp);
    }
    free(word);
    return NULL;
}

void writeHashToBinaryFile(FILE *fp) {
    fseek(fp, 0, SEEK_SET); // reset file pointer to beginning of file
    FILE *fp1 = fopen("hash.bin", "w");
    char *str = parseFileStrean(fp);
    while(str != NULL) {
        if(! isSeparator(str[0])) {
            int hash = determineHashValue(str);
            fprintf(fp1, "%d ", hash);
        }
        free(str);
        str = parseFileStrean(fp);
    }
    free(str);
    fclose(fp1);
}

void writeSelectionToTextFile(int *selection, FILE *fp) { // filter the file with the selected buckets
    fseek(fp, 0, SEEK_SET); // reset file pointer to beginning of file
    FILE *fp1 = fopen("input_text_with_selection.txt", "w"); // save input text with words filtered out that are not in selected buckets
    FILE *fp2 = fopen("input_text_without_selection.txt", "w"); // save input text with words filtered out that are in selected buckets
    char *str = parseFileStrean(fp);
    while(str != NULL) {
        if(! isSeparator(str[0])) {
            int hash = determineHashValue(str);
            if(selection[hash] == 1) {
                fprintf(fp1, "%s", str);
            }
            else {
                fprintf(fp2, "%s", str);
            }
        }
        else {
            fprintf(fp1, "%c", str[0]);
            fprintf(fp2, "%c", str[0]);
        }
        free(str);
        str = parseFileStrean(fp);
    }
    free(str);
    fclose(fp1);
    fclose(fp2);
}

void loadBuckets(Node *buckets, FILE *fp) { // fill buckets with words from file
    fseek(fp, 0, SEEK_SET); // reset file pointer to beginning of file
    char *str = parseFileStrean(fp);
    while(str != NULL) {
        if(! isSeparator(str[0])) {
            int hash = determineHashValue(str);
            Node *newNode = createNode(str);
            addNodeToBucket(newNode, &buckets[hash]);
        }
        else free(str);
        str = parseFileStrean(fp);
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

void appendHashesToBinary() { // add word to hash table
    FILE *fp = fopen("hash.bin", "a");
    char *word = userInputWord();
    while(strcmp(word, "-1") != 0) {
        int hash = determineHashValue(word);
        fprintf(fp, "%d ", hash);
        free(word);
        word = userInputWord();
    }
    free(word);
    fclose(fp);
}

int *selectBuckets(Node *buckets) { // select buckets to filter the file with
    int *selections = calloc(HASH_SIZE, sizeof(int)); // allocate memory for selection
    if(selections == NULL) {
        printf("calloc failed");
        exit(1);
    }
    while(1) { // loop until user enters -1
        printf("Selected buckets: "); // print selected buckets
        for(int i = 0; i < HASH_SIZE; i++) {
            if(selections[i]) {
                printf("%d ", i);
            }
        }
        printf("\n");
        int selection = userInputNumber();
        if(selection >= 0 && selection < HASH_SIZE) { // if user enters invalid selection, print error message
            selections[selection] = !selections[selection];
            printf("Bucket[%d]: ", selection);
            printBucket(&buckets[selection]);
        }
        else if(selection == -1) {
            return selections;
        }
        else { // if user enters valid selection, toggle selection
            printf("Invalid selection\n");
        }
    }
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

char *userInputWord() { // get word from user
    char *word = malloc(MAX_WORD_LENGTH * sizeof(char));
    if(word == NULL) {
        printf("malloc failed");
        exit(1);
    }
    printf("Enter word (-1 to stop): ");
    scanf("%s", word);
    while(getchar() != '\n'); // clear input buffer (to prevent infinite loop)
    return word;
}

int userInputNumber() {
    int number;
    printf("Enter number (-1 to stop): ");
    scanf("%d", &number);
    while(getchar() != '\n'); // clear input buffer (to prevent infinite loop)
    return number;
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