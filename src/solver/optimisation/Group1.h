
#include "sim_structure_probleme_economique.h"
#include "opt_structure_probleme_a_resoudre.h"

class Group1
{
public:
    explicit Group1(PROBLEME_HEBDO* problemeHebdo) : problemeHebdo_(problemeHebdo)
    {
    }

    /*TODO Rename this*/
    void BuildGroup1();

private:
    PROBLEME_HEBDO* problemeHebdo_;
};