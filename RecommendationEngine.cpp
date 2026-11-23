#include "RecommendationEngine.h"
#include <algorithm>
#include <QDebug>

RecommendationEngine::RecommendationEngine()
    : m_vectorizer(std::make_unique<TFIDFVectorizer>())
    , m_isTrained(false)
{
}

RecommendationEngine::~RecommendationEngine() = default;

bool RecommendationEngine::train(const QList<Movie> &movies) {
    if (movies.isEmpty()) {
        m_isTrained = false;
        return false;
    }

    m_movies = movies;
    m_titleIndex.clear();

    std::vector<std::string> corpus;
    corpus.reserve(movies.size());

    for (int i = 0; i < movies.size(); ++i) {
        const Movie &movie = movies[i];
        
        // Exact Python logic: indices = pd.Series(movies.index, index=movies["title"].astype(str).str.lower()).drop_duplicates()
        QString key = movie.getTitle().trimmed().toLower();
        if (!m_titleIndex.contains(key)) {
            m_titleIndex[key] = i;
        }

        corpus.push_back(movie.getSoup().toStdString());
    }

    // Fit vectorizer and transform all movie soups to sparse TF-IDF vectors
    m_matrix = m_vectorizer->fitTransform(corpus);
    m_isTrained = !m_matrix.empty();

    qDebug() << "RecommendationEngine trained on" << m_movies.size() << "movies with vocabulary size:" << m_vectorizer->vocabSize();
    return m_isTrained;
}

int RecommendationEngine::findMovieIndex(const QString &title) const {
    QString key = title.trimmed().toLower();
    if (m_titleIndex.contains(key)) {
        return m_titleIndex.value(key);
    }
    
    // Fallback: search for partial match if exact match not found
    for (auto it = m_titleIndex.constBegin(); it != m_titleIndex.constEnd(); ++it) {
        if (it.key().contains(key) || key.contains(it.key())) {
            return it.value();
        }
    }

    return -1;
}

Movie RecommendationEngine::getMovie(int index) const {
    if (index >= 0 && index < m_movies.size()) {
        return m_movies[index];
    }
    return Movie();
}

QStringList RecommendationEngine::getAllTitles() const {
    QStringList titles;
    titles.reserve(m_movies.size());
    for (const auto &m : m_movies) {
        titles.append(m.getTitle());
    }
    return titles;
}

QStringList RecommendationEngine::searchTitles(const QString &keyword, int maxResults) const {
    QStringList results;
    QString q = keyword.trimmed().toLower();
    if (q.isEmpty()) return results;

    for (const auto &m : m_movies) {
        if (m.getTitle().toLower().contains(q)) {
            results.append(m.getTitle());
            if (results.size() >= maxResults) {
                break;
            }
        }
    }
    return results;
}

QList<MovieRecommendation> RecommendationEngine::getRecommendations(const QString &movieTitle, int topN) const {
    int idx = findMovieIndex(movieTitle);
    if (idx < 0) {
        return QList<MovieRecommendation>();
    }
    return getRecommendationsByIndex(idx, topN);
}

QList<MovieRecommendation> RecommendationEngine::getRecommendationsByIndex(int movieIndex, int topN) const {
    QList<MovieRecommendation> recommendations;

    if (!m_isTrained || movieIndex < 0 || movieIndex >= static_cast<int>(m_matrix.size())) {
        return recommendations;
    }

    const SparseVector &targetVec = m_matrix[movieIndex];
    const size_t totalMovies = m_matrix.size();

    // Compute cosine similarity scores with all other movies
    std::vector<std::pair<double, int>> scores;
    scores.reserve(totalMovies);

    for (size_t i = 0; i < totalMovies; ++i) {
        if (static_cast<int>(i) == movieIndex) {
            continue; // Skip the selected movie itself (cosine_sim = 1.0)
        }

        double sim = targetVec.dot(m_matrix[i]);
        scores.push_back({sim, static_cast<int>(i)});
    }

    // Sort descending by similarity score
    std::sort(scores.begin(), scores.end(), [](const auto &a, const auto &b) {
        return a.first > b.first;
    });

    // Pick top N results
    int count = std::min(topN, static_cast<int>(scores.size()));
    for (int i = 0; i < count; ++i) {
        if (scores[i].first <= 0.0) {
            // Only recommend movies with positive similarity if desired, or include top
            // Even low similarity is included to satisfy topN requirement if needed
        }
        MovieRecommendation rec;
        rec.movie = m_movies[scores[i].second];
        rec.similarityScore = std::max(0.0, std::min(1.0, scores[i].first));
        recommendations.append(rec);
    }

    return recommendations;
}
