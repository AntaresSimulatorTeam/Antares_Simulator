#!/bin/bash
# exit script if any subcommand fails
set -e
# copy reference guide md files and assets
cp -r ../user-guide source/
cp -r ../assets/ source/
# change style for inline latex math \\( -> $ and \\) -> $
find source/user-guide/ -type f -name "*.md" -exec sed -i 's=\\\\)=$=g ; s=\\\\(=$=g' {} \;
# actually make the pdf
sphinx-build -M latexpdf source build
mv build/latex/antaressimulatoruserguide.pdf "./${1}"
# clean
rm -rf source/user-guide source/assets build
