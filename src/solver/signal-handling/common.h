#pragma once

#include <memory>
#include "antares/study.h"

void signalCtrl_term(int);
void signalCtrl_int(int);

namespace Antares::Solver {
/*!
 * Defines the provided study as the unique study managed by the running application.
 */
void setApplicationStudy(std::weak_ptr<Antares::Data::Study> study);

}
