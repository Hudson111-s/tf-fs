cmake_minimum_required(VERSION 3.10)
project(tf-fs C)

# This is WinFsp default install location, change if needed.
set(WINFSP_DIR "C:/Program Files (x86)/WinFsp")

add_executable(tf-fs
    main_fuse.c
    src/bitmap.c
    src/block.c
    src/inode.c
    src/tf-fs.c
    src/utils.c
)

target_include_directories(tf-fs PRIVATE 
    include 
    disk
    "${WINFSP_DIR}/inc"
    "${WINFSP_DIR}/inc/fuse"
)

target_link_libraries(tf-fs PRIVATE "${WINFSP_DIR}/lib/winfsp-x64.lib")

target_compile_definitions(tf-fs PRIVATE FUSE_USE_VERSION=28)

add_custom_command(TARGET tf-fs POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "${WINFSP_DIR}/bin/winfsp-x64.dll"
    $<TARGET_FILE_DIR:tf-fs>
)

set_target_properties(tf-fs PROPERTIES
    C_STANDARD 11
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)