/* XPath interface for RexxXML
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
 * $Header: C:/ptjm/rexx/rexxxml/RCS/xpath.c 1.6 2003/10/15 05:18:09 ptjm Rel $
 */
#include "rexxxml.h"
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>

/* default XPath context -- initialised as it seems useful */
static xmlXPathContextPtr gxpc;

static xmlXPathObjectPtr lookup_var(xmlXPathContextPtr ctxt, const xmlChar * name,
      const xmlChar * ns_uri);
static xmlXPathFunction lookup_func(xmlXPathContextPtr ctxt, const xmlChar * name,
      const xmlChar * ns_uri);

/* evaluate an XPath expression as a scalar
 * val = xmlEvalExpression(xpath_expression[, node][, context])
 * find node using general xpath expression.
 * nodeset = xmlFindNode(xpath_expression[, node][, context])
 */
rxfunc(xmlevalexpression)
{
   xmlnodeptr_t node;
   char * expr, *s;
   xmlXPathContextPtr xpc;
   xmlXPathObjectPtr xop;
   xmlXPathCompExprPtr cep = NULL;
   int scalar = !strcmp(fname, "XMLEVALEXPRESSION");

   checkparam(1, 3);

   /* if no context specified, there's got to be a global context */
   if ((!gxpc && argc == 1) ||
       (argc > 1 && !(argv[1].strlength == sizeof(node) ||
                     (argv[1].strlength == 0 && argc > 2))) ||
       (argc == 3 && (argv[2].strlength != sizeof(node))) ) {
      return BADARGS;
   }

   /* check for pre-compiled expression */
   if (argv[0].strlength == (sizeof(cep) + 1) &&
       argv[0].strptr[0] == '&') {
      memcpy(&cep, argv[0].strptr+1, sizeof(cep));
   }
   else
      rxstrdup(expr, argv[0]);

   if (argc > 2) {
      rxstring_to_bin(xpc, argv+2);
   }
   else {
      xpc = gxpc;
   }

   if (argc > 1 && argv[1].strlength == sizeof(node)) {
      if (xpc == gxpc) {
         RXSTRING av[2];
         av[1] = argv[1];
         av[0].strptr = NULL;
         av[0].strlength = 0;
         xmlsetcontext(fname, 2, av, pSomething, result);
         xpc = gxpc;
      }
      else {
         rxstring_to_bin(node, argv+1);
         xpc->doc = node.node->doc;
         xpc->node = node.node;
      }
   }

   if (cep)
      xop = xmlXPathCompiledEval(cep, xpc);
   else
      xop = xmlXPathEval(expr, xpc);

   /* should maybe raise a syntax error? or set a var? */
   if (!xop && scalar) {
      result->strlength = 0;
   }

   else if (!xop) {
      result_zero();
   }

   /* if invoked as xmlXPathEval(), convert to string now */
   else if (scalar) {
      s = xmlXPathCastToString(xop);
      xmlXPathFreeObject(xop);
      rxresize(result, strlen(s));
      memcpy(result->strptr, s, result->strlength);
   }

   /* otherwise, we either return a node set pointer or 0 */
   else if (xop->type == XPATH_NODESET) {
      bin_to_rxstring(result, xop);
   }

   else {
      result_zero();
      xmlXPathFreeObject(xop);
   }

   return 0;
}


/* compile an xpath expression
 *  ce = xmlCompileExpression(expression) */
rxfunc(xmlcompileexpression)
{
   xmlXPathCompExprPtr cep;
   char * expr;

   checkparam(1, 1);

   rxstrdup(expr, argv[0]);

   cep = xmlXPathCompile(expr);

   if (cep) {
      result->strptr[0] = '&';
      memcpy(result->strptr+1, &cep, sizeof(cep));
      result->strlength = sizeof(cep) + 1;
   }
   else {
      result_zero();
   }

   return 0;
}

/* free one or more pre-compiled expressions
 * call xmlFreeExpression exp1[, exp2,...] */
