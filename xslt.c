/* XSLT interface functions
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
 * $Header: C:/ptjm/rexx/rexxxml/RCS/xslt.c 1.8 2003/10/31 15:02:40 ptjm Rel $
 */


/* this loads all the libxml and rexx headers we need */
#define RX_STRONGTYPING
#include "rexxxml.h"

/* but not the libxslt headers */
#include <libxslt/xslt.h>
#include <libxslt/functions.h>
#include <libxslt/transform.h>
#include <libxslt/extensions.h>
#include <libxslt/xsltutils.h>
#include <libxslt/templates.h>
#include <libexslt/exslt.h>

static RXSTRING here; 

/* parse a stylesheet from a url and return a pointer to it
 * or parse a stylesheet from data
 *  ss = xmlParseXSLT([url], [inline])
 */
rxfunc(xmlparsexslt)
{
   xsltStylesheetPtr ssp;
   xmlDocPtr doc;
   char * stylesheet;

   checkparam(0, 2);
   
   if (argc > 0 && argv[0].strptr) {
      rxstrdup(stylesheet, argv[0]);
      ssp = xsltParseStylesheetFile(stylesheet);
   }
   else if (argc > 1 && argv[1].strptr) {
      doc = xmlParseMemory(argv[0].strptr, argv[0].strlength);
      if (doc) {
         ssp = xsltParseStylesheetDoc(doc);
      }
      else {
         ssp = NULL;
      }
   }

   else if (here.strlength) {
      doc = xmlParseMemory(here.strptr, here.strlength);

      if (doc) {
         ssp = xsltParseStylesheetDoc(doc);
      }
      else {
         ssp = NULL;
      }

      /* don't free here.strptr on the theory it's faster to just realloc
       * next time, although it does lead to a one-time memory leak */
      here.strlength = 0;
   }

   if (ssp) {
      bin_to_rxstring(result, ssp);
   }
   else {
      result_zero();
   }

   return 0;
}

/* free a stylesheet created with one of the parse stylesheet functions */
rxfunc(xmlfreestylesheet)
{
   xsltStylesheetPtr ssp;
   register int i;

   checkparam(1, -1);

   for (i = 0; i < argc; i++)
      if (!argv[i].strptr || argv[i].strlength != sizeof(ssp)) {
         return BADARGS;
      }

   for (i = 0; i < argc; i++) {
      rxstring_to_bin(ssp, argv+i);
      xsltFreeStylesheet(ssp);
   }

   return 0;
}

/* report the output method */
rxfunc(xmloutputmethod)
{
   xsltStylesheetPtr ssp;

   if (argc != 1 || !argv[0].strptr || argv[0].strlength != sizeof(ssp)) {
      return BADARGS;
   }

   rxstring_to_bin(ssp, argv);

   result->strlength = strlen(ssp->method);
   memcpy(result->strptr, ssp->method, result->strlength);

   return 0;
}

/* returns 1 if the string is all digits, or 0 otherwise */
static int all_digits(PRXSTRING str)
{
   static char digits[256];
   register int i, dec = 0;

   /* c99 has a better way of doing this initialisation */
   if (!digits['0']) {
      digits['0'] = digits['1'] = digits['2'] = digits['3'] = digits['4'] =
      digits['5'] = digits['6'] = digits['7'] = digits['8'] = digits['9'] = 1;
   }

   if (str->strlength && str->strptr[0] == '-')
      i = 1;
   else
      i = 0;

   for (; i < str->strlength; i++)
      if (!digits[(unsigned)str->strptr[i]] && (str->strptr[i] != '.' || (++dec > 1)))
         return 0;

   return 1;
}


/* xmlApplyStyleSheet(stylesheet, document[, sstype, doctype]) -> document tree
 *  the stylesheet and document can each be either a file name or
 *  a pointer to the appropriate type of object
 *  sstype and doctype are either u or t, indicating that the corresponding
 *  argument is a url or a document tree (the default).
 */
