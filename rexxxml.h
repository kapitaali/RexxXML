/* common include file for RexxXML
 * This includes the files that are needed by all modules, as well as
 * library-specific datatypes and prototypes.
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
 * $Header: C:/ptjm/rexx/rexxxml/RCS/rexxxml.h 1.9 2003/10/21 20:33:02 ptjm Rel $
 */

/* this loads all the libxml headers we need everywhere */
#include <libxml/HTMLparser.h>
#include <ctype.h>

/* this loads the Rexx headers plus the standard C library headers */
#include "rxproto.h"

/* union of all the node types */
typedef union {
    xmlNodePtr node;                    /* any type */
                                        /* XML_TEXT_NODE */
                                        /* XML_CDATA_SECTION_NODE */
                                        /* XML_ENTITY_REF_NODE */
                                        /* XML_PI_NODE */
                                        /* XML_COMMENT_NODE */
                                        /* XML_XINCLUDE_START */
                                        /* XML_XINCLUDE_END */
                                        /* XML_NOTATION_NODE */
    xmlAttrPtr attr;                    /* XML_ATTRIBUTE_NODE */
    xmlEntityPtr ent;                   /* XML_ENTITY_NODE */
                                        /* XML_ENTITY_DECL */
    xmlDocPtr doc;                      /* XML_DOCUMENT_NODE */
                                        /* XML_DOCB_DOCUMENT_NODE */
                                        /* XML_DOCUMENT_TYPE_NODE */
                                        /* XML_DOCUMENT_FRAG_NODE */
                                        /* XML_HTML_DOCUMENT_NODE */
    xmlDtdPtr dtd;                      /* XML_DTD_NODE */
    xmlElementPtr elemdecl;             /* XML_ELEMENT_DECL */
                                        /* XML_ELEMENT_NODE */
    xmlAttributePtr attrdecl;           /* XML_ATTRIBUTE_DECL */
    xmlNsPtr ns;                        /* XML_NAMESPACE_DECL */
} xmlnodeptr_t;

#define bin_to_rxstring(r, p) (memcpy((r)->strptr, &(p), sizeof(p)), (r)->strlength = sizeof(p))
#define rxstring_to_bin(p, r) memcpy(&(p), (r)->strptr, sizeof(p))

void rxErrorFn(void * ctxt, const char * msg, ...);


void xmlinit(int init);
void xmlfini(int init);

rxfunc(xmlparsexml);
rxfunc(xmlparsehtml);

rxfunc(xmlsavedoc);

rxfunc(xmlnewdoc);
rxfunc(xmlnewhtml);
rxfunc(xmlfreedoc);

rxfunc(xmlloadfuncs);
rxfunc(xmldropfuncs);

/* xpath functions */
void xpathinit(int init);
void xpathfini(int init);
rxfunc(xmlevalexpression);
rxfunc(xmlnodesetitem);
rxfunc(xmlnodesetcount);
rxfunc(xmlnodesetadd);
rxfunc(xmlnewcontext);
rxfunc(xmlsetcontext);
rxfunc(xmlfreecontext);
rxfunc(xmlcompileexpression);
rxfunc(xmlfreeexpression);

/* environments */
APIRET APIENTRY xmlEnv(PRXSTRING cmd, PUSHORT rc, PRXSTRING result);

/* tree manipulation */
rxfunc(xmladdelement);
rxfunc(xmladdtext);
rxfunc(xmladdattribute);
rxfunc(xmladdpi);
rxfunc(xmladdcomment);
rxfunc(xmladdnode);
rxfunc(xmlcopynode);
rxfunc(xmlremoveattribute);
rxfunc(xmlremovecontent);
rxfunc(xmlremovenode);

/* xslt */
void xsltinit(int init);
void xsltfini(int init);
rxfunc(xmlparsexslt);
rxfunc(xmlfreestylesheet);
rxfunc(xmlapplystylesheet);
rxfunc(xmloutputmethod);

/* schemas */
rxfunc(xmlparseschema);
rxfunc(xmlvalidatedoc);
rxfunc(xmlfreeschema);
rxfunc(xmldumpschema);
void xsdinit(int init);
void xsdfini(int init);

/* ftp/http interface */
rxfunc(xmlpost);
rxfunc(xmlget);
