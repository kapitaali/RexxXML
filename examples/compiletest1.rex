/* test of compiling speed -- this uses the uncompiled string to perform
 *                            repeated evaluations. it should be tested
 *                            against compiletest2.rex
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/compiletest1.rex 1.3 2003/10/31 17:15:30 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

/* expressions to be evaluated against each node */
expr1 = 'function/arg/@name = "out"'
expr2 = '@name'
expr3 = 'function/@name'
expr4 = 'function/info'
expr5 = 'function/arg/@name'

doc = xmlParseXML('libxml2-api.xml');

allnodes = xmlFindNode('//.', doc)

do i = 1 to xmlNodesetCount(allnodes)
   thisnode = xmlNodesetItem(allnodes, i)
   somenode = xmlEvalExpression(expr1, thisnode)
   if somenode = 'true' then do
      say 'thisnode' xmlEvalExpression(expr2, thisnode)
      say ' function' xmlEvalExpression(expr3, thisnode)
      say '         ' xmlEvalExpression(expr4, thisnode)
      args = xmlFindNode(expr5, thisnode)
      do j = 1 to xmlNodesetCount(args)
         say xmlNodeContent(xmlNodesetItem(args, j))
         end
      end
   end
