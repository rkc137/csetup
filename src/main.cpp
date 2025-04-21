#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

std::string read_resourse(std::string &&file)
{
    std::ifstream ifs("res/" + file);
    using It = std::istreambuf_iterator<char>;
    return std::string(It(ifs), It());
}

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
                throw std::runtime_error("missing folder path\n");
            std::string arg = argv[++i];
            if(fs::exists(arg))
                throw std::runtime_error(arg + " allready exists\n");
            project_path = fs::absolute({arg});
        }
        else if(cmd == "-n" || cmd == "--name")
        {
            if(!(i < argc - 1))
                throw std::runtime_error("missing name\n");
            project_name = argv[++i];
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

    std::ofstream cmake_file(path / "CMakeLists.txt"); 
    if(!cmake_file.is_open())
        throw std::runtime_error("cannot make cmake file");

    std::string cmake_lists = read_resourse("CMakeLists.template");
    std::string standard = "23";
    cmake_lists = std::regex_replace(cmake_lists, std::regex("CSETUP_NAME"), name);
    cmake_lists = std::regex_replace(cmake_lists, std::regex("CSETUP_STANDARD"), standard);
    cmake_file << cmake_lists;
    
    std::ofstream git_ignore_file(path / ".gitignore");
    if(!git_ignore_file.is_open())
        throw std::runtime_error("cannot make gitignore file");
    git_ignore_file << read_resourse("gitignore.template");

    if(!fs::create_directory(path / "src"))
        throw std::runtime_error("cannot create folder");
    std::ofstream main_cpp_file(path / "src/main.cpp");
    main_cpp_file << read_resourse("main.template");
        
    
    return 0;
}