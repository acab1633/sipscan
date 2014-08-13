/*scan.c - scans an ip range for sip servers using OPTIONS requests*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <ctype.h>
#include <pthread.h>

#include "siplib.h"
#include "opts.h"
#include "ip4_range.h"
#include "scan.h"
  volatile int *done_all;
  int verboseness = 0;
  scan_speed_t scan_speed = normal;
  int output_port = 0;
  char *filename = NULL;
  FILE *f;
  unsigned int remote_port = 5060;
  char* my_ip;
  char* start_ip;
  char* end_ip;

void print_usage()
{
  printf("Usage: optscan [options] your_ip start_ip end_ip\n");
  printf("\n");
  printf("Scans for sip servers by performing 'options' requests on a range of ips. ");
  printf("Outputs the ip's of any servers that respond to stdout & the logfile. ");
  printf("Also outputs the server name given on -v, and additionally the response code on -vv.");
  printf("\n\n");
  printf("[-v[v]] - verbose output\n");
  printf("[--output-port] - causes port to be outputted alongside ip addresses\n");
  printf("[-l logfile] - log output to file\n");
  printf("[-p remote_port] - remote port to send requests to\n");
  printf("[-s speed(number] - scanning speed, 0=slow, 1=normal, 2=fast, 3=very fast. Default is 1. Faster scans mean greater chance of false negatives.\n");
}

int main(int argc, char **argv)
{
  opts_handle opts;
  char* opts_with_parameters[] =
  {
    [0] = "l",
    [1] = "p",
    [2] = "s",
  };
  int parameters[] = {1,1,1};
  char* tmp_str;
  int retval;
  int i;
  send_thread_info *send_info;
  pthread_t *worker_thread;
    srand(time(NULL));

   /*initialise siplib*/
    if(0 != (retval = siplib_init()))
    {
      printf("couldn't start siplib (code: %d)\n", retval);
      return(1);
    }

   /*process arguments*/
    opts.mode = mode_condensed;
    opts.argc = argc - 1;
    opts.argv = argv + 1;
    opts.n_options_with_parameters = 3;
    opts.options_with_parameters = opts_with_parameters;
    opts.parameters = parameters;

    if(3 != opts_freestanding_count(&opts))
    {
      print_usage();
      siplib_finish();
      return(EXIT_FAILURE);
    }

    verboseness = opts_letter_times(&opts, 'v');
    if(opts_string_times(&opts, "-output-port") > 0)
      output_port = 1;
    my_ip = argv[opts_freestanding_index(&opts, 0) + 1];
    start_ip = argv[opts_freestanding_index(&opts, 1) + 1];
    end_ip = argv[opts_freestanding_index(&opts, 2) + 1];

    i = opts_letter_with_parameter(&opts, 'p');
    if(i >= 0)
    {
      tmp_str = argv[i + 2];
     /*check it is really a number*/
      while('\0' != *tmp_str)
      {
        if(!isdigit(*tmp_str))
        {
          printf("ERROR! bad port number\n");
          siplib_finish();
          return(EXIT_FAILURE);
        }
        tmp_str++;
      }
     /*extract number*/
      if((atol(argv[i + 2]) > 65535) || (atol(argv[i + 2]) < 1))
      {
        printf("ERROR! bad port number\n");
        siplib_finish();
        return(EXIT_FAILURE);
      }
      remote_port = atol(argv[i + 2]);
    }

    i = opts_letter_with_parameter(&opts, 'l');
    if(i >= 0)
      filename = argv[i + 2];

    i = opts_letter_with_parameter(&opts, 's');
    if(i >= 0)
    {
      tmp_str = argv[i + 2];
      while('\0' != *tmp_str)
      {
        if(!isdigit(*tmp_str))
        {
          printf("ERROR! bad speed value\n");
          print_usage();
          siplib_finish();
          return(EXIT_FAILURE);
        }
        tmp_str++;
      }
      retval = atoi(argv[i + 2]);
      if((retval < 0) || (retval > 3))
      {
        printf("ERROR! bad speed value\n");
        print_usage();
        siplib_finish();
        return(EXIT_FAILURE);
      } else {
        scan_speed = retval;
      }
    }

   /*check ips valid*/
    errno = 0;
    _ip4_parse_ip_string(my_ip, strlen(my_ip));
    _ip4_parse_ip_string(start_ip, strlen(start_ip));
    _ip4_parse_ip_string(end_ip, strlen(end_ip));
    if(errno != 0)
    {
      printf("ERROR! bad ip specified\n\n");
      print_usage();
      siplib_finish();
      return(EXIT_FAILURE);
    }

   /*open logfile*/
    if(NULL != filename)
    {
      f = fopen(filename, "w");
      if(f == NULL)
      {
        perror("Error opening file!");
        siplib_finish();
        return(EXIT_FAILURE);
      }
    }

   /*allocate memory*/
    done_all = malloc(sizeof(int) * WORKERS);
    if(NULL == done_all)
    {
      perror("");
      return(EXIT_FAILURE);
    }

   /*start workers*/
    send_info = malloc(sizeof(send_thread_info) * WORKERS);
    worker_thread = malloc(sizeof(pthread_t) * WORKERS);
    for(i=0; i<WORKERS; i++)
    {
      send_info->worker_n = i;
      send_info->start_port = 20005 + (i*100);
      pthread_create(worker_thread, NULL, send_thread, send_info);
      worker_thread++;
      send_info++;
      usleep(rand()%1000000);  /*so requests overlap a bit less*/
    }
    worker_thread -= WORKERS;
    send_info -= WORKERS;

   /*wait for workers to be done*/
    for(i=0; i<WORKERS; i++)
    {
      if(pthread_join(*worker_thread, NULL))
        printf("Error joining thread\n");
      worker_thread++;
    }
    worker_thread -= WORKERS;

   /*free stuff*/
    free(send_info);
    free(worker_thread);
    free((void*)done_all);

   /*close file*/
    if(NULL != filename)
      fclose(f);

   /*run siplib tidy-up, return error on fail*/
    if(0 != (retval=siplib_finish()))
    {
      printf("siplib_finish FAIL (code %d)\n", retval);
      return(-2);
    }

  return(0);
}
void *send_thread(void* _info)
{
  send_thread_info *info = _info;
  recv_thread_info recv_info;
  link_handle* link_1;
  unsigned int myport = info->start_port;
  volatile int *done = &(done_all[info->worker_n]);
  *done = 0;
  char via[128];
  char myuri[128];
  char current_host[128];
  char current_uri[256];
  pthread_t listen_thread;
  int i;
   /*calculate our uri & stuff*/
    sprintf(via, "%s:%u", my_ip, myport);
    sprintf(myuri, "sip:test@%s:%u", my_ip, myport);

   /*create link*/
    if(NULL == (link_1=link_udp_create(NULL, myport, NULL, 0)))
    {
      printf("could not create link :-(\n");
      siplib_finish();
      return(NULL);
    }

   /*start recv thread*/
    recv_info.link = link_1;
    recv_info.done = done;
    pthread_create(&listen_thread, NULL, recv_thread, &recv_info);

   /*send loop*/
    for(i=info->worker_n; i<=ip4_range_count(start_ip, end_ip); i+=WORKERS)
    {
     /*construct uri and host*/
      ip4_range_get(start_ip, i, current_host);
      sprintf(current_uri, "sip:s@%s", current_host);

     /*send teh proeb!*/
      send_probe(link_1, via, myuri, current_host, current_uri);

     /*possibly wait for listen thread to catch up*/
      if((0 == (i/WORKERS)%MAX_ASYNC_REQUESTS) && (i != info->worker_n))  /*too many reqests at a time and things get missed*/
      {
       /*wait for listen thread to be done*/
        *done = 1;
        if(pthread_join(listen_thread, NULL))
        {
          printf("Error joining thread\n");
        }
       /*destroy old link*/
        link_udp_destroy(link_1);  /*no point checking for errors*/

       /*change port*/
        myport += 2;
        if(myport > (info->start_port + 99))
          myport = info->start_port + 1;
        sprintf(via, "%s:%u", my_ip, myport);
        sprintf(myuri, "sip:test@%s:%u", my_ip, myport);
       /*re-create link*/
        if(NULL == (link_1=link_udp_create(NULL, myport, NULL, 0)))
        {
         /*on fail, try 1 more time with different port*/
          myport++;
          sprintf(via, "%s:%u", my_ip, myport);
          sprintf(myuri, "sip:test@%s:%u", my_ip, myport);
          if(NULL == (link_1=link_udp_create(NULL, myport, NULL, 0)))
          {
            printf("could not create link :-(\n");
            siplib_finish();
            return(NULL);
          }
        }
       /*restart listen thread*/
        recv_info.link = link_1;
        *done = 0;
        pthread_create(&listen_thread, NULL, recv_thread, &recv_info);
      }
     /*wait a bit - slows send rate, hopefully less packets get lost*/
      usleep(rand()%SEND_WAIT);  /*so requests overlap a bit less*/
    }

    *done = 1;

   /*wait for thread to be done*/  
    if(pthread_join(listen_thread, NULL))
    {
      printf("Error joining thread\n");
    }

   /*close link*/
    link_udp_destroy(link_1);  /*no point checking for errors*/
  return(NULL);
}

