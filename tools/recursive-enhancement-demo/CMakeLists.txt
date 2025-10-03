cmake_minimum_required(VERSION 3.14)

if(LLAMA_BUILD_TOOLS)
    add_executable(llama-recursive-enhancement-demo recursive-enhancement-demo.cpp)
    
    target_link_libraries(llama-recursive-enhancement-demo PRIVATE
        opencog_qat
        llama
        common
    )
    
    target_include_directories(llama-recursive-enhancement-demo PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../../src/opencog-qat
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
        ${CMAKE_CURRENT_SOURCE_DIR}/../../common
    )
    
    target_compile_features(llama-recursive-enhancement-demo PRIVATE cxx_std_17)
endif()