/* test of name spaces in XPath
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/xml10.rex 1.3 2003/10/31 17:19:46 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

address xml
'<ns:doc xmlns:ns="http://ptjm/wobble"><ns:p>here is some text</ns:p><ns:p>and more tex</ns:p>'
'<ns:dataModel><ns:comment>In the middle of which, we find a data model'
'</ns:comment>'
'<ns:table name="customer"><ns:column name="custno" type="number" length="6"/>'
'<ns:column name="branch" type="number" length="2"/>'
'<ns:column name="name" type="string" length="30"/>'
'</ns:table></ns:dataModel>'
'</ns:doc>'

doc = xmlparsexml()

call xmlSetContext , doc, 'mns=http://ptjm/wobble'

say xmlEvalExpression('//mns:dataModel/mns:table/@name', doc)

ce = xmlCompileExpression('//mns:dataModel/mns:table/mns:column[number($n)]/@name')

say c2x(ce)

n=1
say xmlEvalExpression(ce, doc)
n=2
say xmlEvalExpression(ce, doc)
n=3
say xmlEvalExpression(ce, doc)
call xmlFreeExpression ce

say xmlEvalExpression('2=2', doc)

y = 1
x.y = 'hello'
say xmlEvalExpression('$x.y', doc)

say xmlEvalExpression('today()')

say xmlEvalExpression('ts()')

say xmlError();

say xmlEvalExpression('//mns:column[eachcol(.)]/@name')

exit 0

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
