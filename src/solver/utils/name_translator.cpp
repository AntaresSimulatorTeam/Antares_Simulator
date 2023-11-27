#include "name_translator.h"
#include <algorithm>
#include <iterator>

char** RealName::translate(const std::vector<std::string>& src,
                     std::vector<char*>& pointerVec)
{
      std::transform(src.begin(),
                     src.end(),
                     std::back_inserter(pointerVec),
                     [](const std::string& str) { return str.empty() ? nullptr : const_cast<char*>(str.data()); });
      return pointerVec.data();
}

char** NullName::translate(const std::vector<std::string>& src,
                     std::vector<char*>& pointerVec)
{
  pointerVec.assign(src.size(), nullptr);
  return pointerVec.data();
}

std::unique_ptr<NameTranslator> NameTranslator::create(bool useRealNames)
{
  if (useRealNames)
    return std::make_unique<RealName>();
  else
    return std::make_unique<NullName>();
}
