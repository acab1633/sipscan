/*response.h - header for the response interpretation part of siplib*/

#ifndef _H_SIPLIB_RESPONSE
#define _H_SIPLIB_RESPONSE

/*External functions*/
 int siplib_response_get_code(siplib_msg_handle* msg);  /*reads the response code of a sip response; returns: (positive) response code on success, -1 on message handle invalid, -2 on not SIP message, other negative value on failure*/
 uint16_t siplib_response_get_field(siplib_msg_handle* msg, char *field_name, uint16_t n, char* buffer, size_t buffer_size);  /*searches the message message_id for occurance n of the field field_name, and copies the contents of the field into the buffer. Buffer size of 1 = space for 1 character in the buffer; returns: 0 on success, 1 on error finding message, 2 on malloc fail (ie out-of memory), 3 on field not found, other non-zero on unknown error*/
#endif
