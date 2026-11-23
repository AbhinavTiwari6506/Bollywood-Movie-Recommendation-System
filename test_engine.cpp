#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include "../utils/CSVReader.h"
#include "../database/DatabaseManager.h"
#include "../recommender/RecommendationEngine.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "======================================================" << std::endl;
    std::cout << "  BOLLYWOOD RECOMMENDER C++ ENGINE VERIFICATION TEST" << std::endl;
    std::cout << "======================================================" << std::endl;

    // Test 1: CSV Reader
    std::cout << "\n[TEST 1] Testing CSV Reader..." << std::endl;
    QString csvPath = "data/IMDB-Movie-Dataset(2023-1951).csv";
    QString err;
    QList<Movie> movies = CSVReader::readMovies(csvPath, nullptr, &err);
    if (!err.isEmpty()) {
        std::cerr << "CSV Read Error: " << err.toStdString() << std::endl;
        return 1;
    }
    std::cout << "-> Successfully read " << movies.size() << " movies from CSV." << std::endl;
    if (movies.isEmpty()) {
        std::cerr << "FAIL: Movies list is empty!" << std::endl;
        return 1;
    }
    std::cout << "-> Sample Movie: " << movies[0].getTitle().toStdString()
              << " (" << movies[0].getFormattedYear().toStdString() << ")"
              << " | Director: " << movies[0].getDirector().toStdString() << std::endl;

    // Test 2: Database Operations (SQLite verification mode)
    std::cout << "\n[TEST 2] Testing Database Manager..." << std::endl;
    DbConfig testCfg;
    testCfg.driver = "QSQLITE";
    testCfg.databaseName = "test_bollywood.db";
    bool dbConnected = DatabaseManager::instance().connectToDatabase(testCfg, &err);
    if (!dbConnected) {
        std::cerr << "Database connection failed: " << err.toStdString() << std::endl;
        return 1;
    }
    std::cout << "-> Database connected successfully with driver: " << testCfg.driver.toStdString() << std::endl;

    // Import movies into DB
    std::cout << "-> Inserting movies into database..." << std::endl;
    bool importOk = DatabaseManager::instance().importMoviesFromList(movies, nullptr, &err);
    if (!importOk) {
        std::cerr << "Import failed: " << err.toStdString() << std::endl;
        return 1;
    }
    int dbCount = DatabaseManager::instance().getMovieCount();
    std::cout << "-> Successfully verified " << dbCount << " movies in database table." << std::endl;

    // Search query test
    std::cout << "\n[TEST 3] Testing Database Search..." << std::endl;
    QList<Movie> searchRes = DatabaseManager::instance().searchMovies("Khan", "", "", 5);
    std::cout << "-> Search for 'Khan' returned " << searchRes.size() << " results." << std::endl;
    for (const auto &m : searchRes) {
        std::cout << "   - " << m.getTitle().toStdString() << " (" << m.getFormattedYear().toStdString() << ")"
                  << " | Cast: " << m.getCast().left(45).toStdString() << "..." << std::endl;
    }

    // Test 4: Recommendation Engine (TF-IDF & Cosine Similarity)
    std::cout << "\n[TEST 4] Testing Recommendation Engine..." << std::endl;
    RecommendationEngine engine;
    bool trained = engine.train(movies);
    if (!trained) {
        std::cerr << "FAIL: Recommendation engine failed to train!" << std::endl;
        return 1;
    }
    std::cout << "-> Engine trained successfully on " << engine.movieCount() << " movies." << std::endl;

    // Recommendations for "Sholay" (Matches notebook test)
    std::cout << "\n[TEST 5] Generating Recommendations for 'Sholay'..." << std::endl;
    QList<MovieRecommendation> recsSholay = engine.getRecommendations("Sholay", 10);
    if (recsSholay.isEmpty()) {
        std::cerr << "FAIL: No recommendations generated for 'Sholay'!" << std::endl;
        return 1;
    }
    std::cout << "Top recommendations for 'Sholay':" << std::endl;
    for (int i = 0; i < recsSholay.size(); ++i) {
        std::cout << "   " << (i + 1) << ". " << recsSholay[i].movie.getTitle().toStdString()
                  << " (" << recsSholay[i].movie.getFormattedYear().toStdString() << ")"
                  << " | Similarity: " << (recsSholay[i].similarityScore * 100.0) << "%"
                  << " | Genre: " << recsSholay[i].movie.getGenre().toStdString()
                  << std::endl;
    }

    // Recommendations for "3 Idiots"
    std::cout << "\n[TEST 6] Generating Recommendations for '3 Idiots'..." << std::endl;
    QList<MovieRecommendation> recs3Idiots = engine.getRecommendations("3 Idiots", 5);
    std::cout << "Top recommendations for '3 Idiots':" << std::endl;
    for (int i = 0; i < recs3Idiots.size(); ++i) {
        std::cout << "   " << (i + 1) << ". " << recs3Idiots[i].movie.getTitle().toStdString()
                  << " (" << recs3Idiots[i].movie.getFormattedYear().toStdString() << ")"
                  << " | Similarity: " << (recs3Idiots[i].similarityScore * 100.0) << "%"
                  << std::endl;
    }

    std::cout << "\n======================================================" << std::endl;
    std::cout << "  ALL C++ ENGINE VERIFICATION TESTS PASSED SUCCESSFULLY!" << std::endl;
    std::cout << "======================================================" << std::endl;

    return 0;
}
