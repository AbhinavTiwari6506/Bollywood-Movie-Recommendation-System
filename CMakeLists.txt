cmake_minimum_required(VERSION 3.16)

project(BollywoodMovieRecommender
    VERSION 2.0.0
    DESCRIPTION "Bollywood Movie Recommendation System in C++17 (Qt 6 + MySQL 8)"
    LANGUAGES CXX
)

# C++17 Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Enable Qt MOC, UIC, RCC
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt 6 Packages
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Sql)

# Project Include Directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/models
    ${CMAKE_CURRENT_SOURCE_DIR}/src/utils
    ${CMAKE_CURRENT_SOURCE_DIR}/src/database
    ${CMAKE_CURRENT_SOURCE_DIR}/src/recommender
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ui
)

# Source Files
set(SOURCES
    src/main.cpp
    src/models/Movie.h
    src/models/Movie.cpp
    src/utils/CSVReader.h
    src/utils/CSVReader.cpp
    src/database/DatabaseManager.h
    src/database/DatabaseManager.cpp
    src/recommender/TFIDFVectorizer.h
    src/recommender/TFIDFVectorizer.cpp
    src/recommender/RecommendationEngine.h
    src/recommender/RecommendationEngine.cpp
    src/ui/MovieCard.h
    src/ui/MovieCard.cpp
    src/ui/MovieDetailsDialog.h
    src/ui/MovieDetailsDialog.cpp
    src/ui/DbConfigDialog.h
    src/ui/DbConfigDialog.cpp
    src/ui/HomePage.h
    src/ui/HomePage.cpp
    src/ui/SearchPage.h
    src/ui/SearchPage.cpp
    src/ui/RecommendationPage.h
    src/ui/RecommendationPage.cpp
    src/ui/AllMoviesPage.h
    src/ui/AllMoviesPage.cpp
    src/ui/AboutPage.h
    src/ui/AboutPage.cpp
    src/ui/MainWindow.h
    src/ui/MainWindow.cpp
)

# Executable Target
add_executable(BollywoodMovieRecommender ${SOURCES})

# Target Link Libraries
target_link_libraries(BollywoodMovieRecommender PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
)

# Test Verification Target
add_executable(TestEngine
    src/tests/test_engine.cpp
    src/models/Movie.h
    src/models/Movie.cpp
    src/utils/CSVReader.h
    src/utils/CSVReader.cpp
    src/database/DatabaseManager.h
    src/database/DatabaseManager.cpp
    src/recommender/TFIDFVectorizer.h
    src/recommender/TFIDFVectorizer.cpp
    src/recommender/RecommendationEngine.h
    src/recommender/RecommendationEngine.cpp
)
target_link_libraries(TestEngine PRIVATE
    Qt6::Core
    Qt6::Sql
)

# Test MySQL Target
add_executable(TestMySQL
    src/tests/test_mysql.cpp
    src/models/Movie.h
    src/models/Movie.cpp
    src/utils/CSVReader.h
    src/utils/CSVReader.cpp
    src/database/DatabaseManager.h
    src/database/DatabaseManager.cpp
    src/recommender/TFIDFVectorizer.h
    src/recommender/TFIDFVectorizer.cpp
    src/recommender/RecommendationEngine.h
    src/recommender/RecommendationEngine.cpp
)
target_link_libraries(TestMySQL PRIVATE
    Qt6::Core
    Qt6::Sql
)

# Copy dataset and config.ini to output directory for easy execution
add_custom_command(TARGET BollywoodMovieRecommender POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/config.ini"
        "$<TARGET_FILE_DIR:BollywoodMovieRecommender>/config.ini"
    COMMAND ${CMAKE_COMMAND} -E make_directory
        "$<TARGET_FILE_DIR:BollywoodMovieRecommender>/data"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${CMAKE_CURRENT_SOURCE_DIR}/data/IMDB-Movie-Dataset(2023-1951).csv"
        "$<TARGET_FILE_DIR:BollywoodMovieRecommender>/data/IMDB-Movie-Dataset(2023-1951).csv"
    COMMENT "Copying configuration and dataset to target build directory..."
)
