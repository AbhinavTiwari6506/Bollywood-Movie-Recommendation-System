#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QString>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include "../models/Movie.h"

struct DbConfig {
    QString host = "127.0.0.1";
    int port = 3306;
    QString databaseName = "bollywood_recommender";
    QString username = "root";
    QString password = "";
    QString driver = "QMYSQL";
    QString datasetPath = "data/IMDB-Movie-Dataset(2023-1951).csv";
};

/**
 * @brief Thread-safe Database Manager handling MySQL and SQLite connections,
 * prepared statement queries, movie searches, and dataset importation.
 */
class DatabaseManager : public QObject {
    Q_OBJECT

public:
    static DatabaseManager& instance();

    // Configuration
    bool loadConfig(const QString &configFilePath = "config.ini");
    bool saveConfig(const DbConfig &config, const QString &configFilePath = "config.ini");
    DbConfig currentConfig() const { return m_config; }

    // Connection lifecycle
    bool connectToDatabase(const DbConfig &config, QString *errorMessage = nullptr);
    bool connectWithCurrentConfig(QString *errorMessage = nullptr);
    void disconnect();
    bool isConnected() const;
    QString lastError() const { return m_lastError; }
    QString activeDriverName() const;

    // Database Initialization & Schema
    bool createDatabaseIfNotExists(QString *errorMessage = nullptr);
    bool initializeSchema(QString *errorMessage = nullptr);

    // Movie Operations
    int getMovieCount();
    QList<Movie> getAllMovies(int limit = 500, int offset = 0);
    Movie getMovieById(int id);
    Movie getMovieByImdbId(const QString &imdbId);
    QList<Movie> searchMovies(const QString &query, const QString &genre = "", const QString &year = "", int limit = 100);
    QList<Movie> getFeaturedMovies(int limit = 12);
    QList<Movie> getRecentMovies(int limit = 12);
    QStringList getUniqueGenres();
    QStringList getUniqueYears();

    // Dataset Importing
    bool importMoviesFromList(const QList<Movie> &movies,
                              std::function<void(int current, int total)> progressCallback = nullptr,
                              QString *errorMessage = nullptr);
    bool importDatasetCSV(const QString &csvPath,
                          std::function<void(int percent)> progressCallback = nullptr,
                          QString *errorMessage = nullptr);

signals:
    void connectionStatusChanged(bool connected, const QString &message);
    void importProgress(int percentage, const QString &statusText);
    void dataChanged();

private:
    DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    Movie movieFromQuery(const QSqlQuery &query) const;

    DbConfig m_config;
    QString m_connectionName;
    QString m_lastError;
    bool m_isConnected;
};

#endif // DATABASEMANAGER_H
