/* Common functions for RexxXML. This contains all the exported functions,
 *   the error functions, and xmlexpandnode and xmlnodecontent, which are
 *   here for historical reasons.
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
 * $Header: C:/ptjm/rexx/rexxxml/RCS/rexxxml.c 1.20 2003/10/31 16:51:32 ptjm Rel $
 */

/* this loads all the libxml headers we need */

#include "rexxxml.h"
#include <libxslt/xslt.h>

#include <stdarg.h>

static int content_to_string(char * s, int l, const xmlElementContentPtr p)
{
   int i = 0, j;

   if (p) {
      if (p->c1 || p->c2) {
         s[i++] = '(';
         i += content_to_string(s+1, l-1, p->c1);
      }

      switch (p->type) {
         case XML_ELEMENT_CONTENT_PCDATA:
            memcpy(s+i, "#PCDATA", 7);
            i += 7;
            break;
         case XML_ELEMENT_CONTENT_ELEMENT:
            j = strlen(p->name);
            memcpy(s+i, p->name, j);
            i += j;
            break;
         case XML_ELEMENT_CONTENT_SEQ:
            s[i++] = ',';
            break;
         case XML_ELEMENT_CONTENT_OR:
            s[i++] = '|';
            break;
      }

      if (p->c1 || p->c2) {
         i += content_to_string(s+i, l-i, p->c2);
         s[i++] = ')';
      }

      switch (p->ocur) {
         case XML_ELEMENT_CONTENT_ONCE:
            break;
         case XML_ELEMENT_CONTENT_OPT:
            s[i++] = '?';
            break;
         case XML_ELEMENT_CONTENT_MULT:
            s[i++] = '*';
            break;
         case XML_ELEMENT_CONTENT_PLUS:
            s[i++] = '+';
            break;
      }
   }

   s[i] = 0;
   return i;
}

/* properties get set with tail .A.<propertyname>. I wanted to use
 * @<propertyname>, but while this works with Regina and
 * Rexx/IMC, it doesn't work with object rexx */
static void setprops(PRXSTRING stem, xmlAttrPtr prop)
{
   SHVBLOCK shv;
   RXSTRING name, names;
   register int i, l;
   static const char attrpfx[] = ".A.";

   memset(&shv, 0, sizeof(shv));

   /* buffer for the attribute names. Anybody with attribute names > 2048 bytes long,
    * deserves to crash. We set this up as `stem.A.', then repeatedly append
    * the attribute names. */
   name.strptr = alloca(2048);

   /* list of attribute names, assigned to stem.attrpfx */
   names.strptr = NULL;
   names.strlength = 0;

   for (i = 0; i < stem->strlength; i++) {
      name.strptr[i] = toupper(stem->strptr[i]);
   }

   if (name.strptr[stem->strlength-1] == '.') {
      memcpy(name.strptr+stem->strlength, attrpfx+1, sizeof(attrpfx)-2);
      name.strlength = stem->strlength + sizeof(attrpfx)-2;
   }
   else {
      memcpy(name.strptr+stem->strlength, attrpfx, sizeof(attrpfx)-1);
      name.strlength = stem->strlength + sizeof(attrpfx)-1;
   }

   shv.shvcode = RXSHV_SET;
   shv.shvname = name;

   for (; prop; prop = prop->next) {

      if (prop->type != XML_ATTRIBUTE_NODE)
         continue;

      shv.shvvalue.strptr = xmlNodeGetContent((xmlNodePtr)prop);
      if (shv.shvvalue.strptr) {
         shv.shvvalue.strlength = strlen(shv.shvvalue.strptr);
      }
      else {
         shv.shvvalue.strlength = 0;
      }

      shv.shvname.strlength = name.strlength + (l = strlen(prop->name));

      for (i = name.strlength; i < shv.shvname.strlength; i++) {
         name.strptr[i] = toupper(prop->name[i-name.strlength]);
      }

      RexxVariablePool(&shv);

      names.strptr = realloc(names.strptr, names.strlength+1+l);
      if (names.strlength) {
         names.strptr[names.strlength++] = ' ';
      }
      memcpy(names.strptr+names.strlength, shv.shvname.strptr+name.strlength, l);
      names.strlength += l;
   }

   if (names.strlength) {
      shv.shvvalue = names;
      shv.shvname.strlength = name.strlength - 1;
      RexxVariablePool(&shv);
      free(names.strptr);
   }
}


