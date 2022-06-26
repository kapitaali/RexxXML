<?xml version="1.0" encoding="iso-8859-1"?>

<!-- test stylesheet parameters -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0"
     xmlns:rexx='urn://rexxxml/xslt'
     xmlns:ex='http://www.interlog.com/~ptjm/simple-example'
     extension-element-prefixes='rexx'>
  <xsl:output method="text" omit-xml-declaration='yes'/>

<rexx:rexx>
say 'invoking rexx:rexx'
</rexx:rexx>

<rexx:function name='ex:lls'>
  return 'lls'()
</rexx:function>


<xsl:template match="/">result tree!
<xsl:value-of select='ex:lls()'/></xsl:template>

</xsl:stylesheet>
