#!/bin/bash
# copy reference guide md files and assets
cp -r ../reference-guide source/
cp -r ../assets/ source/
# change style for inline latex math \\( -> $ and \\) -> $
find source/reference-guide/ -type f -name "*.md" -exec sed -i 's=\\\\)=$=g ; s=\\\\(=$=g' {} \;
# actually make the pdf
sphinx-build -M latexpdf source build
mv build/latex/antaressimulatoruserguide.pdf .
# clean
rm -rf source/reference-guide source/assets build
