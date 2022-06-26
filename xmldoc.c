/* load either an HTML file or an XML file into a rexx stem,
 * or vice-versa.
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
 * $Header: C:/ptjm/rexx/rexxxml/RCS/xmldoc.c 1.11 2003/10/21 17:24:54 ptjm Rel $
 */


/* this loads all the libxml and rexx headers we need */
#include "rexxxml.h"

#include <libxml/HTMLtree.h>
#include <libxml/globals.h>

#include <libxslt/xsltutils.h>

/* given an XML document pointer, save it to a file. Returns 0
 * if anything went wrong, otherwise the size of the file.
 * In the absence of a URL, return the string that would go in the file
 *  size = xmlSaveDoc(url, doc [, stylesheet])
 *  string = xmlSaveDoc(, doc [, stylesheet])
 */
rxfunc(xmlsavedoc)
{
   xmlDocPtr doc;
   xsltStylesheetPtr ssp;
   char * s;
   int bw;

   checkparam(2, 3);

   if (argv[1].strlength != sizeof(doc)) {
      return BADARGS;
   }

   rxstring_to_bin(doc, argv+1);

   /* libxslt provides a helper which saves things correctly. libxml's
    * functions don't do the right thing when confronted with the text
    * output method. Plus there might be other output methods in the
    * future, for all we know */
   if (argc == 2) {

      if (argv[0].strptr) {
         rxstrdup(s, argv[0]);

         if (doc->type == XML_DOCUMENT_NODE) {
            bw = xmlSaveFile(s, doc);
         }
         else if (doc->type == XML_HTML_DOCUMENT_NODE) {
            bw = htmlSaveFile(s, doc);
         }
         else {
            bw = 0;
         }

         result->strlength = sprintf(result->strptr, "%d", bw);
      }

      /* no URL, so write to result */
      else {
         if (doc->type == XML_DOCUMENT_NODE) {
            xmlDocDumpMemory(doc, (xmlChar **)&s, &bw);
         }
         else if (doc->type == XML_HTML_DOCUMENT_NODE) {
            htmlDocDumpMemory(doc,(xmlChar **) &s, &bw);
         }

         if (s == NULL) {
            result_zero();
         }
         else {
            rxresize(result, bw);
            memcpy(result->strptr, s, bw);
            xmlFree(s);
         }
      }
   }
   else {
      rxstring_to_bin(ssp, argv+2);

      if (argv[0].strptr) {
         rxstrdup(s, argv[0]);

         bw = xsltSaveResultToFilename(s, doc, ssp, 0);
         result->strlength = sprintf(result->strptr, "%d", bw);
      }

      else {
         xsltSaveResultToString((xmlChar **)&s, &bw, doc, ssp);

         if (s == NULL) {
            result_zero();
         }
         else {
            rxresize(result, bw);
            memcpy(result->strptr, s, bw);
            xmlFree(s);
         }
      }
   }

   return 0;
}

static RXSTRING here; 

/* given xml data, load it into a tree. We return a pointer to the tree,
 * which can be expanded with the expand node function
 *   doc = xmlParseXML([url], [inline], [flags])*/
rxfunc(xmlparsexml)
{
   xmlnodeptr_t doc;
   char * url;
   int oldVal, oldLoad, oldSpace;

   checkparam(0, 3);

   if (argc > 2) {
      register int i;

      for (i = 0; i < argv[2].strlength; i++) {
         switch (toupper(argv[2].strptr[i])) {
            case 'V':
               oldVal = xmlDoValidityCheckingDefaultValue;
               xmlDoValidityCheckingDefaultValue = 1;
               break;
            case 'D':
               oldLoad = xmlLoadExtDtdDefaultValue;
               xmlLoadExtDtdDefaultValue = 1;
               break;
            case 'S':
               oldSpace = xmlKeepBlanksDefaultValue;
               xmlKeepBlanksDefaultValue = 0;
               break;
         }
      }
   }

   /* if the first argument is set, open the document from the URL.
    * Otherwise, if the second is set parse the argument, otherwise see if
    * something's been set through the XML environment. */
   if (argc > 0 && argv[0].strptr) {
      rxstrdup(url, argv[0]);
      doc.doc = xmlParseFile(url);
   }
   else if (argc > 1 && argv[1].strptr) {
      doc.doc = xmlParseMemory(argv[1].strptr, argv[1].strlength);
   }
   else if (here.strlength) {
      doc.doc = xmlParseMemory(here.strptr, here.strlength);

      /* don't free here.strptr on the theory it's faster to just realloc
       * next time */
      here.strlength = 0;
   }
   else {
      doc.doc = NULL;
   }

   if (argc > 2) {
      register int i;

      for (i = 0; i < argv[2].strlength; i++) {
         switch (toupper(argv[2].strptr[i])) {
            case 'V':
               xmlDoValidityCheckingDefaultValue = oldVal;
               break;
            case 'D':
               xmlLoadExtDtdDefaultValue = oldLoad;
               break;
            case 'S':
               xmlKeepBlanksDefaultValue = oldSpace;
               break;
         }
      }
   }

   if (doc.doc == NULL) {
      result_zero();
   }
   else {
      bin_to_rxstring(result, doc.doc);
   }

   return 0;
}

