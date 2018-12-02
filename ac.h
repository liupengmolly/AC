#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct trie_node{
    int len;
    int char_sign;
    char *slice;
    int num;
    struct trie_node *parent;
    struct trie_node *left;
    struct trie_node *right;
    struct trie_node *fail;
}trie_node;

typedef struct line_code{
    int len;
    char *s;
    struct line_code *next;
}line_code;

typedef struct queue{
    struct queue_node *first;
    struct queue_node *last;
    int len;
}queue;

typedef struct queue_node{
    struct trie_node *node;
    struct queue_node *next;
}queue_node;

void init_queue(queue *q){
    q->len = 0;
    q->first = NULL;
    q->last = NULL;
    return;
}

queue_node *init_queue_node(trie_node *node){
    queue_node *qnode = (queue_node*)malloc(sizeof(queue_node));
    qnode->node = node;
    qnode->next = NULL;
    return qnode;
}

void queue_push(queue *q,trie_node *node){
    if(q->len == 0){
        q->first = q->last = init_queue_node(node);
        q->len = 1;
    }else{
        q->last->next = init_queue_node(node);
        q->last = q->last->next;
    }
    return;
}