#ifndef __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_COLLECTOR_H__
#define __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_COLLECTOR_H__

#include <ui/common/component/spotlight.h>

using namespace Yuni;

namespace Antares
{
namespace Window
{
namespace OutputViewer
{
namespace Provider
{
class VariableCollector
{
public:
    //! Spotlight (alias)
    typedef Antares::Component::Spotlight Spotlight;

public:
    VariableCollector(Spotlight::IItem::Vector& out,
                      const Spotlight::SearchToken::Vector& tokens,
                      wxBitmap* bmp,
                      wxBitmap* thermalBmp,
                      const String& selection);

    void add(const AnyString& name, const AnyString& /*unit*/, const AnyString& /*comments*/);

    void addCluster(const String& name);

private:
    //! The results
    Spotlight::IItem::Vector& pOut;
    //! Search tokens
    const Spotlight::SearchToken::Vector& pTokens;
    //! Bitmap
    wxBitmap* pBmp;
    wxBitmap* pThermalBmp;
    //!
    const String& pCurrentSelection;

}; // class VariableCollector

} // namespace Provider
} // namespace OutputViewer
} // namespace Window
} // namespace Antares

#endif // __ANTARES_WINDOWS_OUTPUT_PROVIDER_VARIABLES_COLLECTOR_H__