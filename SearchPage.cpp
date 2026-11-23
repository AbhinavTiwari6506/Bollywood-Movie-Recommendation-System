#include "SearchPage.h"
#include "MovieCard.h"
#include "../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>

SearchPage::SearchPage(QWidget *parent)
    : QWidget(parent)
    , m_searchInput(nullptr)
    , m_genreCombo(nullptr)
    , m_yearCombo(nullptr)
    , m_resultCountLabel(nullptr)
    , m_emptyStateLabel(nullptr)
    , m_gridContainer(nullptr)
    , m_gridLayout(nullptr)
{
    setupUI();
    reloadFilters();
}

void SearchPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 24, 30, 24);
    mainLayout->setSpacing(20);

    // Header
    QLabel *pageTitle = new QLabel("Search Bollywood Movies", this);
    QFont titleFont;
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    pageTitle->setFont(titleFont);
    pageTitle->setStyleSheet("color: #FFFFFF; font-weight: bold;");
    mainLayout->addWidget(pageTitle);

    // Filter Bar Row
    QHBoxLayout *filterBar = new QHBoxLayout();
    filterBar->setSpacing(12);

    m_searchInput = new QLineEdit(this);
    m_searchInput->setPlaceholderText("Search by movie name, director, or cast (e.g., Dilwale, Rajkumar Hirani, Shah Rukh Khan)...");
    m_searchInput->setFixedHeight(40);
    m_searchInput->setStyleSheet(
        "QLineEdit {"
        "  background-color: #1A1D26;"
        "  color: #FFFFFF;"
        "  border: 1px solid #323644;"
        "  border-radius: 8px;"
        "  padding: 0 14px;"
        "  font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #E50914;"
        "}");
    connect(m_searchInput, &QLineEdit::returnPressed, this, &SearchPage::onSearchTriggered);
    filterBar->addWidget(m_searchInput, 2);

    m_genreCombo = new QComboBox(this);
    m_genreCombo->setFixedHeight(40);
    m_genreCombo->setStyleSheet(
        "QComboBox {"
        "  background-color: #1A1D26;"
        "  color: #FFFFFF;"
        "  border: 1px solid #323644;"
        "  border-radius: 8px;"
        "  padding: 0 12px;"
        "  font-size: 13px;"
        "  min-width: 140px;"
        "}");
    connect(m_genreCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SearchPage::onSearchTriggered);
    filterBar->addWidget(m_genreCombo);

    m_yearCombo = new QComboBox(this);
    m_yearCombo->setFixedHeight(40);
    m_yearCombo->setStyleSheet(
        "QComboBox {"
        "  background-color: #1A1D26;"
        "  color: #FFFFFF;"
        "  border: 1px solid #323644;"
        "  border-radius: 8px;"
        "  padding: 0 12px;"
        "  font-size: 13px;"
        "  min-width: 110px;"
        "}");
    connect(m_yearCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SearchPage::onSearchTriggered);
    filterBar->addWidget(m_yearCombo);

    QPushButton *searchBtn = new QPushButton("🔍 Search", this);
    searchBtn->setFixedHeight(40);
    searchBtn->setCursor(Qt::PointingHandCursor);
    searchBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #E50914;"
        "  color: #FFFFFF;"
        "  font-weight: bold;"
        "  border-radius: 8px;"
        "  padding: 0 18px;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F40612;"
        "}");
    connect(searchBtn, &QPushButton::clicked, this, &SearchPage::onSearchTriggered);
    filterBar->addWidget(searchBtn);

    QPushButton *clearBtn = new QPushButton("Reset", this);
    clearBtn->setFixedHeight(40);
    clearBtn->setCursor(Qt::PointingHandCursor);
    clearBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #262933;"
        "  color: #A0A5B5;"
        "  border: 1px solid #383D4C;"
        "  border-radius: 8px;"
        "  padding: 0 14px;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #323644;"
        "  color: #FFFFFF;"
        "}");
    connect(clearBtn, &QPushButton::clicked, this, &SearchPage::onClearFilters);
    filterBar->addWidget(clearBtn);

    mainLayout->addLayout(filterBar);

    // Results info bar
    m_resultCountLabel = new QLabel(this);
    m_resultCountLabel->setStyleSheet("color: #94A3B8; font-size: 13px; font-weight: 500;");
    mainLayout->addWidget(m_resultCountLabel);

    // Scrollable Grid Area
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    m_gridContainer = new QWidget(scrollArea);
    m_gridLayout = new QGridLayout(m_gridContainer);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setHorizontalSpacing(16);
    m_gridLayout->setVerticalSpacing(18);

    m_emptyStateLabel = new QLabel("No movies found. Enter a keyword or choose a genre above.", m_gridContainer);
    m_emptyStateLabel->setAlignment(Qt::AlignCenter);
    m_emptyStateLabel->setStyleSheet("color: #64748B; font-size: 14px; padding: 40px;");
    m_gridLayout->addWidget(m_emptyStateLabel, 0, 0);

    scrollArea->setWidget(m_gridContainer);
    mainLayout->addWidget(scrollArea, 1);
}

