#include "main.h"

void PrintSize(size_t size)
{
    if (size < 1024)
    {
        std::cout << size << " B";
    }
    else if (size < 1024 * 1024)
    {
        std::cout << size / 1024.0 << " KB";
    }
    else
    {
        std::cout << size / 1024.0 / 1024.0 << " MB";
    }
}


#ifdef _WIN32
int ProcessFile(const wchar_t file_path[])
{
    std::wcout << L"Processing: " << file_path << std::endl;
#else
// written like this in order to be callback funtion of ftw()
int ProcessFile(const char file_path[], const struct stat *sb = nullptr, int typeflag = FTW_F)
{
    if (typeflag != FTW_F)
    {
        return 0;
    }
    std::cout << "Processing: " << file_path << std::endl;
#endif // _WIN32


    File input_file(file_path);

    if (input_file.IsOK())
    {
        size_t original_size = input_file.GetSize();

        size_t new_size = LeanifyFile(input_file.GetFilePionter(), original_size);

        PrintSize(original_size);
        std::cout << " -> ";
        PrintSize(new_size);
        std::cout << "\tLeanified: ";
        PrintSize(original_size - new_size);

        std::cout << " (" << 100 - 100.0 * new_size / original_size << "%)" << std::endl;

        input_file.UnMapFile(new_size);
    }

    return 0;
}


void PauseIfNotTerminal()
{
    // pause if Leanify is not started in terminal
    // so that user can see the output instead of just a flash of a black box
#ifdef _WIN32
    if (!getenv("PROMPT"))
    {
        system("pause");
    }
#endif // _WIN32
}



void PrintInfo()
{
    std::cout << "Leanify\t" << VERSION << std::endl << std::endl;
    std::cout << "Usage: Leanify [options] path" << std::endl;
    std::cout << "  -i iteration\tMore iterations means slower but better result. Default: 15." << std::endl;
    std::cout << "  -f\t\tFast mode, no recompression." << std::endl;
    std::cout << "  -q\t\tQuiet mode, no output." << std::endl;
    std::cout << "  -v\t\tVerbose output." << std::endl;
    PauseIfNotTerminal();
}


#ifdef _WIN32
int main()
{
    int argc;
    wchar_t** argv = CommandLineToArgvW(GetCommandLineW(), &argc);
#else
int main(int argc, char const *argv[])
{
#endif // _WIN32
    if (argc < 2)
    {

        PrintInfo();

        return 1;
    }
    is_recompress = true;
    is_verbose = false;
    iterations = 15;
    level = 0;

    int i;
    for (i = 1; i < argc && argv[i][0] == L'-'; i++)
    {
        switch (argv[i][1])
        {
        case 'f':
        case 'F':
            is_recompress = false;
            break;
        case 'i':
        case 'I':
            if (i < argc - 1)
            {
#ifdef _WIN32
                iterations = _wtoi(argv[++i]);
#else
                iterations = atoi(argv[++i]);
#endif // _WIN32
            }
            break;
        case 'q':
        case 'Q':
            std::wcout.setstate(std::ios::failbit);
            std::cout.setstate(std::ios::failbit);
            std::cerr.setstate(std::ios::failbit);
            break;
        case 'v':
        case 'V':
            is_verbose = true;
            break;
        default:
            PrintInfo();
            return 1;
        }
    }

#ifdef _WIN32
    wchar_t *file_path = argv[i];
#else
    const char *file_path = argv[i];
#endif // _WIN32


    std::cout << std::fixed;
    std::cout.precision(2);

    if (IsDirectory(file_path))
    {
        // directory
        TraverseDirectory(file_path, ProcessFile);
    }
    else
    {
        // file
        ProcessFile(file_path);
    }

    PauseIfNotTerminal();

    return 0;
}