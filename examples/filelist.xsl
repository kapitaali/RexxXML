<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
  <xsl:template match="node()|text()">
    <xsl:copy>
      <xsl:apply-templates/>
    </xsl:copy>
  </xsl:template>

  <xsl:template match="doc" mode="child">
     <xsl:apply-templates/>
  </xsl:template>

  <xsl:template match="filelist">
    <xsl:apply-templates select="document(*)" mode="child"/>
  </xsl:template>
</xsl:stylesheet>
