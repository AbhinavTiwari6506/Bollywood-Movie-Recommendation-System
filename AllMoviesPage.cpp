#include "AllMoviesPage.h"
#include "MovieCard.h"
#include "../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>

AllMoviesPage::AllMoviesPage(QWidget *parent)
    : QWidget(parent)
    , m_currentPage(0)
    , m_pageSize(24)
    , m_totalCount(0)
    , m_pageInfoLabel(nullptr)
    , m_prevBtn(nullptr)
    , m_nextBtn(nullptr)
    , m_gridContainer(nullptr)
    , m_gridLayout(nullptr)
    , m_scrollArea(nullptr)
{
    setupUI();
    reloadMovies();
}

void AllMoviesPage::setupUI() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(30, 24, 30, 24);
    mainLayout->setSpacing(16);

    // Header and Pagination row
    QHBoxLayout *topRow = new QHBoxLayout();

    QLabel *heading = new QLabel("All Bollywood Movies Catalog", this);
    QFont hFont;
    hFont.setPointSize(18);
    hFont.setBold(true);
    heading->setFont(hFont);
    heading->setStyleSheet("color: #FFFFFF; font-weight: bold;");
    topRow->addWidget(heading, 1);

    m_pageInfoLabel = new QLabel(this);
    m_pageInfoLabel->setStyleSheet("color: #A0A5B5; font-size: 13px; font-weight: 500;");
    topRow->addWidget(m_pageInfoLabel);

    m_prevBtn = new QPushButton("◀ Previous", this);
    m_prevBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #262933;"
        "  color: #FFFFFF;"
        "  border: 1px solid #383D4C;"
        "  border-radius: 6px;"
        "  padding: 6px 14px;"
        "}"
        "QPushButton:hover { background-color: #323644; }"
        "QPushButton:disabled { color: #555A68; border-color: #262933; }");
    connect(m_prevBtn, &QPushButton::clicked, this, &AllMoviesPage::onPrevPage);
    topRow->addWidget(m_prevBtn);

    m_nextBtn = new QPushButton("Next ▶", this);
    m_nextBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #262933;"
        "  color: #FFFFFF;"
        "  border: 1px solid #383D4C;"
        "  border-radius: 6px;"
        "  padding: 6px 14px;"
        "}"
        "QPushButton:hover { background-color: #323644; }"
        "QPushButton:disabled { color: #555A68; border-color: #262933; }");
    connect(m_nextBtn, &QPushButton::clicked, this, &AllMoviesPage::onNextPage);
    topRow->addWidget(m_nextBtn);

    mainLayout->addLayout(topRow);

    // Scrollable Grid
    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setStyleSheet("background: transparent; border: none;");

    m_gridContainer = new QWidget(m_scrollArea);
    m_gridLayout = new QGridLayout(m_gridContainer);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setHorizontalSpacing(16);
    m_gridLayout->setVerticalSpacing(18);

    m_scrollArea->setWidget(m_gridContainer);
    mainLayout->addWidget(m_scrollArea, 1);
}

void AllMoviesPage::reloadMovies() {
    m_totalCount = DatabaseManager::instance().getMovieCount();
    updateGrid();
}

void AllMoviesPage::updateGrid() {
    // Clear existing
    QLayoutItem *item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    int totalPages = (m_totalCount + m_pageSize - 1) / m_pageSize;
    if (totalPages == 0) totalPages = 1;

    m_prevBtn->setEnabled(m_currentPage > 0);
    m_nextBtn->setEnabled(m_currentPage < totalPages - 1);

    m_pageInfoLabel->setText(QString("Page %1 of %2  (%3 total movies)")
                             .arg(m_currentPage + 1)
                             .arg(totalPages)
                             .arg(m_totalCount));

    int offset = m_currentPage * m_pageSize;
    QList<Movie> movies = DatabaseManager::instance().getAllMovies(m_pageSize, offset);

    const int cols = 4;
    for (int i = 0; i < movies.size(); ++i) {
        MovieCard *card = new MovieCard(movies[i], -1.0, m_gridContainer);
        connect(card, &MovieCard::clicked, this, &AllMoviesPage::movieDetailsRequested);
        connect(card, &MovieCard::recommendRequested, this, [this](const Movie &m) {
            emit recommendRequested(m.getTitle());
        });

        int row = i / cols;
        int col = i % cols;
        m_gridLayout->addWidget(card, row, col);
    }
}

void AllMoviesPage::onPrevPage() {
    if (m_currentPage > 0) {
        m_currentPage--;
        updateGrid();
        m_scrollArea->verticalScrollBar()->setValue(0);
    }
}

void AllMoviesPage::onNextPage() {
    int totalPages = (m_totalCount + m_pageSize - 1) / m_pageSize;
    if (m_currentPage < totalPages - 1) {
        m_currentPage++;
        updateGrid();
        m_scrollArea->verticalScrollBar()->setValue(0);
    }
}
