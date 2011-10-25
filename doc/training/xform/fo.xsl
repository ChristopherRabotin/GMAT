<?xml version='1.0'?> 
<xsl:stylesheet  
    xmlns:d="http://docbook.org/ns/docbook"
    xmlns:fo="http://www.w3.org/1999/XSL/Format"
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform" 
    version="1.0"> 

    <xsl:import href="../../build/contrib/docbook-xsl-ns/fo/docbook.xsl"/> 

    <!-- Local variables -->
    <xsl:variable name="blue" select="'#0b3d91'"/>

    <!-- Fonts -->
    <!-- other font option is Helvetica:
    <xsl:param name="body.font.family">Helvetica</xsl:param>
    -->
    <xsl:param name="body.font.family">Garamond</xsl:param>
    <xsl:param name="body.font.master">11</xsl:param>
    <xsl:param name="title.font.family">Helvetica</xsl:param>

    <!-- Titles -->
    <xsl:attribute-set name="component.title.properties">
        <xsl:attribute name="color"><xsl:value-of select="$blue"/></xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="formal.title.properties">
        <xsl:attribute name="text-align">center</xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="section.title.properties">
        <xsl:attribute name="color"><xsl:value-of select="$blue"/></xsl:attribute>
    </xsl:attribute-set>
    <xsl:attribute-set name="section.title.level1.properties">
         <xsl:attribute name="font-size">14pt</xsl:attribute>
    </xsl:attribute-set>

    <!-- Auto-numering -->
    <xsl:param name="chapter.autolabel" select="0"></xsl:param>

    <!-- Program listings -->
    <xsl:param name="shade.verbatim" select="1"/>
    <xsl:attribute-set name="monospace.verbatim.properties">
        <xsl:attribute name="font-family">Consolas, monospace</xsl:attribute>
    </xsl:attribute-set>

    <!-- Formatting for specific tags -->
    <xsl:template match="d:guilabel">
        <xsl:call-template name="inline.boldseq"/>
    </xsl:template>

    <!-- Table of contents -->
    <xsl:param name="generate.toc">
        /appendix toc,title
        article/appendix  nop
        /article  toc,title
        book      toc,title
        /chapter  toc,title
        part      nop
        /preface  toc,title
        reference toc,title
        /sect1    toc
        /sect2    toc
        /sect3    toc
        /sect4    toc
        /sect5    toc
        /section  toc
        set       toc,title
    </xsl:param>

</xsl:stylesheet>
