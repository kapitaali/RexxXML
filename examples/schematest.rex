/* test of schema validation
 *
 * $Header: C:/ptjm/rexx/rexxxml/trip/RCS/schematest.rex 1.2 2003/10/31 17:17:40 ptjm Rel $
 */

rcc = rxfuncadd('XMLLoadFuncs', 'rexxxml', 'xmlloadfuncs')

if rcc then do
  say rxfuncerrmsg()
  exit 1
  end

call xmlloadfuncs

gooddocfile = 'schematestgood.xml'
baddocfile = 'schematestbad.xml'
goodxsdfile = 'schematestgood.xsd'
badxsdfile = 'schematestbad.xsd'
horridxsdfile = 'schematestawful.xsd'
missingxsdfile = 'schematestmissing.xsd'

test = 'using trees'
xsdt = xmlParseSchema(goodxsdfile)

if xsdt = 0 then do
  say 'fail' test 'parse schema' xmlError()
  exit 1
  end

doct = xmlParseXML(gooddocfile)

if doct = 0 then do
  say 'fail' test 'parse doc' xmlError()
  exit 1
  end

rc = xmlValidateDoc(xsdt, doct)

if rc \= "OK" then do
  say 'fail' test 'validate' xmlError()
  exit 1
  end

say 'pass' test

test = 'using urls'
rc = xmlValidateDoc(goodxsdfile, gooddocfile, 'u', 'u')

if rc \= "OK" then do
  say 'fail' test 'validate' xmlError()
  exit 1
  end

say 'pass' test

test = 'using mixture'
rc = xmlValidateDoc(xsdt, gooddocfile,, 'u')

if rc \= "OK" then do
  say 'fail' test 'validate' xmlError()
  exit 1
  end

say 'pass' test

test = 'using mixture II'
rc = xmlValidateDoc(goodxsdfile, doct, 'U', 'T')

if rc \= "OK" then do
  say 'fail' test 'validate' xmlError()
  exit 1
  end

say 'pass' test

test = 'bad schema'
xsdbad = xmlParseSchema(badxsdfile)

if xsdbad \= 0 then do
  say 'fail' test 'parse schema'
  exit 1
  end

else say 'pass' test '('xmlError()')'

test = 'bad schema url'

rc = xmlValidateDoc(badxsdfile, doct, 'U', 'T')

if rc = 'INVALIDSCHEMA' then say 'pass' test '('xmlError()')'
else do
  say 'fail' test rc '('xmlError()')'
  exit 1
  end

test = 'horrid schema'
xsdbad = xmlParseSchema(horridxsdfile)

if xsdbad \= 0 then do
  say 'fail' test 'parse schema'
  exit 1
  end

else say 'pass' test '('xmlError()')'

test = 'horrid schema url'

rc = xmlValidateDoc(horridxsdfile, doct, 'U', 'T')

if rc = 'INVALIDSCHEMA' then say 'pass' test '('xmlError()')'
else do
  say 'fail' test rc '('xmlError()')'
  exit 1
  end

test = 'missing schema'
xsdbad = xmlParseSchema(missingxsdfile)

if xsdbad \= 0 then do
  say 'fail' test 'parse schema'
  exit 1
  end

else say 'pass' test '('xmlError()')'

test = 'missing schema url'

rc = xmlValidateDoc(missingxsdfile, doct, 'U', 'T')

if rc = 'INVALIDSCHEMA' then say 'pass' test '('xmlError()')'
else do
  say 'fail' test rc '('xmlError()')'
  exit 1
  end

test = 'bad doc'

rc = xmlValidateDoc(xsdt, baddocfile,, 'U')

if rc = "OK" then do
  say 'fail' test 'validate'
  exit 1
  end

else say 'pass' test '('rc xmlError()')'

