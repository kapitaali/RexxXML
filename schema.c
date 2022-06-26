/* schema validation for RexxXML.
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
 * $Header: C:/ptjm/rexx/rexxxml/RCS/schema.c 1.3 2003/10/08 02:07:56 ptjm Rel $
 */

#include "rexxxml.h"

#include <libxml/xmlschemas.h>

#include <time.h>

static RXSTRING here; 

/* xsd = xmlParseSchema([url], [inline]) */
rxfunc(xmlparseschema)
{
   xmlSchemaParserCtxtPtr parsectx;
   xmlSchemaPtr xsd;
   char * url;

   checkparam(0, 2);

   if (argc > 0 && argv[0].strlength) {
      rxstrdup(url, argv[0]);
      parsectx = xmlSchemaNewParserCtxt(url);
   }
   else if (argc > 1 && argv[1].strlength) {
      parsectx = xmlSchemaNewMemParserCtxt(argv[1].strptr, argv[1].strlength);
   }
   else if (!argc && here.strlength) {
      parsectx = xmlSchemaNewMemParserCtxt(here.strptr, here.strlength);
   }

   if (!parsectx) {
      result_zero();
   }
   else {
      xmlSchemaSetParserErrors(parsectx, rxErrorFn, rxErrorFn, NULL);

      xsd = xmlSchemaParse(parsectx);
      xmlSchemaFreeParserCtxt(parsectx);

      if (!xsd) {
         result_zero();
      }
      else {
         bin_to_rxstring(result, xsd);
      }
   }

   return 0;
}

/* errcode = xmlValidateDoc(schema, doc, schematype, doctype) */

static void rctoresult(PRXSTRING result, int rc)
{
#  define nmtoresult(x) { \
         static const char nm[] = x; \
         memcpy(result->strptr, nm, sizeof(nm)-1); \
         result->strlength = sizeof(nm) - 1; \
         break; \
   }

   switch (rc) {
      case XML_SCHEMAS_ERR_OK: nmtoresult("OK");
      case XML_SCHEMAS_ERR_NOROOT: nmtoresult("NOROOT");
      case XML_SCHEMAS_ERR_UNDECLAREDELEM: nmtoresult("UNDECLAREDELEM");
      case XML_SCHEMAS_ERR_NOTTOPLEVEL: nmtoresult("NOTTOPLEVEL");
      case XML_SCHEMAS_ERR_MISSING: nmtoresult("MISSING");
      case XML_SCHEMAS_ERR_WRONGELEM: nmtoresult("WRONGELEM");
      case XML_SCHEMAS_ERR_NOTYPE: nmtoresult("NOTYPE");
      case XML_SCHEMAS_ERR_NOROLLBACK: nmtoresult("NOROLLBACK");
      case XML_SCHEMAS_ERR_ISABSTRACT: nmtoresult("ISABSTRACT");
      case XML_SCHEMAS_ERR_NOTEMPTY: nmtoresult("NOTEMPTY");
      case XML_SCHEMAS_ERR_ELEMCONT: nmtoresult("ELEMCONT");
      case XML_SCHEMAS_ERR_HAVEDEFAULT: nmtoresult("HAVEDEFAULT");
      case XML_SCHEMAS_ERR_NOTNILLABLE: nmtoresult("NOTNILLABLE");
      case XML_SCHEMAS_ERR_EXTRACONTENT: nmtoresult("EXTRACONTENT");
      case XML_SCHEMAS_ERR_INVALIDATTR: nmtoresult("INVALIDATTR");
      case XML_SCHEMAS_ERR_INVALIDELEM: nmtoresult("INVALIDELEM");
      case XML_SCHEMAS_ERR_NOTDETERMINIST: nmtoresult("NOTDETERMINIST");
      case XML_SCHEMAS_ERR_CONSTRUCT: nmtoresult("CONSTRUCT");
      case XML_SCHEMAS_ERR_INTERNAL: nmtoresult("INTERNAL");
      case XML_SCHEMAS_ERR_NOTSIMPLE: nmtoresult("NOTSIMPLE");
      case XML_SCHEMAS_ERR_ATTRUNKNOWN: nmtoresult("ATTRUNKNOWN");
      case XML_SCHEMAS_ERR_ATTRINVALID: nmtoresult("ATTRINVALID");
      case XML_SCHEMAS_ERR_VALUE: nmtoresult("VALUE");
      case XML_SCHEMAS_ERR_FACET: nmtoresult("FACET");
      case XML_SCHEMAS_ERR_: nmtoresult("nil");
      case XML_SCHEMAS_ERR_XXX: nmtoresult("XXX");
      default: nmtoresult("unknown");
   }
#  undef nmtoresult
}


