<?xml version='1.0'?> 
<xsl:stylesheet  
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    version="1.0"> 

    <xsl:import href="../../build/contrib/docbook-xsl-ns/fo/docbook.xsl"/> 

    <!-- Fonts -->
    <!--<xsl:param name="body.font.family">Helvetica</xsl:param>-->
    <xsl:param name="body.font.family">Garamond</xsl:param>
    <xsl:param name="body.font.master">12</xsl:param>
    <xsl:param name="title.font.family">Helvetica</xsl:param>
    <xsl:attribute-set name="section.title.level1.properties">
        <xsl:attribute name="font-size">16pt</xsl:attribute>
    </xsl:attribute-set>

    <!-- Auto-numering -->
    <xsl:param name="chapter.autolabel" select="0"></xsl:param>

    <!-- Formatting for specific tags -->
    <xsl:template match="guilabel">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>
</xsl:stylesheet>
