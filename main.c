#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "node.h"
#include "buckets.h"
#include "utility.h"

const int MAX_WORD_LENGTH = 50;
const int HASH_SIZE = 37;
const int WORDS_PER_LINE = 10;
const int MAX_FILE_LENGTH = 100000;

char *loadFileContents(char *filename);
void writeSelectionToTextFile(int *selection, char *fileContents);
void writeHashToBinaryFile(char *fileContents);
void appendHashesToBinary();

int main(int argc, char *argv[]) {
    if(argc != 2) {
        printf("no filename provided");
        exit(1);
    }
    char *fileContents = loadFileContents(argv[1]); // load file contents into string
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

char *loadFileContents(char *filename) { // load file contents into string
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        printf("invalid filename");
        exit(1);
    }
    char *tempContents = malloc((MAX_FILE_LENGTH)*sizeof(char));
    size_t tempContentsSize = fread(tempContents, sizeof(char), MAX_FILE_LENGTH, fp);
    char *fileContents = malloc((tempContentsSize+1)*sizeof(char));
    int fileContentsIndex = 0;
    for(size_t i = 0; i < tempContentsSize; i++) {
        if(tempContents[i] != '\0' && tempContents[i] >= 0)
            fileContents[fileContentsIndex++] = tempContents[i];
    }
    fileContents[fileContentsIndex] = '\0';
    fclose(fp);
    free(tempContents);
    return fileContents;
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

void appendHashesToBinary() { // add word to hash table
    FILE *fp = fopen("hash.bin", "ab");
    if(fp == NULL) {
        printf("could't open hash.bin");
        exit(1);
    }
    printf("\nEnter words to append their hash to hash.bin\n\n");
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