rxfunc(xmlfreeexpression)
{
   xmlXPathCompExprPtr cep;
   register int i;

   checkparam(1, -1);

   for (i = 0; i < argc; i++) {
      if (argv[i].strlength != (sizeof(cep) + 1) ||
          argv[i].strptr[0] != '&')
         return BADARGS;
   }


   for (i = 0; i < argc; i++) {
      memcpy(&cep, argv[i].strptr+1, sizeof(cep));
      xmlXPathFreeCompExpr(cep);
   }

   result_zero();
   return 0;
}


/* create a new xpath context from a document
 * ctxt = xmlNewContext(node) */
rxfunc(xmlnewcontext)
{
   xmlXPathContextPtr xpc;
   xmlNodePtr node;
   register int i;

   checkparam(1, -1);

   if (argv[0].strlength != sizeof(node)) {
      return BADARGS;
   }

   rxstring_to_bin(node, argv);

   xpc = xmlXPathNewContext(node->doc);

   if (xpc) {
      xpc->varLookupFunc = lookup_var;
      xpc->node  = node;

      bin_to_rxstring(result, xpc);

      for (i = 1; i < argc; i++) {
         char * nspre, *nsurl;

         nspre = xmlMalloc(argv[i].strlength+1);
         memcpy(nspre, argv[i].strptr, argv[i].strlength);
         nspre[argv[i].strlength] = 0;
         nsurl = strchr(nspre, '=');
         if (nsurl) {
            *nsurl++ = 0;

            xmlXPathRegisterNs(xpc, nspre, nsurl);
         }
         else {
            xmlFree(nspre);
         }
      }
   }
   else {
      result_zero();
   }

   return 0;
}


/* set the context pointer to a new node, if no context is given,
 * this operates on the global context
 * rc = xmlSetContext([context], node[, namespaces]) */
rxfunc(xmlsetcontext)
{
   xmlXPathContextPtr xpc;
   xmlNodePtr node;
   register int i;

   checkparam(2, -1);

   if (argv[1].strlength != sizeof(node) ||
       ((argv[0].strlength!=sizeof(xpc)) && (argv[0].strlength!=0)) ) {
      return BADARGS;
   }

   rxstring_to_bin(node, argv+1);

   if (argv[0].strlength) {
      rxstring_to_bin(xpc, argv);
   }
   else {
      if (gxpc == NULL) {
         gxpc = xmlXPathNewContext(node->doc);
         if (gxpc) {
            gxpc->varLookupFunc = lookup_var;
            gxpc->funcLookupFunc = lookup_func;
            gxpc->funcLookupData = gxpc;
         }
      }

      xpc = gxpc;
   }

   if (!xpc) {
      result_zero();
   }
   else {
      xpc->node  = node;
      xpc->doc = node->doc;
      result_one();
      for (i = 2; i < argc; i++) {
         char * nspre, *nsurl;

         nspre = xmlMalloc(argv[i].strlength+1);
         memcpy(nspre, argv[i].strptr, argv[i].strlength);
         nspre[argv[i].strlength] = 0;

         nsurl = strchr(nspre, '=');
         if (nsurl) {
            *nsurl++ = 0;
            xmlXPathRegisterNs(xpc, nspre, nsurl);
         }
         else {
            xmlFree(nspre);
         }
      }

   }

   return 0;
}


/* free a context that was allocated with xmlxpathcontext
 * call xmlFreeContext [ctxt][, ctxt...]
 */ 
rxfunc(xmlfreecontext)
{
   xmlXPathContextPtr xpc;
   register int i;

   for (i = 0; i < argc; i++)
      if (argv[i].strlength != sizeof(xpc))
         return BADARGS;

   if (!argc) {
      if (gxpc) {
         xmlXPathFreeContext(gxpc);
         gxpc = NULL;
      }
   }
   else {
      for (i = 0; i < argc; i++) {
         rxstring_to_bin(xpc, argv+i);
         xmlXPathFreeContext(xpc);
      }
   }


   return 0;
}


/* return the number of nodes in the node set
 * count = node = xmlNodeSetCount(ns) */