rxfunc(xmlapplystylesheet)
{
   xsltStylesheetPtr ssp = NULL;
   xmlDocPtr doc = NULL, res;
   int ssurl = 0, docurl = 0;
   char ** params;
   register int i;

   checkparam(2, -1);

   /* there can be any number of parameters, but they'd better all have a
    * value */
   if (argc > 4) {
      if (argc & 1)
         return BADARGS;
      for (i = 4; i < argc; i++) {
         if (!argv[i].strptr)
            return BADARGS;
      }
   }

   if (argc > 2 && argv[2].strlength > 0 && tolower(argv[2].strptr[0]) == 'u') {
      char * ss;
      rxstrdup(ss, argv[0]);
      ssurl = 1;
      ssp = xsltParseStylesheetFile(ss);

      if (!ssp) {
         result_zero();
         return 0;
      }
   }

   if (argc > 3 && argv[3].strlength > 0 && tolower(argv[3].strptr[0]) == 'u') {
      char * document;
      rxstrdup(document, argv[1]);
      docurl = 1;
      doc = xmlParseFile(document);

      if (!doc) {
         if (ssurl) {
            xsltFreeStylesheet(ssp);
         }

         result_zero();
         return 0;
      }
   }

   if (!ssurl)
      rxstring_to_bin(ssp, argv);

   if (!docurl)
      rxstring_to_bin(doc, argv+1);

   if (argc > 4) {
      params = alloca(sizeof(*params)*(argc - 2));

      /* can pass fixed strings, numbers, or true()/false().
       * libxslt also allows certain fixed string functions, but I don't
       * think there's any loss of generality in putting the quotes in here
       * rather than forcing the user (me at this point) to do it */
      for (i = 4; i < argc; i ++) {
         static const char nil[] = "''", tr[] = "true()", fl[] = "false()";

         if (argv[i].strlength > 0) {
                /* quoted */
            if ((!(i&1)) || (argv[i].strptr[0] == argv[i].strptr[argv[i].strlength-1] &&
                (argv[i].strptr[0] == '\'' || argv[i].strptr[0] == '"')) ||
                /* number */
                all_digits(argv+i) ||
                /* true()/false() */
               (argv[i].strlength == (sizeof(tr)-1) && !memcmp(argv[i].strptr, tr, sizeof(tr)-1)) ||
               (argv[i].strlength == (sizeof(fl)-1) && !memcmp(argv[i].strptr, fl, sizeof(fl)-1)))
              rxstrdup(params[i-4], argv[i]);
           else {
              /* need to check for embedded quotes */
              if (memchr(argv[i].strptr, '\'', argv[i].strlength)) {
                 register int qc = 0;
                 unsigned char * qt = memchr(argv[i].strptr, '"', argv[i].strlength);

                 while (qt) {
                    qc++;
                    qt = memchr(qt+1, '"', argv[i].strlength - 
                         (qt - (unsigned char *)argv[i].strptr) - 1);
                 }
                 
                 if (!qc) {
                    params[i-4] = alloca(argv[i].strlength+3);
                    memcpy(params[i-4]+1, argv[i].strptr, argv[i].strlength);
                    params[i-4][0] = params[i-4][argv[i].strlength+1] = '"';
                 }
                 else {
                    static const char concat[] = "concat(";
                    unsigned char * s1, *s2, * tgt, delim, *p = argv[i].strptr;
                    int l = argv[i].strlength;

                    /* damn. we turn this into concat(...) where ... is
                     * quoted correctly */

                    /* rationale behind this sizing: qc quote marks divide
                     * the string into qc+1 pieces. Each piece needs quotes
                     * around it and a comma after it (hence +3*(qc+1)).
                     * Each of the quotes needs quotes around it (+qc*2),
                     * and we need a ) and null-terminator at the end, which
                     * gets absorbed in the extra 3 bytes
                     * actually, I'll need less than this, but as long as I
                     * allocate enough, things will be fine */
                    params[i-4] = alloca(l+(qc+1)*5+sizeof(concat)+1);
                    memcpy(params[i-4], concat, sizeof(concat)-1);
                    tgt = params[i-4]+sizeof(concat)-1;
                    delim = '"';
                    for (s1 = p, s2 = memchr(s1, delim, l-(s1-p));
                         s2;
                         s1=s2, s2 = memchr(s1, delim, l-(s1-p))) {
                       if (s2 > s1) {
                          *tgt++ = delim;
                          memcpy(tgt, s1, s2-s1);
                          tgt += s2 - s1;
                          *tgt++ = delim;
                          *tgt++ = ',';
                       }
                       delim = delim == '"' ? '\'' : '"';
                    }

                    /* at this point, tgt is just after a comma and we need
                     * to copy the rest of the string, delimited by delim, then
                     * follow it with ) */
                    *tgt++ = delim;
                    memcpy(tgt, s1, l - (s1-p));
                    tgt += l - (s1-p);
                    *tgt++ = delim;
                    *tgt++ = ')';
                    *tgt++ = 0;
                 }
              }
              else {
                 params[i-4] = alloca(argv[i].strlength+3);
                 memcpy(params[i-4]+1, argv[i].strptr, argv[i].strlength);
                 params[i-4][0] = params[i-4][argv[i].strlength+1] = '\'';
              }
           }
         }
         else
            params[i-4] = (char *)nil;
      }
      params[argc-4] = NULL;
      params[argc-3] = NULL;
   }
   else {
      params = NULL;
   }

   res = xsltApplyStylesheet(ssp, doc, (const char **)params);

   if (res) {
      bin_to_rxstring(result, res);
   }
   else {
      result_zero();
   }

   if (ssurl)
      xsltFreeStylesheet(ssp);

   if (docurl)
      xmlFreeDoc(doc);

   return 0;
}

