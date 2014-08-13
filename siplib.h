/*siplib.h - the main header for SipLib*/
#ifndef _H_SIPLIB_MAIN
#define _H_SIPLIB_MAIN
/*Includes*/
 #include <stdint.h>

 #include "config.h"
 #include "link.h"
 #include "message.h"
 #include "request.h"
 #include "response.h"

/*Structures*/
typedef struct
{
  int response_code;  /*eg 400,200. -n on SipLib error*/
  char *contact_s;  /*if this is null, or size is 0 -> field is not searched for or extracted*/
  uint16_t contact_s_size;
  char *from_tag_s;
  uint16_t from_tag_s_size;
  char *to_tag_s;
  uint16_t to_tag_s_size;
  char *server_s;
  uint16_t server_s_size;
  char *allow_s;
  uint16_t allow_s_size;
  char *supported_s;
  uint16_t supported_s_size;
  char *accept_s;
  uint16_t accept_s_size;
  char *accept_encoding_s;
  uint16_t accepts_encoding_size;
  char *accept_language_s;
  uint16_t accept_language_s_size;
  char *content_type_s;
  uint16_t content_type_s_size;
  size_t content_length;
  char *body;
  uint16_t body_size;
} siplib_options_response;

/*Functions*/
 uint16_t siplib_init();  /*returns: 0 on success, 1 on message module problem, 2 on link module problem, other non-zero on unknown error*/
 uint16_t siplib_finish();  /*returns: 0 on success, |1 on message_finish problem, |2 on link_finish problem, other non-zero on unknown error*/

 uint16_t siplib_get_options(siplib_options_response *options_structure, char *remote_ip, char *remote_port, char *remote_extension, char *remote_domain, char *local_ip, char *from_port, char *from_extension, char *from_domain);
#endif
