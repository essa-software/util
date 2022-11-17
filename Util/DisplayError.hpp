#pragma once

#include <EssaUtil/SourceLocation.hpp>
#include <EssaUtil/Stream/Stream.hpp>
#include <EssaUtil/UString.hpp>

namespace Util {

struct DisplayedError {
    UString message;
    size_t start_offset;
    size_t end_offset;
};

void display_error(ReadableStream& input, DisplayedError);

}
