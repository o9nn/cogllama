cmake_minimum_required(VERSION 3.14)

# CogLlama Core Library
add_library(cogllama-core
    src/atomspace.c
    src/cogserver.c
)

target_include_directories(cogllama-core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ggml/include
)

target_link_libraries(cogllama-core PUBLIC
    ggml
)

# Set C standard
target_compile_features(cogllama-core PUBLIC c_std_99)

# Install targets
install(TARGETS cogllama-core
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/cogllama-core
    FILES_MATCHING PATTERN "*.h"
)
