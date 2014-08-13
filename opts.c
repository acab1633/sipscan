/*opts.c - code for dealing w/ cmd line options*/
#include <string.h>
#include "opts.h"
int opts_letter_times(opts_handle* handle, char letter)
{
  char **argv;
  char *str;
  int i;
  int count = 0;
    argv = handle->argv;

   /*loop through options*/
    for(i=0; i<handle->argc; i++)
    {
      str = *argv;
     /*is it a proper, single - option?*/
      if(('-' == str[0]) && ('-' != str[1]))
      {
        str++;
       /*check this string*/
        if(mode_long == handle->mode)
        {
          if((letter == str[0]) && ('\0' == str[1]))
            count++;
        } else {
          while('\0' != *str)
          {
            if(letter == *str)
              count++;
            str++;
          }
        }
      }
      argv++;
    }
  return(count);
}

int opts_letter_with_parameter(opts_handle* handle, char letter)
{
  int i;
  char **argv;
  char *str;
    argv = handle->argv;
    if(handle->argc < 2)
      return(-2);

   /*loop backwards through options*/
    for(i=(handle->argc - 2); i>=0; i--)
    {
      str = argv[i];
     /*is it an option?*/
      if('-' == str[0])
       /*is it our letter?*/
        if(letter == str[1])
        {
         /*is it just our letter?*/
          if(i < handle->argc)  /*if it's the last argument, can't be anything following*/
            if('\0' == str[2])
            {
              str = argv[i + 1];
             /*is the next argument an option? (eg -)*/
              if(*str != '-')
                return(i);
            }
        }
    }
  return(-1);
}

int opts_string_times(opts_handle *handle, char *str)
{
  char **argv;
  char *arg;
  int i;
  int count = 0;
    argv = handle->argv;

   /*loop through options*/
    for(i=0; i<handle->argc; i++)
    {
      arg = argv[i];
     /*is it a proper - option?*/
      if('-' == arg[0])
      {
        arg++;
       /*check this string*/
        if(0 == strcmp(arg, str))
          count++;
      }
    }
  return(count);
}

int opts_string_parameter(opts_handle *handle, char *str)
{
  int i;
  char **argv;
  char *arg;
    argv = handle->argv;
    if(handle->argc < 2)
      return(-2);

   /*loop backwards through options*/
    for(i=handle->argc - 2; i>=0; i--)
    {
      arg = argv[i];
     /*is it an option?*/
      if('-' == arg[0])
      {
        arg++;
       /*is it our string?*/
        if(0 == strcmp(arg, str))
        {
         /*is the next argument an option? (eg -)*/
          if(*(argv[i+1]) != '-')
           return(i);
        }
      }
    }
  return(-1);
}

int opts_freestanding_count(opts_handle *handle)
{
  int i=0;
  int index;
  int count = 0;
  char *arg;
  char **argv;
    argv = handle->argv;
   /*loop through parameters*/
    while(i < handle->argc)
    {
      arg = argv[i];
     /*if it's an option*/
      if('-' == *arg)
      {
        arg++;
        index = _opts_is_option_with_parameters(handle, arg);
        if(index >= 0)
          i += handle->parameters[index];
      } else {
        count++;
      }
      i++;
    }
  return(count);
}

int opts_freestanding_index(opts_handle *handle, unsigned int n)
{
  int i=0;
  int index;
  int count = 0;
  char *arg;
  char **argv;
    argv = handle->argv;
   /*loop through parameters*/
    while(i < handle->argc)
    {
      arg = argv[i];
     /*if it's an option*/
      if('-' == *arg)
      {
        arg++;
        index = _opts_is_option_with_parameters(handle, arg);
        if(index >= 0)
          i += handle->parameters[index];
      } else {
        if(count == n)
          return(i);
        count++;
      }
      i++;
    }
  return(-1);
}

int _opts_is_option_with_parameters(opts_handle* handle, char *str)
{
  int i;
    for(i=0; i < handle->n_options_with_parameters; i++)
      if(0 == strcmp(str, handle->options_with_parameters[i]))
        return(i);
  return(-1);
}
