#!/bin/bash

(
  echo "<AllTestResults>"
  for file in report_*.xml; do
    # Strip <?xml ...> if present
    sed '/<?xml/d' "$file"
  done
  echo "</AllTestResults>"
) > all_reports.xml

xsltproc $1 all_reports.xml > report.html
