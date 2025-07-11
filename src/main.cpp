#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>
#include <regex>

#include <rkc/string_literal.hpp>

#include "battery/embed.hpp"

namespace fs = std::filesystem;

template <rkc::string_literal_t msg>
void exit_if(bool expr)
{
    if(!expr) return; 
    // i dont care if you cant grep this message btw
    std::cout << msg << "\nusage: csetup\n"
        << "\t\t-f, --folder <path>: Specify the folder path for the project. Must not already exist.\n"
        << "\t\t-n, --name <name>: Specify the project name (must be at least 1 character long). \n";
    std::exit(0);
};

int main(int argc, char *argv[])
{
    std::optional<std::string> input_project_name;
    std::optional<std::string> cxx_standart;
    std::optional<fs::path> input_project_path;

    for(int i = 1; i < argc; i++)
    {
        std::string cmd = argv[i];
        if(cmd == "-f" || cmd == "--folder")
        {
            exit_if<"missing folder path">(i >= argc - 1);
            std::string arg = argv[++i];
            exit_if<"folder with this name allready exists">(fs::exists(arg));
            input_project_path = fs::absolute({arg});
        }
        else if(cmd == "-n" || cmd == "--name")
        {
            exit_if<"missing name">(i >= argc - 1);
            input_project_name = argv[++i];
        }
        else exit_if<"">(cmd == "-h" || cmd == "--help");
    }

    std::string project_name = input_project_name.value_or("project");
    fs::path project_path = [&](){
        if(input_project_path.has_value()) return input_project_path.value();
        auto crtpath = fs::current_path();
        std::string name = project_name; 
        for(int i = 1; fs::exists(crtpath / name); i++)
            name = project_name + std::to_string(i);
        return crtpath / name;
    }();
    if(!fs::create_directory(project_path))
        throw std::runtime_error("cannot create " + project_path.string());
                
    //  cmake
    std::ofstream cmake_file(project_path / "CMakeLists.txt"); 
    if(!cmake_file.is_open())
        throw std::runtime_error("cannot make cmake file");
    std::string cmake_lists =  b::embed<"res/templates/CMakeLists.txt">();
    std::string standard = "23";
    cmake_lists = std::regex_replace(cmake_lists, std::regex("CSETUP_NAME"), project_name);
    cmake_lists = std::regex_replace(cmake_lists, std::regex("CSETUP_STANDARD"), standard);
    
    // gitignore
    std::ofstream git_ignore_file(project_path / ".gitignore");
    if(!git_ignore_file.is_open())
        throw std::runtime_error("cannot make gitignore file");
    auto git_ignore = b::embed<"res/templates/.gitignore">().str();

    // source
    if(!fs::create_directory(project_path / "src"))
        throw std::runtime_error("cannot create folder");
    std::ofstream main_cpp_file(project_path / "src/main.cpp");
    auto main_cpp = b::embed<"res/templates/main.cpp">().str();

    
    // clear all that \r\n bullshit
    for(auto s : {&cmake_lists, &git_ignore, &main_cpp})
        s->erase(std::remove(s->begin(), s->end(), '\r'), s->end());
    
    cmake_file.write(cmake_lists.c_str(), cmake_lists.size());
    git_ignore_file.write(git_ignore.c_str(), git_ignore.size());
    main_cpp_file.write(main_cpp.c_str(), main_cpp.size());
    
    return 0;
}