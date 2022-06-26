/* submit a yahoo search
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/yahoo4.rex 1.1 2003/10/27 21:08:28 ptjm Rel $
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

lis = xmlFindNode("//span[@class='ygbody']/big[1]", doc)

if xmlNodesetCount(lis) = 1 then do
  call xmlExpandNode 'node', xmlNodesetItem(lis, 1)

  do forever
    if node.name = 'big' then do
      if symbol(href) \= 'LIT' then do
        say href
        say cont
        end
      ohref = xmlEvalExpression('a[1]/@href', node.self)
      parse var ohref . '*-' href
      if href = '' then href = ohref
      cont = xmlNodeContent(node.self)
      end
    else
      cont = cont || xmlNodeContent(node.self)

    if node.next = 0 then do
      say href
      say cont
      leave
      end
    else
      call xmlExpandNode 'node', node.next
    end
  end

nxt = xmlEvalExpression("//div[@id='results']//table/tr/td/a[starts-with(., 'next')]/@href", doc)
say nxt