/* same as xmlparsexml, but for an html file -- we also have an encoding string
 *  doc = xmlParseHTML([url], [inline], [flags], [encoding]) */
rxfunc(xmlparsehtml)
{
   xmlnodeptr_t doc;
   char * url, * encoding = "ISO-8859-1";
   int oldVal, oldLoad, oldSpace;

   checkparam(0, 4);

   if (argc > 2) {
      register int i;

      for (i = 0; i < argv[2].strlength; i++) {
         switch (toupper(argv[2].strptr[i])) {
            case 'V':
               oldVal = xmlDoValidityCheckingDefaultValue;
               xmlDoValidityCheckingDefaultValue = 1;
               break;
            case 'D':
               oldLoad = xmlLoadExtDtdDefaultValue;
               xmlLoadExtDtdDefaultValue = 1;
               break;
            case 'S':
               oldSpace = xmlKeepBlanksDefaultValue;
               xmlKeepBlanksDefaultValue = 0;
               break;
         }
      }
   }

   if (argc > 3)
      rxstrdup(encoding, argv[3]);

   if (argc > 0 && argv[0].strptr) {
      rxstrdup(url, argv[0]);
      doc.doc = htmlParseFile(url, encoding);
   }
   else if (argc > 1 && argv[1].strptr) {
      rxstrdup(url, argv[1]);
      doc.doc = htmlParseDoc(url, encoding);
   }
   else if (here.strlength) {
      doc.doc = htmlParseDoc(here.strptr, encoding);
      here.strlength = 0;
   }
   else {
      doc.doc = NULL;
   }

   if (argc > 2) {
      register int i;

      for (i = 0; i < argv[2].strlength; i++) {
         switch (toupper(argv[2].strptr[i])) {
            case 'V':
               xmlDoValidityCheckingDefaultValue = oldVal;
               break;
            case 'D':
               xmlLoadExtDtdDefaultValue = oldLoad;
               break;
            case 'S':
               xmlKeepBlanksDefaultValue = oldSpace;
               break;
         }
      }
   }

   if (doc.doc == NULL) {
      result_zero();
   }
   else {
      bin_to_rxstring(result, doc.doc);
   }

   return 0;
}

/* create a new xml document */
rxfunc(xmlnewdoc)
{
   char * v = "1.0";
   xmlDocPtr doc;
   
   checkparam(0, 2);
   if (argc > 0 && argv[0].strlength)
      rxstrdup(v, argv[0]);

   doc = xmlNewDoc(v);

   if (doc) {
      bin_to_rxstring(result, doc);
      if (argc > 1) {
         xmlChar * e = xmlMalloc(argv[1].strlength+1);
         memcpy(e, argv[1].strptr, argv[1].strlength);
         e[argv[1].strlength] = 0;
         doc->encoding = e;
      }
   }
   else
      result_zero();

   return 0;
}

rxfunc(xmlnewhtml)
{
   unsigned char * pubid = "-//W3C//DTD HTML 4.01 Transitional//EN", * sysid=NULL;
   htmlDocPtr doc;

   checkparam(0, 2);

   if (argc > 0 && argv[0].strlength)
      rxstrdup(pubid, argv[0]);

   if (argc > 1)
      rxstrdup(sysid, argv[1]);

   doc = htmlNewDoc(pubid, sysid);

   if (doc) {
      bin_to_rxstring(result, doc);
   }
   else
      result_zero();

   return 0;
}

rxfunc(xmlfreedoc)
{
   xmlDocPtr doc;
   register int i;

   checkparam(1, -1);

   for (i = 0; i < argc; i++)
      if (argv[i].strlength != sizeof(doc))
         return BADARGS;


   for (i = 0; i < argc; i++) {
      memcpy(&doc, argv[0].strptr, argv[0].strlength);
      if (doc->type != XML_DOCUMENT_NODE && doc->type != XML_HTML_DOCUMENT_NODE)
         continue;

      xmlFreeDoc(doc);
   }

   return 0;
}


/* XML environment -- just adds the command to the current document,
 * waiting for the moment to parse it */

static APIRET APIENTRY hereEnv(PRXSTRING cmd, PUSHORT rc, PRXSTRING result)
{
   here.strptr = realloc(here.strptr, here.strlength + 1 + cmd->strlength);

   if (!here.strptr)
      return NOMEMORY;

   if (here.strlength)
      here.strptr[here.strlength++] = '\n';

   memcpy(here.strptr + here.strlength, cmd->strptr, cmd->strlength);
   here.strlength += cmd->strlength;
   here.strptr[here.strlength] = 0;

   *rc = 0;
   result->strlength = 0;

   return 0;
}

void xmlinit(int init)
{
   RexxRegisterSubcomExe("XML", hereEnv, NULL);
   if (init) {
      xmlInitParser();
   }
}

void xmlfini(int init)
{
   if (here.strptr) {
      free(here.strptr);
      here.strptr = NULL;
   }
   if (init)
      xmlCleanupParser();
}
