/*message.c - message creation part of siplib*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "siplib.h"

uint16_t siplib_msg_init()
{
  srand(time(NULL));
  return(0);
}

uint16_t siplib_msg_finish()
{
  return(0);
}

siplib_msg_handle* siplib_msg_create()
{
  siplib_msg_handle *msg;
   /*allocate memory for descriptor*/
    msg = malloc(sizeof(siplib_msg_handle));
    if(NULL == msg)
      return(NULL);

   /*fill message descriptor*/
    msg->message_size = 0;
    msg->content = malloc(SIPLIB_MAX_MESSAGE_SIZE + 1 + 1);  /*+1 since constant starts at 0, +1 since it needs a \0 at the end*/
    if(NULL == msg->content)
    {
      free(msg);
      return(NULL);
    }
    *(msg->content) = '\0';
    msg->ip_from = malloc(19);
    if(NULL == (msg->ip_from))
    {
      free(msg->content);
      free(msg);
      return(NULL);
    }
    *(msg->ip_from) = '\0';
    msg->port_from = 0;

  return(msg);
}

uint16_t siplib_msg_destroy(siplib_msg_handle* msg)
{
   /*free message text*/
    free(msg->content);

   /*free ip*/
    free(msg->ip_from);

   /*free descriptor*/
    free(msg);
  return(0);
}

uint16_t siplib_msg_method_string(siplib_msg_handle* msg, char *method_string)
{
  uint16_t field_size;
   /*make sure there's nothing in the message already*/
    if(msg->message_size > 1)
      return(2);

   /*check size will not get too big*/
    field_size = strlen(method_string) + 2;  /*+2 for \r\n*/
    if((field_size > (UINT16_MAX - (msg->message_size))) || (((msg->message_size)+field_size) > SIPLIB_MAX_MESSAGE_SIZE))  /*make sure not fooled if field size so big it can loop around, and that max message size not exceeded*/
      return(3);

   /*append method to message*/
    strcat(msg->content, method_string);
    strcat(msg->content, "\r\n");

   /*update descriptor*/
    msg->message_size  += field_size;
  return(0);
}

uint16_t siplib_msg_method_options(siplib_msg_handle* msg, char *uri)
{
  uint16_t uri_length;
  char *method_string;
  uint16_t retval;
   /*check values*/
    if(NULL == uri)
      return(4);

   /*allocate memory for and make the method string*/
    uri_length = strlen(uri);
    method_string = malloc(uri_length + 16 + 1);  /*16 extra characters to make the options request, 1 for a \0 at the end of the string*/
    if(NULL == method_string)
      return(5);

    sprintf(method_string, "OPTIONS %s SIP/2.0", uri);

   /*add it to the message*/
    retval = siplib_msg_method_string(msg, method_string);

   /*free allocated memory and return whatever siplib_msg_method_string returned*/
    free(method_string);
  return(retval);
}

uint16_t siplib_msg_add_field(siplib_msg_handle* msg, char *field)
{
  uint16_t field_size;
   /*check size will not get too big*/
    field_size = strlen(field) + 2;  /*+2 for \r\n*/
    if((field_size > (UINT16_MAX - (msg->message_size))) || (((msg->message_size)+field_size) > SIPLIB_MAX_MESSAGE_SIZE))  /*make sure not fooled if field size so big it can loop around, and that max message size not exceeded*/
      return(2);

   /*append field to message*/
    strcat(msg->content, field);
    strcat(msg->content, "\r\n");

   /*update descriptor*/
    msg->message_size  += field_size;
  return(0);
}

uint16_t siplib_msg_add_via_field(siplib_msg_handle* msg, char *host, char *branch)
{
  uint16_t host_length;
  uint16_t branch_length;
  char *field_string;
  uint16_t retval;
   /*check values*/
    if((NULL == host) || (NULL == branch))
      return(4);

   /*allocate memory for and make the field string*/
    host_length = strlen(host);
    branch_length = strlen(branch);
    field_string = malloc(17 + host_length + 8 + branch_length + 1);  /*17 for "Via: SIP/2.0/UDP " + host length + 8 for ";branch=" + branch_length + 1 for \0*/
    if(NULL == field_string)
      return(5);

    sprintf(field_string, "Via: SIP/2.0/UDP %s;branch=%s", host, branch);

   /*add it to the message*/
    retval = siplib_msg_add_field(msg, field_string);

   /*free allocated memory and return whatever siplib_msg_add_field returned*/
    free(field_string);
  return(retval);
}