/* XSLT environment -- just adds the command to the current stylesheet,
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

/* extension functions and elements */

/* URL: I ought to clear this with the rexx language association, eh? */
static const char rexxxmluri[] = "urn://rexxxml/xslt";

/* convert a string representation of a Boolean to 1 or 0 */
static int getBool(const char * val)
{
   /* I allow true/false, on/off, yes/no y/n 1/0 */
   if (!strcasecmp(val, "true") ||
       !strcasecmp(val, "on") ||
       !strcasecmp(val, "yes") ||
       !strcasecmp(val, "y") ||
       !strcasecmp(val, "1"))
      return 1;
   /* but I need to have a default value, so anything else is
    * false */
   else
       return 0;
}


/* there's no way to precompile the rexx function. We simply save the
 * contents, register the function, and hope that it doesn't fail later... */

#define ALLSTRINGFLAG 1

typedef struct rxfcn_T {
   xmlChar * name, * url;
   RXSTRING code[2];
   xmlNodePtr inst;
   xmlXPathObjectType rt;
   unsigned int flags;
   struct rxfcn_T * N;
} * rxfcn_t;

/* all the functions are stored in a linked list, on the theory that we
 * won't be defining too many */
static rxfcn_t fcnlist = NULL;


#if LIBXSLT_VERSION < 10032
void xsltTransformError(void * ctxt, void * octxt, void * lctxt, char * s, ...)
{
   va_list val;
   va_start(val, s);
   vfprintf(stderr, s, val);
   va_end(val);

}
#endif

/* we don't have nearly as much context available in a function */
static LONG APIENTRY rxFunctionVars (LONG exit, LONG subfunc, PEXIT parms)
{
   static const unsigned char ctxt[] = "xmlContext";
   xmlXPathContextPtr pctxt;
   unsigned char env[8];
   unsigned short flag = 0;
   SHVBLOCK shv;

   RexxQueryExit("vars", NULL, &flag, env);
   memcpy(&pctxt, env, sizeof(pctxt));
   
   shv.shvnext = NULL;
   shv.shvcode = RXSHV_SYSET;

   /* `built-in' variable */
   shv.shvname.strptr = (unsigned char *)ctxt;
   shv.shvname.strlength = sizeof(ctxt)-1;
   shv.shvvalue.strptr = (unsigned char *)&pctxt;
   shv.shvvalue.strlength = sizeof(pctxt);
   RexxVariablePool(&shv);

   return RXEXIT_HANDLED;
}


