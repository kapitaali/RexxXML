/* test of tree traversal
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/traverse.rex 1.3 2003/10/31 17:17:53 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

doc = xmlParseXML('poement.xml',, 's')

call traverse doc, 0

return 0

traverse: procedure
  parse arg node, depth
  if node = 0 then return

  if xmlExpandNode('n.', node) then do
    say copies(' ', depth) || n.name '('n.type')'
    if n.type = 'ELEMENT_NODE' & n.name = 'line' then
      say copies(' ', depth) xmlNodeContent(node)
/*    if n.type = 'TEXT_NODE' then
      say copies(' ', depth) n.content */
    call traverse n.children, depth+1
    call traverse n.next, depth
    end
  return
