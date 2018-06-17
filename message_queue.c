#include <string.h>
#include <stdlib.h>

#include "message_queue.h"

typedef struct element {
  message_t data;
  struct element *next;
}element_t;

struct queue {
  element_t *head;
  element_t *tail;
  int added;
  int removed;
} mqueue_t;

mqueue_t* init_queue()
{
  mqueue_t *mqueue = calloc(1, sizeof(mqueue_t));
  if (!mqueue) {
    return NULL;
  }
  mqueue->head = calloc(1, sizeof(element_t));
  mqueue->tail = mqueue->head;

  if (!mqueue->head) {
    return NULL;
  }
  return mqueue;
}

int add_to_queue(mqueue_t *mqueue, message_t *m)
{
  memcpy(mqueue->tail->data, m, MESSAGE_SIZE);
  mqueue->tail->next = calloc(1, sizeof(element_t));
  mqueue->tail = mqueue->tail->next;
  ++(mqueue->added);

  if (!mqueue->tail) {
    return 1;
  }

  return 0;
}

int pop_from_queue(mqueue_t *mqueue, message_t *m)
{
  if (mqueue->added > mqueue->removed)
  {
    ++(mqueue->removed);
    memcpy(m, mqueue->head, MESSAGE_SIZE);
    element_t *tmp = mqueue->head;
    mqueue->head = mqueue->head->next;
    free(tmp);
    return 0;
  }

  return 1;
}

void destroy_queue(mqueue_t *mqueue)
{
  while (mqueue->head) {
    mqueue_t *tmp = mqueue->head->next;
    free(mqueue->head);
    mqueue->head = tmp;
  }
  free(mqueue);
  mqueue = NULL;
}
