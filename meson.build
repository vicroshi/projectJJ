project('projectJJ', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++23'])

# These arguments are only used to build the shared library
# not the executables that use the library.
includes = include_directories('include')
subdir('src')
executable('projectJJ', ['main.cpp'], include_directories : includes, link_with : vamana_lib)