rxfunc(xmlnodesetcount)
{
   xmlXPathObjectPtr o;

   if (argc != 1 || argv[0].strlength != sizeof(o)) {
      return BADARGS;
   }

   rxstring_to_bin(o, argv);

   if (o->type == XPATH_NODESET) {
      result->strlength = sprintf(result->strptr, "%d", o->nodesetval ? o->nodesetval->nodeNr : 0);
   }
   else {
      result_zero();
   }
   
   return 0;
}

/* return the nth node in the node set
 * node = xmlNodeSetItem(ns, n) */
rxfunc(xmlnodesetitem)
{
   xmlXPathObjectPtr o;
   int node;
   char * nodes;

   if (argc != 2 || !argv[0].strptr || argv[0].strlength != sizeof(o)) {
      return BADARGS;
   }

   rxstring_to_bin(o, argv);

   rxstrdup(nodes, argv[1]);
   node = atoi(nodes) - 1;

   if (o->type == XPATH_NODESET && node >= 0 && node < o->nodesetval->nodeNr) {
      bin_to_rxstring(result, o->nodesetval->nodeTab[node]);
   }
   else {
      result_zero();
   }
   
   return 0;
}

/* adds a node to a node set. creates the node set if needed
 * ns = xmlNodeSetAdd([ns], [node], [...]) */
rxfunc(xmlnodesetadd)
{
   xmlXPathObjectPtr o;
   xmlNodePtr node;
   register int i;

   checkparam(0, -1);

   if ((argv[0].strlength != 0 && argv[0].strlength != sizeof(o)))
      return BADARGS;

   for (i = 2; i < argc; i++)
      if (argv[i].strlength != sizeof(node))
         return BADARGS;

   if (argc > 1)
      rxstring_to_bin(node, argv+1);
   else
      node = NULL;

   if (argv[0].strlength) {
      rxstring_to_bin(o, argv);
      if (o->type == XPATH_NODESET)
         xmlXPathNodeSetAdd(o->nodesetval, node);
      else
         o = NULL;
   }
   else
      o = xmlXPathNewNodeSet(node);

   if (!o) {
      result_zero();
   }
   else {
      for (i = 2; i < argc; i++) {
         rxstring_to_bin(node, argv+i);
         xmlXPathNodeSetAdd(o->nodesetval, node);
      }

      bin_to_rxstring(result, o);
   }
   
   return 0;
}

/* this is just an xpath command. If it returns a value, the value is
 * assigned to rc, which the caller is supposed to deal with. It's
 * basically the same as using xmlxpatheval, but with an environment
 * rather than function calls. */ 
static APIRET APIENTRY xPathEnv(PRXSTRING cmd, PUSHORT rc, PRXSTRING result)
{
   xmlXPathObjectPtr xop;
   int release = 1;
   char * cmds, *s;

   /* global context has to be allocated first */
   if (!gxpc) {
      *rc = 1;
      result_zero();
      return 0;
   }

   rxstrdup(cmds, cmd[0]);

   xop = xmlXPathEval(cmds, gxpc);

   if (!xop) {
      *rc = 1;
      result_zero();
      release = 0;
   }

   else {
      /* if the result of the expression is a scalar, we convert it to a
       * string and get rid of xop. Otherwise, we return xop, meaning the
       * caller is responsible for getting rid of it. */
      switch (xop->type) {

         /* treat this the same way as a NULL return value */
         case XPATH_UNDEFINED:
            *rc = 1;
            result_zero();
            break;

         case XPATH_NODESET:

         /* I don't know how to deal with any of these... */
         case XPATH_RANGE:
         case XPATH_LOCATIONSET:
         case XPATH_POINT:
         case XPATH_USERS:
         case XPATH_XSLT_TREE:
            release = 0;
            bin_to_rxstring(result, xop);
            break;
         case XPATH_BOOLEAN:
         case XPATH_NUMBER:
         case XPATH_STRING:
            s = xmlXPathCastToString(xop);
            rxresize(result, strlen(s));
            memcpy(result->strptr, s, result->strlength);
            break;

         default:
            *rc = 2;
            result_zero();
            break;
      }
   }

   if (release) {
      xmlXPathFreeObject(xop);
   }

   return 0;
}

