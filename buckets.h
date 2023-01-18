#ifndef BUCKETS_H
#define BUCKETS_H
#include "node.h"
Node *initBuckets();
void loadBuckets(Node *buckets, char *fileContents);
void freeBuckets(Node *buckets);
int *selectBuckets(Node *buckets);
void printBucket(Node *bucket);
void printBuckets(Node *buckets);
#endif