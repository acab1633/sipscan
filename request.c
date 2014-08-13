/*request.c - request creation module of siplib*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "siplib.h"


uint16_t siplib_request_options(siplib_msg_handle* msg, char* uri, char* via_host, char* from_uri, char* contact, char* to_display_name, char* from_display_name, int max_forwards, char* branch, char* call_id, char* from_tag, char* to_tag)
{
  char branch_s[18];
  char from_tag_s[11];
  char call_id_s[16];
  char max_forwards_s[32];
   /*check parameters*/
    if((NULL==msg) || (NULL==uri) || (NULL==via_host) || (NULL==from_uri) || (NULL==contact))
      return(1);

   /*build message*/
    if(0 != siplib_msg_method_options(msg, uri))
      return(2);
    if(NULL == branch)
    {
      branch = branch_s;
      siplib_msg_generate_branch(branch, 17);
    }
    if(0 != siplib_msg_add_via_field(msg, via_host, branch))
      return(2);
    if(NULL == from_tag)
    {
      from_tag = from_tag_s;
      siplib_msg_generate_string(from_tag, 10);
    }
    if(0 != siplib_msg_add_from_field(msg, from_display_name, from_uri, from_tag))
      return(2);
    if(0 != siplib_msg_add_to_field(msg, to_display_name, uri, to_tag))
      return(2);
    if(NULL == call_id)
    {
      call_id = call_id_s;
      siplib_msg_generate_string(call_id, 15);
    }
    if(0 != siplib_msg_add_call_id_field(msg, call_id))
      return(2);
    if(0 != siplib_msg_add_cseq_field(msg, 1, "OPTIONS"))
      return(2);
    if(0 != siplib_msg_add_contact_field(msg, contact))
      return(2);
    if(max_forwards < 0)
      max_forwards = 70;
    sprintf(max_forwards_s, "Max-Forwards: %d", max_forwards);
    if(0 != siplib_msg_add_field(msg, max_forwards_s))
      return(2);
    if(0 != siplib_msg_add_field(msg, "Accept: application/sdp"))
      return(2);
    if(0 != siplib_msg_add_field(msg, "Content-Length: 0"))
      return(2);
  return(0);
}
