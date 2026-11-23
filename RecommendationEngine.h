#ifndef RECOMMENDATIONENGINE_H
#define RECOMMENDATIONENGINE_H

#include <QString>
#include <QList>
#include <QMap>
#include <vector>
#include <memory>
#include "../models/Movie.h"
#include "TFIDFVectorizer.h"

/**
 * @brief Represents a single recommended movie with its similarity score.
 */
struct MovieRecommendation {
    Movie movie;
    double similarityScore; // 0.0 to 1.0
    int percentageMatch() const {
        return static_cast<int>(similarityScore * 100.0 + 0.5);
    }
};

/**
 * @brief Independent C++ Movie Recommendation Engine.
 * Implements content-based filtering using TF-IDF and Cosine Similarity on movie metadata 'soup'.
 */
class RecommendationEngine {
public:
    RecommendationEngine();
    ~RecommendationEngine();

    /**
     * @brief Train/initialize the engine with movie data.
     * Extracts 'soup' text, fits TF-IDF vectorizer, and precomputes sparse representations.
     * @param movies The complete list of movies.
     * @return true if initialized successfully.
     */
    bool train(const QList<Movie> &movies);

    /**
     * @brief Check if the model has been trained.
     */
    bool isTrained() const { return m_isTrained; }

    /**
     * @brief Get count of movies in the recommendation model.
     */
    int movieCount() const { return static_cast<int>(m_movies.size()); }

    /**
     * @brief Get recommendations for a given movie title.
     * @param movieTitle The title of the movie (case-insensitive lookup).
     * @param topN Number of recommendations to retrieve (default: 10).
     * @return List of MovieRecommendation objects sorted by similarity descending.
     */
    QList<MovieRecommendation> getRecommendations(const QString &movieTitle, int topN = 10) const;

    /**
     * @brief Get recommendations for a specific movie index in the model.
     */
    QList<MovieRecommendation> getRecommendationsByIndex(int movieIndex, int topN = 10) const;

    /**
     * @brief Find movie index by title (case-insensitive, trimmed).
     * @return -1 if movie not found.
     */
    int findMovieIndex(const QString &title) const;

    /**
     * @brief Get movie at specific model index.
     */
    Movie getMovie(int index) const;

    /**
     * @brief Get all movie titles known to the recommender.
     */
    QStringList getAllTitles() const;

    /**
     * @brief Search movie titles containing a keyword.
     */
    QStringList searchTitles(const QString &keyword, int maxResults = 20) const;

private:
    std::unique_ptr<TFIDFVectorizer> m_vectorizer;
    std::vector<SparseVector> m_matrix;
    QList<Movie> m_movies;
    QMap<QString, int> m_titleIndex; // Lowercase title -> row index
    bool m_isTrained;
};

#endif // RECOMMENDATIONENGINE_H
