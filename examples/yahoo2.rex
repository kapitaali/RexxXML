/* submit a yahoo search
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/yahoo2.rex 1.1 2003/10/27 18:59:27 ptjm Rel $
 */

parse arg f .

if f = '' then f = 'yahoo2.html'


rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

doc = xmlParseHTML(f)

lis = xmlFindNode('//li', doc)

do i = 1 to xmlNodesetCount(lis)
  li = xmlNodesetItem(lis, i)
  cont = xmlNodeContent(li)
  href = xmlEvalExpression('a[1]/@href', li)
  parse var href . '*-' href
  say href
  say cont
  end

nxt = xmlEvalExpression("//div[@id='page']/table/tr/td/a[1]/@href", doc)
say nxt

nxt = xmlEvalExpression("//div[@id='page']/table/tr/td/a[starts-with(., 'next')]/@href", doc)
say nxt