void *recv_thread(void* _info)
{
  recv_thread_info *info;
    info = _info;
    recv_loop(info->link, info->done);
  return(NULL);
}

int send_probe(link_handle* link, char* via, char* local_uri, char* remote_ip, char* remote_uri)
{
  siplib_msg_handle* message_out;
   /*construct sip request*/
    message_out = siplib_msg_create();
    if(NULL == message_out)
    {
      printf("could not create message_out! :-(\n");
      siplib_finish();
      return(1);
    }
    
    siplib_request_options(message_out,
          remote_uri,
          via,
          local_uri,
          local_uri,
          NULL, NULL,
          60,
          NULL, NULL, NULL, NULL);

   /*send*/
    if( 0!= siplib_msg_sendto(message_out, link, remote_ip, remote_port) )
    {
      perror("err -");
      return(2);
    }

  siplib_msg_destroy(message_out);
  return(0);
}

int recv_loop(link_handle* link, volatile int *done)
{
  siplib_msg_handle* message_in;
  int am_done = 0;
    while(1)
    {
   /*create message holder*/
    message_in = siplib_msg_create();
    if(NULL == message_in)
    {
      printf("could not create message_in! :-(\n");
      siplib_finish();
      return(1);
    }
     /*recieve*/
      if(0 != siplib_msg_recv(message_in, link))
      {
        if((EAGAIN == errno) || (EWOULDBLOCK == errno))
        {
          if(1 == *done)
          {
            if(RECV_TIMEOUT_RETRIES != am_done)
            {
              am_done++;
            } else {
              return(0);
            }
          //} else {
          //  printf(".\n");
          }
        } else {
          perror("could not get udp message -");
        }
      } else {
        do_log(message_in);
      }

     /*destroy message holder*/
      siplib_msg_destroy(message_in);
    }
}

