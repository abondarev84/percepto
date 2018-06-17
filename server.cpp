#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros


#include "server.h"

#define BUFFER_SIZE MESSAGE_SIZE
#define MAX_CLIENTS 64

void ServerFunc(mqueue_t *mqueue, bool *ServerActive, unsigned int port, void (*f)(message_t *m))
{
  int opt = 1, i; //for setsockopt
  int master_socket, addrlen, new_socket, activity, valread, sd, max_sd;
  int client_socket[MAX_CLIENTS];
  struct sockaddr_in address;
  uint8_t buffer[BUFFER_SIZE];

  fd_set readfds;

  //OK reply message
  const char *message = "OK";

  //setting all initial client sockets to 0 to not check them
  for (i = 0; i < MAX_CLIENTS; i++)
  {
    client_socket[i] = 0;
  }

  //the listening master socket
  if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
  {
    perror("socket failed");
    return;
  }

  //set master socket to allow multiple connections
  if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0 )
  {
    perror("setsockopt");
    return;
  }

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( port );

  //bind the socket to localhost port from input parameter
  if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
  {
    perror("bind failed");
    return;
  }

  //half the maximum clients may be queued during connection
  if (listen(master_socket, MAX_CLIENTS/2) < 0)
  {
    perror("listen");
    return;
  }

  addrlen = sizeof(address);

  while(ServerActive)
  {
    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(master_socket, &readfds);
    max_sd = master_socket;

    //add child sockets to set
    for ( i = 0 ; i < MAX_CLIENTS ; i++)
    {
      sd = client_socket[i];

      //if active socket, add to read list
      if(sd > 0)
      FD_SET( sd , &readfds);

      //highest file descriptor number update
      if(sd > max_sd)
      max_sd = sd;
    }

    //wait for an activity on one of the sockets , timeout is NULL = wait indefinitely
    activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

    if ((activity < 0) && (errno!=EINTR))
    {
      printf("select error");
    }

    //If something happened on the master socket , then its an incoming connection
    if (FD_ISSET(master_socket, &readfds))
    {
      if ((new_socket = accept(master_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
      {
        perror("accept");
        return;
      }

      //add new socket to array of sockets
      for (i = 0; i < MAX_CLIENTS; i++)
      {
        //if position is empty
        if (client_socket[i] == 0)
        {
          client_socket[i] = new_socket;
          break;
        }
      }
    }

    //else its some IO operation on  other socket
    for (i = 0; i < MAX_CLIENTS; i++)
    {
      sd = client_socket[i];

      if (FD_ISSET( sd, &readfds))
      {
        //Check if it socket closed
        //read the incoming message
        if ((valread = read( sd , buffer, BUFFER_SIZE)) == 0)
        {

          //Close the socket and mark as 0 in list for reuse
          close( sd );
          client_socket[i] = 0;
        }
        //move message to queued
        //call callback function
        //send ok
        else
        {
          //set the string terminating NULL byte on the end of the data read
          if (add_to_queue(mqueue, (message_t*)buffer))
          {
            printf("add operation failed\nadvised to close server\n");
          }
          f((message_t*)buffer);
          send(sd , message , strlen(message) , 0 );
        }
      }
    }
  }

}


Server::Server(unsigned int ListenPort, void (*f)(message_t *m))
{
  ServerActive = 1;
  this->mqueue = init_queue();
  thread ServerThread(ServerFunc, this->mqueue, &this->ServerActive, ListenPort, f);
  ServerThread.detach();
}

Server::~Server()
{
  ServerActive = 0;
  destroy_queue(this->mqueue);
  this->mqueue = NULL;
}

int Server::Pop(message_t *m)
{
  return pop_from_queue(this->mqueue, m);
}
