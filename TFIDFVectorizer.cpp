#include "TFIDFVectorizer.h"
#include <cmath>
#include <algorithm>
#include <cctype>
#include <map>

TFIDFVectorizer::TFIDFVectorizer()
    : m_docCount(0)
{
    initStopWords();
}

void TFIDFVectorizer::initStopWords() {
    // Complete standard scikit-learn 'english' stop words list
    static const char *const stopWordsList[] = {
        "a", "about", "above", "across", "after", "afterwards", "again", "against", "all",
        "almost", "alone", "along", "already", "also", "although", "always", "am", "among",
        "amongst", "amoungst", "amount", "an", "and", "another", "any", "anyhow", "anyone",
        "anything", "anyway", "anywhere", "are", "around", "as", "at", "back", "be",
        "became", "because", "become", "becomes", "becoming", "been", "before", "beforehand",
        "behind", "being", "below", "beside", "besides", "between", "beyond", "bill", "both",
        "bottom", "but", "by", "call", "can", "cannot", "cant", "co", "con", "could",
        "couldnt", "cry", "de", "describe", "detail", "do", "done", "down", "due", "during",
        "each", "eg", "eight", "either", "eleven", "else", "elsewhere", "empty", "enough",
        "etc", "even", "ever", "every", "everyone", "everything", "everywhere", "except", "few",
        "fifteen", "fifty", "fill", "find", "fire", "first", "five", "for", "former", "formerly",
        "forty", "found", "four", "from", "front", "full", "further", "get", "give", "go",
        "had", "has", "hasnt", "have", "he", "hence", "her", "here", "hereafter", "hereby",
        "herein", "hereupon", "hers", "herself", "him", "himself", "his", "how", "however",
        "hundred", "i", "ie", "if", "in", "inc", "indeed", "interest", "into", "is", "it",
        "its", "itself", "keep", "last", "latter", "latterly", "least", "less", "ltd", "made",
        "many", "may", "me", "meanwhile", "might", "mill", "mine", "more", "moreover", "most",
        "mostly", "move", "much", "must", "my", "myself", "name", "namely", "neither", "never",
        "nevertheless", "next", "nine", "no", "nobody", "none", "noone", "nor", "not", "nothing",
        "now", "nowhere", "of", "off", "often", "on", "once", "one", "only", "onto", "or",
        "other", "others", "otherwise", "our", "ours", "ourselves", "out", "over", "own", "part",
        "per", "perhaps", "please", "put", "rather", "re", "same", "see", "seem", "seemed",
        "seeming", "seems", "serious", "several", "she", "should", "show", "side", "since",
        "sincere", "six", "sixty", "so", "some", "somehow", "someone", "something", "sometime",
        "sometimes", "somewhere", "still", "such", "system", "take", "ten", "than", "that",
        "the", "their", "them", "themselves", "then", "thence", "there", "thereafter",
        "thereby", "therefore", "therein", "thereupon", "these", "they", "thick", "thin",
        "third", "this", "those", "though", "three", "through", "throughout", "thru", "thus",
        "to", "together", "too", "top", "toward", "towards", "twelve", "twenty", "two", "un",
        "under", "until", "up", "upon", "us", "very", "via", "was", "we", "well", "were",
        "what", "whatever", "when", "whence", "whenever", "where", "whereafter", "whereas",
        "whereby", "wherein", "whereupon", "wherever", "whether", "which", "while", "whither",
        "who", "whoever", "whole", "whom", "whose", "why", "will", "with", "within", "without",
        "would", "yet", "you", "your", "yours", "yourself", "yourselves"
    };

    for (const char *word : stopWordsList) {
        m_stopWords.insert(word);
    }
}

bool TFIDFVectorizer::isStopWord(const std::string &word) const {
    return m_stopWords.find(word) != m_stopWords.end();
}

std::vector<std::string> TFIDFVectorizer::tokenize(const std::string &text) const {
    std::vector<std::string> tokens;
    std::string current;

    for (char c : text) {
        if (std::isalnum(static_cast<unsigned char>(c))) {
            current.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(c))));
        } else {
            // Token boundary: keep only if length >= 2 and not a stop word
            if (current.length() >= 2 && !isStopWord(current)) {
                tokens.push_back(current);
            }
            current.clear();
        }
    }

    if (current.length() >= 2 && !isStopWord(current)) {
        tokens.push_back(current);
    }

    return tokens;
}

void TFIDFVectorizer::fit(const std::vector<std::string> &corpus) {
    m_vocab.clear();
    m_idf.clear();
    m_docCount = corpus.size();

    if (m_docCount == 0) return;

    // Count document frequencies (df) for each term
    std::unordered_map<std::string, int> docFreq;

    for (const auto &doc : corpus) {
        auto tokens = tokenize(doc);
        std::unordered_set<std::string> uniqueTokens(tokens.begin(), tokens.end());
        for (const auto &token : uniqueTokens) {
            docFreq[token]++;
        }
    }

    // Build vocabulary index sorted for deterministic IDs
    std::vector<std::pair<std::string, int>> sortedVocab(docFreq.begin(), docFreq.end());
    std::sort(sortedVocab.begin(), sortedVocab.end(),
              [](const auto &a, const auto &b) { return a.first < b.first; });

    m_idf.resize(sortedVocab.size());
    const double N = static_cast<double>(m_docCount);

    for (size_t i = 0; i < sortedVocab.size(); ++i) {
        const std::string &term = sortedVocab[i].first;
        m_vocab[term] = static_cast<int>(i);

        // Scikit-learn formula: idf(t) = ln((1 + N) / (1 + df(t))) + 1.0
        double df = static_cast<double>(sortedVocab[i].second);
        m_idf[i] = std::log((1.0 + N) / (1.0 + df)) + 1.0;
    }
}

std::vector<SparseVector> TFIDFVectorizer::transform(const std::vector<std::string> &corpus) const {
    std::vector<SparseVector> matrix;
    matrix.reserve(corpus.size());

    for (const auto &doc : corpus) {
        auto tokens = tokenize(doc);

        // Count term frequency in document
        std::map<int, double> termCounts; // map keeps term_id sorted!
        for (const auto &token : tokens) {
            auto it = m_vocab.find(token);
            if (it != m_vocab.end()) {
                termCounts[it->second] += 1.0;
            }
        }

        SparseVector vec;
        double sumSq = 0.0;

        for (const auto &pair : termCounts) {
            int termId = pair.first;
            double tf = pair.second;
            double tfidf = tf * m_idf[termId];
            vec.terms.push_back({termId, tfidf});
            sumSq += tfidf * tfidf;
        }

        // L2 normalization: vector / sqrt(sum(tfidf^2))
        if (sumSq > 0.0) {
            double norm = std::sqrt(sumSq);
            for (auto &pair : vec.terms) {
                pair.second /= norm;
            }
        }

        matrix.push_back(std::move(vec));
    }

    return matrix;
}

std::vector<SparseVector> TFIDFVectorizer::fitTransform(const std::vector<std::string> &corpus) {
    fit(corpus);
    return transform(corpus);
}
