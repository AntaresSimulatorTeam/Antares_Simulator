#pragma once

#include <memory>
#include <writer/i_writer.h>

void signalCtrl_term(int);
void signalCtrl_int(int);

namespace Antares::Solver {
/*!
 * Defines the provided writer as the unique writer managed by the running application.
 */
void setApplicationResultWriter(std::weak_ptr<IResultWriter> writer);

}
