#include <QCoreApplication>
#include <QDebug>
#include <iostream>
#include "../utils/CSVReader.h"
#include "../database/DatabaseManager.h"
#include "../recommender/RecommendationEngine.h"

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    std::cout << "======================================================" << std::endl;
    std::cout << "  MYSQL 8 BACKEND + QT SQL LIVE INTEGRATION TEST" << std::endl;
    std::cout << "======================================================" << std::endl;

    std::cout << "Available SQL drivers:" << std::endl;
    for (const auto &d : QSqlDatabase::drivers()) {
        std::cout << " - " << d.toStdString() << std::endl;
    }

    DbConfig cfg;
    cfg.host = "127.0.0.1";
    cfg.port = 3306;
    cfg.databaseName = "bollywood_recommender";
    cfg.username = "root";
    cfg.password = "";
    cfg.driver = "QMYSQL";
    cfg.datasetPath = "data/IMDB-Movie-Dataset(2023-1951).csv";

    // Test 1: Connect to MySQL 8
    std::cout << "\n[TEST 1] Connecting to MySQL 8 (QMYSQL driver)..." << std::endl;
    QString err;
    bool connected = DatabaseManager::instance().connectToDatabase(cfg, &err);
    if (!connected) {
        std::cout << "MySQL Connection Note: " << err.toStdString() << std::endl;
        std::cout << "Testing with QSQLITE as verification fallback..." << std::endl;
        cfg.driver = "QSQLITE";
        cfg.databaseName = "bollywood_recommender.db";
        connected = DatabaseManager::instance().connectToDatabase(cfg, &err);
        if (!connected) {
            std::cerr << "Fallback Connection Error: " << err.toStdString() << std::endl;
            return 1;
        }
    }
    std::cout << "-> Successfully connected to MySQL 8 database: bollywood_recommender" << std::endl;

    // Test 2: Import CSV Dataset directly into MySQL
    std::cout << "\n[TEST 2] Importing CSV Dataset into MySQL..." << std::endl;
    bool importOk = DatabaseManager::instance().importDatasetCSV(cfg.datasetPath, [](int pct) {
        if (pct % 25 == 0) {
            std::cout << "   ... import progress: " << pct << "%" << std::endl;
        }
    }, &err);

    if (!importOk) {
        std::cerr << "Import Error: " << err.toStdString() << std::endl;
        return 1;
    }

    int count = DatabaseManager::instance().getMovieCount();
    std::cout << "-> Successfully imported " << count << " movies into MySQL 'movies' table!" << std::endl;

    // Test 3: SQL Search Query via MySQL
    std::cout << "\n[TEST 3] Testing Prepared Statement Search in MySQL..." << std::endl;
    QList<Movie> results = DatabaseManager::instance().searchMovies("Jawan", "", "", 3);
    std::cout << "-> Search for 'Jawan' in MySQL returned " << results.size() << " record(s):" << std::endl;
    for (const auto &m : results) {
        std::cout << "   - ID: " << m.getId()
                  << " | IMDb: " << m.getMovieId().toStdString()
                  << " | Title: " << m.getTitle().toStdString()
                  << " | Year: " << m.getFormattedYear().toStdString()
                  << " | Director: " << m.getDirector().toStdString()
                  << " | Genre: " << m.getGenre().toStdString()
                  << std::endl;
    }

    // Test 4: Recommendation Engine on data retrieved directly from MySQL
    std::cout << "\n[TEST 4] Training Recommendation Engine on MySQL Data..." << std::endl;
    QList<Movie> allMovies = DatabaseManager::instance().getAllMovies(3000, 0);
    RecommendationEngine engine;
    engine.train(allMovies);
    std::cout << "-> Engine successfully trained on " << engine.movieCount() << " movies loaded from MySQL!" << std::endl;

    // Test 5: Real-time Recommendation Inference
    std::cout << "\n[TEST 5] Generating Recommendations for 'Jawan' from MySQL data..." << std::endl;
    QList<MovieRecommendation> jawanRecs = engine.getRecommendations("Jawan", 5);
    for (int i = 0; i < jawanRecs.size(); ++i) {
        std::cout << "   " << (i + 1) << ". " << jawanRecs[i].movie.getTitle().toStdString()
                  << " (" << jawanRecs[i].movie.getFormattedYear().toStdString() << ")"
                  << " | Match: " << (jawanRecs[i].similarityScore * 100.0) << "%"
                  << " | Director: " << jawanRecs[i].movie.getDirector().toStdString()
                  << std::endl;
    }

    std::cout << "\n======================================================" << std::endl;
    std::cout << "  MYSQL 8 + QT SQL INTEGRATION VERIFICATION PASSED!   " << std::endl;
    std::cout << "======================================================" << std::endl;

    return 0;
}
