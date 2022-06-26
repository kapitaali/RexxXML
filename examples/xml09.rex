/* test of xmlNodeSetContent
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/xml09.rex 1.2 2003/10/31 17:19:36 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

doc = xmlParseXML('poem.xml')

dms = xmlFindNode('//line[2]|//line[6]', doc)

do i = 1 to xmlNodeSetCount(dms)
   say i xmlNodeContent(xmlNodeSetItem(dms, i))
   end

x = xmlEvalExpression('17 div 3', doc)

if x = 0 then say xmlError() 'boop'
else say x

exit 0
