#ifndef CSVREADER_H
#define CSVREADER_H

#include <QString>
#include <QList>
#include <functional>
#include "../models/Movie.h"

/**
 * @brief High-performance RFC 4180 compliant CSV parser for the Bollywood dataset.
 */
class CSVReader {
public:
    CSVReader();

    /**
     * @brief Parse CSV file into a list of Movie objects.
     * @param filePath Path to the CSV file.
     * @param progressCallback Optional callback reporting progress (0-100%).
     * @param errorMessage Output parameter for error messages if parsing fails.
     * @return List of parsed Movie objects.
     */
    static QList<Movie> readMovies(const QString &filePath,
                                  std::function<void(int)> progressCallback = nullptr,
                                  QString *errorMessage = nullptr);

    /**
     * @brief Parse a single CSV line into tokens, respecting quotes and escapes.
     */
    static QStringList parseCSVLine(const QString &line);
};

#endif // CSVREADER_H
