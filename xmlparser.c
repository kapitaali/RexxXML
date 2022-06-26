/* parser manipulation and query for rexxxml
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
 * $Header: C:/ptjm/rexx/rexxxml/RCS/xmlparser.c 1.1 2003/06/24 22:06:11 ptjm Rel $
 */


/* this loads all the libxml and rexx headers we need */
#include "rexxxml.h"

static xmlParserCtxt ctxt;

/* get a list of features from the parser */
rxfunc(xmlgetfeaturelist)
{
   int count = 100;
   const char ** list = malloc(sizeof(*list)*count);
   int rc, len;
   register int i;

   checkparam(0, 0);

   xmlInitParserCtxt(&ctxt);

   rc = xmlGetFeaturesList(&count, list);

   if (rc == -1) {
      result_zero();
   }

   else {
      len = 1000;
      result->strptr = malloc(len);
      for (result->strlength = i = 0; i < count; i++) {
         rc = strlen(list[i]);
         while ((rc+result->strlength) > len) {
            len += 1000;
            result->strptr = realloc(result->strptr, len);
         }
         memcpy(result->strptr + result->strlength, list[i], rc);
         result->strptr[result->strlength+rc] = ';';
         result->strlength += rc + 1;
      }
   }

   return 0;
}

/* get feature's value from the parser */
rxfunc(xmlgetfeature)
{
   int rc;
   register int i;

   checkparam(1, 1);

   rc = xmlGetFeature(&ctxt, argv[0].strptr, result->strptr);

   if (rc == -1) {
      result->strlength = 0;
   }

   else
      result->strlength = strlen(result->strptr);

   return 0;
}

/* set feature's value from the parser */
rxfunc(xmlsetfeature)
{
   int rc;
   register int i;

   checkparam(2, 2);

   rc = xmlSetFeature(&ctxt, argv[0].strptr, argv[1].strptr);

   if (rc == -1) {
      result_zero();
   }

   else
      result_one();

   return 0;
}
