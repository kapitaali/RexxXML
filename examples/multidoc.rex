/* test of searching more than one document with a single context
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/multidoc.rex 1.3 2003/10/31 17:17:04 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

files = 'moodnames.xml moodnames2.xml names.xml'

findname = xmlCompileExpression('//@name')

do i = 1 to words(files)

  doc = xmlParseXML(word(files, i))
  if doc = 0 then do
    say 'error' xmlError() 'opening' word(files,i)
    iterate
    end

  if i = 1 then ctxt =  xmlNewContext(doc, 'boo=http://boodle/poo')
  else call xmlSetContext ctxt, doc

  names = xmlFindNode(findname,,ctxt)
  do j = 1 to xmlNodesetCount(names)
     say xmlNodeContent(xmlNodesetItem(names, j))
     end
  call xmlFreeDoc doc
  end

call xmlFreeContext ctxt
call xmlFreeExpression findname
