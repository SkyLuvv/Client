#include "FileStuff.h"

#include "FileStuff.h"
#include "Windows.h"
FileStuff::File::File(std::string path, filetype ft)

    :
    fpath(path)
{
    switch (ft)
    {
    case filetype::read:

        is.open(path.c_str(), std::ios::binary | std::ios::in);
        if (!is.is_open())
            last_error = "FileStuff::File::File() constructor failed: Couldnt open the file for reading";
        else isopen = true;
        break;

    case filetype::write:
        os.open(path.c_str(), std::ios::out | std::ios_base::binary);
        if (!os.is_open())
            last_error = "FileStuff::File::File() constructor failed: Couldnt open the file for writing";
        else isopen = true;
        break;
    }
}
bool FileStuff::File::WriteData(std::vector<char>& buffer, const size_t& length)
{

    //get position of file pointer before writing
    auto before = os.tellp();

    os.write(buffer.data(), length);

    //writing to file modifies fileptr. so the pos after writing - the pos before writing tells us how many bytes
    //were written
    auto byteswritten = os.tellp() - before;

    if (byteswritten == length)
        return true;
    else if (os.fail())
    {
        last_error = "FileStuff::File::WriteData() failed: Failed to write to the file";
        return false;
    }
    else
    {
        last_error = "FileStuff::File::WriteData() failed: Either didn't finish writing or something went wrong during";
        return false;
    }

}

bool FileStuff::File::CopyTo(File& file)

{

    std::vector<char> buffer(Length().value());

    std::cout << "Copying ... " << buffer.size() << "bytes to " << file.fpath << std::endl;

    //check if "is" has not reached the end of the file and if it has set any fail flags. 
    //if it reached the end of the file the flag will be set and it will break the loop so we can make sure we read everything
    //as well as if any error flags have been set
    if (is.good() && file.os.good())
    {

        //copy the data to our buffer then use the buffer to write the data to the new file
        is.read(buffer.data(), buffer.size());

        if (is.fail())
        {
            std::cout << "Could not read data." << std::endl;
            return false;
        }

        //gcount() tells us how much was read and stored successfully by ifstream::read after each call
        //std::cout << "read... " << is.gcount() << "bytes" << std::endl;
        auto bytesread = is.gcount();

        while (true)
        {

            auto before = file.os.tellp();

            file.os.write(buffer.data(), bytesread);

            auto byteswritten = file.os.tellp() - before;

            std::cout << "wrote... " << byteswritten << " bytes " << "out of " << bytesread << std::endl;


            if (byteswritten == bytesread)
                return true;
            else if (file.os.fail())
                return false;
            else
            {
                std::cout << "Trying to copy again..." << std::endl;

                //reset input file pointer
                is.seekg(0, is.beg);

                //close and open the file whie clearing its contents ::trunt specifies this 
                //so that we can try and copy the data again
                file.os.close();
                file.os.open(file.fpath, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);
            }
        }
    }

    if (file.os.fail())
        return false;


    return false;

}
std::optional<size_t> FileStuff::File::Length()

{

    if (!is.is_open())
    {
        last_error = "FileStuff::File::FileLength() failed: File wasn't opened.";
        return {};
    }

    //move file pointer to end of file
    is.seekg(0, is.end);
    //the end of the file represents the size in bytes
    size_t length = is.tellg();
    //reset the pointer
    is.seekg(0, is.beg);

    return length;

}
std::optional<std::vector<char>> FileStuff::File::GetData()
{

    //check if "is" has not reached the end of the file and if it has set any fail flags. 
    //if it reached the end of the file the flag will be set and it will break the loop so we can make sure we read everything
    //as well as if any error flags have been set
    if (!is.good())
    {
        last_error = "FileStuff::File::GetFileBytes() failed: is.good() return false. Maybe the file wasn't opened properly.";
        return {};
    }

    auto len{ Length().value() };
    std::vector<char> buffer(len);

    //copy the data to our buffer then use the buffer
    is.read(buffer.data(), buffer.size());

    if (is.fail())
    {
        last_error = "FileStuff::File::GetFileBytes() failed: Failed to read the file";
        return {};
    }

    //gcount() tells us how much was read and stored successfully by ifstream::read after each call

    auto bytesread = is.gcount();

    if (bytesread == len)
        return { std::move(buffer) };           //remember this is actually moved instead of copied as it returns
    else
    {
        last_error = "FileStuff::File::GetFileBytes() failed: The bytes that were read was not equal to the file length";
        return {};
    }

}
bool FileStuff::File::is_opened()
{
    return isopen;
}
const std::string FileStuff::File::GetLastError() const
{
    return last_error;
}
std::optional<std::string> FileStuff::OpenfileDialogueBox()
{
    //used to init getopenfilename and stores selection info once getopenfilename returns
    //c++ zero initialization
    OPENFILENAMEA ofn = { 0 };
    ofn.hwndOwner = NULL;
    ofn.lStructSize = sizeof(ofn);

    //set all elements to null terminator so when the functions checks the first element is a null terminator, it will acknowledge theres no default file to start with
    char filename[200];
    ofn.lpstrFile = filename;
    ofn.lpstrFile[0] = '\0';
    ofn.lStructSize = sizeof(ofn);
    ofn.nMaxFile = sizeof(filename);
    //we can put a name we want and the corresponding file type and it appears in the dialogue box
    //it allows us to filter the files we want
    ofn.lpstrFilter = "All Files\0*.*\0Text Files\0*.TXT\0Zip Files\0*.zip\0MP3 Files\0*.mp3\0M4A Files\0*.m4a\0Flac Files\0*.flac\0";

    if (!GetOpenFileNameA(&ofn))
        return {};
    else
        return filename;


}
std::optional<std::string> FileStuff::GetFilenamefromPath(const std::string& path)
{
    std::string path_temp = path;
    //start from the end of the string. -1 because the last element is the null terminator
    //we need to include 0 bc its the first element, so make it < -1 
    for (size_t i = path_temp.length() - 1; i < -1; i--)
    {
        if (path_temp[i] == '\\')
        {
            //start at the first letter of the file, and include all the letters after till the end of the string
            std::string filename = path_temp.substr(i + 1, (path.length() - i));
            filename.push_back('\0');
            return filename;
        }
    }

    return {};
}