static void attrtype(SHVBLOCK * pshv, int attrtype)
{
   switch (attrtype) {
      case XML_ATTRIBUTE_CDATA: {
         static const char nm[] = "ATTRIBUTE_CDATA";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_ID: {
         static const char nm[] = "ATTRIBUTE_ID";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_IDREF: {
         static const char nm[] = "ATTRIBUTE_IDREF";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_IDREFS: {
         static const char nm[] = "ATTRIBUTE_IDREFS";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_ENTITY: {
         static const char nm[] = "ATTRIBUTE_ENTITY";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_ENTITIES: {
         static const char nm[] = "ATTRIBUTE_ENTITIES";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_NMTOKEN: {
         static const char nm[] = "ATTRIBUTE_NMTOKEN";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_NMTOKENS: {
         static const char nm[] = "ATTRIBUTE_NMTOKENS";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_ENUMERATION: {
         static const char nm[] = "ATTRIBUTE_ENUMERATION";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
      case XML_ATTRIBUTE_NOTATION: {
         static const char nm[] = "ATTRIBUTE_NOTATION";
         pshv->shvvalue.strptr = (unsigned char *)nm;
         pshv->shvvalue.strlength = sizeof(nm) - 1;
         break;
      }
   }
}



/* guts of xmlexpandnode. This was originally put in a separate function
 * because I had a lot of functions returning expanded nodes. I decided the
 * expanded nodes weren't all that convenient, so I stopped doing that. */
static void setnode(PRXSTRING stem, xmlnodeptr_t node)
{
   unsigned const char * content, *nshref, *nsprefix, *ExternalID, *SystemID, *URL;
   unsigned char * ctnt;
   RXSTRING type, zero;
   xmlAttrPtr properties;
   unsigned char buf[1024], namebuf[1024];
   SHVBLOCK shv[30];
   register int i, l, bl, j;

   if (node.node) {
      struct {
         unsigned char name[11];
         void ** ptr;
      } ptrs[] = {
         { "SELF", (void **)&node.node },
         { "NEXT", (void **)&node.node->next },
         { "PREV", (void **)&node.node->prev },
         { "CHILDREN", (void **)&node.node->children },
         { "LAST", (void **)&node.node->last },
         { "PARENT", (void **)&node.node->parent },
         { "ATTRIBUTES", (void **)&properties }
      };

      zero.strptr = "0";
      zero.strlength = 1;

      content = node.node->content;
      properties = node.node->properties;

      /* these are not supported by all node types ... */
      nshref = nsprefix = ExternalID = SystemID = URL = NULL;
      type.strptr = NULL;

      memset(shv, 0, sizeof(shv));
      shv[0].shvname = *stem;
      shv[0].shvcode = RXSHV_DROPV;
      shv[0].shvnext = shv + 1;

      i = 1;
      l = 0;
      bl = 0;

      if (node.node->name) {
         shv[i].shvname.strptr = namebuf + l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sNAME", (int)stem->strlength, stem->strptr)) + 1;
         shv[i].shvvalue.strptr = (unsigned char *)node.node->name;
         shv[i].shvvalue.strlength = strlen(node.node->name);

         shv[i].shvcode = RXSHV_SET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }

      switch (node.node->type) {
         case XML_ELEMENT_NODE: {
            static const unsigned char nm[] = "ELEMENT_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            if (node.node->ns) {
               nshref = node.node->ns->href;
               nsprefix = node.node->ns->prefix;
            }
            break;
         }
         case XML_ATTRIBUTE_NODE: {
            static const unsigned char nm[] = "ATTRIBUTE_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            if (node.node->ns) {
               nshref = node.node->ns->href;
               nsprefix = node.node->ns->prefix;
            }
            properties = NULL; /* past the end of node.attr */
            content = NULL;

            attrtype(shv+i, node.attr->atype);

            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sATYPE", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;
            break;
         }
         case XML_TEXT_NODE: {
            static const unsigned char nm[] = "TEXT_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         case XML_CDATA_SECTION_NODE: {
            static const unsigned char nm[] = "CDATA_SECTION_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         case XML_ENTITY_REF_NODE: {
            static const unsigned char nm[] = "ENTITY_REF_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         case XML_ENTITY_DECL: {
            static const unsigned char nm[] = "ENTITY_DECL";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            goto entnode;
         }
         case XML_ENTITY_NODE: {
            static const unsigned char nm[] = "ENTITY_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;

            entnode:

            properties = NULL;          /* node.ent->length */
            ExternalID = node.ent->ExternalID;
            SystemID = node.ent->SystemID;
            URL = node.ent->URI;

            switch (node.ent->etype) {
               case XML_INTERNAL_GENERAL_ENTITY: {
                  static const char nm[] = "INTERNAL_GENERAL_ENTITY";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }

               case XML_EXTERNAL_GENERAL_PARSED_ENTITY: {
                  static const char nm[] = "EXTERNAL_GENERAL_PARSED_ENTITY";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_EXTERNAL_GENERAL_UNPARSED_ENTITY: {
                  static const char nm[] = "EXTERNAL_GENERAL_UNPARSED_ENTITY";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_INTERNAL_PARAMETER_ENTITY: {
                  static const char nm[] = "INTERNAL_PARAMETER_ENTITY";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_EXTERNAL_PARAMETER_ENTITY: {
                  static const char nm[] = "EXTERNAL_PARAMETER_ENTITY";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_INTERNAL_PREDEFINED_ENTITY: {
                  static const char nm[] = "INTERNAL_PREDEFINED_ENTITY";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               default:  {
                  static const char nm[] = "unknown";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
               }
            }

            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sETYPE", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;
            break;
         }
         case XML_PI_NODE: {
            static const unsigned char nm[] = "PI_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         case XML_COMMENT_NODE: {
            static const unsigned char nm[] = "COMMENT_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         case XML_HTML_DOCUMENT_NODE: {
            static const unsigned char nm[] = "HTML_DOCUMENT_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            goto docnode;
         }
         case XML_DOCUMENT_NODE: {
            static const unsigned char nm[] = "DOCUMENT_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            goto docnode;
         }
         case XML_DOCUMENT_FRAG_NODE: {
            static const unsigned char nm[] = "DOCUMENT_FRAG_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            docnode:

            content = NULL;
            properties = NULL;

            if (node.doc->oldNs) {
               nshref = node.doc->oldNs->href;
               nsprefix = node.doc->oldNs->prefix;
            }

            shv[i].shvvalue.strptr = buf+bl;
            bl += (shv[i].shvvalue.strlength = sprintf(buf+bl, "%d", node.doc->compression)) + 1;
            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sCOMPRESSION", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;

            shv[i].shvvalue.strptr = buf+bl;
            bl += (shv[i].shvvalue.strlength = sprintf(buf+bl, "%d", node.doc->standalone)) + 1;
            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sSTANDALONE", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;

            if (node.doc->intSubset) {
               shv[i].shvvalue.strptr = (unsigned char *)&node.doc->intSubset;
               shv[i].shvvalue.strlength = sizeof(node.doc->intSubset);
            }
            else {
               shv[i].shvvalue = zero;
            }
            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sINTSUBSET", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;

            if (node.doc->extSubset) {
               shv[i].shvvalue.strptr = (unsigned char *)&node.doc->extSubset;
               shv[i].shvvalue.strlength = sizeof(node.doc->extSubset);
            }
            else {
               shv[i].shvvalue = zero;
            }
            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sEXTSUBSET", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;

            if (node.doc->version) {
               shv[i].shvvalue.strptr = (unsigned char *)node.doc->version;
               shv[i].shvvalue.strlength = strlen(node.doc->version);
               shv[i].shvname.strptr = namebuf + l;
               l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sVERSION", (int)stem->strlength, stem->strptr)) + 1;
               shv[i].shvcode = RXSHV_SYSET;
               shv[i].shvnext = shv + i + 1;
               i++;
            }

            if (node.doc->encoding) {
               shv[i].shvvalue.strptr = (unsigned char *)node.doc->encoding;
               shv[i].shvvalue.strlength = strlen(node.doc->encoding);
               shv[i].shvname.strptr = namebuf + l;
               l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sENCODING", (int)stem->strlength, stem->strptr)) + 1;
               shv[i].shvcode = RXSHV_SYSET;
               shv[i].shvnext = shv + i + 1;
               i++;
            }

            URL = node.doc->URL;
            break;
         }
         case XML_NOTATION_NODE: {
            static const unsigned char nm[] = "NOTATION_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         case XML_DOCUMENT_TYPE_NODE: {
            static const unsigned char nm[] = "DOCUMENT_TYPE_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;

            ExternalID = node.dtd->ExternalID;
            SystemID = node.dtd->SystemID;
            break;
         }
         case XML_DTD_NODE: {
            static const unsigned char nm[] = "DTD_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            content = NULL;
            properties = NULL;

            ExternalID = node.dtd->ExternalID;
            SystemID = node.dtd->SystemID;
            break;
         }
         case XML_ELEMENT_DECL: {
            static const unsigned char nm[] = "ELEMENT_DECL";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;

            /* the content of an element declaration is a tree containing the
             * content model */
            ctnt = alloca(1024);
            content_to_string(ctnt, 1024, node.elemdecl->content);
            if (!*ctnt)
               content = NULL;
            else
               content = ctnt;

            nsprefix = node.elemdecl->prefix;
            switch (node.elemdecl->etype) {
               case XML_ELEMENT_TYPE_UNDEFINED: {
                  static const char nm[] = "ELEMENT_TYPE_UNDEFINED";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_ELEMENT_TYPE_EMPTY: {
                  static const char nm[] = "ELEMENT_TYPE_EMPTY";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_ELEMENT_TYPE_ANY: {
                  static const char nm[] = "ELEMENT_TYPE_ANY";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_ELEMENT_TYPE_MIXED: {
                  static const char nm[] = "ELEMENT_TYPE_MIXED";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_ELEMENT_TYPE_ELEMENT: {
                  static const char nm[] = "ELEMENT_TYPE_ELEMENT";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               default: {
                  static const char nm[] = "unknown";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
            }

            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sETYPE", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;

            break;
         }
         case XML_ATTRIBUTE_DECL: {
            static const unsigned char nm[] = "ATTRIBUTE_DECL";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            content = node.attrdecl->defaultValue;
            properties = NULL;
            nsprefix = node.attrdecl->prefix;

            switch (node.attrdecl->def) {
               case XML_ATTRIBUTE_NONE: {
                  static const char nm[] = "ATTRIBUTE_NONE";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_ATTRIBUTE_REQUIRED: {
                  static const char nm[] = "ATTRIBUTE_REQUIRED";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_ATTRIBUTE_IMPLIED: {
                  static const char nm[] = "ATTRIBUTE_IMPLIED";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
               case XML_ATTRIBUTE_FIXED: {
                  static const char nm[] = "ATTRIBUTE_FIXED";
                  shv[i].shvvalue.strptr = (unsigned char *)nm;
                  shv[i].shvvalue.strlength = sizeof(nm) - 1;
                  break;
               }
            }

            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sMANDATORY", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;

            attrtype(shv+i, node.attrdecl->atype);

            shv[i].shvname.strptr = namebuf + l;
            l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sATYPE", (int)stem->strlength, stem->strptr)) + 1;
            shv[i].shvcode = RXSHV_SYSET;
            shv[i].shvnext = shv + i + 1;
            i++;

            /* node.attrdecl->tree */

            break;
         }
         case XML_NAMESPACE_DECL: {
            static const unsigned char nm[] = "NAMESPACE_DECL";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         case XML_XINCLUDE_START: {
            static const unsigned char nm[] = "XINCLUDE_START";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         case XML_XINCLUDE_END: {
            static const unsigned char nm[] = "XINCLUDE_END";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
         default: {
            static const unsigned char nm[] = "UNKNOWN_NODE";
            type.strptr = (unsigned char *)nm;
            type.strlength = sizeof(nm)-1;
            break;
         }
      }

      if (content) {
         shv[i].shvname.strptr = namebuf+l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sCONTENT", (int)stem->strlength, stem->strptr)) + 1;
         shv[i].shvvalue.strptr = (unsigned char *)content;
         shv[i].shvvalue.strlength = strlen(content);

         shv[i].shvcode = RXSHV_SET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }
      
      if (type.strptr) {
         shv[i].shvname.strptr = namebuf+l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sTYPE", (int)stem->strlength, stem->strptr)) + 1;
         shv[i].shvvalue = type;

         shv[i].shvcode = RXSHV_SET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }
      

      if (ExternalID) {
         shv[i].shvvalue.strptr = (unsigned char *)ExternalID;
         shv[i].shvvalue.strlength = strlen(ExternalID);
         shv[i].shvname.strptr = namebuf + l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sEXTERNALID", (int)stem->strlength, stem->strptr)) + 1;
         shv[i].shvcode = RXSHV_SYSET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }

      if (SystemID) {
         shv[i].shvvalue.strptr = (unsigned char *)SystemID;
         shv[i].shvvalue.strlength = strlen(SystemID);
         shv[i].shvname.strptr = namebuf + l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sSYSTEMID", (int)stem->strlength, stem->strptr)) + 1;
         shv[i].shvcode = RXSHV_SYSET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }

      if (URL) {
         shv[i].shvvalue.strptr = (unsigned char *)URL;
         shv[i].shvvalue.strlength = strlen(URL);
         shv[i].shvname.strptr = namebuf + l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sURL", (int)stem->strlength, stem->strptr)) + 1;
         shv[i].shvcode = RXSHV_SYSET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }

      if (nsprefix) {
         shv[i].shvvalue.strptr = (unsigned char *)nsprefix;
         shv[i].shvvalue.strlength = strlen(nsprefix);
         shv[i].shvname.strptr = namebuf + l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sNAMESPACEPREFIX", (int)stem->strlength, stem->strptr)) + 1;
         shv[i].shvcode = RXSHV_SYSET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }

      if (nshref) {
         shv[i].shvvalue.strptr = (unsigned char *)nshref;
         shv[i].shvvalue.strlength = strlen(nshref);
         shv[i].shvname.strptr = namebuf + l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*sNAMESPACEURL", (int)stem->strlength, stem->strptr)) + 1;
         shv[i].shvcode = RXSHV_SYSET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }

      /* we always need to set these. When they're null, set to 0 because
       * there's no good, portable way to test for a null pointer in
       * Rexx. */

      for (j = 0; j < DIM(ptrs); j++) {
         shv[i].shvname.strptr = namebuf+l;
         l += (shv[i].shvname.strlength = sprintf(namebuf+l, "%.*s%s",
                    (int)stem->strlength, stem->strptr, ptrs[j].name)) + 1;
         if (*ptrs[j].ptr) {
            shv[i].shvvalue.strptr = (unsigned char *)ptrs[j].ptr;
            shv[i].shvvalue.strlength = sizeof(*ptrs[j].ptr);
         }
         else {
            shv[i].shvvalue = zero;
         }
         shv[i].shvcode = RXSHV_SET;
         shv[i].shvnext = shv + i + 1;
         i++;
      }

      shv[i-1].shvnext = NULL;
      
      RexxVariablePool(shv);
      setprops(stem, properties);
   }
}


/* put a node pointer into a stem, with properties expanded
 * rc = xmlExpandNode(stem, node)
 */

static rxfunc(xmlexpandnode)
{
   xmlnodeptr_t doc;
   RXSTRING stem;

   if (argc != 2 || !argv[0].strptr || ! argv[1].strptr) {
      return BADARGS;
   }

   if (argv[1].strlength != sizeof(doc)) {
      return BADARGS;
   }

   rxstring_to_bin(doc, argv+1);

   if (argv[0].strptr[argv[0].strlength-1] == '.')
      stem = argv[0];
   else {
      stem.strptr = alloca(argv[0].strlength+1);
      memcpy(stem.strptr, argv[0].strptr, argv[0].strlength);
      stem.strptr[argv[0].strlength] = '.';
      stem.strlength = argv[0].strlength + 1;
   }

   if (doc.doc) {
      setnode(&stem, doc);
      result_one();
   }
   else {
      result_zero();
   }

   return 0;
}

/* returns a string with the content of the argument node */
static rxfunc(xmlnodecontent)
{
   xmlnodeptr_t node;
   char * content;

   checkparam(1, 1);
   
   if (argv[0].strlength != sizeof(node)) {
      return BADARGS;
   }

   rxstring_to_bin(node, argv);

   content = xmlNodeGetContent(node.node);
   if (content) {
      rxresize(result, strlen(content));
      memcpy(result->strptr, content, result->strlength);
   }
   else
      result->strlength = 0;

   return 0;
}


/* xmlFree thing, [thing2, ... */
rxfunc(xmlfree)
{
   register int i;
   void * ptr;

   for (i = 0; i < argc; i++) {
      if (argv[i].strlength != sizeof(ptr))
         return BADARGS;
   }

   for (i = 0; i < argc; i++) {
      rxstring_to_bin(ptr, argv+i);
      xmlFree(ptr);
   }


   result->strlength = 0;
   return 0;
}


rxfunc(xmlversion)
{
   result->strlength = sprintf(result->strptr, "1.0.0 %s %s", xmlParserVersion, xsltEngineVersion);
   return 0;
}

static char * errs;
static int errc, erra;

#ifdef _WIN32
/* compilers using the microsoft C run-time need to have vsnprintf defined.
 * MS provides _vsnprintf, but it doesn't do the same thing as the ANSI
 * function */
int __cdecl vsnprintf(char * s, size_t l, const char * fmt, va_list cdr)
{
   int len = _vsnprintf(s, l, fmt, cdr);
   int bufl;
   char * buf;

   if (len == -1) {
      bufl = l+1;
      buf = NULL;

      while (len == -1) {
         bufl *= 2;
         buf = realloc(buf, bufl);
         len = _vsnprintf(buf, bufl, fmt, cdr);
      }

      /* don't need to copy since the appropriate data is already written to
       * s */
      free(buf);
   }

   return len;
}
#endif


void rxErrorFn(void * ctxt, const char * msg, ...)
{
   va_list val;
   int need;

   va_start(val, msg);

   need = vsnprintf(errs+errc, erra-errc, msg, val);

   if ((erra-errc) < need) {
      erra += need+1024;
      errs = realloc(errs, erra);
      need = vsnprintf(errs+errc, erra-errc, msg, val);
   }

   errc += need;

   va_end(val);
}

static rxfunc(xmlerror)
{
   rxresize(result, errc);
   memcpy(result->strptr, errs, errc);
   errc = 0;
   return 0;
}


static struct {
    char * name;
    RexxFunctionHandler *funcptr;
} funclist[] = {
   "XMLPARSEXML", (RexxFunctionHandler *)xmlparsexml,
   "XMLPARSEHTML", (RexxFunctionHandler *)xmlparsehtml,
   "XMLEXPANDNODE", (RexxFunctionHandler *)xmlexpandnode,
   "XMLNODECONTENT", (RexxFunctionHandler *)xmlnodecontent,
   "XMLADDELEMENT", (RexxFunctionHandler *)xmladdelement,
   "XMLADDATTRIBUTE", (RexxFunctionHandler *)xmladdattribute,
   "XMLADDTEXT", (RexxFunctionHandler *)xmladdtext,
   "XMLADDPI", (RexxFunctionHandler *)xmladdpi,
   "XMLADDCOMMENT", (RexxFunctionHandler *)xmladdcomment,
   "XMLADDNODE", (RexxFunctionHandler *)xmladdnode,
   "XMLCOPYNODE", (RexxFunctionHandler *)xmlcopynode,
   "XMLREMOVEATTRIBUTE", (RexxFunctionHandler *)xmlremoveattribute,
   "XMLREMOVECONTENT", (RexxFunctionHandler *)xmlremovecontent,
   "XMLREMOVENODE", (RexxFunctionHandler *)xmlremovenode,
   "XMLEVALEXPRESSION", (RexxFunctionHandler *)xmlevalexpression,
   "XMLFINDNODE", (RexxFunctionHandler *)xmlevalexpression,
   "XMLNODESETITEM", (RexxFunctionHandler *)xmlnodesetitem,
   "XMLNODESETCOUNT", (RexxFunctionHandler *)xmlnodesetcount,
   "XMLNODESETADD", (RexxFunctionHandler *)xmlnodesetadd,
   "XMLNEWCONTEXT", (RexxFunctionHandler *)xmlnewcontext,
   "XMLSETCONTEXT", (RexxFunctionHandler *)xmlsetcontext,
   "XMLFREECONTEXT", (RexxFunctionHandler *)xmlfreecontext,
   "XMLCOMPILEEXPRESSION", (RexxFunctionHandler *)xmlcompileexpression,
   "XMLFREEEXPRESSION", (RexxFunctionHandler *)xmlfreeexpression,
   "XMLFREE", (RexxFunctionHandler *)xmlfree,
   "XMLSAVEDOC", (RexxFunctionHandler *)xmlsavedoc,
   "XMLVERSION", (RexxFunctionHandler *)xmlversion,
   "XMLERROR", (RexxFunctionHandler *)xmlerror,
   "XMLNEWDOC", (RexxFunctionHandler *)xmlnewdoc,
   "XMLNEWHTML", (RexxFunctionHandler *)xmlnewhtml,
   "XMLFREEDOC", (RexxFunctionHandler *)xmlfreedoc,
   "XMLPARSEXSLT", (RexxFunctionHandler *)xmlparsexslt,
   "XMLFREESTYLESHEET", (RexxFunctionHandler *)xmlfreestylesheet,
   "XMLAPPLYSTYLESHEET", (RexxFunctionHandler *)xmlapplystylesheet,
   "XMLOUTPUTMETHOD", (RexxFunctionHandler *)xmloutputmethod,
   "XMLPARSESCHEMA", (RexxFunctionHandler *)xmlparseschema,
   "XMLVALIDATEDOC", (RexxFunctionHandler *)xmlvalidatedoc,
   "XMLFREESCHEMA", (RexxFunctionHandler *)xmlfreeschema,
   "XMLDUMPSCHEMA", (RexxFunctionHandler *)xmldumpschema,
   "XMLPOST", (RexxFunctionHandler *)xmlpost,
   "XMLGET", (RexxFunctionHandler *)xmlget,
   "XMLDROPFUNCS", (RexxFunctionHandler *)xmldropfuncs,
   "XMLLOADFUNCS", (RexxFunctionHandler *)xmlloadfuncs,
};

static int noinit = 0;

void rexxXMLInit(int initparser)
{
   register int i;

   
   for (i = 0; i < DIM(funclist); i++) {
      RexxRegisterFunctionExe(funclist[i].name, funclist[i].funcptr);
   }

   xmlinit(initparser);
   xpathinit(initparser);
   xsltinit(initparser);
   xsdinit(initparser);

   if (initparser) {
      xmlGenericError = rxErrorFn;
   }
   else {
      noinit = 1;
   }
}


/* the only valid entry point, since it's the place initparser is called */
rxfunc(xmlloadfuncs)
{
   checkparam(0,1);

   rexxXMLInit(argc == 0);

   result_zero();

   return 0;
}

void rexxXMLFini(void)
{
   xmlfini(!noinit);
   xpathfini(!noinit);
   xsltfini(!noinit);
   xsdfini(!noinit);
}


rxfunc(xmldropfuncs)
{
   checkparam(0,0);

   rexxXMLFini();

   result_zero();

   return 0;
}

