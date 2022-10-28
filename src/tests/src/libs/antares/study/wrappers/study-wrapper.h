
#include <study.h>

namespace Antares
{
namespace Data
{
class Study;
/*!
** \brief Antares StudyWrapper
*/

using pStudy = std::shared_ptr<Study>;
class StudyWrapper
{

public:
    using Ptr = std::shared_ptr<StudyWrapper>;

    explicit StudyWrapper(pStudy _pstudy):
        pstudy(_pstudy){};
    
    //! \name Loading wrapper
    //@{
    //! Load a study from a folder
    bool internalLoadFromFolder(const YString& path, const StudyLoadOptions& options){
        return pstudy->internalLoadFromFolder(path, options);
    }

    //! Load the study header
    bool internalLoadHeader(const YString& folder){
        return pstudy->internalLoadHeader(folder);
    }
    //! Load all correlation matrices
    bool internalLoadCorrelationMatrices(const StudyLoadOptions& options){
        return pstudy->internalLoadCorrelationMatrices(options);
    }
    //! Load all binding constraints
    bool internalLoadBindingConstraints(const StudyLoadOptions& options){
        return pstudy->internalLoadBindingConstraints(options);
    }
    //! Load all set of areas and links
    bool internalLoadSets(){
        return pstudy->internalLoadSets();
    }
    //@}

    bool initializeInternalData(const StudyLoadOptions& options){
        return pstudy->initializeInternalData(options);
    }
    void initializeSetsData(){
        pstudy->initializeSetsData();
    }

    //! \name Misc
    //@{
    //! Reset the input extension according the study version
    void inputExtensionCompatibility(){
        pstudy->inputExtensionCompatibility();
    }
    //! Release all unnecessary buffers
    void reduceMemoryUsage(){
        pstudy->reduceMemoryUsage();
    }
    //@}

    

private:
    pStudy pstudy;
    

}; // class StudyWrapper

} // namespace Data
} // namespace Antares

#include "runtime.h"

