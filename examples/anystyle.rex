/* roughly equivalent to xsltproc
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/anystyle.rex 1.4 2003/10/31 17:15:15 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

parse arg ssf  docfile

ss = xmlParseXSLT(ssf)

doc = xmlApplyStylesheet( ss, docfile, , 'u')
if doc = 0 then say xmlError()

say xmlSaveDoc(, doc, ss)
call xmlFreeDoc doc
call xmlFreeStylesheet ss

return 0
