/*ip4_range.c - calculates ips in a range*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <ctype.h>
#include "ip4_range.h"

uint32_t ip4_range_count(char* start_ip, char* end_ip)
{
  uint32_t ip1, ip2;
    errno = 0;

    ip1 = _ip4_parse_ip_string(start_ip, strlen(start_ip));
    ip2 = _ip4_parse_ip_string(end_ip, strlen(end_ip));

    if(0 != errno)
      return(0);

    if(ip1>ip2)
    {
      errno = 1;
      return(0);
    }

 return(ip2 - ip1);
}

int ip4_range_get(char* start_ip, uint32_t index, char* current_ip)
{
  uint32_t ip_i;
    errno = 0;

    ip_i = _ip4_parse_ip_string(start_ip, strlen(start_ip));

    if(0 != errno)
      return(-1);

    _ip4_write_ip_string(ip_i + index, current_ip);

  return(0);
}

uint32_t _ip4_parse_ip_string(char *str, int n)
{
  int i;
  int j;
  uint32_t retval = 0;
  int tmp_i = 0;
  char num[4];  /*3 nummers + \0*/
   /*expect four numbers*/
    for(i=0; i<4; i++)
    {
     /*too many .'s?*/
      if(4 == i)
      {
        errno = 1;
        return(0);
      }

     /*reset number string*/
      num[0] = '\0';
      num[1] = '\0';
      num[2] = '\0';
      num[3] = '\0';

     /*get number str*/
      for(j=0; j<4; j++)
      {
        if('.' == *str)
        {
          if(0 == j)
          {
            errno = 1;
            return(0);
          } else {
            break;
          }
        }
        if(n<1)
        {
          if((0 == j) || (i < 3))
          {
            errno = 1;
            return(0);
          } else {  /*end of ip string*/
            break;
          }
        }
        if(!isdigit(*str))  /*not a number or a .*/
        {
          if((3==i) && (j>0))  /*we have a full ip now, must be end of string!*/
          {
            break;
          } else {
            errno = 1;
            return(0);
          }
        }
        if(3 == j)  /*too many digits!*/
        {
          errno = 1;
          return(0);
        }

        num[j] = *str;
        str++;
        n--;
      }
     /*convert str to number*/
      tmp_i = atol(num);
      if(tmp_i > 255)
      {
        errno = 1;
        return(0);
      }

     /*put it into retval*/
      retval <<= 8;
      retval += tmp_i;

     /*end of ip?*/
      if((n<1) && (3==i))
        return(retval);
      
     /*advance string*/
      str++;
      n--;

     /*end of string/ip?*/
      if(((!isdigit(*str)) || (n<0))  && (3==i))
        return(retval);
    }
  errno = 1;
  return(0);
}

int _ip4_write_ip_string(uint32_t ip, char *str)
{
  int nums[4];
  int i;
    for(i=0; i<4; i++)
      nums[3 - i] = (ip>>(i*8)) & 0xff;
    sprintf(str, "%d.%d.%d.%d", nums[0], nums[1], nums[2], nums[3]);
  return(0);
}
