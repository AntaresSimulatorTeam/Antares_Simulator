# Antares_Simulator documentation sources

This directory contains the source files for building the *Antares Simulator* [documentation website](https://antares-simulator.readthedocs.io/), 
as well as the [Doxygen documentation](https://antaressimulatorteam.github.io/Antares_Simulator/doxygen/).

Please help us keep this documentation alive:
- If you find that something is missing, outdated, or some mistake, report it in a [new issue](https://github.com/AntaresSimulatorTeam/Antares_Simulator/issues/new) 
  or, better yet, open a [pull request](https://github.com/AntaresSimulatorTeam/Antares_Simulator/compare)!
- If you add a new feature, change a parameter, change something in the technical workflow, etc., update these 
  sources in the same pull request. Pull requests that do not update the documentation when needed will not be approved.  

## Contents of this directory

As noted above, these sources serve two purposes:

- Building the website
- Building the Doxygen documentation

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

Like mentioned above, the same source material is used to automatically generate 2 different formats (mkdocs website, 
and doxygen website). When writing pages that have to be published in more than one format, please make sure the 
syntax you are using is compatible with the other target format. 
When you add a page, make sure you add it to all target supports. 
For instance, adding a page to the mkdocs website requires creating a `nav` 
entry in the [mkdocs.yml](../mkdocs.yml) file
