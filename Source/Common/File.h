// Created by mooming.go@gmail.com 2016

#pragma once

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace OS
{
    class File
    {
    private:
        std::string path;

    public:
        explicit File(const char* path)
            : path(path)
        {
        }

        File(const File& src) = default;
        File(File&& src) noexcept = default;

        File& operator=(const File& src) = default;
        File& operator=(File&& src) noexcept = default;

        bool operator<(const File& rhs) const { return path < rhs.path; }

        friend std::ostream& operator<<(std::ostream& os, const File& file)
        {
            os << file.path;

            return os;
        }

        [[nodiscard]] auto& GetPath() const { return path; }
    };

    using Files = std::vector<File>;
} // namespace OS
