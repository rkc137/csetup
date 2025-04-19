#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    std::optional<std::string> project_name;
    std::optional<fs::path> project_path;

    for(int i = 1; i < argc; i++)
    {
        std::string cmd = argv[i];
        if(cmd == "-f" || cmd == "--folder")
        {
            if(!(i < argc - 1))
            {
                std::cerr << "missing folder path\n";
                return 1;
            }
            std::string arg = argv[++i];
            if(fs::exists(arg))
            {
                std::cerr << arg << "allready exists\n";
                return 2;
            }
            project_path = fs::absolute({arg});
        }
        else if(cmd == "-n" || cmd == "--name")
        {
            if(!(i < argc - 1))
            {
                std::cerr << "missing name\n";
                return 1;
            }
            std::string arg = argv[++i];
            if(arg.empty())
            {
                std::cerr << "name should be even 1 long\n";
                return 3;
            }
            project_name = arg;
        }
    }
    std::string name = project_name.value_or("project");
    fs::path path = [&](){
        if(project_path.has_value()) return project_path.value();
        std::string retname = name; 
        for(int i = 1; fs::exists(retname); i++)
            retname = name + std::to_string(i);
        return fs::absolute({retname});
    }();
    if(!fs::create_directory(path))
    {
        std::cerr << "cannot make directory\n";
        return 4;
    }

    std::string standard = "23";
    std::string cmake_lists = [&](){
        std::ifstream ifs("res/CMakeLists.template");
        using It = std::istreambuf_iterator<char>;
        return std::string(It(ifs), It());
    }();
    cmake_lists = std::regex_replace(cmake_lists, std::regex("CSETUP_NAME"), name);
    cmake_lists = std::regex_replace(cmake_lists, std::regex("CSETUP_STANDARD"), standard);
    
    std::ofstream cmake_file(path / "CMakeLists.txt");
    cmake_file << cmake_lists;
    
    std::ofstream git_ignore_file(path / ".gitignore");
    git_ignore_file << "build\n.vscode";

    fs::create_directory(path / "src");
    std::ofstream main_cpp(path / "src/main.cpp");
    main_cpp << "\n\nint main()\n{\n\t\n\treturn 0;\n}";
    
    return 0;
}