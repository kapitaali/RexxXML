/* test of xpath context searching
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/sc4.rex 1.4 2003/10/31 17:17:30 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

address xml

'<?xml version="1.0" encoding="ISO-8859-1"?>'
'<!-- A dreary poem -->'
'<poem><stanza><line>Winter has come early.</line>'
'         <line>The frost has iced over the empty fields</line>'
'         <line>and ruined the late corn.</line>'
'         <line>It will be a cold November.</line>'
' </stanza>	 '
' <stanza><line>Once I wondered why the world was so cruel</line>'
'         <line>but I have given up wondering,</line>'
'         <line>for who can say how the North wind blows?</line>'
' </stanza></poem>'

doc = xmlparsexml()

if doc = 0 then do
  say 'oops' xmlError()
  exit 1
  end

call xmlsetcontext , doc
address xpath

'string(/poem/stanza[position() = 2]/line[contains(self::*, "wondering")])'
say rc
'string(/child::poem/child::stanza[position() = 2]/line[contains(self::*, "wondering")])'
say 'child::'rc
say 'oks'
'string(/child::poem[true()]/child::stanza[position() = 2]/child::line[contains(self::*, "wondering")])'
say rc
say 'boks'
'string(/poem/stanza[position()=2]/line[position() = 3])'
say rc

'string(/poem/stanza/line/text()[contains(., "corn")])'
say rc


'string(/poem/stanza[contains(., "corn")])'
say rc

'string(//line[contains(., "corn")]/..)'
say rc

'string(/descendant::line[contains(self::node(), "corn")]/parent::node())'
say rc

if rc = 0 then say xmlError()

'string(/poem/stanza[1] | /poem/stanza[2])'
say rc

if rc = 0 then say xmlError()

'string(/poem/stanza[position() = 1 or position() = 2])'
say rc

if rc = 0 then say xmlError()
