#include <iostream>
#include <unistd.h>
#include <string.h>

#include "server.h"
#include "message_queue.h"

using namespace std;

void dump_message(message_t *m)
{
  cout<<"auto:type="<<m->type<<", data="<<m->data<<endl;
}

int main()
{
  message_t message = {};
  Server srv(9999, &dump_message);
  sleep(20);
  while (!srv.Pop(&message))
  {
    cout<<message.type<<" "<<message.data<<endl;
    memset(&message, 0, MESSAGE_SIZE);
  }

  return 0;
}
