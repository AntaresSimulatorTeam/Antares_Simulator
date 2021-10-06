#!/bin/bash
# copy reference guide md files and assets
cp -r ../reference-guide source/
cp -r ../assets/ source/
# change style for inline latex math \\( -> $ and \\) -> $
find source/reference-guide/ -type f -name "*.md" -exec sed -i 's=\\\\)=$=g' {} \;
find source/reference-guide/ -type f -name "*.md" -exec sed -i 's=\\\\(=$=g' {} \;
# actually make the pdf
make latexpdf
mv build/latex/antaressimulatoruserguide.pdf .
# clean
rm -rf source/reference-guide source/assets build
