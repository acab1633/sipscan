/*message.h - header for message creation part of siplib*/
#ifndef _H_SIPLIB_MESSAGE
#define _H_SIPLIB_MESSAGE
/*Constants:*/
 #define SIPLIB_MESSAGE_NORMAL  0

/*Structures:*/
 typedef struct
 {
   char *content;
   size_t message_size;
   uint32_t __slack;  /*this stops the program crashing when freeing the array ip_from points to. I have NO idea why!*/
   char *ip_from;
   uint16_t port_from;
 } siplib_msg_handle;

/*External functions:*/
 uint16_t siplib_msg_init(); /*returns 0 on success, other non-zero on unknown error*/
 uint16_t siplib_msg_finish(); /*returns 0 on success, nonzero on failure*/
 siplib_msg_handle* siplib_msg_create(); /*returns 0 on success, NULL on failure*/
 uint16_t siplib_msg_destroy(siplib_msg_handle* msg); /*returns 0 on success, nonzero on failure*/

 uint16_t siplib_msg_method_string(siplib_msg_handle* msg, char *method_string);  /*method MUST be set first. returns: 0 on success, 1 on invalid or non-existant message descriptor, 2 if this wasn't called first, 3 if line is too long to fit into message*/
 uint16_t siplib_msg_method_options(siplib_msg_handle* msg, char *uri);  /*as method_string function, but also returns: 4 on bad (ie NULL) uri pointer, 5 on malloc fail (ie out of memory)*/
 uint16_t siplib_msg_add_field(siplib_msg_handle* msg, char *field); /*returns: 0 on success, 1 on invalid or non-existant message descriptor, 2 if line is too long to fit into message, 3 on malloc fail (needed internally)*/
 uint16_t siplib_msg_add_via_field(siplib_msg_handle* msg, char *host, char *branch); /*branch MUST start with "z9hG4bK"; returns: as add_field function, and 4 on bad pointers, 5 on malloc fail (ie out of memory)*/
 uint16_t siplib_msg_add_to_field(siplib_msg_handle* msg, char *display_name, char *uri, char *tag);  /*tag may be NULL, or else must be a string, with at least 32 bits of cryptographic randomness, returns as add_via_field*/
 uint16_t siplib_msg_add_from_field(siplib_msg_handle* msg, char *display_name, char *uri, char *tag);  /*tag may be NULL, or else must be a string, with at least 32 bits of cryptographic randomness. returns as add_to_field*/
 uint16_t siplib_msg_add_call_id_field(siplib_msg_handle* msg, char *call_id);  /*returns as add_via_field*/
 uint16_t siplib_msg_add_cseq_field(siplib_msg_handle* msg, uint32_t sequence_num, char *method);  /*returns as add_via_field*/
 uint16_t siplib_msg_add_contact_field(siplib_msg_handle* msg, char *uri);  /*returns as add_via_field*/
 uint16_t siplib_msg_add_content_fields(siplib_msg_handle* msg, char *content_type, size_t content_length);
 uint16_t siplib_msg_add_content(siplib_msg_handle* msg, char *content);
 uint16_t siplib_msg_end(siplib_msg_handle* msg);  /*returns as add_field*/

 uint16_t siplib_msg_generate_string(char *str, uint16_t length);  /*length of 0 = no characters generated (string is just \0); returns: 0 on success, non-zero on error*/
 uint16_t siplib_msg_generate_branch(char *branch, uint16_t length);  /*length must be greater than 8; returns: 0 on success, non-zero on error*/
 char* siplib_msg_generate_call_id(uint16_t rand_string_length, char* host);

 uint16_t siplib_msg_sendto(siplib_msg_handle* msg, link_handle* link, char *ip, uint16_t port);  /*sends messsage msg across link, to ip:port. ip of NULL and port of 0 implies both are defined for the link already; returns: 0 on success, other non-zero on unknown error*/
 uint16_t siplib_msg_recv(siplib_msg_handle* msg, link_handle* link);  /*waits for message on link stores it in msg. Puts the ip and port recieved from in the message descriptor. returns: 0 on success, 1 on link_id invalid, 2 on msg_id invalid, 3 on recieving error, 4 on memory error, other non-zero on other unknown error*/
 uint16_t siplib_msg_print(siplib_msg_handle* msg);  /*returns: 0 on success, 1 on invalid or non-existant message descriptor, 2 on malloc fail*/

 char* siplib_msg_get_content(siplib_msg_handle* msg);  /*returns: pointer to message content on success, NULL on failure*/
 size_t siplib_msg_get_size(siplib_msg_handle* msg);  /*returns: the size of the message on success, U_LONGEST_TYPE_FAIL on failure*/
#endif
