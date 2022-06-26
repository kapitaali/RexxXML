/* list the contents of all the forms in a web page
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/listform2.rex 1.1 2003/10/22 05:13:14 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

parse arg url .

if url = '' then do
  say 'usage: listform url'
  exit 1
  end

page = xmlParseHTML(url)
forms = xmlFindNode('//form', page)
do i = 1 to xmlNodesetCount(forms) 
  form = xmlNodesetItem(forms, i)
  controls = xmlfindNode('descendant::input|descendant::textarea|' ||,
                         'descendant::buttom|descendant::select', form)

  say 'Form' i 'method='xmlEvalExpression('@method', form) ,
               'action='xmlEvalExpression('@action', form)
  
  do j = 1 to xmlNodesetCount(controls)
    control = xmlNodesetItem(controls, j)
    say '  "'xmlEvalExpression('@name', control)'"' ,
          '"'xmlEvalExpression('@type', control)'"' ,
          '"'xmlEvalExpression('string(.)', control)'"' ,
          '"'xmlEvalExpression('@value', control)'"'
    end
  end

