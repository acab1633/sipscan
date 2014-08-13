/*opts.h - header for options handling*/
#ifndef _H_OPTS
#define _H_OPTS
/*constants*/
 typedef enum
 {
   mode_condensed,  /*-vhq == -v -h -q*/
   mode_long        /*-vhq != -v -h -q :- -vhq is an option in and of itself*/
 } opts_mode_t;

/*structs*/
 typedef struct
 {
   opts_mode_t mode;
   int argc;
   char **argv;  /*NOTE - all argvs should NOT include the program name (ie do a ++(*argv); --argc;  first)*/
   int n_options_with_parameters;
   char **options_with_parameters;  /*strings should not include first '-'. 1st string following these will be ignored when looking for freestanding options*/
   int *parameters;  /*n of parameters corresponding to each string in above array. 0 means none*/
 } opts_handle;

/*external functions*/
 /*NOTE - all argvs should NOT include the program name (ie do a ++(*argv); --argc;  first)*/
 /*EXPECTS - all strings passed to it (inc argv[x]) NULL terminated. Uses strstr, strcmp, etc*/
 int opts_letter_times(opts_handle* handle, char letter);  /*number of times -[letter] occurs. -n on error, 0 on char not found*/
 int opts_letter_with_parameter(opts_handle* handle, char letter);  /*returns index of last ocurrance of letter that is followed by a string. -1 on not found, other -n on error*/
 int opts_string_times(opts_handle *handle, char *str);  /*as above, but with string (for -- options, use 1 -, eg for --host, do opts_string_time(handle,"-host") )*/
 int opts_string_parameter(opts_handle *handle, char *str);  /*as above*/
 int opts_freestanding_count(opts_handle *handle);  /*number of 'freestanding' strings. Freestanding string is any that doesn't follow a string (eg "-h") listed in handle->options_with_parameters. 0 means not found. -n means error*/
 int opts_freestanding_index(opts_handle *handle, unsigned int n);  /*returns index of freestanding string n. n of 0 means first string. returns -1 on not found, other -n on error*/

/*internal functions*/
 int _opts_is_option_with_parameters(opts_handle* handle, char *str);  /*returns index if str is an option w/ parameters. -n on not exists*/
#endif
