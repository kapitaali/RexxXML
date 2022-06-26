/* test of adding an entity using the reverse copy method
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/addentity2.rex 1.2 2003/10/31 17:13:18 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

/* load the document to change */
tdoc = xmlParseXML('needrice.xml')

if tdoc = 0 then do
  say xmlError()
  exit 1
  end

/* create a document with the desired entity declaration and the
 * appropriate root element, but no actual data */
root = xmlEvalExpression('name(/*)', tdoc)

address xml
'<!DOCTYPE' root '[<!ENTITY rice "ordinary, boring rice">]><'root'/>'
edoc = xmlParseXML()

if edoc = 0 then do
  say xmlerror()
  exit 1
  end


/* now find the children of that document node, copy them, and graft
 * a copy on to edoc */

/* get rid of the stub root element */
call xmlExpandNode 'enode', edoc
call xmlExpandNode 'tnode', tdoc

call xmlAddNode enode.last, xmlCopyNode(tnode.children), 'replace'

call xmlSaveDoc 'hasrice.xml', edoc

call xmlFreeDoc tdoc, edoc
