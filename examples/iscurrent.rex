/* check software.html to see if we're up-to-date
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/iscurrent.rex 1.3 2003/10/31 17:16:45 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

software.html = 'http://www.interlog.com/~ptjm/software.html'

parse value xmlVersion() with myversion .
package = 'RexxXML'

sw = xmlParseHTML(software.html)

if sw = 0 then do
  say xmlError()
  exit 1
  end 

/* software.html has a single table. Each row of the table has the
   package name in the first column, and the version number in the second.
   The first occurrance of the package is the most current one. */

prow = xmlFindNode('/html/body/table/tbody/tr[td[1] = $package][1]', sw)
if xmlNodesetCount(prow) \= 1 then do
  say 'unexpected row count' xmlNodesetCount(prow)
  exit 1
  end

curver = xmlEvalExpression('td[2]', xmlNodesetItem(prow, 1))

if curver \= myversion then do
   say 'My version is' myversion
   say 'Current version is' curver
   say 'which was released'  xmlEvalExpression('td[3]', xmlNodesetItem(prow, 1))
   end
else
   say 'All up-to-date!'
