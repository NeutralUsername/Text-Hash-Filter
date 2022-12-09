#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#if __has_include("strings.h")
#include "strings.h"
#endif

#define MAX_WORD_LENGTH 100
#define HASH_SIZE 37
#define WORDS_PER_LINE 10

typedef struct Node {
    char *word;
    int count;
    struct Node *next;
} Node;

FILE *openFile();
Node *createNode(char *word);
int genHash(char *word);
void insertIntoBucket(Node *head, char *word, Node *bucket);
void freeBuckets(Node *buckets);
int *selectBuckets(Node *buckets);
void printBucket(Node *bucket);
void filterSelections(Node *buckets, int *selection, FILE *fp);
void filLBuckets(Node *buckets, FILE *fp);

int main() { 
    FILE *fp = openFile();
    Node buckets[HASH_SIZE];
    for(int i = 0; i < HASH_SIZE; i++) {
        buckets[i].next = NULL;
    }
    filLBuckets(buckets, fp); // fill buckets with words from file
    for(int i = 0; i < HASH_SIZE; i++) { // print buckets 
        printf("Bucket[%d]: ", i); 
        printBucket(&buckets[i]); 
    }
    int *selection = selectBuckets(buckets); // select buckets to filter the file with and print individual buckets
    filterSelections(buckets, selection, fp); // filter the file with the selected buckets
    fclose(fp); // close file 
    freeBuckets(buckets); // free buckets
    free(selection); // free selection
    return 0;
}

void filLBuckets(Node *buckets, FILE *fp) { // fill buckets with words from file
    char *word = malloc( MAX_WORD_LENGTH * sizeof(char)); // allocate memory for word
    int wordIndex = 0;
    int c;
    do {
        c = fgetc(fp);
        if(c < -1 || c > 127) { // if c is not a valid character or -1
            continue; // skip to next character
        }
        if(c == ' ' || c == '.' || c == '\n' || c ==';' || c==':' || c == ',' || c == '?' || c == '\t' || wordIndex == MAX_WORD_LENGTH-1 || c == EOF) { // if c is a delimiter
            if(wordIndex > 0) {  // if word is not empty. (This is to prevent empty words from being added to the buckets)
                word[wordIndex] = '\0';  // add null terminator to end of word
                char *wordCopy = malloc( (wordIndex + 1) * sizeof(char)); // allocate memory for wordCopy
                strcpy(wordCopy, word); // copy word to wordCopy
                Node *head = createNode(wordCopy); // create a new node for the word
                insertIntoBucket(head, wordCopy, &buckets[genHash(wordCopy)]); // insert the word into the proper bucket
                wordIndex = 0; // reset wordIndex to 0 in order for the next word to be read.
            }
        }
        else { // if c is not a delimiter, add c to word and increment wordIndex
            word[wordIndex++] = c; 
        }
    } while(c != EOF);
    free(word);
}

void insertIntoBucket(Node *newNode, char *word, Node *bucket) { // insert word into bucket
    Node *prev = bucket;
    Node *current = bucket->next;
    while(current != NULL) { 
        if(strcmp(current->word, word) == 0) { // if word is already in bucket increment count
            current->count++;
            free(newNode->word);
            free(newNode);
            return;
        }
        else if(strcasecmp(current->word, word) > 0) { // if word is less than current word(alphabetically), insert word before current word
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

int *selectBuckets(Node *buckets) { // select buckets to filter the file with
    int *selections = calloc(HASH_SIZE, sizeof(int)); // allocate memory for selection
    int selectionCount = 0;
    while(1) { // loop until user enters -1
        printf("Selected buckets: "); // print selected buckets
        for(int i = 0; i < HASH_SIZE; i++) {
            if(selections[i]) {
                printf("%d ", i);
            }
        }
        int index; 
        printf("\nEnter index (-1 to stop): "); // prompt user for index to select
        scanf("%d", &index); // read index
        while(getchar() != '\n'); // clear input buffer (to prevent infinite loop)
        if(index == -1) { // if user enters -1 
            return selections; // return selections
        }
        else if(index >= 0 && index < HASH_SIZE) {  // if index is valid
            if(selections[index]) { // if bucket is already selected, unselect it
                selections[index] = 0;
                selectionCount--;
            }
            else { // if bucket is not selected, select it
                printf("Bucket[%d]: ", index); 
                printBucket(&buckets[index]); 
                selections[index] = 1;
                selectionCount++;
            }
        }
        else {
            printf("Invalid index\n");
        }
    }
}

void filterSelections(Node *buckets, int *selection, FILE *fp) { // filter the file with the selected buckets
    FILE *fp1 = fopen("input_with_selected_buckets.txt", "w"); // save input text with words filtered out that are not in selected buckets
    FILE *fp2 = fopen("input_without_selected_buckets.txt", "w"); // save input text with words filtered out that are in selected buckets
    fseek(fp, 0, SEEK_SET); // reset file pointer to beginning of file
    char *word = malloc( MAX_WORD_LENGTH * sizeof(char)); // allocate memory for word
    int wordIndex = 0; 
    int c; 
    do {
        c = fgetc(fp);
        if(c < -1 || c > 127) { // if c is not a valid character, skip to next character
            continue; 
        }
        if(c == ' ' || c == '.' || c == '\n' || c ==';' || c==':' || c == ',' || c == '?' || c == '\t' || wordIndex == MAX_WORD_LENGTH-1 || c == EOF) { // if c is a delimiter
            if(wordIndex > 0) { 
                word[wordIndex] = '\0'; 
                int hash = genHash(word); 
                if(selection[hash]) { // if the bucket is selected
                    fprintf(fp1, "%s", word);
                }
                else { // if the bucket is not selected
                    fprintf(fp2, "%s", word); 
                }
                wordIndex = 0; // reset wordIndex to 0.
            }
            if(c != EOF) { // if c is not end of file, print the delimiter
                fprintf(fp1, "%c", c); 
                fprintf(fp2, "%c", c);  
            }
        }
        else { // if c is not a delimiter, add c to word and increment wordIndex
            word[wordIndex++] = c; 
        }
    } while(c != EOF);
    fclose(fp1);
    fclose(fp2);
    free(word);
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
}

Node *createNode(char *word) {
    Node *node = malloc(sizeof(Node));
    node->next = NULL;
    node->count = 1;
    node->word = word;
    return node;
}

FILE *openFile() {
    while(1) {
        char *filename = malloc(MAX_WORD_LENGTH * sizeof(char));
        printf("Enter filename: ");
        scanf("%s", filename);
        FILE *fp = fopen(filename, "r");
        free(filename);
        if(fp) {
            return fp;
        }
    }
}

int genHash(char *word) {
    int hash = 0;
    int i = 0;
    while(word[i] != '\0') {
        hash += word[i];
        i++;
    }
    return hash % HASH_SIZE;
}