rxfunc(xmlvalidatedoc)
{
   xmlSchemaPtr xsd;
   xmlSchemaValidCtxtPtr vctxt;
   xmlDocPtr doc;
   int sdurl = 0, docurl = 0, rc;

   checkparam(2, 4);

   
   if (argc > 2 && argv[2].strlength > 0 && tolower(argv[2].strptr[0]) == 'u') {
      char * url;
      xmlSchemaParserCtxtPtr parsectx;

      rxstrdup(url, argv[0]);
      sdurl = 1;
      parsectx = xmlSchemaNewParserCtxt(url);

      if (!parsectx) {
         static const char nm[] = "BADSCHEMA";
         memcpy(result->strptr, nm, sizeof(nm)-1);
         result->strlength = sizeof(nm) - 1;
         
         return 0;
      }

      else {
         xmlSchemaSetParserErrors(parsectx, rxErrorFn, rxErrorFn, NULL);
         xsd = xmlSchemaParse(parsectx);
         xmlSchemaFreeParserCtxt(parsectx);
      }
      if (!xsd) {
         static const char nm[] = "INVALIDSCHEMA";
         memcpy(result->strptr, nm, sizeof(nm)-1);
         result->strlength = sizeof(nm) - 1;

         return 0;
      }
   }
   else {
      if (argv[0].strlength != sizeof(xsd))
         return BADARGS;
   }

   if (argc > 3 && argv[3].strlength > 0 && tolower(argv[3].strptr[0]) == 'u') {
      char * document;
      rxstrdup(document, argv[1]);
      docurl = 1;
      doc = xmlParseFile(document);

      if (!doc) {
         static const char nm[] = "BADDOC";

         if (sdurl) {
            xmlSchemaFree(xsd);
         }

         memcpy(result->strptr, nm, sizeof(nm)-1);
         result->strlength = sizeof(nm) - 1;

         return 0;
      }
   }
   else {
      if (argv[1].strlength != sizeof(doc)) {
         if (sdurl)
            xmlSchemaFree(xsd);
         return BADARGS;
      }
   }

   if (!sdurl)
      rxstring_to_bin(xsd, argv);

   if (!docurl)
      rxstring_to_bin(doc, argv+1);

   vctxt = xmlSchemaNewValidCtxt(xsd);
   if (!vctxt) {
      result_zero();
      return 0;
   }

   xmlSchemaSetValidErrors(vctxt, rxErrorFn, rxErrorFn, NULL);
   rc = xmlSchemaValidateDoc(vctxt, doc);
   xmlSchemaFreeValidCtxt(vctxt);

   rctoresult(result, rc);

   if (sdurl)
      xmlSchemaFree(xsd);
   if (docurl)
      xmlFreeDoc(doc);

   return 0;
}

/* xmlFreeSchema xsd, [xsd2], ... */

rxfunc(xmlfreeschema)
{
   register int i;
   xmlSchemaPtr xsd;

   for (i = 0; i < argc; i++) {
      if (argv[i].strlength != sizeof(xsd))
         return BADARGS;
   }

   for (i = 0; i < argc; i++) {
      rxstring_to_bin(xsd, argv+i);
      xmlSchemaFree(xsd);
   }

   result->strlength = 0;
   return 0;
}



rxfunc(xmldumpschema)
{
   xmlSchemaPtr xsd;
   FILE * fp;
   char * fn;
   register int i;
   time_t now;
   struct tm *tm;

   checkparam(2, -1);

   for (i = 1; i < argc; i++)
      if (argv[i].strlength != sizeof(xsd)) {
         return BADARGS;
      }

   rxstrdup(fn, argv[0]);

   fp = fopen(fn, "a");

   if (!fp) {
      result->strlength = sprintf(result->strptr, "%s: %s\n", fn, strerror(errno));
      return 0;
   }

   for (i = 1; i < argc; i++) {
      rxstring_to_bin(xsd, argv+i);

      time(&now);
      tm = localtime(&now);
      fprintf(fp, "Dump at %04d.%02d.%02d-%02d.%02d.%02d\n",
              tm->tm_year+1900, tm->tm_mon+1, tm->tm_mday,
              tm->tm_hour, tm->tm_min, tm->tm_sec);

      xmlSchemaDump(fp, xsd);

      fprintf(fp, "\n\n");
   }


   fclose(fp);

   result->strlength = 0;
   return 0;
}

/* XSD environment -- this is the same as the XML and XSLT environments.
 * What I really want to do allow the user to define his own environment
 *   xmlNewEnvironment("SPREADSHEET")
 *   address spreadsheet '<ss><row><col/><col/>...'
 *   mydoc = xmlHere("SPREADSHEET")
 * but I don't see a way of knowing which environment I'm in at the
 * command-processing stage, so I've just created an environment for
 * each type of embedded document, and put in a one-document-at-a-time
 * cap 
 */
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

void xsdinit(int init)
{
   RexxRegisterSubcomExe("XSD", hereEnv, NULL);
}

void xsdfini(int init)
{
   if (here.strptr) {
      free(here.strptr);
      here.strptr = NULL;
   }
}
