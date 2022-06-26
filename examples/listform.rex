/* list the contents of all the forms in a web page
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/listform.rex 1.1 2003/10/22 05:12:48 ptjm Rel $
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

if page = 0 then do
  say 'error parsing' url
  say xmlError()
  exit 1
  end

forms = xmlFindNode('//form', page)

if forms = 0 then do
  say 'error evaluating XPath expression'
  say xmlError()
  exit 1
  end

if xmlNodesetCount(forms) = 0 then do
  say 'no forms on' url
  end
else do i = 1 to xmlNodesetCount(forms)
  form = xmlNodesetItem(forms, i)
  controls = xmlfindNode('descendant::input|descendant::textarea|' ||,
                         'descendant::buttom|descendant::select', form)

  say 'Form' i 'method='xmlEvalExpression('@method', form) ,
               'action='xmlEvalExpression('@action', form)
  if xmlNodesetCount(controls) = 0 then do
    say '  No controls'
    end
  else do j = 1 to xmlNodesetCount(controls)
    control = xmlNodesetItem(controls, j)
    say '  "'xmlEvalExpression('@name', control)'"' ,
          '"'xmlEvalExpression('@type', control)'"' ,
          '"'xmlEvalExpression('string(.)', control)'"' ,
          '"'xmlEvalExpression('@value', control)'"'
    end
  call xmlFree controls
  end

  call xmlFree forms

call xmlFreeDoc page
