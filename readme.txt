queue was created to allow enqueue and dequeue operations without the need for mutex.
message type is 128B with first byte being message type (example in header) and the rest 127B is data.

the server itself is a select server that accepts up to 64 connections and uses one thread to accept
new connections or receive data on any of the opened connections.

the majority of the code is written basically in C is since I'm not familiar enough with the boost
ASIO library to use it freely (just read about it after the assignment) and afaik there are no sockets
otherwise in C++.

as it turns out, I remember much less from C++ than I would like to admit.
as a result of the above (I'm not sure if this can be classified as a C++ code), I didn't bother really
with a proper makefile (would take me a little longer probably).

to build the test, run the compilation executable from the folder.

to use netcat as a client:
the command I used was <netcat 127.0.0.1 9999 -p 12345>
incremented by one the outgoing port number for several connections.
