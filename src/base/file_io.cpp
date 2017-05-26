#include "file_io.hpp"
#include <fstream>

namespace kvant {
namespace base {

    std::string read_textfile(const char* filename)
    {
        std::string result;

        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (file.is_open()) 
        {
            const size_t filesize = static_cast<size_t>(file.tellg());
            file.seekg(0, std::ios::beg);
            result.resize(filesize);
            file.read(&result[0], filesize);
        }

        return result;
    }

} // namespace base
} // namespace kvant
