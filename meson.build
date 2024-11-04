project('projectJJ', 'cpp',
        version : '1.0.0',
        default_options : ['warning_level=3', 'cpp_std=c++23','optimization=3'])

cpp = meson.get_compiler('cpp')
# Setting compiler arguments for AVX/AVX2, if supported
if cpp.has_argument('-march=native')
    add_project_arguments('-march=native', language: 'cpp')
elif cpp.has_argument('-mavx2')
    add_project_arguments('-mavx2', language: 'cpp')
elif cpp.has_argument('-mavx')
    add_project_arguments('-mavx', language: 'cpp')
else
    error('AVX or AVX2 is not supported on this compiler or CPU.')
endif

# Adding optimization flag -O3
add_project_arguments('-O3', language: 'cpp')

# These arguments are only used to build the shared library
# not the executables that use the library.
includes = include_directories('include')
subdir('src')
executable('projectJJ', ['main.cpp'], include_directories : includes, link_with : vamana_lib)

test_src = ['tests/tests_all.cpp',
            'tests/test_matrix.cpp',
            'tests/test_utils.cpp',
            'tests/test_vamana.cpp']

e = executable('unit_tests',
                sources: test_src,
                include_directories: includes,
                link_with : vamana_lib
                )

test('unit_tests',e)
