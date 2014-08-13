/*scan.h - header file for scan.c*/
#ifndef _SCAN_H
#define _SCAN_H

/*constants/macros*/
 #define MAX_ASYNC_REQUESTS  get_MAX_ASYNC_REQESTS()
 #define RECV_TIMEOUT_RETRIES get_RECV_TIMEOUT_RETRIES()  /*number of times to retry read on timeout after sending all done*/
 #define WORKERS get_WORKERS()
 #define SEND_WAIT get_SEND_WAIT()  /*in usec*/

/*structures*/
 typedef struct
 {
   unsigned int worker_n;
   unsigned int start_port;
 } send_thread_info;

 typedef struct
 {
   volatile int *done;
   link_handle* link;
 } recv_thread_info;

 typedef enum
 {
   slow = 0,
   normal = 1,
   fast = 2,
   very_fast = 3
 } scan_speed_t;

/*functions*/
 int send_probe(link_handle* link, char* via, char* local_uri, char* remote_ip, char* remote_uri);
 int recv_loop(link_handle* link, volatile int *done);

 void *recv_thread(void* _info);
 void *send_thread(void* _info);

 void do_log(siplib_msg_handle *message);
 void print_usage();

 inline unsigned int get_MAX_ASYNC_REQESTS();
 inline unsigned int get_RECV_TIMEOUT_RETRIES();
 inline unsigned int get_WORKERS();
 inline unsigned int get_SEND_WAIT();
#endif
