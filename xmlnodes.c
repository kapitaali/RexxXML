/* document tree manipulation functions
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
 * $Header: C:/ptjm/rexx/rexxxml/RCS/xmlnodes.c 1.4 2003/10/07 03:26:34 ptjm Rel $
 */
/* this loads all the libxml and rexx headers we need */
#include "rexxxml.h"

static xmlNodePtr handleWhere(xmlNodePtr par, xmlNodePtr res, unsigned char where)
{
   if (!res)
      return res;


   switch (where) {
      case 'r':
      case 'R':
         xmlReplaceNode(par, res);
         xmlFreeNode(par);
         break;
      case 'b':
      case 'B':
         xmlAddPrevSibling(par, res);
         break;
      case 'a':
      case 'A':
         xmlAddNextSibling(par, res);
         break;
      default:
         xmlFreeNode(res);
         res = 0;
         break;
      }

      return res;
}




/* creates an element, optionally with text content, and adds it as a child
 * of the {node}. If {where} is set, the new elment is added as a sibling of
 * {node}, either `b'efore, `a'fter, or `r'eplacing it.
 * newnode = xmladdelement(node, name, [text], [where]) */
rxfunc(xmladdelement)
{
   xmlNodePtr par, res;
   char * name, *cont = NULL;

   checkparam(2, 4);

   if (!argv[0].strptr || ! argv[1].strptr || argv[0].strlength != sizeof(par)) {
      return 40;
   }

   rxstring_to_bin(par, argv);

   rxstrdup(name, argv[1]);

   if (argc > 2 && argv[2].strptr)
      rxstrdup(cont, argv[2]);

   if (argc > 3 && argv[3].strlength > 0) {
      res = handleWhere(par, xmlNewDocNode(par->doc, NULL, name, cont),
                        argv[3].strptr[0]);
   }

   else {
      res = xmlNewChild(par, NULL, name, cont);
   }

   if (!res)
      result_zero();
   else {
      bin_to_rxstring(result, res);
   }

   return 0;
}


/* xmladdattribute node, name, [value] */
rxfunc(xmladdattribute)
{
   xmlAttrPtr res;
   xmlNodePtr par;
   char * name, *cont;

   checkparam(2, 3);

   if (!argv[0].strptr || ! argv[1].strptr || argv[0].strlength != sizeof(par)) {
      return 40;
   }

   rxstring_to_bin(par, argv);

   rxstrdup(name, argv[1]);

   if (argc > 2)
      rxstrdup(cont, argv[2]);
   else
      cont = NULL;

   res = xmlSetProp(par, name, cont);

   if (!res)
      result_zero();
   else {
      bin_to_rxstring(result, res);
   }

   return 0;
}


/* xmladdtext node, text, [where] */
rxfunc(xmladdtext)
{
   xmlNodePtr par, res;

   checkparam(2, 3);

   if (!argv[0].strptr || ! argv[1].strptr || argv[0].strlength != sizeof(par)) {
      return 40;
   }

   rxstring_to_bin(par, argv);

   res = xmlNewTextLen(argv[1].strptr, argv[1].strlength);

   if (argc > 2 && argv[2].strlength > 0) {
      res = handleWhere(par, res, argv[2].strptr[0]);
   }
   else if (res) {
      xmlAddChild(par, res);
   }

   if (!res)
      result_zero();
   else {
      bin_to_rxstring(result, res);
   }

   return 0;
}

/* xmladdpi node, name, [text], [where] */
rxfunc(xmladdpi)
{
   xmlNodePtr par, res;
   char * name, *cont;

   checkparam(2, 4);

   if (!argv[0].strptr || ! argv[1].strptr || argv[0].strlength != sizeof(par)) {
      return BADARGS;
   }

   rxstring_to_bin(par, argv);

   rxstrdup(name, argv[1]);

   if (argc > 2)
      rxstrdup(cont, argv[2]);
   else
      cont = NULL;

   res = xmlNewPI(name, cont);

   if (argc > 3 && argv[3].strlength > 0) {
      res = handleWhere(par, res, argv[3].strptr[0]);
   }
   else if (res) {
      xmlAddChild(par, res);
   }


   if (!res)
      result_zero();
   else {
      bin_to_rxstring(result, res);
   }

   return 0;
}

