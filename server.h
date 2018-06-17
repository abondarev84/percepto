#ifndef _SERVER_H_
#define _SERVER_H_

#include <thread>
#include <cstdint>

#include "message_queue.h"

using namespace std;

class Server {
  public:
    Server(unsigned int ListernPort, void (*f)(message_t *m));
    ~Server();

    //success = 0
    //failure = 1
    int Pop(message_t *m);


  private:
    mqueue_t *mqueue;
    bool ServerActive;
};


#endif //_SERVER_H_
