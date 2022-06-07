project(antares-doc NONE)
cmake_minimum_required(VERSION 3.14)

include(../version.cmake)

configure_file(index.md.in index.md)

configure_file(reference-guide/01-introduction.md.in reference-guide/01-introduction.md)
configure_file(reference-guide/09-system_requirements.md.in reference-guide/09-system_requirements.md)
configure_file(reference-guide/10-command_line.md.in reference-guide/10-command_line.md)
configure_file(reference-guide/11-modeling.md.in reference-guide/11-modeling.md)
configure_file(reference-guide/12-attribution_notices.md.in reference-guide/12-attribution_notices.md)