/* xmladdcomment node, [text], [where] */
rxfunc(xmladdcomment)
{
   xmlNodePtr par, res;
   char *cont;

   checkparam(1, 3);

   if (!argv[0].strptr || argv[0].strlength != sizeof(par)) {
      return BADARGS;
   }

   rxstring_to_bin(par, argv);

   if (argc > 1)
      rxstrdup(cont, argv[1]);
   else
      cont = "";

   res = xmlNewComment(cont);

   if (argc > 2 && argv[2].strlength > 0) {
      res = handleWhere(par, res, argv[2].strptr[0]);
   }
   else if (res) {
      xmlAddChild(par, res);
   }


   if (!res)
      result_zero();
   else {
      bin_to_rxstring(result, res);
   }

   return 0;
}

/* xmladdnode node, child, [where] */
rxfunc(xmladdnode)
{
   xmlNodePtr par, res;

   checkparam(2, 3);

   if (argv[0].strlength != sizeof(par) || argv[1].strlength != sizeof(par)) {
      return BADARGS;
   }

   rxstring_to_bin(par, argv);
   rxstring_to_bin(res, argv+1);

   if (argc > 2 && argv[2].strlength > 0) {
      res = handleWhere(par, res, argv[2].strptr[0]);
   }
   else if (res) {
      xmlAddChild(par, res);
   }

   if (!res)
      result_zero();
   else {
      bin_to_rxstring(result, res);
   }

   return 0;
}

/* xmlcopynode node */
rxfunc(xmlcopynode)
{
   xmlNodePtr old, res;

   checkparam(1, 1);

   if (argv[0].strlength != sizeof(old)) {
      return BADARGS;
   }

   rxstring_to_bin(old, argv);

   res = xmlCopyNode(old, 1);

   if (!res)
      result_zero();
   else {
      bin_to_rxstring(result, res);
   }

   return 0;
}

/* xmlremoveattribute node, name[, name...] */
rxfunc(xmlremoveattribute)
{
   xmlNodePtr par;
   char * name;
   register int i;

   checkparam(1, -1);

   if (!argv[0].strptr || argv[0].strlength != sizeof(par)) {
      return 40;
   }

   rxstring_to_bin(par, argv);

   if (par->type != XML_ELEMENT_NODE)
      return 40;

   if (par->properties == NULL)
      return 0;

   if (argc > 1) {
      for (i = 1; i < argc; i++) {
         rxstrdup(name, argv[i]);
         xmlUnsetProp(par, name);
      }
   }
   else {
      xmlFreeNodeList((xmlNodePtr) par->properties);
      par->properties = NULL;
   }

   return 0;
}


/* xmlremovecontent node[, node,...] */
rxfunc(xmlremovecontent)
{
   xmlNodePtr par;
   register int i;

   checkparam(1, -1);

   for (i = 0; i < argc; i++)
      if (!argv[i].strptr || argv[i].strlength != sizeof(par)) {
         return 40;
      }

   for (i = 0; i < argc; i++) {
      rxstring_to_bin(par, argv+i);

      if (par->children == NULL)
         continue;

      xmlFreeNodeList(par->children);
      par->children = NULL;
   }

    return 0;
}

/* xmlremovenode node[, node,...]
 * unlinks nodes from tree and deletes them and their children */
rxfunc(xmlremovenode)
{
   xmlNodePtr par;
   register int i;

   checkparam(1, -1);

   for (i = 0; i < argc; i++)
      if (!argv[i].strptr || argv[i].strlength != sizeof(par)) {
         return 40;
      }

   for (i = 0; i < argc; i++) {
      rxstring_to_bin(par, argv+i);

      xmlUnlinkNode(par);
      xmlFreeNode(par);
   }

    return 0;
}
