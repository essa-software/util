#include "Color.hpp"

#include <iomanip>
#include <sstream>

namespace Util {

Util::UString Color::to_html_string() const {
    std::ostringstream oss;
    oss << "#" << std::hex << std::setfill('0');
    oss << std::setw(2) << (int)r << std::setw(2) << (int)g << std::setw(2) << (int)b;
    return Util::UString { oss.str() };
}

}
