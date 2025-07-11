#include <iostream>
#include <fstream>
#include <optional>
#include <filesystem>
#include <regex>

#include <string_literal.hpp>

namespace fs = std::filesystem;

std::string read_resourse(fs::path &&file)
{
    std::ifstream ifs(file);
    using It = std::istreambuf_iterator<char>;
    return std::string(It(ifs), It());
}

template <rkc::string_literal_t msg>
void exit_if(bool expr)
{
    if(!expr) return; 
    // i dont care if you cant grep this message btw
    std::cout << msg << "\nusage: csetup\n"
        << "\t- -f, --folder <path>: Specify the folder path for the project. Must not already exist.\n"
        << "\t- -n, --name <name>: Specify the project name (must be at least 1 character long). \n";
    std::exit(0);
};

int main(int argc, char *argv[])
{
    std::optional<std::string> input_project_name;
    std::optional<std::string> cxx_standart;
    std::optional<fs::path> input_project_path;

    auto templates_path = [&]() {
        std::string path(argv[0]);
        #ifdef _WIN32
            char slash = '\\';
        #elif __unix__
            char slash = '/';
        #endif
        return fs::path{path.begin(), path.begin() + path.find_last_of(slash)} / "templates";
    }();

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

    std::ofstream cmake_file(project_path / "CMakeLists.txt"); 
    if(!cmake_file.is_open())
        throw std::runtime_error("cannot make cmake file");

    std::string cmake_lists = read_resourse(templates_path / "CMakeLists.template");
    std::string standard = "23";
    cmake_lists = std::regex_replace(cmake_lists, std::regex("CSETUP_NAME"), project_name);
    cmake_lists = std::regex_replace(cmake_lists, std::regex("CSETUP_STANDARD"), standard);
    cmake_file << cmake_lists;
    
    std::ofstream git_ignore_file(project_path / ".gitignore");
    if(!git_ignore_file.is_open())
        throw std::runtime_error("cannot make gitignore file");
    git_ignore_file << read_resourse(templates_path / "gitignore.template");

    if(!fs::create_directory(project_path / "src"))
        throw std::runtime_error("cannot create folder");
    std::ofstream main_cpp_file(project_path / "src/main.cpp");
    main_cpp_file << read_resourse(templates_path / "main.template");
    
    return 0;
}