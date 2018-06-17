#ifndef _MESSAGE_H_
#define _MESSAGE_H_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum message_types {
  type1 = 1,
  type2,
  type3,
  type4,
  TYPES,
};

typedef struct queue mqueue_t;

#define MESSAGE_SIZE 128

typedef struct message {
  uint8_t type;
  uint8_t data[MESSAGE_SIZE - sizeof(uint8_t)];
} message_t;

mqueue_t* init_queue();

int add_to_queue(mqueue_t *mqueue, message_t *m);

int pop_from_queue(mqueue_t *mqueue, message_t *m);

void destroy_queue(mqueue_t *mqueue, );

#ifdef __cplusplus
extern "C" {
#endif // extern "C"
#endif //_MESSAGE_H_
