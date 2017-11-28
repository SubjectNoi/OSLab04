//
// Created by subjectnoi on 11/28/17.
//

#include "list.h"

void list_init(list_t* t) {
    t->head = NULL;
    lock_init(&t->lock);
}

void list_insert(list_t* t, unsigned int __val) {
    lock_acquire(&t->lock);
    node_t *ins = (node_t*)malloc(sizeof(node_t));
    ins->val = __val;
    ins->next = t->head;
    t->head = ins;
    lock_release(&t->lock);
    return;
}

void list_delete(list_t* t, unsigned int __val) {
    lock_acquire(&t->lock);
    node_t *pos = t->head, *pre = pos;
    while (pos->val == __val) {
        t->head = pos->next;
        pos = pos->next;
        free(pre);
        pre = pos;
    }
    pos = pos->next;
    while (pos) {
        if (pos->val == __val) {
            pre->next = pos->next;
            free(pos);
            pos = pre->next;
        }
        else {
            pre = pos;
            pos = pos->next;
        }
    }
    lock_release(&t->lock);
}

void* list_lookup(list_t* t, unsigned int __val) {
    lock_acquire(&t->lock);
    node_t *res = t->head;
    while (res) {
        if (res->val == __val) {
            lock_release(&t->lock);
            return (void*)res;
        }
        res = res->next;
    }
    lock_release(&t->lock);
    return NULL;
}

void list_print(list_t* t) {
    lock_acquire(&t->lock);
    node_t* res = t->head;
    while (res) {
        printf("%d%c", res->val, res->next ? '-' : '\n');
        res = res->next;
    }
    lock_release(&t->lock);
}