/* File transfer and http post operations for RexxXML
 *
 * The contents of this file are subject to the Mozilla Public License
 * Version 1.1 (the "License"); you may not use this file except in
 * compliance with the License. You may obtain a copy of the License at
 * http://www.mozilla.org/MPL/
 *
 * Software distributed under the License is distributed on an "AS IS"
 * basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * The Original Code is RexxXML.
 *
 * The Initial Developer of the Original Code is Patrick TJ McPhee.
 * Portions created by Patrick McPhee are Copyright © 2003
 * Patrick TJ McPhee. All Rights Reserved.
 *
 * Contributors:
 *
 * $Header: C:/ptjm/rexx/rexxxml/RCS/fetch.c 1.1 2003/10/21 20:32:41 ptjm Rel $
 */

#include <libxml/nanohttp.h>
#include <libxml/nanoftp.h>

#include "rxproto.h"
#include <ctype.h>

#define READ_BUFFER_SIZE 4096


/* result = xmlpost(url, [data], [format], [headers], [contvar])
 *  uses the http POST method to send data to url. The default format
 *  is application/x-www-form-urlencoded. The format of data depends
 *  on the program handling the post. headers can be any http headers
 *  excluding Content-Type: and Content-Length:. Each header must be
 *  followed by a new-line ('0a'x). contvar is the name of a variable
 *  which will contain the content type. */

rxfunc(xmlpost)
{
   char * headers, * type, *url;
   void * connection;
   char *rptr;
   RXSTRING data;
   int br, rsize;

   checkparam(1, 5);

   rxstrdup(url, argv[0]);

   if (argc > 1)
      data = argv[1];
   else {
      data.strptr = NULL;
      data.strlength = 0;
   }

   if (argc > 2 && argv[2].strlength > 0) {
      rxstrdup(type, argv[2]);
   }
   else {
      type = "application/x-www-form-urlencoded";
   }

   if (argc > 3 && argv[3].strlength > 0) {
      rxstrdup(headers, argv[3]);
   }
   else {
      headers = NULL;
   }

   connection = xmlNanoHTTPMethod(url, "POST", data.strptr, &type, headers,
                                  (int)data.strlength);

   if (connection) {
      rsize = READ_BUFFER_SIZE;
      result->strlength = 0;
      /* always allocate memory so I never have to test whether to free */
      result->strptr = REXXALLOCATEMEMORY(rsize);
      while ((br = xmlNanoHTTPRead(connection, result->strptr+result->strlength,
                                   rsize - result->strlength)) > 0) {
         rsize += READ_BUFFER_SIZE;
         rptr = REXXALLOCATEMEMORY(rsize);
         if (!rptr)
            return NOMEMORY;

         result->strlength += br;
         memcpy(rptr, result->strptr, result->strlength);
         REXXFREEMEMORY(result->strptr);
         result->strptr = rptr;
      }
      xmlNanoHTTPClose(connection);
   }
   else {
      result->strlength = 0;
      type = "Error";
   }

   if (argc > 4) {
      SHVBLOCK shv;
      memset(&shv, 0, sizeof(shv));
      shv.shvcode = RXSHV_SYSET;
      shv.shvnext = NULL;

      shv.shvname = argv[4];
      shv.shvvalue.strptr = type;
      shv.shvvalue.strlength = strlen(type);
      RexxVariablePool(&shv);
   }


   xmlNanoHTTPCleanup();

   return 0;
}

/* result = xmlget(url, [contvar])
 *  contvar is the name of a variable which will contain the content type
 *  after the get
 * the result is the data */

rxfunc(xmlget)
{
   char * type = NULL, *url;
   void * connection;
   char *rptr;
   int br, rsize;
   static const char http[] = "http", ftp[] = "ftp";


   checkparam(1, 2);

   rxstrdup(url, argv[0]);

   if (argv[0].strlength > sizeof(http) && !memcmp(url, http, sizeof(http)-1)) {
      connection = xmlNanoHTTPOpen(url, &type);
      if (connection) {
         rsize = READ_BUFFER_SIZE;
         result->strlength = 0;
         /* always allocate memory so I never have to test whether to free */
         result->strptr = REXXALLOCATEMEMORY(rsize);
         while ((br = xmlNanoHTTPRead(connection, result->strptr+result->strlength,
                                      rsize - result->strlength)) > 0) {
            rsize += READ_BUFFER_SIZE;
            rptr = REXXALLOCATEMEMORY(rsize);
            if (!rptr)
               return NOMEMORY;

            result->strlength += br;
            memcpy(rptr, result->strptr, result->strlength);
            REXXFREEMEMORY(result->strptr);
            result->strptr = rptr;
         }
         xmlNanoHTTPClose(connection);
         xmlNanoHTTPCleanup();
      }
      else {
         result->strlength = 0;
         type = "Error";
      }
   }
   else if (argv[0].strlength > sizeof(ftp) && !memcmp(url, ftp, sizeof(ftp)-1)) {
      connection = xmlNanoFTPOpen(url);
      if (connection) {
         rsize = READ_BUFFER_SIZE;
         result->strlength = 0;
         /* always allocate memory so I never have to test whether to free */
         result->strptr = REXXALLOCATEMEMORY(rsize);
         while ((br = xmlNanoFTPRead(connection, result->strptr+result->strlength,
                                      rsize - result->strlength)) > 0) {
            rsize += READ_BUFFER_SIZE;
            rptr = REXXALLOCATEMEMORY(rsize);
            if (!rptr)
               return NOMEMORY;

            result->strlength += br;
            memcpy(rptr, result->strptr, result->strlength);
            REXXFREEMEMORY(result->strptr);
            result->strptr = rptr;
         }
         xmlNanoFTPClose(connection);
         xmlNanoFTPCleanup();
      }
      else {
         result->strlength = 0;
         type = "Error";
      }
   }
   else {
      result->strlength = 0;
   }

   if (argc > 1) {
      SHVBLOCK shv;
      memset(&shv, 0, sizeof(shv));
      shv.shvnext = NULL;
      shv.shvname = argv[1];

      if (type) {
         shv.shvcode = RXSHV_SYSET;

         shv.shvvalue.strptr = type;
         shv.shvvalue.strlength = strlen(type);
      }
      else {
         shv.shvcode = RXSHV_DROPV;
      }
      RexxVariablePool(&shv);
   }


   xmlNanoHTTPCleanup();

   return 0;
}
