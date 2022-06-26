/* submit a yahoo search
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/yahoo3.rex 1.1 2003/10/27 20:56:57 ptjm Rel $
 */

parse arg f .

if f = '' then f = 'yahoo3.html'


rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

doc = xmlParseHTML(f)

lis = xmlFindNode("//span[@class='ygbody']/big", doc)

do i = 1 to xmlNodesetCount(lis)
  li = xmlNodesetItem(lis, i)
  cont = xmlNodeContent(li)
  nxt = xmlFindNode('following-sibling::node()', li)
  ohref = xmlEvalExpression('a[1]/@href', li)
  parse var ohref . '*-' href
  if href = '' then href = ohref

  if nxt \= 0 then do
    do j = 1 to xmlNodesetCount(nxt)
      n = xmlNodesetItem(nxt, j)
      if xmlEvalExpression('boolean(self::big)', n) = 'true' then leave j
      cont = cont || xmlNodeContent(n)
      end
    call xmlFree nxt
    end
  say href
  say cont
  end

