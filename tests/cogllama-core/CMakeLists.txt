cmake_minimum_required(VERSION 3.14)

# CogLlama Core Unit Tests
add_executable(test-cogllama-core test-cogllama-core.c)

target_include_directories(test-cogllama-core PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/cogllama-core/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../ggml/include
)

target_link_libraries(test-cogllama-core PRIVATE
    cogllama-core
    ggml
)

target_compile_features(test-cogllama-core PRIVATE c_std_99)

# Add test to CTest
add_test(NAME cogllama-core COMMAND test-cogllama-core)
