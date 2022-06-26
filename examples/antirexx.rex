/* test of XML being the anti-rexx
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/antirexx.rex 1.2 2003/10/31 17:13:25 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

doc = xmlNewDoc()
node = xmlAddElement(doc, 'data')

chunks.0 = 7
chunks.1 = 'hi'
chunks.2 = 23
chunks.3 = 'Fei'
chunks.4 = 'Patrick'
chunks.5 = 'Austin'
chunks.6 = 'Noodles'
chunks.7 = 'Egg rolls'

do i = 1 to chunks.0
   call xmlAddElement node, 'chunk', chunks.i
   end

say xmlSaveDoc( , doc)

call xmlAddAttribute node, 'noodle', 'poodle'
call xmlRemoveAttribute node, 'noodle'
call xmlAddAttribute node, 'noodle', 'boodle'

say xmlSaveDoc( , doc)

call xmlFreeDoc doc
