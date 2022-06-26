/* test of xpath sub-command
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/sc2.rex 1.3 2003/10/01 20:02:55 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

say xmlversion()

doc = xmlParseXML('denombre.xml')

call xmlsetcontext , doc

address xpath

'//two'

if rc = 0 then say 'oops' xmlerror()
else do
  rcc = rc
  myp = xmlNodesetItem(rcc, 1)
  do i = 2 while myp \= 0
    call xmlExpandNode 'sc.', myp
    say sc.name xmlNodeContent(myp)
    myp = xmlNodesetItem(rcc, i)
    end
  end

'8+3'
if rc = 0 then say 'oops' xmlerror()
else say rc


'sum(/numbers/*)'
if rc = 0 then say 'oops' xmlerror()
else say rc

'/numbers/* < 2'
say rc

'/numbers/* > 2'
say rc

'/numbers/* = //two'
say rc

'/numbers/* != //two'
say rc

'//one != //two'
say rc

'8+//two'
if rc = 0 then say 'oops' xmlerror()
else say rc

'/numbers/*[2]'
if rc = 0 then say 'oops' xmlerror()
else do
  rcc = rc
  myp = xmlNodesetItem(rcc, 1)
say c2x(rcc)
  do i = 2 while myp \= 0
    call xmlExpandNode 'sc.', myp
    say sc.name xmlNodeContent(myp)
    myp = xmlNodesetItem(rcc, i)
    end
  end
