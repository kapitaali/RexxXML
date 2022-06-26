<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
 <xsl:output method="text"/>

 <xsl:template match="*|text()"><xsl:apply-templates/></xsl:template>


 <xsl:template id="b" match="line[contains(., 'corn')]">
   Most of the lines in this poem are rather dreary, but there's one wonderful
   line which, for me, evokes thoughts of the late summer days of my youth,
   when the corn fields, which in those days grew just outside the city, held
   an almost mystical appeal to me. That line is `<xsl:value-of select="."/>'.
 </xsl:template>

</xsl:stylesheet>