static void rexxExec(xmlXPathParserContextPtr ctxt, int argc)
{
   RXSTRING * argv;
   int rc;
   RXSTRING result;
   register int i;
   xmlXPathObjectPtr o = NULL;
   rxfcn_t fcn;
   RXSYSEXIT exits[2];
   unsigned char env[8];

   for (fcn = fcnlist;
        fcn && (strcmp(fcn->name, ctxt->context->function) ||
                strcmp(fcn->url, ctxt->context->functionURI));
        fcn = fcn->N)
      ;

   if (!fcn) {
      xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                    "Function %s:%s not registered with RexxXML.\n",
                    ctxt->context->functionURI, ctxt->context->function);
      
      o = xmlXPathNewCString("");
      valuePush(ctxt, o);
      return;
   }

   memcpy(env, &ctxt->context, sizeof(ctxt->context));

   RexxRegisterExitExe("vars", rxFunctionVars, (unsigned char *)env);
   exits[0].sysexit_name = "vars";
   exits[0].sysexit_code = RXINI;
   exits[1].sysexit_name = "vars";
   exits[1].sysexit_code = RXENDLST;

   result.strptr = REXXALLOCATEMEMORY(DEFAULTSTRINGSIZE+1);
   result.strlength = 0;

   if (argc) {
      argv = alloca(sizeof(*argv) * argc);
   }
   else {
      argv = NULL;
   }

   for (i = argc-1; i >= 0; i--) {
      o = valuePop(ctxt);
      if (fcn->flags & ALLSTRINGFLAG) {
         argv[i].strptr = xmlXPathCastToString(o);
         argv[i].strlength = strlen(argv[i].strptr);
      }

      else
         switch (o->type) {
            case XPATH_BOOLEAN:
            case XPATH_NUMBER:
            case XPATH_STRING:
               argv[i].strptr = xmlXPathCastToString(o);
               argv[i].strlength = strlen(argv[i].strptr);
               break;
            default:
               argv[i].strptr = xmlMalloc(sizeof(o));
               memcpy(argv[i].strptr, &o, sizeof(o));
               argv[i].strlength = sizeof(o);
         }
      xmlXPathFreeObject(o);
   }

   rc = RexxStart(argc, argv, fcn->name, fcn->code, NULL, RXFUNCTION, exits, NULL, &result);
   RexxDeregisterExit("vars", NULL);

   if (rc) {
      xmlChar * path = xmlGetNodePath(fcn->inst);

      /* I'm not sure when xsltTransformError was introduced */
      xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                    "Rexx script %s failed with rc %d\n", path, rc);
      xmlFree(path);
   }

   else {
      if (result.strlength) {
         result.strptr[result.strlength] = 0;

         switch (fcn->rt) {
            case XPATH_BOOLEAN:
               if (getBool(result.strptr)) {
                  o = xmlXPathNewBoolean(1);
               }
               else  {
                  o = xmlXPathNewBoolean(0);
               }
               break;

            case XPATH_NUMBER:
               o = xmlXPathNewFloat(atof(result.strptr));
               break;

            case XPATH_STRING:
               o = xmlXPathNewCString(result.strptr);
               break;

            case XPATH_NODESET:
               if (result.strlength != sizeof(xmlNodePtr)) {
      xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                    "Node set return value should be %d bytes long. Got %d bytes.\n", sizeof(xmlNodePtr), result.strlength);

                  o = xmlXPathNewNodeSet(NULL);
               }
               else {
                  memcpy(&o, result.strptr, result.strlength);
               }
               break;

            case XPATH_XSLT_TREE:
               if (result.strlength != sizeof(xmlNodePtr)) {
      xsltTransformError(xsltXPathGetTransformContext(ctxt), NULL, NULL,
                    "Value tree return value should be %d bytes long. Got %d bytes.\n", sizeof(xmlNodePtr), result.strlength);

                  o = xmlXPathNewValueTree(NULL);
               }
               else {
                  o = xmlXPathNewValueTree((xmlNodePtr)result.strlength);
               }
               break;

            default:
               /* don't know what this is -- I've already given an error
                * message, so now I'll just do nothing */
               ;
         }
      }
   }

   if (!o)
      o = xmlXPathNewCString("");

   valuePush(ctxt, o);
}

/* map return-type attribute to an xpath return type */
static xmlXPathObjectType getReturnType(xmlNodePtr inst)
{
   /* map of type names to return types. This is searched linearly, so put
    * the expected most common requests at the top. String is last since
    * it's the default, so I'd never bother putting it in at all */
   struct typemap {
      char name[10];
      xmlXPathObjectType rt;
   } types[] = {
      "number", XPATH_NUMBER,
      "tree", XPATH_XSLT_TREE,
      "Boolean", XPATH_BOOLEAN,
      "node set", XPATH_NODESET,
      "string", XPATH_STRING
   };
   xmlChar * type = xmlGetProp(inst, "return-type");
   xmlXPathObjectType rt = XPATH_UNDEFINED;
   register int i;

   if (type == NULL)
      rt = XPATH_STRING;
   else {
      for (i = 0; i < 5; i++) {
         if (!strcasecmp(type, types[i].name)) {
            rt = types[i].rt;
            break;
         }
      }
      if (rt == XPATH_UNDEFINED) {
         xmlChar * path = xmlGetNodePath(inst);

         xsltTransformError(NULL, NULL, inst,
            "%s: return-type='%s' is not one of 'number', 'tree', 'Boolean', 'node set' or 'string'\n", path, type);
         xmlFree(path);
      }

      xmlFree(type);
   }

   return rt;
}


