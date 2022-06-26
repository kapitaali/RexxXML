# General Unix make file for RexxXML

#  The contents of this file are subject to the Mozilla Public License
#  Version 1.1 (the "License"); you may not use this file except in
#  compliance with the License. You may obtain a copy of the License at
#  http://www.mozilla.org/MPL/

#  Software distributed under the License is distributed on an "AS IS"
#  basis, WITHOUT WARRANTY OF ANY KIND, either express or implied. See the
#  License for the specific language governing rights and limitations
#  under the License.

#  The Original Code is RexxXML.

#  The Initial Developer of the Original Code is Patrick TJ McPhee.
#  Portions created by Patrick McPhee are Copyright © 2003
#  Patrick TJ McPhee. All Rights Reserved.

#  Contributors:

# $Header: C:/ptjm/rexx/rexxxml/RCS/Makefile.inc 1.9 2003/10/21 20:16:02 ptjm Rel $

DEBUG=-g $(PDEBUG)
OPT=

# path to rexxsaa.h
REXX_INCLUDE=-I/usr/local/include

# path to libxml/ and libxslt/ (which in my case is /usr/local/include,
# so I don't bother repeating it) (note libxml is for libxml2)
XML_INCLUDE=-I/usr/local/include/libxml2
XML_LIBDIR=-L/usr/local/lib

CFLAGS=$(DEBUG) $(OPT) $(PCFLAGS) $(PCDEFS) $(REXX_INCLUDE) $(XML_INCLUDE)
LDFLAGS=$(PLDFLAGS)
LIBS=$(XML_LIBDIR) -lxslt -lxml2 -lexslt $(PLIBS)
so=so

OBJ=rexxxml.o xmldoc.o xpath.o xmlnodes.o xslt.o schema.o fetch.o


all : librexxxml.$(so)


librexxxml.$(so) : $(OBJ)
	$(CC) $(CFLAGS) -o librexxxml.$(so) $(OBJ) $(LDFLAGS) $(LIBS)

clean :
	-rm *.o librexxxml.$(so) $(PCLEAN)