void SearchPage::reloadFilters() {
    m_genreCombo->blockSignals(true);
    m_yearCombo->blockSignals(true);

    m_genreCombo->clear();
    m_genreCombo->addItem("All Genres");
    const QStringList genres = DatabaseManager::instance().getUniqueGenres();
    for (const QString &g : genres) {
        m_genreCombo->addItem(g);
    }

    m_yearCombo->clear();
    m_yearCombo->addItem("All Years");
    const QStringList years = DatabaseManager::instance().getUniqueYears();
    for (const QString &y : years) {
        m_yearCombo->addItem(y);
    }

    m_genreCombo->blockSignals(false);
    m_yearCombo->blockSignals(false);
}

void SearchPage::setSearchQuery(const QString &query) {
    m_searchInput->setText(query);
    executeSearch();
}

void SearchPage::onSearchTriggered() {
    executeSearch();
}

void SearchPage::onClearFilters() {
    m_searchInput->clear();
    m_genreCombo->setCurrentIndex(0);
    m_yearCombo->setCurrentIndex(0);
    executeSearch();
}

void SearchPage::clearGrid() {
    QLayoutItem *item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void SearchPage::executeSearch() {
    QString q = m_searchInput->text().trimmed();
    QString genre = m_genreCombo->currentText();
    QString year = m_yearCombo->currentText();

    QList<Movie> results = DatabaseManager::instance().searchMovies(q, genre, year, 120);
    displayResults(results);
}

void SearchPage::displayResults(const QList<Movie> &movies) {
    clearGrid();

    if (movies.isEmpty()) {
        m_resultCountLabel->setText("Found 0 movies");
        m_emptyStateLabel = new QLabel("No movies matched your search criteria.\nTry searching by a different title, actor, or genre.", m_gridContainer);
        m_emptyStateLabel->setAlignment(Qt::AlignCenter);
        m_emptyStateLabel->setStyleSheet("color: #7B8191; font-size: 14px; padding: 60px;");
        m_gridLayout->addWidget(m_emptyStateLabel, 0, 0);
        return;
    }

    m_resultCountLabel->setText(QString("Found %1 movie%2").arg(movies.size()).arg(movies.size() == 1 ? "" : "s"));

    const int cols = 4; // 4 cards per row
    for (int i = 0; i < movies.size(); ++i) {
        MovieCard *card = new MovieCard(movies[i], -1.0, m_gridContainer);
        connect(card, &MovieCard::clicked, this, &SearchPage::movieDetailsRequested);
        connect(card, &MovieCard::recommendRequested, this, [this](const Movie &m) {
            emit recommendRequested(m.getTitle());
        });

        int row = i / cols;
        int col = i % cols;
        m_gridLayout->addWidget(card, row, col);
    }
}
