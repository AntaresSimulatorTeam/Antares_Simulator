# Antares_Simulator documentation sources

This directory contains the source files for building the *Antares Simulator* [documentation website](https://antares-simulator.readthedocs.io/), 
the PDF user guides that are published in every [release](https://github.com/AntaresSimulatorTeam/Antares_Simulator/releases), 
as well as the [Doxygen documentation](https://antaressimulatorteam.github.io/Antares_Simulator/doxygen/).

Please help us keep this documentation alive:
- If you find that something is missing, outdated, or some mistake, report it in a [new issue](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/new) 
  or, better yet, open a [pull request](https://github.com/AntaresSimulatorTeam/Antares_Simulator/compare)!
- If you add a new feature, change a parameter, change something in the technical workflow, etc., update these 
  sources in the same pull request. Pull requests that do not update the documentation when needed will not be approved.  

## Contents of this directory

As noted above, these sources serve two purposes:
1. Building the website
2. Building the PDF user guide
3. Building the Doxygen documentation

While most of the source code is common, some code is specific.

### Building the website
The website is hosted on [readthedocs](https://readthedocs.org/). The build workflow requires a configuration file which 
is currently at the root of the project: [readthedocs.yml](../readthedocs.yml). This platform presents many advantages, 
thanks to its workflow of automatic branch/tag building & publication:
- Multiple versions are activated: you can browse different versions of the documentation for different releases of *Antares Simulator*
- Pull requests are built automatically and the built status is reported in the PR's checks (`docs/readthedocs.org:antares-simulator`).  
  You can preview the built website inside this check (just click on `Details`), thus making it easier for reviewers to review changes of the docs. 

HTML content of the website is built automatically from source files written in [Markdown](https://fr.wikipedia.org/wiki/Markdown), 
using [mkdocs](https://www.mkdocs.org/).  
The mkdocs configuration file is [mkdocs.yml](../mkdocs.yml), at the root of the project.  
It contains all customization options for the website (for example, the theme used is [mkdocs-material](https://squidfunk.github.io/mkdocs-material/)). 
Here is the most common cases when you'll need to tweak this file:
- When you add a new page (written in Markdown), it is invisible by default. In order to make it visible, you have to 
  add it to the navigation list of the website in mkdocs.yml (`nav` section).
- If you want to customize the theme's behaviour, you will probably have to change the mkdocs.yml file (refer to the 
  used theme's documentation first)
- If you want to customize mkdoc's behaviour, add plugins, css, etc., you need to change this file (refer to the 
  [mkdocs doc](https://www.mkdocs.org/user-guide/) first)
  
Sometimes, the possibilities offered by mkdocs & the used theme can be limiting. Mkdocs offers the possibility to use 
custom html & css code. Currently, two methods are used:
- Custom CSS: the [extra.css](stylesheets/extra.css) files contains custom CSS classes (read more about this [here](https://squidfunk.github.io/mkdocs-material/customization/#additional-css)). 
  Feel free to add classes to it if you find it necessary.
- HTML overrides: the [overrides](overrides) directory contains HTML files that override objects inherited from mkdocs 
  or its theme (read more about this [here](https://squidfunk.github.io/mkdocs-material/customization/#extending-the-theme)). 
  You can use this method when other, lighter methods (i.e. tweaking mkdocs.yml & CSS styles) are not enough.

When modifying the website content, you can easily preview the result on your PC by navigating to the root of the 
project and running:
```bash
mkdocs serve
```
Then click on the produced link (most likely `http://127.0.0.1:8000/`) to open the website in your browser.  
As long as this process is running, updating any doc file automatically regenerates the website and refreshed the browser. 

### Building the PDF user guide
In every *Antares Simulator* [release](https://github.com/AntaresSimulatorTeam/Antares_Simulator/releases), 
the user guide is printed to a PDF file and published in the assets. This document can be downloaded by users who 
want to install *Antares Simulator* and be able to consult its documentation off-line.  

In practice, the PDF is generated automatically from Markdown files under [user-guide](./user-guide) by 
[Sphinx](https://www.sphinx-doc.org/) (using LaTeX). The script for this generation is in [pdf-doc-generation-with-sphinx/create_pdf_doc.sh](./pdf-doc-generation-with-sphinx/create_pdf_doc.sh); 
it is automatically run by a GitHub [action](../.github/workflows/build-userguide.yml) during every release, and for 
each pull request (in order to verify that the PDF builds).  

While the source material used for the PDF user guide are the same as the ones used for the mkdocs website (i.e. 
Markdown files under `user-guide`), some extra source files are needed:
- As for all Sphinx projects, this one needs a configuration file: [conf.py](./pdf-doc-generation-with-sphinx/source/conf.py). 
  This file allows you to customize the Sphinx build, in the same way mkdoks.yml allows you to customize mkdocs build. 
- Sphinx navigation is built iteratively, using "index" files that refer to each other. The top-most index file is 
  [pdf-doc-generation-with-sphinx/source/index.rst](./pdf-doc-generation-with-sphinx/source/index.rst). It points to 
  [user-guide/00-index.md](user-guide/00-index.md), which in turns points to other pages or indexes, 
  defining the whole navigation tree of the documentation. **This [user-guide/00-index.md](user-guide/00-index.md) 
  file should be updated** in the same way mkdocs.yml is, in order to keep the navigation tree of the PDF document 
  synchronized with the navigation tree of the mkdocs website.

When modifying the user guide content, you can easily preview the resulting PDF on your PC by navigating to the 
root of the project and running:
```bash
cd docs/pdf-doc-generation-with-sphinx
bash create_pdf_doc.sh user-guide.pdf
```
Sphinx will create a `user-guide.pdf` file in `docs/pdf-doc-generation-with-sphinx`.

### Doxygen
[//]: # (TODO)
We strive to enrich code documentation as it evolves. The doxygen doc is generated automatically each time the develop 
branch is updated. It is hosted [here](https://antaressimulatorteam.github.io/Antares_Simulator/doxygen), and can also 
be generated locally with the follow command at the root of the project:  

**doxygen-executable** docs/Doxyfile  
  
If you'd like the same rendering as the one published checkout [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css) 
and do this step before:  
```bash
git clone https://github.com/jothepro/doxygen-awesome-css.git
cd doxygen-awesome-css
git checkout v2.2.1
git apply ../docs/antares-simulator-doxygen.patch
```

## Extra considerations

Like mentioned above, the same source material is used to automatically generate three different formats (mkdocs website, 
PDF, doxygen website). When writing pages that have to be published in more than one format, please make sure the 
syntax you are using is compatible with all the target formats.  
Here is a **non-exhaustive** list of points to watch out for:
- When you add a page, make sure you add it to all target supports. For instance, adding a page to the mkdocs website 
  requires creating a `nav` entry in the [mkdocs.yml](../mkdocs.yml) file, while adding it to the Sphinx PDF requires 
  adding an extra entry to an existing index.md file, and, sometimes, creating an extra index.md file (to add a section).
- Do not use non-standard characters (such as emojis) in the user guide, as they cannot be rendered by LaTeX 
  in PDF.
