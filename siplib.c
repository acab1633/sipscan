/* siplib.c - contains various SipLib functions, including init,
 *           options requests
 */

#include <stdio.h>

#include "siplib.h"

uint16_t siplib_init()
{
   /*initialise modules*/
    if(0 != link_init())
    {
      return(1);
    }
    if(0 != siplib_msg_init())
    {
      link_finish();
      return(2);
    }
  return(0);
}

uint16_t siplib_finish()
{
  uint16_t retval = 0;
   /*close down modules*/
    if(0 != siplib_msg_finish())
      retval += 1;
    if(0 != link_finish())
      retval += 2;
  return(retval);
}

uint16_t siplib_get_options(siplib_options_response *options_structure,
          char *remote_ip,
          char *remote_port,
          char *remote_extension,
          char *remote_domain,
          char *local_ip,
          char *from_port,
          char *from_extension,
          char *from_domain)
{
   /*create link & message*/

   /*construct & send request*/
   /*get response - re-send if 1xx, not on 3xx...*/
   /*extract fields*/
  return(0);
}