/* get the attribute called name and map it to 0 or 1 */
static int getBoolFlag(xmlNodePtr inst, const xmlChar * name)
{
   xmlChar * flag = xmlGetProp(inst, name);
   int rc;


   if (flag) {
      rc = getBool(flag);
      xmlFree(flag);
   }
   else {
      rc = 0;
   }

   return rc;
}

static void
rexxDef (xsltStylesheetPtr style, xmlNodePtr inst)
{
   xmlChar * QName = xmlGetProp(inst, "name");
   rxfcn_t fcn;

   if (QName == NULL) {
      xmlChar * path = xmlGetNodePath(inst);

      xsltTransformError(NULL, style, NULL,
         "%s: no name attribute\n", path);
      xmlFree(path);
   }
   else {
      xmlChar *fname, *pname;
      xmlNsPtr url;
      
      fname = xmlSplitQName2(QName, &pname);
      if (fname == NULL || pname == NULL) {
         xmlChar * path = xmlGetNodePath(inst);
         xsltTransformError(NULL, style, NULL,
            "%s: name attribute %s does not have a name space prefix\n", path, QName);
         xmlFree(path);

         xmlFree(QName);
         if (fname)
            xmlFree(fname);
         if (pname)
            xmlFree(pname);
         return;
      }

      xmlFree(QName);

      url = xmlSearchNs(inst->doc, inst, pname);
      if (url == NULL) {
         xmlChar * path = xmlGetNodePath(inst);
         xsltTransformError(NULL, style, NULL,
            "%s: name space prefix %s is not registered\n", path, pname);
         xmlFree(path);
      }

      /* all we can do at this point is save the value -- the Rexx API
       * doesn't have a precompute function. We use a linked list because
       * we don't expect a lot of these */
      else {
         int l1 = strlen(fname), l2 = strlen(url->href);

         fcn = xmlMalloc(sizeof(*fcn)+l1+l2+2);
         fcn->N = fcnlist;
         fcnlist = fcn;

         /* if there's a conflict, we silently ignore it. A future version
          * might use a hash table which handle it differently */
         fcn->name = (xmlChar *)(fcn+1);
         memcpy(fcn->name, fname, l1+1);
         fcn->url = fcn->name + l1 + 1;
         memcpy(fcn->url, url->href, l2+1);

         xsltRegisterExtModuleFunction(fname, url->href, rexxExec);
         fcn->rt = getReturnType(inst);
         fcn->inst = inst;
         fcn->flags = 0;
         if (getBoolFlag(inst, "all-strings"))
            fcn->flags |= ALLSTRINGFLAG;

         fcn->code[0].strptr = xmlNodeGetContent(inst);
         if (fcn->code[0].strptr) {
            fcn->code[0].strlength = strlen(fcn->code[0].strptr);
            fcn->code[1].strptr = NULL;
            fcn->code[1].strlength = 0;
         }
      }

      xmlFree(pname);
      xmlFree(fname);
   }
}


/* execute inline rexx code */
static void
rexxInline (xsltStylesheetPtr style, xmlNodePtr inst)
{
   RXSTRING funcdef[2];
   int rc;
   short src;
      
   funcdef[0].strptr = xmlNodeGetContent(inst);

   if (funcdef[0].strptr) {
      funcdef[0].strlength = strlen(funcdef[0].strptr);
      funcdef[1].strlength = 0;
      funcdef[1].strptr = 0;
      
      rc = RexxStart(0, NULL, "", funcdef, NULL, RXCOMMAND, NULL, &src, NULL);

      xmlFree(funcdef[0].strptr);
      if (funcdef[1].strptr)
         REXXFREEMEMORY(funcdef[1].strptr);
   }
}

struct rxdata {
   xsltTransformContextPtr ctxt;
   xmlNodePtr inst;
};

