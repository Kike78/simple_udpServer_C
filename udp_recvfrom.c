/* udp_recvfrom.c 
 * A simple udp server */
#include "chap04.h"

int main()
{
#if defined(_WIN32)
   WSADATA d;
   if (WASAStartup(MAKEWORD(2, 2), &d))
   {
      fprintf(stderr, "Failed to initialize.\n");
      return 1;
   }
#endif

   /* Then, we must configure the local address that our server listen on. We use getaddrinfo() for this,
    * as follows: */
   printf("Configuring local address...\n");
   struct addrinfo hints;

   memset(&hints, 0, sizeof(hints));
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_DGRAM;  //UDP
   hints.ai_flags = AI_PASSIVE;

   struct addrinfo *bind_address;
   getaddrinfo(0, "8080", &hints, &bind_address);

   /* We can create the socket, as follows: */
   printf("Creating socket...\n");
   SOCKET socket_listen;
   socket_listen = socket(bind_address -> ai_family, bind_address -> ai_socktype, bind_address -> ai_protocol);

   if (!ISVALIDSOCKET(socket_listen))
   {
      fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
   }
   /* This code is exactly the same as in the tcp case */
   /* We must then bind the new socket to the local address that we got from getaddrinfo(). This is as follows: */
   printf("Binding socket to local address...\n");
   if (bind(socket_listen, bind_address -> ai_addr, bind_address -> ai_addrlen))
   {
      fprintf(stderr, "bind() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
   }
   freeaddrinfo(bind_address);   //again, that code is exactly the same as in the tcp case.

   /* Here is where udp server diverges from the tcp server. There is no need to call listen() or accept(). 
    * We listen for incoming data using recvfrom(), as shown here: */
   struct sockaddr_storage client_address;
   socklen_t client_len = sizeof(client_address);
   char read[1024];
   int bytes_received = recvfrom(socket_listen, read, 1024, 0,(struct sockaddr*) &client_address, &client_len);
   /* you can think of recvfrom() as a combination of the tcp server accept() and recv(). */

   /* Once we've received data, we can print it out. Keep in mind that the data may not be null terminated.
    * It can be safely printed with the %.*s printf() format specifier, as shown in the following code: */
   printf("Received (%d bytes): %.*s\n", bytes_received, bytes_received, read);

   /* It may also be useful to print the sender's address and port number. We can use the getnameinfo() function
    * to convert this data into a printable string, as shown in the following code: */
   printf("Remote address is: ");
   char address_buffer[100];
   char service_buffer[100];

   getnameinfo(((struct sockaddr*)&client_address), client_len, address_buffer, sizeof(address_buffer),
         service_buffer, sizeof(service_buffer), NI_NUMERICHOST | NI_NUMERICSERV);

   printf("%s %s\n", address_buffer, service_buffer);
   /* the las argument to getnameinfo() (NI_NUMERICHOST | NI_NUMERICSERV) tells that we want both the client address
    * and port number to be in numeric form. Without this, it would attempt to return a hostname or protocol name
    * if the port number matches a known protocol. */

   /* Once the data has been received, we'll end our simple udp server by closing connection, cleaning up winsock,
    * and ending the program: */
   CLOSESOCKET(socket_listen);

#if defined(_WIN32)
   WSACleanup();
#endif

   printf("Finished.\n");
   
   return 0;
}
