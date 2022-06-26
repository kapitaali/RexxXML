/* Load HTML data from a URL, then dump its structure using treewalk.rex
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/dumphtml.rex 1.1 2003/10/16 17:52:04 ptjm Rel $
 */

parse arg uri .

if uri = '' then do
  say 'usage: dumphtml <filename>'
  exit 1
  end

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say 'error' rcc 'loading RexxXML'
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

nd = xmlparsehtml( uri)

if nd = 0 then say 'bad' uri xmlError()

call 'treewalk' nd, 0, uri

exit 0