static LONG APIENTRY rxTemplateVars (LONG exit, LONG subfunc, PEXIT parms)
{
   static const unsigned char output[] = "xmlResultTree",
                              ctxt[] = "xmlContext",
                              outnode[] = "xmlResultNode";
   struct rxdata *pdata;
   unsigned char env[8];
   unsigned short flag = 0;
   xmlAttributePtr attr;
   xmlElementPtr inst;
   SHVBLOCK shv;

   RexxQueryExit("vars", NULL, &flag, env);
   memcpy(&pdata, env, sizeof(pdata));
   
   shv.shvnext = NULL;

   /* on init, set a bunch of variables. On termination, we just want to
    * read the result tree and result node pointers */

   shv.shvcode = exit == RXINI ? RXSHV_SYSET : RXSHV_SYFET;

   /* `built-in' variables */
   if (pdata->ctxt) {
      shv.shvname.strptr = (unsigned char *)output;
      shv.shvname.strlength = sizeof(output)-1;
      shv.shvvalue.strptr = (unsigned char *)&pdata->ctxt->output;
      shv.shvvaluelen = shv.shvvalue.strlength = sizeof(pdata->ctxt->output);
      RexxVariablePool(&shv);

      shv.shvname.strptr = (unsigned char *)outnode;
      shv.shvname.strlength = sizeof(outnode)-1;
      shv.shvvalue.strptr = (unsigned char *)&pdata->ctxt->insert;
      shv.shvvaluelen = shv.shvvalue.strlength = sizeof(pdata->ctxt->insert);
      RexxVariablePool(&shv);

      if (exit == RXTER)
         return RXEXIT_HANDLED;

      shv.shvname.strptr = (unsigned char *)ctxt;
      shv.shvname.strlength = sizeof(ctxt)-1;
      shv.shvvalue.strptr = (unsigned char *)&pdata->ctxt->xpathCtxt;
      shv.shvvalue.strlength = sizeof(pdata->ctxt->xpathCtxt);
      RexxVariablePool(&shv);
   }

   else
      if (exit == RXTER)
         return RXEXIT_HANDLED;

   if (pdata->inst) {
      inst = (xmlElementPtr)pdata->inst;

      for (attr = inst->attributes; attr; attr = (xmlAttributePtr)attr->next) {
         shv.shvvalue.strptr = xsltEvalAttrValueTemplate(pdata->ctxt, pdata->inst, attr->name,
                                         rexxxmluri);
         if (shv.shvvalue.strptr) {
            shv.shvname.strptr = (unsigned char *)attr->name;
            shv.shvname.strlength = strlen(shv.shvname.strptr);
            shv.shvvalue.strlength = strlen(shv.shvvalue.strptr);
            RexxVariablePool(&shv);
            xmlFree(shv.shvvalue.strptr);
         }
      }
   }

   return RXEXIT_HANDLED;
}


typedef struct {
   xsltElemPreCompPtr next;
   xsltStyleType type;
   xsltTransformFunction func;
   xmlNodePtr inst;
   xmlXPathObjectType objtype;
   RXSTRING prog[2];
} * rexxPrecompPtr;


static void
rexxTemplate (xsltTransformContextPtr ctxt, xmlNodePtr node, xmlNodePtr inst,
              xsltElemPreCompPtr comp)
{
   RXSTRING autofuncdef[2], *funcdef;
   int rc;
   short src;
   RXSTRING result;
   RXSYSEXIT exits[3];
   unsigned char env[8];
   struct rxdata data, *pdata = &data;
   rexxPrecompPtr pc = (rexxPrecompPtr)comp;

   if (pc->prog[0].strlength) {
      funcdef = pc->prog;
   }
   else {
      funcdef = autofuncdef;
      funcdef[0].strptr = xsltEvalTemplateString(ctxt, node, inst);
      funcdef[0].strlength = strlen(funcdef[0].strptr);
      funcdef[1].strlength = 0;
      funcdef[1].strptr = 0;
   }


   memcpy(env, &pdata, sizeof(pdata));
   data.ctxt = ctxt;
   data.inst = inst;

   RexxRegisterExitExe("vars", rxTemplateVars, (unsigned char *)env);
   exits[0].sysexit_name = "vars";
   exits[0].sysexit_code = RXINI;
   exits[1].sysexit_name = "vars";
   exits[1].sysexit_code = RXTER;
   exits[2].sysexit_name = "vars";
   exits[2].sysexit_code = RXENDLST;

   result.strptr = REXXALLOCATEMEMORY(10250);
   result.strlength = 0;


   rc = RexxStart(0, NULL, "", funcdef, NULL, RXCOMMAND, exits, &src, &result);
   RexxDeregisterExit("vars", NULL);

   if (rc) {
      xsltTransformError(ctxt, NULL, node,
         "Rexx script %s failed with rc %d\n", funcdef[0].strptr, rc);
   }



   /* always get rid of the text of the function, as it's no longer needed */
   if (funcdef[0].strptr) {
      xmlFree(funcdef[0].strptr);
      funcdef[0].strptr = NULL;
   }

   /* get rid of the pre-parsed version of the function only if this is
    * a one-off (i.e., it wasn't a constant string, so it's likely to
    * be different next time out */
   if (funcdef == autofuncdef && funcdef[1].strptr) {
      REXXFREEMEMORY(funcdef[1].strptr);
   }

   if (result.strlength) {
      /* this case is a bit simpler than the XPath case, since node sets
       * don't really make sense, so we can just hope they're the same as
       * value trees, and booleans/numbers/strings are all basically the same
       * thing in the output tree. */

      switch (pc->objtype) {
         case XPATH_NODESET:
         case XPATH_XSLT_TREE:
            break;

            if (result.strlength != sizeof(xmlNodePtr)) {
               xsltTransformError(ctxt, NULL, NULL,
                    "Value tree return value should be %d bytes long. Got %d bytes.\n", sizeof(xmlNodePtr), result.strlength);

               /* no need to add anything in this case */
            }
            
            else
               xmlAddChild(ctxt->insert, (xmlNodePtr)result.strptr);
         default:
            xmlAddChild(ctxt->insert, xmlNewTextLen(result.strptr,
                        result.strlength));
      }
   }
}

