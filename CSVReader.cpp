#include "CSVReader.h"
#include <QFile>
#include <QTextStream>
#include <QFileInfo>
#include <QMap>
#include <QDebug>

CSVReader::CSVReader() {}

QStringList CSVReader::parseCSVLine(const QString &line) {
    QStringList fields;
    QString currentField;
    bool inQuotes = false;
    const int len = line.length();

    for (int i = 0; i < len; ++i) {
        QChar c = line[i];

        if (c == '"') {
            if (inQuotes && i + 1 < len && line[i + 1] == '"') {
                // Escaped double quotes: "" -> "
                currentField += '"';
                ++i; // skip second quote
            } else {
                // Toggle quotes
                inQuotes = !inQuotes;
            }
        } else if (c == ',' && !inQuotes) {
            fields.append(currentField.trimmed());
            currentField.clear();
        } else {
            currentField += c;
        }
    }
    fields.append(currentField.trimmed());
    return fields;
}

QList<Movie> CSVReader::readMovies(const QString &filePath,
                                  std::function<void(int)> progressCallback,
                                  QString *errorMessage) {
    QList<Movie> movies;
    QFile file(filePath);

    if (!file.exists()) {
        if (errorMessage) {
            *errorMessage = QString("Dataset file not found at: %1").arg(filePath);
        }
        return movies;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (errorMessage) {
            *errorMessage = QString("Cannot open dataset file: %1").arg(file.errorString());
        }
        return movies;
    }

    qint64 totalBytes = file.size();
    qint64 processedBytes = 0;

    QTextStream in(&file);
    in.setEncoding(QStringConverter::Utf8);

    // Read header line
    if (in.atEnd()) {
        if (errorMessage) {
            *errorMessage = "Dataset file is empty.";
        }
        return movies;
    }

    QString headerLine = in.readLine();
    processedBytes += headerLine.toUtf8().size() + 1;
    QStringList headers = parseCSVLine(headerLine);

    // Normalize header names to lowercase and map them
    QMap<QString, int> colMap;
    for (int i = 0; i < headers.size(); ++i) {
        QString h = headers[i].trimmed().toLower();
        
        // Match the rename mapping from the original Python project
        if (h == "movie_name" || h == "name" || h == "title") {
            colMap["title"] = i;
        } else if (h == "movie id" || h == "movie_id") {
            colMap["movie_id"] = i;
        } else if (h == "year") {
            colMap["year"] = i;
        } else if (h == "genre" || h == "genres") {
            colMap["genre"] = i;
        } else if (h == "overview" || h == "synopsis" || h == "description") {
            colMap["overview"] = i;
        } else if (h == "director") {
            colMap["director"] = i;
        } else if (h == "cast" || h == "actors") {
            colMap["cast"] = i;
        } else if (h == "rating" || h == "imdb_rating") {
            colMap["rating"] = i;
        } else if (h == "poster_url" || h == "poster") {
            colMap["poster_url"] = i;
        }
    }

    // Verify required columns: title, genre, overview
    if (!colMap.contains("title") || !colMap.contains("genre") || !colMap.contains("overview")) {
        if (errorMessage) {
            *errorMessage = QString("Required columns (title, genre, overview) missing from CSV.\nFound: %1")
                                .arg(headers.join(", "));
        }
        return movies;
    }

    int rowIndex = 0;
    QString currentRecord;
    bool inMultiLineQuote = false;

    while (!in.atEnd()) {
        QString line = in.readLine();
        processedBytes += line.toUtf8().size() + 1;

        if (inMultiLineQuote) {
            currentRecord += "\n" + line;
        } else {
            currentRecord = line;
        }

        // Count unescaped quotes in line to determine multi-line status
        int quoteCount = 0;
        for (int i = 0; i < line.size(); ++i) {
            if (line[i] == '"') {
                if (i + 1 < line.size() && line[i + 1] == '"') {
                    ++i; // skip escaped
                } else {
                    quoteCount++;
                }
            }
        }

        if (quoteCount % 2 != 0) {
            inMultiLineQuote = !inMultiLineQuote;
        }

        if (inMultiLineQuote) {
            continue; // Continue reading remaining lines of this record
        }

        if (currentRecord.trimmed().isEmpty()) {
            continue;
        }

        QStringList row = parseCSVLine(currentRecord);
        currentRecord.clear();

        auto getCol = [&](const QString &key) -> QString {
            if (colMap.contains(key)) {
                int idx = colMap[key];
                if (idx < row.size()) {
                    return row[idx].trimmed();
                }
            }
            return QString();
        };

        QString title = getCol("title");
        if (title.isEmpty()) {
            continue;
        }

        QString movieId = getCol("movie_id");
        if (movieId.isEmpty()) {
            movieId = QString("id_%1").arg(rowIndex);
        }

        QString year = getCol("year");
        QString genre = getCol("genre");
        QString overview = getCol("overview");
        QString director = getCol("director");
        QString cast = getCol("cast");
        
        double rating = -1.0;
        if (colMap.contains("rating")) {
            bool ok = false;
            double r = getCol("rating").toDouble(&ok);
            if (ok) rating = r;
        }

        QString posterUrl = getCol("poster_url");

        Movie movie(rowIndex + 1, movieId, title, year, genre, overview, director, cast, rating, posterUrl);
        movies.append(movie);
        rowIndex++;

        if (progressCallback && totalBytes > 0 && rowIndex % 100 == 0) {
            int pct = static_cast<int>((processedBytes * 100) / totalBytes);
            progressCallback(qMin(100, pct));
        }
    }

    if (progressCallback) {
        progressCallback(100);
    }

    return movies;
}
