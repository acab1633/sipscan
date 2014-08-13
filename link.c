/*link.c - network link management module of siplib*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "siplib.h"

uint16_t link_init()
{
 #ifdef WIN32
  WSADATA wsdata; WORD wsver=MAKEWORD(2, 0);

   /*initialise winsock*/
    if(WSAStartup(wsver, &wsdata) != 0)
    {
      WSACleanup();
      return(100);  /*if couldn't initialise winsock - return error*/
    }
 #endif
  return(0);
}

uint16_t link_finish()
{
 #ifdef WIN32
   /*shut down winsock*/
    WSACleanup();
 #endif
  return(0);
}

link_handle* link_udp_create(char* ip_local, uint16_t port_local, char * ip_remote, uint16_t port_remote)
{
  link_handle *link;
  socklen_t address_size;
  struct timeval tv;
   /*allocate memory for descriptor*/
    link = malloc(sizeof(link_handle));
    //printf("%lu \n", sizeof(link_handle));
    if(NULL == link)
    {
      return(NULL);
    }

   /*fill in values in descriptor*/
    link->ip_local = ip_local;
    link->port_local = port_local;
    link->ip_remote = ip_remote;
    link->port_remote = port_remote;
    if((NULL == ip_remote) && (0 == port_remote))  /*select correct link type*/
      link->type = SIPLIB_LINK_TYPE_UDP_LISTEN;
    else
      link->type = SIPLIB_LINK_TYPE_UDP_OUT;
    bzero(&(link->info_local), sizeof(link->info_local));
    bzero(&(link->info_remote), sizeof(link->info_remote));

   /*create socket*/
    link->sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    if(INVALID_SOCKET == link->sock)
    {
      free(link);
      return(NULL);
    }

    tv.tv_sec = RECV_TIMEOUT;
    tv.tv_usec = 0;
    setsockopt(link->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

    link->info_local.sin_family = AF_INET;
    link->info_local.sin_port = htons(port_local);  /*value of 0 means system assigns value!*/
    if(NULL != ip_local)
    {
#ifdef WIN32
      link->info_local.sin_addr.s_addr = inet_addr(ip_local);
#else
      if(0 == inet_aton(ip_local, &(link->info_local.sin_addr)))
      {
        perror("badness");
        return(NULL);
      }
#endif
    } else {
      link->info_local.sin_addr.s_addr = htonl(INADDR_ANY);
    }

    link->info_remote.sin_family = AF_INET;
    if(0 != port_remote)
      link->info_remote.sin_port = htons(port_remote);
    if(NULL != ip_remote)
    {
#ifdef WIN32
      link->info_remote.sin_addr.s_addr = htonl(inet_addr(ip_remote));
#else
      if(0==inet_aton(ip_remote, &(link->info_remote.sin_addr)))
      {
        perror("badness");
        return(NULL);
      }
#endif
    }

   /*bind, then listen or connect*/
    if(SOCKET_ERROR ==  bind(link->sock,
                             (SOCKADDR *)&(link->info_local),
                             sizeof(link->info_local)))
    {
      CLOSESOCKET(link->sock);
      free(link);
      return(NULL);
    }

    if(SIPLIB_LINK_TYPE_UDP_OUT == link->type)
    {
      if(SOCKET_ERROR == connect(link->sock,
                                 (SOCKADDR *)&(link->info_remote),
                                 sizeof(struct sockaddr_in)))
      {
        perror("");
        CLOSESOCKET(link->sock);
        free(link);
        return(NULL);
      }
    } 

   /*find out what port we're using, if necessary...*/
    if(0 == port_local)
    {
      address_size = sizeof(link->info_local);
      if(SOCKET_ERROR == getsockname(link->sock, (struct sockaddr *)&(link->info_local), &address_size))
        return(NULL);  /*this really shouldn't happen - so not doing any proper error message*/ 
      link->port_local = ntohs(link->info_local.sin_port);
    }
  return(link);
}

uint16_t link_udp_destroy(link_handle* link)
{
   /*close socket*/
    if(SOCKET_ERROR == CLOSESOCKET(link->sock))
      return(101);

   /*free descriptor*/
    free(link);
  return(0);
}

uint16_t link_sendto(link_handle* link, void* buffer, size_t length, int *n, char *ip_remote, uint16_t port_remote)
{
  struct sockaddr_in *info_remote;
  int retval;
   /*sort info_remote structure*/
    if(NULL == (link->ip_remote))
    {
      if((NULL == ip_remote) || (0 == port_remote))  /*if no ip/port specified in descriptor or function call*/
        return(2);
      info_remote = malloc(sizeof(struct sockaddr_in));
      if(NULL == info_remote)
        return(4);
      bzero(info_remote, sizeof(info_remote));
      info_remote->sin_family = AF_INET;
      info_remote->sin_port = htons(port_remote);
#ifdef WIN32
      info_remote->sin_addr.s_addr = htonl(inet_addr(ip_remote));
#else
      inet_aton(ip_remote, &(info_remote->sin_addr));
#endif
    } else {
      info_remote = &(link->info_remote);
    }

   /*send the datas*/
    retval = sendto(link->sock, buffer, length, 0, (SOCKADDR *)info_remote, sizeof(struct sockaddr_in));

   /*free alloc'ed memory*/
    if(NULL == (link->ip_remote))
      free(info_remote);

   /*figure out what to return*/
    if(SOCKET_ERROR == retval)
      return(3);
    if(NULL != n)
      *n = retval;

  return(0);
}

uint16_t link_recvfrom(link_handle* link, void* buffer, size_t size, int *n, char *ip, uint16_t *port)
{
  char *tmp_str;
  struct sockaddr_in info_remote;
  socklen_t info_remote_size;
  int retval;
   /*recieve the datas*/
    info_remote_size = sizeof(info_remote);
    retval = recvfrom(link->sock, buffer, size, 0, (struct sockaddr*)&info_remote, &info_remote_size);
    if(SOCKET_ERROR == retval)
      return(3);

   /*extract bytes sent, ip, and port info if wanted*/
    if(NULL != n)
      *n = retval;

    if(NULL != ip)
    {
      tmp_str = inet_ntoa(info_remote.sin_addr);
      if(NULL != tmp_str)
        strcpy(ip, tmp_str);
    }
    if(NULL != port)
      *port = ntohs(info_remote.sin_port);
  return(0);
}
