#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new = malloc(sizeof(struct list_head));
    if (!new)
        return NULL;
    INIT_LIST_HEAD(new);
    return new;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;
    element_t *it, *safe = NULL;

    // list_for_each_entry_safe(it, safe, head, list)
    // it：遍歷時的當前節點（指向 element_t *）。
    // safe：存儲 下一個節點，確保刪除當前節點後，仍能繼續遍歷。
    // head：鏈表的 頭節點，它是 struct list_head * 類型，代表鏈表的起點。
    // list：這是 element_t 結構中的 list 成員，用於從 element_t 取得 struct
    // list_head

    list_for_each_entry_safe(it, safe, head, list) {
        list_del(&it->list);    // 先從鏈表中刪除節點
        q_release_element(it);  // 釋放 element_t 結構
    }
    free(head);  // 最後釋放 queue 頭部
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    new->value = strdup(s);

    if (!new)
        return false;
    list_add(&new->list, head);
    return true;
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, char *s)
{
    if (!head)
        return false;

    element_t *new = malloc(sizeof(element_t));
    new->value = strdup(s);

    if (!new)
        return false;
    list_add_tail(&new->list, head);  // why
    return true;
}

/* Remove an element from head of queue */
element_t *q_remove_head(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *first = list_first_entry(head, element_t, list);
    list_del(&first->list);
    if (sp) {
        strncpy(sp, first->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }
    return first;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *last = list_last_entry(head, element_t, list);
    list_del(&last->list);
    if (sp) {
        strncpy(sp, last->value, bufsize - 1);
        sp[bufsize - 1] = '\0';
    }

    return NULL;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return 0;
    }
    int size = 0;
    struct list_head *p;
    list_for_each(p, head)
        size++;
    return size;
}
/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;
    struct list_head *left = head->next;
    struct list_head *right = head->prev;

    while (left != right) {
        left = left->next;
        if (left == right) {
            break;
        }
        right = right->prev;
    }
    list_del(right);
    element_t *elem = list_entry(right, element_t, list);
    free(elem);  // 要把 elem 處理乾淨

    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(struct list_head *head)
{
    if (!head || list_empty(head)) {
        return false;
    }
    element_t *it, *safe = NULL;
    bool dup = false;

    list_for_each_entry_safe(it, safe, head, list) {
        // 錯在終止條件中，一開始是判斷 safe 是否為 null，但回去看 list.h後發現
        // it 會走到最後一個節點，所以 safe 要判斷是否回到 head 值!
        if (&safe->list != head && !strcmp(it->value, safe->value)) {
            list_del(&it->list);
            q_release_element(it);
            dup = true;
        } else if (dup) {
            list_del(&it->list);
            q_release_element(it);
            dup = false;
        }
    }
    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    struct list_head *cur;
    cur = head->next;
    while (cur && cur->next != head) {
        list_move(cur, cur->next);
        cur = cur->next;
    }
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    element_t *cur;
    struct list_head *tmp;
    tmp = head->next;
    head->next = head->prev;
    head->prev = tmp;
    list_for_each_entry(cur, head, list) {
        tmp = cur->list.next;
        cur->list.next = cur->list.prev;
        cur->list.prev = tmp;
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head)) {
        return;
    }
    struct list_head *i_ptr = head;
    struct list_head *cur, *key_ptr;
    const char *key_val;
    if (descend) {
        while (i_ptr->next != head) {
            cur = i_ptr->next;
            key_ptr = cur;
            key_val = list_entry(cur, element_t, list)->value;
            while (cur != head) {
                if (strcmp(list_entry(cur, element_t, list)->value, key_val) >
                    0) {
                    key_val = list_entry(cur, element_t, list)->value;
                    key_ptr = cur;
                }
                cur = cur->next;
            }
            list_move(key_ptr, i_ptr);
            i_ptr = i_ptr->next;
        }
    } else {
        while (i_ptr->next != head) {
            cur = i_ptr->next;
            key_ptr = cur;
            key_val = list_entry(cur, element_t, list)->value;
            while (cur != head) {
                if (strcmp(list_entry(cur, element_t, list)->value, key_val) <
                    0) {
                    key_val = list_entry(cur, element_t, list)->value;
                    key_ptr = cur;
                }
                cur = cur->next;
            }
            list_move(key_ptr, i_ptr);
            i_ptr = i_ptr->next;
        }
    }
}


/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    int cnt = 0;
    struct list_head *next = head->next, *tmp;
    const element_t *entry = list_entry(next, element_t, list);
    char *max = strdup(entry->value);

    while (next && next != head) {
        element_t *cur = list_entry(next, element_t, list);
        if (strcmp(max, cur->value) > 0) {
            tmp = next->next;
            list_del(next);
            q_release_element(cur);
            next = tmp;
        } else {
            free(max);
            max = strdup(cur->value);
            next = next->next;
            cnt = cnt + 1;
        }
    }
    return cnt;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    int cnt = 0;
    struct list_head *prev = head->prev, *tmp;
    const element_t *entry = list_entry(prev, element_t, list);
    char *max = strdup(entry->value);
    while (prev && prev != head) {
        element_t *cur = list_entry(prev, element_t, list);
        if (strcmp(max, cur->value) > 0) {
            tmp = prev->prev;
            list_del(prev);
            q_release_element(cur);
            prev = tmp;
        } else {
            free(max);
            max = strdup(cur->value);
            prev = prev->prev;
            cnt = cnt + 1;
        }
    }
    return cnt;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/'
    if (!head || list_empty(head))
        return 0;
    if (list_is_singular(head))
        return q_size(
            list_first_entry(head, queue_contex_t, chain)->q);  // why??

    return 0;
}

/*
    利用 Fisher–Yates shuffle 演算法來實作洗牌（shuffle）
    1. 先用 q_size 取得 queue 的大小 len。
    2. 隨機從 0 ~ (len - 1) 中抽出一個數字 random，然後 old 將指向從前面數來第
   random 個節點，new 會指向最後一個未被抽到的節點，將 old 和 new
   指向的節點的值交換，再將 len - 1。
    3. 隨著 len 大小變小，已經被抽到過，並交換值到 queue
   後面的會愈來愈多，直到所有的節點都已經被抽到過，shuffle 就結束。
*/

void swap(struct list_head *a, struct list_head *b)
{
    if (a == NULL || b == NULL || a == b)
        return;

    struct list_head *a_prev = a->prev;
    struct list_head *b_prev = b->prev;

    if (a->prev != b)
        list_move(b, a_prev);
    list_move(a, b_prev);
}

/* 有點問題 */
int q_shuffle(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    int len = q_size(head);
    struct list_head *last = head->prev;
    for (int i = 0; i < len; i++) {
        int random = rand() % (len - i);
        struct list_head *old, *new;
        old = head->next;
        for (int j = 0; j < random - 1; j++) {
            old = old->next;
        }
        new = last;
        swap(old, new);
        last = last->prev;
    }
    return len;
}