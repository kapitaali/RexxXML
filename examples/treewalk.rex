/* routine for walking a rexxxml document tree.
 * this can be called from any program that has loaded rexxxml
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/treewalk.rex 1.4 2003/10/16 17:25:42 ptjm Rel $
 */


parse arg tree, indent, title


/* determine which tails should be presented in hex notation */
hex. = 0
hex.intsubset = 1
hex.extsubset = 1
hex.self = 1
hex.next = 1
hex.prev = 1
hex.children = 1
hex.last = 1
hex.parent = 1
hex.attributes = 1
hex.null = 1

/* here's a list of all the tails in the order in which I'd like them
 * presented. I originally used regStemDoOver, but it's not portable
 * and its output is harder to read because the order is not controlled.
 * These must be upper-case */

tails = 'TYPE NAME SELF PREV NEXT CHILDREN LAST PARENT NAMESPACEPREFIX' ,
        'NAMESPACEURL URL EXTERNALID SYSTEMID CONTENT ENCODING VERSION' ,
        'INTSUBSET EXTSUBSET COMPRESSION STANDALONE ATYPE ETYPE'        ,
        'MANDATORY ATTRIBUTES A'

call treewalk tree, indent, title

return



treewalk: procedure expose hex. tails

  parse arg cur, ind, title

  if cur = 0 then return

  if ind > 0 then say ''

  say title
  indent = copies('  ', ind)

  call xmlexpandnode 'sft.', cur

  rest = tails

  do until rest = ''
    parse var rest i rest

    if symbol('sft.'i) = 'LIT' then iterate

    if hex.i then do
      if sft.i = 0 then say indent || i sft.i
      else say indent || i c2x(sft.i)'x'
      end
    else  say indent || i '«'sft.i'»'
    end

  if symbol('sft.'a) = 'VAR' then do
     rest = sft.a
     do until rest = ''
       parse var rest i rest
       say indent || 'A.'i sft.A.i
     end
    end

  call treewalk sft.attributes, ind+1, indent || 'Attributes'
  call treewalk sft.children, ind+1, indent || 'Children'
  call treewalk sft.next, ind, indent || 'Next'
  return
