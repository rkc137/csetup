# csetup

`csetup` is a cl utility for quick setup CMake & C++ project.

## Usage

```
csetup [options]
```

### Options

- `-f`, `--folder <path>`: Specify the folder path for the project. Must not already exist.
- `-n`, `--name <name>`: Specify the project name (must be at least 1 character long).

### Example

1. Create a project named "MyProject" in folder where it calls was:

   ```bash
   csetup -n MyProject
   ```

2. Create a project in a specified folder (by default project name will be the same as folders):

   ```bash
   csetup -f /path/to/folder
   ```

## Generated Files

output folder looks like this

```
/MyProject
├── CMakeLists.txt
├── .gitignore
└── src
    └── main.cpp
```
