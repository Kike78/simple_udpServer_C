/* A simple udp client */

#include "chap04.h"

int main()
{
#if defined(_WIN32)
   WSADATA d;
   if (WSAStartup(MAKEWORD(2, 2), &d))
   {
      fprintf(stderr, "Failed to initialize.\n");
      return 1;
   }
#endif

   /* We then configure the remote address using getaddrinfo(). In this minimal example, we use 127.0.0.1 as
    * the remote address and 8080 as the remote port. This means that it connects to udp server only if it's
    * running on the same computer. Here is how the remote address is configured: */
   printf("Configuring remote address...\n");
   struct addrinfo hints;

   memset(&hints, 0, sizeof(hints));
   hints.ai_socktype = SOCK_DGRAM;

   struct addrinfo *peer_address;
   if (getaddrinfo("127.0.0.1", "8080", &hints, &peer_address))
   {
      fprintf(stderr, "getaddrinfo() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
   }

   /* We can print the configured address using getnameinfo(). The call to getnameinfo() is the same as in the
    * previous udp server, udp_recvfrom.c. It works as follows: */
   printf("Remote address is: ");
   char address_buffer[100];
   char service_buffer[100];

   getnameinfo(peer_address -> ai_addr, peer_address -> ai_addrlen, address_buffer, sizeof(address_buffer),
         service_buffer, sizeof(service_buffer), NI_NUMERICHOST | NI_NUMERICSERV);

   printf("%s %s\n", address_buffer, service_buffer);

   /* Now that we've stored the remote address, we are ready to create our socket with a call to socket().
    * We pass in fields from peer_address to create the appropiate socket type. The code for this is as 
    * follows: */
   printf("Creating socket...\n");
   SOCKET socket_peer;
   socket_peer = socket(peer_address -> ai_family, peer_address -> ai_socktype, peer_address -> ai_protocol);
   if (!ISVALIDSOCKET(socket_peer))
   {
      fprintf(stderr, "socket() failed. (%d)\n", GETSOCKETERRNO());
      return 1;
   }

   /* Once the socket has been created, we can go straight to sending data with sendto(). There is no need
    * to call connect(). Here is the code to send Hello World to our udp server: */
   const char *message = "Hello Fucking World!";
   printf("Sending: %s\n", message);

   int bytes_sent = sendto(socket_peer, message, strlen(message), 0, peer_address -> ai_addr,
         peer_address -> ai_addrlen);

   printf("Sent %d bytes.\n", bytes_sent);

   /* We'll end our program by freeing the memory for peer_address, closing the socket, cleaning up winsock, 
    * and finishing main(), as follows: */
   freeaddrinfo(peer_address);
   CLOSESOCKET(socket_peer);

#if defined(_WIN32)
   WSACleanup();
#endif

   printf("Finished.\n");

   return 0;
}
