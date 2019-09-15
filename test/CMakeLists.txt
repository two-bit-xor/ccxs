
include_directories("${PROJECT_SOURCE_DIR}/src")

list(APPEND CMAKE_MODULE_PATH "$ENV{ROOT_WORKING_DIR}/cmocka/cmake/Modules/")

option(ENABLE_COVERAGE "Enable coverage reporting for gcc/clang" FALSE)

if (ENABLE_COVERAGE)
    add_compile_options(--coverage -O0 -g)
endif ()

option(ENABLE_TESTS "Perform unit tests after build" ON)
include(AddCMockaTest)
enable_testing()

add_library(cmocka_lib SHARED IMPORTED)
if (APPLE)
    set_target_properties(cmocka_lib PROPERTIES IMPORTED_LOCATION $ENV{ROOT_WORKING_DIR}/install/cmocka/1.1.5/lib/libcmocka.dylib)
else ()
    set_target_properties(cmocka_lib PROPERTIES IMPORTED_LOCATION $ENV{ROOT_WORKING_DIR}/install/cmocka/1.1.5/lib/libcmocka.so)
endif ()

include_directories($ENV{ROOT_WORKING_DIR}/install/cmocka/1.1.5/include)

file(COPY resources DESTINATION .)