uint16_t siplib_msg_add_to_field(siplib_msg_handle* msg, char *display_name, char *uri, char *tag)
{
  uint16_t display_name_length;
  uint16_t uri_length;
  uint16_t tag_length;
  char *field_string;
  char *tmp_str;
  uint16_t retval;
   /*check values*/
    if(NULL == uri)
      return(4);

   /*allocate memory for and make the field string*/
    if(NULL == display_name)
      display_name_length = 0;
    else
      display_name_length = strlen(display_name);

    uri_length = strlen(uri);

    if(NULL == tag)
      tag_length = 0;
    else
      tag_length = strlen(tag);

    field_string = malloc(4 + display_name_length + 2 + uri_length + 6 + tag_length + 1);  /*4 for "To: " + display name + 2 for " <" + uri + 6 for ">;tag=" + tag + 1 for \0*/
    if(NULL == field_string)
      return(5);
    tmp_str = field_string;  /*keep track of where field string should start*/

    sprintf(field_string, "To: ");
    field_string += 4;
    if(0 != display_name_length)
    {
      sprintf(field_string, "%s ", display_name);
      field_string += (display_name_length + 1);
    }
    sprintf(field_string, "<%s>", uri);
    field_string += (uri_length + 2);
    if(0 != tag_length)
      sprintf(field_string, ";tag=%s", tag);
    field_string = tmp_str;

   /*add it to the message*/
    retval = siplib_msg_add_field(msg, field_string);

   /*free allocated memory and return whatever siplib_msg_add_field returned*/
    free(field_string);
  return(retval);
}

uint16_t siplib_msg_add_from_field(siplib_msg_handle* msg, char *display_name, char *uri, char *tag)
{
  uint16_t display_name_length;
  uint16_t uri_length;
  uint16_t tag_length;
  char *field_string;
  char *tmp_str;
  uint16_t retval;
   /*check values*/
    if(NULL == uri)
      return(4);

   /*allocate memory for and make the field string*/
    if(NULL == display_name)
      display_name_length = 0;
    else
      display_name_length = strlen(display_name);

    uri_length = strlen(uri);

    if(NULL == tag)
      tag_length = 0;
    else
      tag_length = strlen(tag);

    field_string = malloc(6 + display_name_length + 2 + uri_length + 6 + tag_length + 1);  /*6 for "From: " + display name + 2 for " <" + uri + 6 for ">;tag=" + tag + 1 for \0*/
    if(NULL == field_string)
      return(5);
    tmp_str = field_string;  /*keep track of where field string should start*/

    sprintf(field_string, "From: ");
    field_string += 6;
    if(0 != display_name_length)
    {
      sprintf(field_string, "%s ", display_name);
      field_string += (display_name_length + 1);
    }
    sprintf(field_string, "<%s>", uri);
    field_string += (uri_length + 2);
    if(0 != tag_length)
      sprintf(field_string, ";tag=%s", tag);
    field_string = tmp_str;

   /*add it to the message*/
    retval = siplib_msg_add_field(msg, field_string);

   /*free allocated memory and return whatever siplib_msg_add_field returned*/
    free(field_string);
  return(retval);
}

uint16_t siplib_msg_add_call_id_field(siplib_msg_handle* msg, char *call_id)
{
  uint16_t call_id_length;
  char *field_string;
  uint16_t retval;
   /*check values*/
    if(NULL == call_id)
      return(4);

   /*allocate memory for and make the field string*/
    call_id_length = strlen(call_id);
    field_string = malloc(9 + call_id_length + 1);  /*9 for "Call-ID: " + call id + 1 for \0*/
    if(NULL == field_string)
      return(5);

    sprintf(field_string, "Call-ID: %s", call_id);

   /*add it to the message*/
    retval = siplib_msg_add_field(msg, field_string);

   /*free allocated memory and return whatever siplib_msg_add_field returned*/
    free(field_string);
  return(retval);
}

