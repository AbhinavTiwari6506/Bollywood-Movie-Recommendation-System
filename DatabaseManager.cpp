#include "DatabaseManager.h"
#include "../utils/CSVReader.h"
#include <QSettings>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QSqlRecord>

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
    , m_connectionName("BollywoodRecommenderConnection")
    , m_isConnected(false)
{
    loadConfig();
}

DatabaseManager::~DatabaseManager() {
    disconnect();
}

bool DatabaseManager::loadConfig(const QString &configFilePath) {
    QFileInfo checkFile(configFilePath);
    if (!checkFile.exists() || !checkFile.isFile()) {
        qDebug() << "Config file does not exist, using default config:" << configFilePath;
        return false;
    }

    QSettings settings(configFilePath, QSettings::IniFormat);
    settings.beginGroup("Database");
    m_config.host = settings.value("host", "127.0.0.1").toString();
    m_config.port = settings.value("port", 3306).toInt();
    m_config.databaseName = settings.value("database", "bollywood_recommender").toString();
    m_config.username = settings.value("username", "root").toString();
    m_config.password = settings.value("password", "").toString();
    m_config.driver = settings.value("driver", "QMYSQL").toString();
    m_config.datasetPath = settings.value("dataset_path", "data/IMDB-Movie-Dataset(2023-1951).csv").toString();
    settings.endGroup();

    return true;
}

bool DatabaseManager::saveConfig(const DbConfig &config, const QString &configFilePath) {
    m_config = config;
    QSettings settings(configFilePath, QSettings::IniFormat);
    settings.beginGroup("Database");
    settings.setValue("host", config.host);
    settings.setValue("port", config.port);
    settings.setValue("database", config.databaseName);
    settings.setValue("username", config.username);
    settings.setValue("password", config.password);
    settings.setValue("driver", config.driver);
    settings.setValue("dataset_path", config.datasetPath);
    settings.endGroup();
    settings.sync();
    return settings.status() == QSettings::NoError;
}

bool DatabaseManager::connectWithCurrentConfig(QString *errorMessage) {
    return connectToDatabase(m_config, errorMessage);
}

bool DatabaseManager::connectToDatabase(const DbConfig &config, QString *errorMessage) {
    disconnect();
    m_config = config;

    // Check if the requested driver is available
    if (!QSqlDatabase::isDriverAvailable(config.driver)) {
        m_lastError = QString("SQL Driver '%1' is not available in this Qt installation. Available drivers: %2")
                          .arg(config.driver, QSqlDatabase::drivers().join(", "));
        qWarning() << m_lastError;
        if (errorMessage) *errorMessage = m_lastError;
        emit connectionStatusChanged(false, m_lastError);
        return false;
    }

    // If using MySQL, attempt to create database if not exists
    if (config.driver == "QMYSQL") {
        createDatabaseIfNotExists(nullptr);
    }

    QSqlDatabase db = QSqlDatabase::addDatabase(config.driver, m_connectionName);
    db.setHostName(config.host);
    db.setPort(config.port);
    db.setDatabaseName(config.driver == "QSQLITE" ? (config.databaseName.endsWith(".db") ? config.databaseName : config.databaseName + ".db") : config.databaseName);
    db.setUserName(config.username);
    db.setPassword(config.password);

    if (config.driver == "QMYSQL") {
        db.setConnectOptions("CLIENT_MULTI_STATEMENTS=1;MYSQL_OPT_RECONNECT=1;MYSQL_OPT_CONNECT_TIMEOUT=5");
    }

    if (!db.open()) {
        m_lastError = QString("Database connection failed: %1").arg(db.lastError().text());
        qWarning() << m_lastError;
        if (errorMessage) *errorMessage = m_lastError;
        m_isConnected = false;
        emit connectionStatusChanged(false, m_lastError);
        return false;
    }

    m_isConnected = true;
    m_lastError.clear();

    // Ensure schema tables exist
    initializeSchema(nullptr);

    emit connectionStatusChanged(true, QString("Connected successfully via %1 to %2").arg(config.driver, config.databaseName));
    return true;
}

void DatabaseManager::disconnect() {
    if (QSqlDatabase::contains(m_connectionName)) {
        {
            QSqlDatabase db = QSqlDatabase::database(m_connectionName);
            if (db.isOpen()) {
                db.close();
            }
        }
        QSqlDatabase::removeDatabase(m_connectionName);
    }
    m_isConnected = false;
    emit connectionStatusChanged(false, "Disconnected");
}