static xmlXPathObjectPtr lookup_var(xmlXPathContextPtr ctxt, const xmlChar * name,
      const xmlChar * ns_uri)
{
   SHVBLOCK shv;
   xmlXPathObjectPtr o;

   memset(&shv, 0, sizeof(shv));

   shv.shvname.strptr = (unsigned char *)name;
   shv.shvname.strlength = strlen(name);
   shv.shvcode = RXSHV_SYFET;
   RexxVariablePool(&shv);

   /* regina null terminates. others might not */
   if (shv.shvvalue.strptr[shv.shvvalue.strlength] == 0)
      o = xmlXPathNewCString(shv.shvvalue.strptr);
   else {
      char * s = alloca(shv.shvvalue.strlength+1);
      memcpy(s, shv.shvvalue.strptr, shv.shvvalue.strlength);
      s[shv.shvvalue.strlength] = 0;
      o = xmlXPathNewCString(s);
   }
   REXXFREEMEMORY(shv.shvvalue.strptr);

   return o;
}

/* execute an external rexx function. This is similar to rexx:exec() from the
 * XSLT code, but the function name is taken from the XPath context.
 * The function name must be suitable for the interpreter to find it
 * (i.e., on Unix it must match the case of the routine's file name).
 * Arguments are passed as is.
 */

static void exec_fcn(xmlXPathParserContextPtr ctxt,
                      int argc)
{
   const char * fn = ctxt->context->function;
   PRXSTRING argv;
   RXSTRING result;
   xmlXPathObjectPtr o = NULL;
   register int i;
   short src;
   int rc;

   if (argc > 0) {
      argv = alloca(sizeof(*argv) * argc);
      for (i = argc-1; i >= 0; i--) {
         o = valuePop(ctxt);

         switch (o->type) {
            case XPATH_BOOLEAN:
            case XPATH_NUMBER:
            case XPATH_STRING:
               argv[i].strptr = xmlXPathCastToString(o);
               argv[i].strlength = strlen(argv[i].strptr);
               xmlFree(o);
               break;

            /* can't free o yet -- don't know when to do it */
            default:
               argv[i].strptr = xmlMalloc(sizeof(o));
               memcpy(argv[i].strptr, &o, sizeof(o));
               argv[i].strlength = sizeof(o);
         }
      }
   }
   else {
      argv = NULL;
   }

   result.strptr = REXXALLOCATEMEMORY(DEFAULTSTRINGSIZE+1);

   rc = RexxStart(argc, argv, fn, NULL, "system", RXFUNCTION, NULL, &src, &result);

   if (rc) {
      xmlParserError(ctxt, "Error %d evaluating external function %s\n", rc, fn);

      if (rc == -1) {
         xmlXPathSetError(ctxt, XPATH_INVALID_OPERAND);
      }
      else if (rc == -3) {
         xmlXPathSetError(ctxt, XPATH_UNKNOWN_FUNC_ERROR);
      }
      else {
         xmlXPathSetError(ctxt, XPATH_EXPR_ERROR);
      }
   }

   else {
      if (result.strptr) {
         result.strptr[result.strlength] = 0;
         xmlXPathReturnString(ctxt, result.strptr);
      }
      else {
         xmlXPathReturnEmptyString(ctxt);
      }
   }

   for (i = 0; i < argc; i++)
      xmlFree(argv[i].strptr);
}

/* chain the look-up. If the default XPath look-up function doesn't return
 * something, then assume it's a rexx function */
static xmlXPathFunction lookup_func(xmlXPathContextPtr ctxt, const xmlChar * name,
      const xmlChar * ns_uri)
{
   xmlXPathFunction fn;
   
   /* look over there for a moment */
   ctxt->funcLookupFunc = NULL;
   fn = xmlXPathFunctionLookup(ctxt, name);
   ctxt->funcLookupFunc = lookup_func;
   /* hmm, I could have sworn there was something interesting */

   if (!fn)
      fn = exec_fcn;

   return fn;
}

/* register the xpath environment */
void xpathinit(int init)
{
   RexxRegisterSubcomExe("XPATH", xPathEnv, NULL);
   if (init)
      xmlXPathInit();
}

void xpathfini(int init)
{
   RexxDeregisterSubcom("XPATH", NULL);
}
