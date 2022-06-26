<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:me="http:/www.interlog.com/~ptjm/disclaimer">

  <me:disclaimer mode="strong"><p>This software is distributed in the
  hopes that it will be useful, but without any warranty. If you attempt
  to contact me, I will deny any knowledge of the software, of you, and
  even of myself. You may, at my option, receive a visit at a later
  date, and you may or may not, at your option, regret having called me at
  that time.</p></me:disclaimer>

  <me:disclaimer mode="normal"><p>This software is distributed in the
  hopes that it will be useful, but without any warranty.</p></me:disclaimer>

  <me:disclaimer mode="weak"><p>Have fun. Try not to break anything.
  </p></me:disclaimer>

  <xsl:template match="disclaimer[@mode and not(*|text())]">
    <xsl:variable name="mode" select="@mode"/>
    <disclaimer mode='{$mode}'>
      <xsl:copy-of
         select="document('')/xsl:stylesheet/me:disclaimer[@mode=$mode]/*"/>
    </disclaimer>
</xsl:template>
</xsl:stylesheet>