void do_log(siplib_msg_handle *message)
{
  char _log_str[256];  /*easily enough for ip + port + response code + server name*/
  char* log_str = _log_str;  /*hack so we can treat array like a pointer (no REAL difference, you see? Just stops warnings)*/
  int log_str_size=256;;
  int response_code;
  char field_in[32];
  int retval;

   /*log ip*/
    snprintf(log_str, log_str_size, "%s", message->ip_from);
    log_str_size -= strlen(log_str);
    log_str += strlen(log_str);

   /*log port*/
    if(0 != output_port)
    {
      snprintf(log_str, log_str_size, ":%d", remote_port);
      log_str_size -= strlen(log_str);
      log_str += strlen(log_str);
    }

   /*log response code*/
    if(verboseness >= 2)
    {
      response_code = siplib_response_get_code(message);
      snprintf(log_str, log_str_size, " %d", response_code);
      log_str_size -= strlen(log_str);
      log_str += strlen(log_str);
    }

   /*log server name*/
    if(verboseness >= 1)
    {
      if(0 != (retval=siplib_response_get_field(message, "Server", 0, field_in, 32)))
      {
        snprintf(log_str, log_str_size, " -");
        log_str_size -= strlen(log_str);
        log_str += strlen(log_str);
      } else {
        snprintf(log_str, log_str_size, " %s", field_in);
        log_str_size -= strlen(log_str);
        log_str += strlen(log_str);
      }
    }

   /*finish up log string*/
    snprintf(log_str, log_str_size, "\n");
    log_str -= (256 - log_str_size);

   /*output log*/
    printf("%s", log_str);
    if(NULL != filename)
      fprintf(f, "%s", log_str);
  return;
}

inline unsigned int get_MAX_ASYNC_REQESTS()
{
  if(very_fast == scan_speed)
    return(128);
  if(fast == scan_speed)
    return(96);
  if(normal == scan_speed)
    return(64);
  if(slow == scan_speed)
    return(32);
  return(64);
}

inline unsigned int get_RECV_TIMEOUT_RETRIES()
{
  if(very_fast == scan_speed)
    return(9);
  if(fast == scan_speed)
    return(9);
  if(normal == scan_speed)
    return(4);
  if(slow == scan_speed)
    return(9);
  return(4);
}

inline unsigned int get_WORKERS()
{
  if(very_fast == scan_speed)
    return(16);
  if(fast == scan_speed)
    return(16);
  if(normal == scan_speed)
    return(4);
  if(slow == scan_speed)
    return(1);
  return(4);
}

inline unsigned int get_SEND_WAIT()
{
  if(very_fast == scan_speed)
    return(40000);
  if(fast == scan_speed)
    return(60000);
  if(normal == scan_speed)
    return(100000);
  if(slow == scan_speed)
    return(100000);
  return(100000);
}
