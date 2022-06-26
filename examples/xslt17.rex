/* test of setting parameters
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/xslt17.rex 1.1 2003/10/02 02:34:08 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

stylesheet = 'xslt17.xsl'
document = 'poem.xml'

res = xmlApplyStylesheet(stylesheet, document, 'url', 'url', 'godfrey', "72", 'noodle', "concat('noodle ', ""'"", 'from', ""'"", ' rexx')", 'rigged', 'true()')

if res = 0 then say xmlError()
else do
   say xmlSaveDoc(, res)
   end
