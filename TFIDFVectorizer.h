#ifndef TFIDFVECTORIZER_H
#define TFIDFVECTORIZER_H

#include <QString>
#include <QStringList>
#include <QVector>
#include <QMap>
#include <QSet>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

/**
 * @brief Represents a sparse TF-IDF vector for a document.
 * Pairs of (term_id, normalized_tfidf_weight) sorted by term_id for fast dot products.
 */
struct SparseVector {
    std::vector<std::pair<int, double>> terms;

    // Fast dot product between two sparse L2-normalized vectors
    double dot(const SparseVector &other) const {
        double sum = 0.0;
        size_t i = 0, j = 0;
        const size_t n1 = terms.size();
        const size_t n2 = other.terms.size();

        while (i < n1 && j < n2) {
            if (terms[i].first == other.terms[j].first) {
                sum += terms[i].second * other.terms[j].second;
                ++i;
                ++j;
            } else if (terms[i].first < other.terms[j].first) {
                ++i;
            } else {
                ++j;
            }
        }
        return sum;
    }
};

/**
 * @brief C++ implementation of Scikit-Learn's TfidfVectorizer with 'english' stop words.
 */
class TFIDFVectorizer {
public:
    TFIDFVectorizer();

    /**
     * @brief Fit vocabulary and compute IDF values on a corpus of text documents.
     */
    void fit(const std::vector<std::string> &corpus);

    /**
     * @brief Transform corpus documents into sparse L2-normalized vectors.
     */
    std::vector<SparseVector> transform(const std::vector<std::string> &corpus) const;

    /**
     * @brief Fit and transform in a single pass.
     */
    std::vector<SparseVector> fitTransform(const std::vector<std::string> &corpus);

    /**
     * @brief Tokenize a document into lowercased alphanumeric words, removing stop words.
     */
    std::vector<std::string> tokenize(const std::string &text) const;

    /**
     * @brief Get vocabulary size.
     */
    size_t vocabSize() const { return m_vocab.size(); }

    /**
     * @brief Check if a word is in English stop words list.
     */
    bool isStopWord(const std::string &word) const;

private:
    void initStopWords();

    std::unordered_map<std::string, int> m_vocab;
    std::vector<double> m_idf;
    std::unordered_set<std::string> m_stopWords;
    size_t m_docCount;
};

#endif // TFIDFVECTORIZER_H
