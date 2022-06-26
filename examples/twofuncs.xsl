<?xml version="1.0" encoding="iso-8859-1"?>

<!-- rexx:function example. this defines more than one function,
      which was not possible until a few seconds ago -->

<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:rexx="urn://rexxxml/xslt"
  xmlns:tf="http://www.interlog.com/~ptjm/twofuncs"
  extension-element-prefixes='rexx'>
 <rexx:function name="tf:dreary" return-type='Boolean'>
     /* use xmlContext to get text of current node */
     curtext = xmlEvalExpression('.',,xmlContext)
     /* anything which doesn't mention corn is dreary */
     if pos('corn', curtext) = 0 then return 1
     else return 0
 </rexx:function>

 <!-- return the current date and time -->
 <rexx:function name="tf:maintenant">
   return date('e') time('n')
 </rexx:function>

 <xsl:template match="text()"/>

 <xsl:template match='node()[not(tf:dreary())]'>
    <xsl:text>
</xsl:text>
    <xsl:comment>Generated at <xsl:value-of select="tf:maintenant()"/></xsl:comment>
    <xsl:text>
</xsl:text>
    <xsl:copy>
       <xsl:apply-templates/>
    </xsl:copy>
 </xsl:template>
</xsl:stylesheet>