/* if the node consists only of text, get the content and store it with the
 * instance. At execution time, we free it up if necessary. Otherwise,
 * I assume there are xslt or extension elements which could change the
 * content of the program, so I get the text each time. */
static xsltElemPreCompPtr
rexxTemplatePrecomp (xsltStylesheetPtr style, xmlNodePtr inst, xsltTransformFunction txfcn)
{
   xmlNodePtr ch;
   int haselems = 0;
   rexxPrecompPtr comp;

   for (ch = inst->children; ch; ch = ch->next) {
      if (ch->type != XML_TEXT_NODE && (ch->type != XML_ELEMENT_NODE ||
          !IS_XSLT_ELEM(ch) || !IS_XSLT_NAME(ch, "fallback"))) {
         haselems = 1;
         break;
      }
   }

   comp = xmlMalloc(sizeof(*comp));

   if (!comp) {
      xsltTransformError(NULL, style, NULL,
		"failed to allocate %d bytes for precomputed Rexx information\n", sizeof(*comp));
      return NULL;
   }


   /* it seems like I have to set this, but it seems a bit
    * unreasonable */
   comp->next = NULL;
   comp->type = XSLT_FUNC_EXTENSION;
   comp->func = (void *)rexxTemplate;
   comp->inst = inst;
   comp->objtype = getReturnType(inst);
   comp->prog[1].strptr = NULL;
   comp->prog[1].strlength = 0;

   if (haselems) {
      comp->prog[0].strptr = NULL;
      comp->prog[0].strlength = 0;
   }
   else {
      comp->prog[0].strptr = xmlNodeGetContent(inst);
      comp->prog[0].strlength = strlen(comp->prog[0].strptr);
   }  

   return (xsltElemPreCompPtr)comp;
}


static void * extInit(xsltTransformContextPtr ctxt,
                      const xmlChar * uri)
{
   return NULL;
}


static void extShutdown(xsltTransformContextPtr ctxt,
                      const xmlChar * uri,
                      void * data)
{
}


void xsltinit(int init)
{
   RexxRegisterSubcomExe("XSLT", hereEnv, NULL);
   xsltRegisterExtModule(rexxxmluri,
                         extInit, extShutdown);

   xsltRegisterExtModuleTopLevel("rexx", rexxxmluri, rexxInline);
   xsltRegisterExtModuleTopLevel("function", rexxxmluri, rexxDef);
   xsltRegisterExtModuleElement("template", rexxxmluri, rexxTemplatePrecomp, rexxTemplate);

   if (init) {
      exsltRegisterAll();
   }
}

void xsltfini(int init)
{
   xsltUnregisterExtModuleElement("template", rexxxmluri);
   xsltUnregisterExtModuleTopLevel("rexx", rexxxmluri);
   xsltUnregisterExtModuleTopLevel("function", rexxxmluri);
   xsltUnregisterExtModule(rexxxmluri);
   if (here.strptr) {
      free(here.strptr);
      here.strptr = NULL;
   }
}
