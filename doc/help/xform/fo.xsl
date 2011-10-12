<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform" version="1.0">
<xsl:import href="../../build/contrib/docbook-xsl-ns/fo/docbook.xsl"/>

<!-- caution, warning, note, information blocks -->
<xsl:attribute-set name="admonition.properties">
</xsl:attribute-set>
<xsl:attribute-set name="nongraphical.admonition.properties">
    <xsl:attribute name="background-color">#F9D9D8</xsl:attribute>
    <xsl:attribute name="border-top-color">#F9B9B8</xsl:attribute>
    <xsl:attribute name="border-top-style">solid</xsl:attribute>
    <xsl:attribute name="border-top-width">thick</xsl:attribute>
    <xsl:attribute name="border-bottom-color">#F9B9B8</xsl:attribute>
    <xsl:attribute name="border-bottom-style">solid</xsl:attribute>
    <xsl:attribute name="border-bottom-width">thick</xsl:attribute>
    <xsl:attribute name="border-left-color">#F9B9B8</xsl:attribute>
    <xsl:attribute name="border-left-style">solid</xsl:attribute>
    <xsl:attribute name="border-left-width">thick</xsl:attribute>
    <xsl:attribute name="border-right-color">#F9B9B8</xsl:attribute>
    <xsl:attribute name="border-right-style">solid</xsl:attribute>
    <xsl:attribute name="border-right-width">thick</xsl:attribute>
    <xsl:attribute name="padding-top">1em</xsl:attribute>
    <xsl:attribute name="padding-bottom">1em</xsl:attribute>
    <xsl:attribute name="padding-left">1em</xsl:attribute>
    <xsl:attribute name="padding-right">1em</xsl:attribute>
</xsl:attribute-set>

<!-- equations -->
<xsl:attribute-set name="informalequation.properties" use-attribute-sets="formal.object.properties">
    <xsl:attribute name="font-style">italic</xsl:attribute>
</xsl:attribute-set>

<!-- titles -->
<xsl:attribute-set name="component.title.properties">
    <xsl:attribute name="color">#004080</xsl:attribute>
</xsl:attribute-set>
<xsl:attribute-set name="section.title.properties">
    <xsl:attribute name="color">#004080</xsl:attribute>
</xsl:attribute-set>
<xsl:attribute-set name="refentry.title.properties">
    <xsl:attribute name="color">#004080</xsl:attribute>
</xsl:attribute-set>

<!-- lists -->
<xsl:attribute-set name="variablelist.term.properties">
    <xsl:attribute name="font-weight">bold</xsl:attribute>
</xsl:attribute-set>
<xsl:attribute-set name="list.item.spacing">
    <xsl:attribute name="space-before.minimum">0.8em</xsl:attribute>
    <xsl:attribute name="space-before.maximum">1.2em</xsl:attribute>
    <xsl:attribute name="space-before.optimum">1em</xsl:attribute>
    <xsl:attribute name="space-after.minimum">0em</xsl:attribute>
    <xsl:attribute name="space-after.maximum">0em</xsl:attribute>
    <xsl:attribute name="space-after.optimum">0em</xsl:attribute>
</xsl:attribute-set>

</xsl:stylesheet>
