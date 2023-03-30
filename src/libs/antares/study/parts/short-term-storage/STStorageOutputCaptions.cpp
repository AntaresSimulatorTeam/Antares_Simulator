
#include <vector>
#include "STStorageOutputCaptions.h"

namespace Antares::Data::ShortTermStorage
{
const std::vector<std::string> groups = {
    "PSP_open", 
    "PSP_closed", 
    "Pondage", 
    "Battery", 
    "Other1", 
    "Other2", 
    "Other3", 
    "Other4", 
    "Other5"};

const std::vector<std::string> variables = {"injection", "withdrawal", "level"};

const unsigned int nb_groups = groups.size();
const unsigned int nb_variables = variables.size();


std::string getVariableCaptionFromColumnIndex(int col_index)
{
    if (col_index < nb_groups * nb_variables)
    {
        unsigned int group_index = col_index / nb_variables;
        unsigned int variable_index = col_index % nb_variables;
        return groups[group_index] + "_" + variables[variable_index];
    }
    else
        return "<unknown>";
}

} // End namespace Antares::Data::ShortTermStorage