bool DatabaseManager::isConnected() const {
    if (!m_isConnected) return false;
    if (!QSqlDatabase::contains(m_connectionName)) return false;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    return db.isOpen();
}

QString DatabaseManager::activeDriverName() const {
    if (isConnected()) {
        return QSqlDatabase::database(m_connectionName).driverName();
    }
    return m_config.driver;
}

bool DatabaseManager::createDatabaseIfNotExists(QString *errorMessage) {
    if (m_config.driver != "QMYSQL") {
        return true; // SQLite auto-creates files
    }

    const QString tempConnName = "TempBootstrapMySQL";
    {
        QSqlDatabase tempDb = QSqlDatabase::addDatabase("QMYSQL", tempConnName);
        tempDb.setHostName(m_config.host);
        tempDb.setPort(m_config.port);
        tempDb.setUserName(m_config.username);
        tempDb.setPassword(m_config.password);
        tempDb.setConnectOptions("MYSQL_OPT_CONNECT_TIMEOUT=4");

        if (tempDb.open()) {
            QSqlQuery q(tempDb);
            QString sql = QString("CREATE DATABASE IF NOT EXISTS `%1` CHARACTER SET utf8mb4 COLLATE utf8mb4_unicode_ci")
                              .arg(m_config.databaseName);
            bool ok = q.exec(sql);
            if (!ok && errorMessage) {
                *errorMessage = q.lastError().text();
            }
            tempDb.close();
        } else {
            if (errorMessage) {
                *errorMessage = tempDb.lastError().text();
            }
        }
    }
    QSqlDatabase::removeDatabase(tempConnName);
    return true;
}

bool DatabaseManager::initializeSchema(QString *errorMessage) {
    if (!isConnected()) {
        if (errorMessage) *errorMessage = "Not connected to database.";
        return false;
    }

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);

    QString createTableSql;
    if (db.driverName() == "QSQLITE") {
        createTableSql = 
            "CREATE TABLE IF NOT EXISTS movies ("
            "  id INTEGER PRIMARY KEY AUTOINCREMENT,"
            "  movie_id TEXT NOT NULL UNIQUE,"
            "  movie_title TEXT NOT NULL,"
            "  release_year TEXT,"
            "  genre TEXT,"
            "  overview TEXT,"
            "  director TEXT,"
            "  actors TEXT,"
            "  rating REAL,"
            "  poster_url TEXT,"
            "  soup TEXT,"
            "  created_at DATETIME DEFAULT CURRENT_TIMESTAMP"
            ");";
    } else {
        createTableSql = 
            "CREATE TABLE IF NOT EXISTS movies ("
            "  id INT AUTO_INCREMENT PRIMARY KEY,"
            "  movie_id VARCHAR(50) NOT NULL UNIQUE,"
            "  movie_title VARCHAR(255) NOT NULL,"
            "  release_year VARCHAR(20) DEFAULT NULL,"
            "  genre VARCHAR(255) DEFAULT NULL,"
            "  overview TEXT DEFAULT NULL,"
            "  director VARCHAR(255) DEFAULT NULL,"
            "  actors TEXT DEFAULT NULL,"
            "  rating DECIMAL(3, 1) DEFAULT NULL,"
            "  poster_url VARCHAR(500) DEFAULT NULL,"
            "  soup TEXT DEFAULT NULL,"
            "  created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,"
            "  INDEX idx_movie_title (movie_title),"
            "  INDEX idx_release_year (release_year)"
            ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;";
    }

    if (!query.exec(createTableSql)) {
        m_lastError = query.lastError().text();
        if (errorMessage) *errorMessage = m_lastError;
        qWarning() << "Schema initialization error:" << m_lastError;
        return false;
    }

    return true;
}

Movie DatabaseManager::movieFromQuery(const QSqlQuery &query) const {
    Movie movie;
    movie.setId(query.value("id").toInt());
    movie.setMovieId(query.value("movie_id").toString());
    movie.setTitle(query.value("movie_title").toString());
    movie.setYear(query.value("release_year").toString());
    movie.setGenre(query.value("genre").toString());
    movie.setOverview(query.value("overview").toString());
    movie.setDirector(query.value("director").toString());
    movie.setCast(query.value("actors").toString());
    
    QVariant ratingVal = query.value("rating");
    if (ratingVal.isValid() && !ratingVal.isNull()) {
        movie.setRating(ratingVal.toDouble());
    } else {
        movie.setRating(-1.0);
    }

    movie.setPosterUrl(query.value("poster_url").toString());
    movie.setSoup(query.value("soup").toString());
    return movie;
}

