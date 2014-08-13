/*link.h - header for network link management part of siplib*/

#ifndef _H_SIPLIB_LINK
#define _H_SIPLIB_LINK
 #include <errno.h>
 #ifdef WIN32              /*Windows*/
   #include "StdAfx.h"
   #include <windows.h>
   #include <winsock2.h>
   #include <winsock.h>
 #else                     /*Unix*/
   #include <unistd.h>
   #include <sys/types.h>
   #include <netinet/in.h>
   #include <netinet/tcp.h>
   #include <sys/socket.h>
   #include <sys/select.h>
   #include <sys/time.h>
   #include <arpa/inet.h>
   #include <netdb.h>
 #endif

/*Constants*/
 #define RECV_TIMEOUT   1

 #define SIPLIB_LINK_TYPE_UDP_OUT       1   /*link will send messages to a specific host (ie acting as a client)*/
 #define SIPLIB_LINK_TYPE_UDP_LISTEN    2   /*link used to recieve messages on a specified port from many hosts (ie server)*/

 #ifdef WIN32
   #define socket_t SOCKET
   #define CLOSESOCKET(x)  closesocket(x)
   #ifndef socklen_t
     #define socklen_t  int
   #endif
 #else
   #define socket_t int
   #define INVALID_SOCKET  -1
   #define SOCKET_ERROR  -1
   #define SOCKADDR struct sockaddr
   #define CLOSESOCKET(x)  close(x)
 #endif

/*Structures*/
 typedef struct
 {
   char *ip_local;
   uint16_t port_local;
   char *ip_remote;
   uint16_t port_remote;
   socket_t sock;
   struct sockaddr_in info_local;
   struct sockaddr_in info_remote;
   uint16_t type;
 } link_handle;

/*External functions:*/
 uint16_t link_init();  /*initialises sockets (eg winsock). returns: 0 on success, other non-zero on unknown error*/
 uint16_t link_finish();  /*closes winsock; returns: 0 on success, other non-zero on unknown error*/

 uint16_t link_host_to_ip(char* host, char* ip);

 link_handle* link_udp_create(char* ip_local, uint16_t port_local, char * ip_remote, uint16_t port_remote);  /*creates a link. port_local of 0 == system chooses for us; ip_local of NULL == all; port_remote of 0 and ip_remote of NULL == listening; returns pointer to new link handle on success, NULL on failure*/
 uint16_t link_udp_destroy(link_handle* link);  /*returns: 0 on success, other non-zero on unknown error*/

 uint16_t link_sendto(link_handle* link, void* buffer, size_t length, int *n, char *ip_remote, uint16_t port_remote);  /*sends data of ammount length in buffer across link link. Ammount of data sent is stored in n (if n is not a NULL pointer). ip_remote of NULL will send to whatever IP was specified in create call - so long as one was specified there; returns: 0 on success, 1 on link handle invalid, 2 on need ip_remote or port_remote to be specified, 3 on sending error, 4 on memory error, other non-zero on unknown error*/
 uint16_t link_recvfrom(link_handle* link, void* buffer, size_t size, int *n, char *ip, uint16_t *port);  /*waits until data appears on link link, then recieves data on that link. Ammount of data sent is stored in n (if n is not a NULL pointer). The ip and port recieved from will be copied to *ip and *port. ip should have room for at least 16 characters in an ip4 environment, and 46 in an ip6 environment. Both ip and port may be NULL pointers, in which case no data is copied; returns: 0 on success, 1 on link descriptor invalid, 2 on need ip_remote or port_remote to be specified, 3 on recieving error, 4 on memory error, other non-zero on unknown error*/
#endif
