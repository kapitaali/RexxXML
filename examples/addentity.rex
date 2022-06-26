/* test of adding an entity despite there being no API
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/addentity.rex 1.2 2003/10/31 17:13:14 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

/* create a document with the desired entity declaration */
address xml
'<!DOCTYPE data [<!ENTITY rice "ordinary, boring rice">]><data/>'
edoc = xmlParseXML()

call 'treewalk' edoc, 0, 'ricealone'

/* now find the internal subset and the entity declaration, which
 * will be the first child of the internal subset */

call xmlExpandnode 'enode', edoc

intss = enode.intsubset
rcc = xmlExpandNode('enode', intss)

do until \rcc | enode.type = 'ENTITY_DECL'
  rcc = xmlExpandNode('enode', enode.children)
  end

if rcc then entdecl = enode.self

/* now we load the new document */
doc = xmlParseXML('needrice.xml')

if doc = 0 then do
  say xmlError()
  exit 1
  end


/* Here's the part where you need to know something about the
 * node structure and pray. If there's an internal subset, then
 * we add the entity declaration as a child of that. Otherwise,
 * if there's an external subset, we add the internal subset
 * as an `after' sibling of that. Otherwise, if the document
 * node has children, we add the internal subset before those
 * children. Otherwise, we add the internal subset as the
 * only child. In any case, we only need to expand the document
 * node */

call xmlExpandNode 'tnode', doc

if tnode.intSubset \= 0 then do
   call xmlAddNode tnode.intSubset, entdecl
   end
else if tnode.extSubset \= 0 then
   call xmlAddNode tnode.extSubset, intss, 'before'
else if tnode.children \= 0 then
   call xmlAddNode tnode.children, intss, 'before'
else
   call xmlAddNode tnode.self, intss

call 'treewalk' doc, 0, 'hasrice'

call xmlSaveDoc 'hasrice.xml', doc

/* don't free edoc because of cross-linking issues */ 
call xmlFreeDoc doc