int DatabaseManager::getMovieCount() {
    if (!isConnected()) return 0;
    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query("SELECT COUNT(*) FROM movies", db);
    if (query.next()) {
        return query.value(0).toInt();
    }
    return 0;
}

QList<Movie> DatabaseManager::getAllMovies(int limit, int offset) {
    QList<Movie> list;
    if (!isConnected()) return list;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM movies ORDER BY id ASC LIMIT :limit OFFSET :offset");
    query.bindValue(":limit", limit);
    query.bindValue(":offset", offset);

    if (query.exec()) {
        while (query.next()) {
            list.append(movieFromQuery(query));
        }
    }
    return list;
}

Movie DatabaseManager::getMovieById(int id) {
    if (!isConnected()) return Movie();

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM movies WHERE id = :id LIMIT 1");
    query.bindValue(":id", id);

    if (query.exec() && query.next()) {
        return movieFromQuery(query);
    }
    return Movie();
}

Movie DatabaseManager::getMovieByImdbId(const QString &imdbId) {
    if (!isConnected()) return Movie();

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM movies WHERE movie_id = :id LIMIT 1");
    query.bindValue(":id", imdbId);

    if (query.exec() && query.next()) {
        return movieFromQuery(query);
    }
    return Movie();
}

QList<Movie> DatabaseManager::searchMovies(const QString &searchQuery, const QString &genre, const QString &year, int limit) {
    QList<Movie> list;
    if (!isConnected()) return list;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);

    QString sql = "SELECT * FROM movies WHERE 1=1";
    
    if (!searchQuery.trimmed().isEmpty()) {
        sql += " AND (movie_title LIKE :kw OR director LIKE :kw OR actors LIKE :kw)";
    }
    if (!genre.trimmed().isEmpty() && genre != "All Genres") {
        sql += " AND genre LIKE :genre";
    }
    if (!year.trimmed().isEmpty() && year != "All Years") {
        sql += " AND release_year LIKE :year";
    }

    sql += " ORDER BY id ASC LIMIT :limit";
    query.prepare(sql);

    if (!searchQuery.trimmed().isEmpty()) {
        query.bindValue(":kw", QString("%%1%").arg(searchQuery.trimmed()));
    }
    if (!genre.trimmed().isEmpty() && genre != "All Genres") {
        query.bindValue(":genre", QString("%%1%").arg(genre.trimmed()));
    }
    if (!year.trimmed().isEmpty() && year != "All Years") {
        query.bindValue(":year", QString("%%1%").arg(year.trimmed()));
    }
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            list.append(movieFromQuery(query));
        }
    } else {
        qWarning() << "Search query failed:" << query.lastError().text();
    }
    return list;
}

QList<Movie> DatabaseManager::getFeaturedMovies(int limit) {
    QList<Movie> list;
    if (!isConnected()) return list;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    // Return sample featured movies (e.g. from 2000 onwards or top rated)
    query.prepare("SELECT * FROM movies WHERE release_year >= '2000' ORDER BY id ASC LIMIT :limit");
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            list.append(movieFromQuery(query));
        }
    }
    return list;
}

QList<Movie> DatabaseManager::getRecentMovies(int limit) {
    QList<Movie> list;
    if (!isConnected()) return list;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM movies ORDER BY release_year DESC, id ASC LIMIT :limit");
    query.bindValue(":limit", limit);

    if (query.exec()) {
        while (query.next()) {
            list.append(movieFromQuery(query));
        }
    }
    return list;
}

QStringList DatabaseManager::getUniqueGenres() {
    QStringList genres;
    if (!isConnected()) return genres;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query("SELECT DISTINCT genre FROM movies WHERE genre IS NOT NULL AND genre != ''", db);
    
    QSet<QString> uniqueSet;
    while (query.next()) {
        QString raw = query.value(0).toString();
        const auto parts = raw.split(',', Qt::SkipEmptyParts);
        for (const auto &p : parts) {
            uniqueSet.insert(p.trimmed());
        }
    }

    genres = uniqueSet.values();
    genres.sort();
    return genres;
}

