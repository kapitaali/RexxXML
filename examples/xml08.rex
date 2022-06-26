/* test of xmlFindNode
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/xml08.rex 1.3 2003/10/31 17:19:28 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

address xml
'<doc><p>here is some text</p><p>and more tex</p>'
'<dataModel><comment>In the middle of which, we find a data model'
'</comment>'
'<table name="customer"><column name="custno" type="number" length="6"/>'
'<column name="branch" type="number" length="2"/>'
'<column name="name" type="string" length="30"/>'
'</table></dataModel>'
'</doc>'

doc = xmlparsexml()

say 'Traverse slowly'
call traverse_slowly doc

say 'Zoom through with XPath'
dms = xmlFindNode('//dataModel', doc)

do i = 1 to xmlNodeSetCount(dms)
  node = xmlNodeSetItem(dms, i)
  call process node
  end
say 'voom!'

say xmlEvalExpression('//dataModel/table/@name', doc)

ce = xmlCompileExpression('//dataModel/table/@name')

say c2x(ce)

say xmlEvalExpression(ce, doc)
say xmlEvalExpression(ce, doc)
say xmlEvalExpression(ce, doc)
say xmlEvalExpression('2=2', doc)
y = 1
x.y = 'hello'
say xmlEvalExpression('$x.y', doc)

say xmlEvalExpression('today()')

call xmlFreeExpression ce

exit 0

traverse_slowly: procedure
  parse arg node
  if node = 0 then return

  if xmlExpandNode('n.', node) then do
    if n.type = ELEMENT_NODE & n.name = 'dataModel' then
      call process node

    call traverse_slowly n.children
    call traverse_slowly n.next
    end
  return

process: procedure
  parse arg node
  if node = 0 then return

  if xmlExpandNode('n.', node) then do
    if n.type = 'ELEMENT_NODE' & n.name = 'dataModel' then call process n.children
    else do
      if n.type = 'ELEMENT_NODE' & n.name = 'table' then do
        say n.a.name
        call process n.children
        end
      else if n.type = 'ELEMENT_NODE' & n.name = 'column' then do
        if symbol('n.a.precision') = 'VAR' then len = n.a.length ',' n.a.precision
        else len = n.a.length
        say '   ' n.a.name n.a.type'('len')'
        end
      call process n.next
      end

    end
  return
