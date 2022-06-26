/* submit a yahoo search -- final version
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/yahoosearch.rex 1.1 2003/10/28 06:50:25 ptjm Rel $
 */

parse arg query

if query = '' then do
  say 'usage: yahoosearch "query"'
  exit 1
  end

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

searchurl = 'http://ca.rd.yahoo.com/home/hps/*http://ca.search.yahoo.com/search/ca'
text=urlify(query)

ss = searchurl || '?' || 'vc=&n=100&p=' || text

/* return a maximum of 100 pages */
do 100 until ss = ''
  doc = xmlParseHTML(ss)

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

  ss = xmlEvalExpression("//div[@id='results']//table/tr/td/a[starts-with(., 'next')]/@href", doc)
  call xmlFreeDoc doc
  end

exit 0

/* convert a string into something suitable for use in a URL
 * this is probably overkill */
urlify: procedure
  parse arg s

  /* this is ascii-specific */
  r = ''
  do while s \= ''
    parse var s t +1 s
    if ('A' <= t & 'Z' >= t) | ('a' <= t & 'z' >= t) | ,
       ('0' <= t & '9' >= t) then
      r = r || t
    else if t = ' ' then
      r = r || '+'
    else
      r = r || '%'c2x(t)
    end

  return r;
