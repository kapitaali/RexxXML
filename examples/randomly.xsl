<?xml version="1.0" encoding="iso-8859-1"?>

<!-- rexx:function example. this can be applied to any XML file.
      it will output all text, but only a random selection of
      XML tags -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:rexx="urn://rexxxml/xslt"
  xmlns:prd="http://www.interlog.com/ptjm/predicate-example"
  extension-element-prefixes='rexx'>
 <rexx:function name="prd:random-selection" return-type='Boolean'>
   if random() > 500 then return 'true'
   else return 'false'
 </rexx:function>

 <xsl:template match='node()[prd:random-selection()]'>
    <xsl:copy>
       <xsl:apply-templates/>
    </xsl:copy>
 </xsl:template>
</xsl:stylesheet>
