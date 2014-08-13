/*range.h - header file for working out ip range things*/
#ifndef _H_IP4_RANGE
#define _H_IP4_RANGE

 uint32_t ip4_range_count(char* start_ip, char* end_ip);
 int ip4_range_get(char* start_ip, uint32_t index, char* current_ip);

 uint32_t _ip4_parse_ip_string(char *str, int n);
 int _ip4_write_ip_string(uint32_t ip, char *str);
#endif
