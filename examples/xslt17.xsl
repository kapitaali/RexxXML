<?xml version="1.0" encoding="iso-8859-1"?>

<!-- test stylesheet parameters -->

<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    version="1.0">
  <xsl:output method="text"/>

  <xsl:param name="godfrey" select="27"/>
  <xsl:param name="noodle" select="'noodle'"/>
  <xsl:param name="rigged" select="false()"/>

  <xsl:variable name="nl"><xsl:text>
</xsl:text></xsl:variable>

     <xsl:template match="/">
        <xsl:value-of select="$godfrey"/>
        <xsl:value-of select="$nl"/>
        <xsl:value-of select="$noodle"/>
        <xsl:value-of select="$nl"/>
        <xsl:value-of select="$rigged"/>
        <xsl:value-of select="$nl"/>
     </xsl:template>

</xsl:stylesheet>
