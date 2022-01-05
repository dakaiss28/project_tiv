cmake_minimum_required(VERSION 3.5)
project(Projet_OpenCV_CMake)

set(CMAKE_CXX_STANDARD 14)
find_package(OpenCV REQUIRED)

include_directories(include ${OpenCV_INCLUDE_DIRS})


add_executable(Projet_OpenCV_CMake
        include/utility/histogram.hpp
        src/utility/histogram.cpp
        include/utility/DataPathGenerator.hpp
        src/utility/DataPathGenerator.cpp
        include/utility/SnippetExtractor.hpp
        src/utility/SnippetExtractor.cpp
        include/utility/TextExtractionManager.hpp src/utility/TextExtractionManager.cpp
        include/utility/ImageRecognitionManager.hpp src/utility/ImageRecognitionManager.cpp
        include/utility/QualityChecker.hpp src/utility/QualityChecker.cpp
        src/main.cpp)

target_link_libraries(Projet_OpenCV_CMake ${OpenCV_LIBS})
