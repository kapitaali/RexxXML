/* test of highlighting the word `infrastructure'
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/infrastructure.rex 1.3 2003/10/31 17:16:41 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

doc = xmlParseHtml('infrastructure.html')

if doc = 0 then do
  say xmlError()
  exit 1
  end

inf = 'INFRASTRUCTURE'
linf = length(inf)

expr = "//text()[contains(., 'infrastructure') or contains(., 'Infrastructure')]"
nds = xmlFindNode(expr, doc)

if nds = 0 then do
  say xmlerror()
  exit 1
  end

do i = 1 to xmlNodesetCount(nds)
   node = xmlNodesetItem(nds, i)
   call xmlExpandNode 'nd', node
   text = nd.content
   txt = translate(text)
   pos = pos(inf, txt)
   rep = 'Replace'

   do while pos > 0
     if pos > 1 then do
       node = xmlAddText(node, substr(text, 1, pos-1), rep)
       rep = 'After'
       end
     node = xmlAddElement(node, 'em', substr(text, pos, linf), rep)
     rep = 'After'
     txt = substr(txt, pos+linf)
     text = substr(text, pos+linf)
     pos = pos(inf, txt)
     end

   if length(text) > 0 then
     node = xmlAddText(node, text, rep)
   end

call xmlSaveDoc 'Infrastructure!.html', doc
