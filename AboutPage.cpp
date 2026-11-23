#include "AboutPage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QFrame>

AboutPage::AboutPage(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void AboutPage::setupUI() {
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    QWidget *container = new QWidget(scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(36, 30, 36, 30);
    mainLayout->setSpacing(24);

    // Title Section
    QLabel *heading = new QLabel("About the Project", container);
    QFont hFont;
    hFont.setPointSize(20);
    hFont.setBold(true);
    heading->setFont(hFont);
    heading->setStyleSheet("color: #FFFFFF; font-weight: bold;");
    mainLayout->addWidget(heading);

    QLabel *lead = new QLabel(
        "Bollywood Movie Recommendation System is a high-performance desktop software application "
        "built using Modern C++ (C++17), Qt 6 Widgets, and MySQL 8 Database. Originally prototyped in Python, "
        "it has been re-engineered into an enterprise-grade C++ architecture suitable for college demonstrations and defense.", container);
    lead->setStyleSheet("color: #CBD5E1; font-size: 14px; line-height: 1.5;");
    lead->setWordWrap(true);
    mainLayout->addWidget(lead);

    auto createSectionCard = [container](const QString &title, const QString &content) -> QFrame* {
        QFrame *card = new QFrame(container);
        card->setStyleSheet(
            "QFrame {"
            "  background-color: #181A20;"
            "  border: 1px solid #2B2F3A;"
            "  border-radius: 12px;"
            "  padding: 20px;"
            "}");
        QVBoxLayout *l = new QVBoxLayout(card);
        l->setContentsMargins(18, 16, 18, 16);
        l->setSpacing(10);

        QLabel *t = new QLabel(title, card);
        QFont tf;
        tf.setPointSize(13);
        tf.setBold(true);
        t->setFont(tf);
        t->setStyleSheet("color: #E50914; font-weight: bold;");
        l->addWidget(t);

        QLabel *c = new QLabel(content, card);
        c->setStyleSheet("color: #E2E8F0; font-size: 13px; line-height: 1.5;");
        c->setWordWrap(true);
        l->addWidget(c);

        return card;
    };

    // Tech Stack
    mainLayout->addWidget(createSectionCard(
        "🛠️ Technologies & Architectural Stack",
        "• <b>Programming Language:</b> C++17 with Modern Standard Library (STL)<br>"
        "• <b>GUI Framework:</b> Qt 6 Widgets (QMainWindow, QStackedWidget, QScrollArea, QDialog)<br>"
        "• <b>Backend Database:</b> MySQL 8.0 / InnoDB (with SQLite local fallback driver)<br>"
        "• <b>Database Integration:</b> Qt SQL Module (QSqlDatabase, QSqlQuery prepared statements)<br>"
        "• <b>Build System:</b> CMake (Cross-platform, Qt Creator and VS Code compatible)<br>"
        "• <b>Target Platform:</b> Windows 10 / 11"
    ));

    // Data Science Pipeline & Algorithm
    mainLayout->addWidget(createSectionCard(
        "🧠 Machine Learning & Recommendation Algorithm",
        "The system implements Content-Based Filtering using Natural Language Processing (NLP):<br><br>"
        "1. <b>Feature Soup Engineering:</b> For each movie, metadata tokens from <i>Genre</i>, <i>Overview/Synopsis</i>, <i>Director</i>, and <i>Cast</i> are fused into a unified document representation.<br>"
        "2. <b>Tokenization & Filtering:</b> Text is normalized to lowercase, punctuation stripped, and 318 standard English stop words are filtered.<br>"
        "3. <b>TF-IDF Vectorization:</b> Computes Term Frequency (TF) and Inverse Document Frequency (IDF) with smooth IDF: <i>idf(t) = ln((1 + N) / (1 + df(t))) + 1.0</i>, followed by L2 vector normalization.<br>"
        "4. <b>Cosine Similarity Calculation:</b> Calculates the cosine angle (dot product) between sparse document vectors: <i>cos_sim(A, B) = A · B</i>.<br>"
        "5. <b>Top-N Recommendation Inference:</b> Sorts all movies by similarity descending and returns the top 5–20 closest matches."
    ));

    // Dataset Info
    mainLayout->addWidget(createSectionCard(
        "📊 Bollywood Movie Dataset",
        "• <b>Source:</b> IMDb Bollywood Movies Collection (1951 to 2023)<br>"
        "• <b>Total Records:</b> 2,199 Movies<br>"
        "• <b>Metadata Attributes:</b> IMDb ID, Movie Title, Release Year, Genres, Plot Synopsis, Directors, Lead Actors, Ratings<br>"
        "• <b>Storage:</b> Persisted inside the MySQL database table <code>movies</code> with indexing on title, year, and genre."
    ));

    // Architectural Transition
    mainLayout->addWidget(createSectionCard(
        "🔄 Evolution from Python to Modern C++",
        "• <b>Python Prototype:</b> Used Streamlit web server, Pandas in-memory DataFrames, and Scikit-Learn.<br>"
        "• <b>C++ Desktop System:</b> Native Qt 6 GUI with zero runtime Python dependency, dedicated C++ TFIDFVectorizer, real-time sparse dot product engine, and persistent MySQL 8 enterprise database."
    ));

    mainLayout->addStretch();
    scrollArea->setWidget(container);
    outerLayout->addWidget(scrollArea);
}