QStringList DatabaseManager::getUniqueYears() {
    QStringList years;
    if (!isConnected()) return years;

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    QSqlQuery query("SELECT DISTINCT release_year FROM movies WHERE release_year IS NOT NULL AND release_year != '' ORDER BY release_year DESC", db);

    while (query.next()) {
        QString y = query.value(0).toString().trimmed();
        if (!y.isEmpty() && !years.contains(y)) {
            years.append(y);
        }
    }
    return years;
}

bool DatabaseManager::importMoviesFromList(const QList<Movie> &movies,
                                          std::function<void(int, int)> progressCallback,
                                          QString *errorMessage) {
    if (!isConnected()) {
        if (errorMessage) *errorMessage = "Database is not connected.";
        return false;
    }

    if (movies.isEmpty()) {
        return true;
    }

    QSqlDatabase db = QSqlDatabase::database(m_connectionName);
    db.transaction();

    QString insertSql;
    if (db.driverName() == "QSQLITE") {
        insertSql = 
            "INSERT OR REPLACE INTO movies (movie_id, movie_title, release_year, genre, overview, director, actors, rating, poster_url, soup) "
            "VALUES (:mid, :title, :year, :genre, :overview, :director, :cast, :rating, :poster, :soup)";
    } else {
        insertSql = 
            "INSERT INTO movies (movie_id, movie_title, release_year, genre, overview, director, actors, rating, poster_url, soup) "
            "VALUES (:mid, :title, :year, :genre, :overview, :director, :cast, :rating, :poster, :soup) "
            "ON DUPLICATE KEY UPDATE movie_title=VALUES(movie_title), release_year=VALUES(release_year), genre=VALUES(genre), "
            "overview=VALUES(overview), director=VALUES(director), actors=VALUES(actors), rating=VALUES(rating), "
            "poster_url=VALUES(poster_url), soup=VALUES(soup)";
    }

    QSqlQuery query(db);
    query.prepare(insertSql);

    const int total = movies.size();
    for (int i = 0; i < total; ++i) {
        const Movie &m = movies[i];
        query.bindValue(":mid", m.getMovieId());
        query.bindValue(":title", m.getTitle());
        query.bindValue(":year", m.getYear());
        query.bindValue(":genre", m.getGenre());
        query.bindValue(":overview", m.getOverview());
        query.bindValue(":director", m.getDirector());
        query.bindValue(":cast", m.getCast());
        
        if (m.getRating() >= 0.0) {
            query.bindValue(":rating", m.getRating());
        } else {
            query.bindValue(":rating", QVariant(QMetaType(QMetaType::Double)));
        }

        query.bindValue(":poster", m.getPosterUrl());
        query.bindValue(":soup", m.getSoup());

        if (!query.exec()) {
            db.rollback();
            m_lastError = QString("Failed to insert movie '%1': %2").arg(m.getTitle(), query.lastError().text());
            if (errorMessage) *errorMessage = m_lastError;
            return false;
        }

        if (progressCallback && i % 50 == 0) {
            progressCallback(i + 1, total);
        }
    }

    db.commit();
    if (progressCallback) {
        progressCallback(total, total);
    }

    emit dataChanged();
    return true;
}

bool DatabaseManager::importDatasetCSV(const QString &csvPath,
                                      std::function<void(int)> progressCallback,
                                      QString *errorMessage) {
    emit importProgress(5, "Reading CSV file...");

    QString parseErr;
    QList<Movie> movies = CSVReader::readMovies(csvPath, [&](int pct) {
        emit importProgress(static_cast<int>(5 + pct * 0.45), "Parsing CSV dataset...");
        if (progressCallback) progressCallback(static_cast<int>(5 + pct * 0.45));
    }, &parseErr);

    if (!parseErr.isEmpty()) {
        m_lastError = parseErr;
        if (errorMessage) *errorMessage = parseErr;
        return false;
    }

    emit importProgress(55, QString("Importing %1 movies into database...").arg(movies.size()));

    bool ok = importMoviesFromList(movies, [&](int current, int total) {
        int pct = 55 + static_cast<int>((current * 45) / total);
        emit importProgress(pct, QString("Writing records %1/%2...").arg(current).arg(total));
        if (progressCallback) progressCallback(pct);
    }, errorMessage);

    if (ok) {
        emit importProgress(100, "Dataset import completed successfully!");
    }

    return ok;
}
