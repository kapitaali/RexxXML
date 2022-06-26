/* test of XML being the anti-rexx, with node replacement
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/antirep.rex 1.2 2003/10/31 17:13:21 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

chunks.0 = 7
chunks.1 = 'hi'
chunks.2 = 23
chunks.3 = 'Fei'
chunks.4 = 'Patrick'
chunks.5 = 'Austin'
chunks.6 = 'Noodles'
chunks.7 = 'Egg rolls'

doc = xmlNewDoc()
node = xmlAddElement(doc, 'data')
call xmlAddComment node, ' Example wherein new elements replace each other '

node = xmlAddElement(node, 'chunk', 'huzzah')

do i = 1 to chunks.0
   node =  xmlAddElement(node, 'chunk', chunks.i, 'replace')
   end

say xmlSaveDoc( , doc)

call xmlAddAttribute node, 'noodle', 'poodle'

say xmlSaveDoc( , doc)

call xmlAddAttribute node, 'noodle', 'boodle'

say xmlSaveDoc( , doc)

call xmlRemoveAttribute node, 'noodle'
call xmlAddAttribute node, 'noodle'

say xmlSaveDoc( , doc)

call xmlFreeDoc doc

doc = xmlNewDoc()
node = xmlAddElement(doc, 'data')
call xmlAddComment node, ' Example wherein new elements follow each other '

node = xmlAddElement(node, 'chunk', 'huzzah')

do i = 1 to chunks.0
   node =  xmlAddElement(node, 'chunk', chunks.i, 'after')
   end

say xmlSaveDoc( , doc)

call xmlFreeDoc doc


doc = xmlNewDoc()
node = xmlAddElement(doc, 'data')
call xmlAddComment node, ' Example wherein new elements precede each other '

node = xmlAddElement(node, 'chunk', 'huzzah')

do i = 1 to chunks.0
   node =  xmlAddElement(node, 'chunk', chunks.i, 'before')
   end

say xmlSaveDoc( , doc)

call xmlFreeDoc doc

doc = xmlNewDoc()
node = xmlAddElement(doc, 'data')
call xmlAddComment node, ' Example wherein new elements precede the first node, giving the effect that they follow each other, except for the first node, which seems to follow all the others '

node = xmlAddElement(node, 'chunk', 'huzzah')

do i = 1 to chunks.0
   call  xmlAddElement node, 'chunk', chunks.i, 'before'
   end

say xmlSaveDoc( , doc)

call xmlFreeDoc doc