uint16_t siplib_msg_add_cseq_field(siplib_msg_handle* msg, uint32_t sequence_num, char *method)
{
  uint16_t method_length;
  uint16_t sequence_num_string_length;
  char sequence_num_string[11];
  char *field_string;
  uint16_t retval;
  unsigned long sequence_num_l = sequence_num;
   /*check values*/
    if(NULL == method)
      return(4);

   /*turn sequence_num into a string*/
    sprintf(sequence_num_string, "%lu", sequence_num_l);

   /*allocate memory for and make the field string*/
    sequence_num_string_length = strlen(sequence_num_string);
    method_length = strlen(method);
    field_string = malloc(6 + sequence_num_string_length + 1 + method_length + 1);  /*6 for "CSeq: " + sequence number + " " + method + 1 for \0 @ end*/
    if(NULL == field_string)
      return(5);

    sprintf(field_string, "CSeq: %s %s", sequence_num_string, method);

   /*add it to the message*/
    retval = siplib_msg_add_field(msg, field_string);

   /*free allocated memory and return whatever siplib_msg_add_field returned*/
    free(field_string);
  return(retval);
}

uint16_t siplib_msg_add_contact_field(siplib_msg_handle* msg, char *uri)
{
  uint16_t uri_length;
  char *field_string;
  uint16_t retval;
   /*check values*/
    if(NULL == uri)
      return(4);

   /*allocate memory for and make the field string*/
    uri_length = strlen(uri);
    field_string = malloc(10 + uri_length + 1 + 1);  /*10 for "Contact: <" + uri + ">" + 1 for \0 @ end*/
    if(NULL == field_string)
      return(5);

    sprintf(field_string, "Contact: <%s>", uri);

   /*add it to the message*/
    retval = siplib_msg_add_field(msg, field_string);

   /*free allocated memory and return whatever siplib_msg_add_field returned*/
    free(field_string);
  return(retval);
}

uint16_t siplib_msg_end(siplib_msg_handle* msg)
{
  return(siplib_msg_add_field(msg, ""));
}
uint16_t siplib_msg_generate_string(char *str, uint16_t length)
{
  uint16_t i;
  char current_char;

  if(NULL == str)
    return(1);

  *str = '\0'; /*initialise string*/
    for(i=0; i<length; i++)
    {
      if((rand()%5)<1) //roughly a fith of characters should be numbers
        current_char = (char)(rand()%10)+48; /*make single digit number to ascii char*/
	  else {
	    if(rand()%2 == 0) //capitals
	      current_char = (char)(rand()%26)+65;
        else //lower case
          current_char = (char)(rand()%26)+97;
  	  }
  	  strncat(str, &current_char, 1);
    }
  return(0);
}

uint16_t siplib_msg_generate_branch(char *branch, uint16_t length)
{
   /*length must be greater than 8 in order to make valid branch, branch must not be NULL*/
    if(length <= 8)
      return(2);

    if(NULL == branch)
      return(1);

   /*put initial bit into the string*/
    strcpy(branch, "z9hG4bK");

   /*fill the rest of the string with random and return*/
    branch += 7;
  return(siplib_msg_generate_string(branch, length-7));
}

uint16_t siplib_msg_sendto(siplib_msg_handle* msg, link_handle* link, char *ip, uint16_t port)
{
 /*send the message*/
  return(link_sendto(link, msg->content, msg->message_size, NULL, ip, port));
}

uint16_t siplib_msg_recv(siplib_msg_handle* msg, link_handle* link)
{
  int msg_size;
  uint16_t retval;
   /*recieve*/
    retval = link_recvfrom(link,
                           msg->content,
                           SIPLIB_MAX_MESSAGE_SIZE-3,
                           &msg_size,
                           msg->ip_from,
                           &(msg->port_from));
    if(0 != retval)
      return(retval);
    msg->message_size = msg_size;

   /*make message null-terminated for easy printing*/
    msg->content += msg->message_size;
    *(msg->content) = '\0';
    msg->content -= msg->message_size;
  return(0);
}

uint16_t siplib_msg_print(siplib_msg_handle* msg)
{
   /*print message text*/
    printf("%s\n", msg->content);
  return(0);
}

char* siplib_msg_get_content(siplib_msg_handle* msg)
{
 /*return pointer to content*/
  return(msg->content);
}

size_t siplib_msg_get_size(siplib_msg_handle* msg)
{
  /*return content size*/
  return(msg->message_size);
}

