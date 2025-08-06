<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0"
  xmlns:xsl="http://www.w3.org/1999/XSL/Transform">

  <xsl:output method="html" indent="yes"/>

  <xsl:template match="/AllTestResults">
    <html>
      <head>
        <title>Combined Boost Test Report</title>
        <style>
          body { font-family: sans-serif; padding: 1em; }
          .passed { color: green; } .failed { color: red; }
          table { border-collapse: collapse; width: 100%; margin-bottom: 2em; }
          th, td { padding: 8px; border: 1px solid #ccc; text-align: left; }
        </style>
      </head>
      <body>
        <h1>Combined Boost Test Report</h1>
        <xsl:apply-templates select="//TestSuite"/>
      </body>
    </html>
  </xsl:template>

  <xsl:template match="TestSuite">
    <h2>
      Test Suite: <xsl:value-of select="@name"/>
      <span class="{@result}">(<xsl:value-of select="@result"/>)</span>
    </h2>
    <table>
      <tr>
        <th>Test Case</th>
        <th>Result</th>
        <th>Assertions Passed</th>
        <th>Assertions Failed</th>
      </tr>
      <xsl:apply-templates select="TestCase"/>
    </table>
  </xsl:template>

  <xsl:template match="TestCase">
    <tr>
      <td><xsl:value-of select="@name"/></td>
      <td class="{@result}"><xsl:value-of select="@result"/></td>
      <td><xsl:value-of select="@assertions_passed"/></td>
      <td><xsl:value-of select="@assertions_failed"/></td>
    </tr>
  </xsl:template>

</xsl:stylesheet>
