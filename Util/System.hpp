#pragma once

#include "Error.hpp"
#include "UString.hpp"

namespace Util::System {

OsErrorOr<UString> getenv_or_error(std::string const& name);

}
