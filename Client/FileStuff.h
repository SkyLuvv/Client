#pragma once

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
#include <optional>
enum class filetype
{
    read, write
};

namespace FileStuff
{
    std::optional<std::string> OpenfileDialogueBox();
    std::optional<std::string> GetFilenamefromPath(const std::string& path);
    class File
    {
    public:
        File(std::string path, filetype ft);
        File() = delete;
        bool WriteData(std::vector<char>& buffer, const size_t& length);

        bool CopyTo(File& file);
        std::optional<size_t> Length();
        std::optional<std::vector<char>> GetData();
        bool is_opened();
        const std::string GetLastError() const;
        ~File()
        {
            if (is.is_open())
                is.close();
            if (os.is_open())
                os.close();
        }
    public:
        std::ifstream is;
        std::ofstream os;
        std::string fpath;
    private:
        bool isopen = false;
        std::string last_error;
    };
}