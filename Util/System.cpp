#include "System.hpp"

#include "Error.hpp"

namespace Util::System {

OsErrorOr<UString> getenv_or_error(std::string const& name) {
    auto env = getenv(name.c_str());
    if (!env) {
        fmt::print("getenv_or_error: {} is not set\n", name);
        return OsError { .error = ENOENT, .function = "getenv_or_error" };
    }
    return UString { env };
}

}
