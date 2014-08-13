/*request.h - header for the request creation module of siplib*/

#ifndef _H_SIPLIB_REQUEST
#define _H_SIPLIB_REQUEST

/*External functions*/
 uint16_t siplib_request_options(siplib_msg_handle* msg, char* uri, char* via_host, char* from_uri, char* contact, char* to_display_name, char* from_display_name, int max_forwards, char* branch, char* call_id, char* from_tag, char* to_tag);  /*assembles an options request in msg. The uri, via, from_uri, and contact fields are mandatory - the other's may be set to NULL (or -1 in the case of max_forwards), in which case they will be generated/excluded as necessary; returns: 0 on success, 1 on invalid parameters, 2 on message-building fail, other non-zero on unknown error*/
#endif
