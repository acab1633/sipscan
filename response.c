/*response.c - response interpretation module of siplib*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "siplib.h"


int siplib_response_get_code(siplib_msg_handle* msg)
{
  char *message_content;
  size_t message_size;
  char number_string[4];
  uint32_t i=0;
  long retval;
   /*get and validate message descriptor*/
    message_content = siplib_msg_get_content(msg);
    message_size = siplib_msg_get_size(msg);
    if((NULL == message_content))
      return(-1);

   /*check "SIP/2.0" present*/
    if(0 != strncmp(message_content, "SIP/2.0", 7))
      return(-2);

   /*get response code*/
    while(!isspace(*message_content) && (i<message_size))  /*advance to space*/
      {message_content++; i++;}
    while(isspace(*message_content) && (i<message_size))  /*advance past first space*/
      {message_content++; i++;}
    if(i >= message_size)  /*wtf?*/
      return(-100);
    strncpy(number_string, message_content, 3);
    number_string[3] = '\0';

   /*convert to int*/
    errno = 0;
    retval = strtol(number_string, NULL, 0);
    if(0 != errno)
      return(-101);
  return(retval);
}

uint16_t siplib_response_get_field(siplib_msg_handle* msg, char *field_name, uint16_t n, char* buffer, size_t buffer_size)
{
  char *message_content;
  //size_t message_size;
  char *search_string;
  char *tmp_string;
  uint16_t i;
   /*get the size and content of message*/
    message_content = siplib_msg_get_content(msg);
    //message_size = siplib_msg_get_size(msg);
    if(NULL == message_content)
      return(1);

   /*create search string*/
    search_string = malloc(strlen(field_name) + 2);
    if(NULL == search_string)
      return(2);
    sprintf(search_string, "%s:", field_name);

   /*find instance n of field*/
    i = 0;
    while(i<=n)
    {
      tmp_string = message_content;
      message_content = strstr(message_content, search_string);
      if((NULL == message_content) || (tmp_string == message_content))  /*not found, or starts w/ "SIP/2.0"*/
      {
        free(search_string);
  return(3);
      }
      message_content--;
      if((*message_content == '\n') || (*message_content == '\r'))  /*so long as we're looking at a field*/
      {
        message_content++;
        message_content+=strlen(search_string);  /*advance to end of field name*/
        if(i!=n)
          i++;
        else {
         /*advance past any spaces*/
          while((*message_content!='\0') && (isblank(*message_content)))
            message_content++;
         /*copy string*/
          i = 0;
          while((*message_content!='\0')
                 && (*message_content!='\n')
                 && (*message_content!='\r')
                 && (i<(buffer_size-1)))
          {
            *buffer = *message_content;
            buffer++;
            message_content++;
            i++;
          }
          *buffer = '\0';
          free(search_string);
  return(0);
        }
      }else{
        message_content++;
        message_content+=strlen(search_string);
      }
    }
  return(3);
}
