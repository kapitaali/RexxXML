/* Load XML data from a URL, then dump its structure using treewalk.rex
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/dumpfile.rex 1.7 2003/10/16 17:20:17 ptjm Rel $
 */

parse arg uri .

if uri = '' then do
  say 'usage: dumpfile <filename>'
  exit 1
  end

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say 'error' rcc 'loading RexxXML'
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

nd = xmlparsexml( uri)

if nd = 0 then say 'bad' uri xmlError()

call 'treewalk' nd, 0, uri

exit 0
