#include "HomePage.h"
#include "MovieCard.h"
#include "../database/DatabaseManager.h"
#include <QVBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

HomePage::HomePage(QWidget *parent)
    : QWidget(parent)
    , m_quickSearchInput(nullptr)
    , m_totalMoviesLabel(nullptr)
    , m_engineStatusLabel(nullptr)
    , m_dbStatusLabel(nullptr)
    , m_featuredLayout(nullptr)
    , m_recentLayout(nullptr)
{
    setupUI();
}

void HomePage::setupUI() {
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    QWidget *container = new QWidget(scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(26);

    // ==========================================
    // 1. HERO BANNER SECTION (Netflix / IMDb inspired)
    // ==========================================
    QFrame *heroCard = new QFrame(container);
    heroCard->setObjectName("HeroCard");
    heroCard->setStyleSheet(
        "#HeroCard {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1, stop:0 #250B11, stop:0.5 #161922, stop:1 #0F1117);"
        "  border: 1px solid #36171E;"
        "  border-radius: 16px;"
        "}");

    QVBoxLayout *heroLayout = new QVBoxLayout(heroCard);
    heroLayout->setContentsMargins(32, 32, 32, 32);
    heroLayout->setSpacing(14);

    QLabel *badge = new QLabel("✨ AI CONTENT-BASED RECOMMENDATIONS", heroCard);
    badge->setStyleSheet(
        "background-color: rgba(229, 9, 20, 0.2); color: #FF4D58; "
        "font-weight: bold; font-size: 11px; padding: 4px 10px; border-radius: 6px;");
    badge->setFixedWidth(275);
    heroLayout->addWidget(badge);

    QLabel *heroTitle = new QLabel("Bollywood Movie Recommendation System", heroCard);
    QFont heroFont;
    heroFont.setPointSize(22);
    heroFont.setBold(true);
    heroTitle->setFont(heroFont);
    heroTitle->setStyleSheet("color: #FFFFFF; font-weight: 800;");
    heroLayout->addWidget(heroTitle);

    QLabel *heroSubtitle = new QLabel(
        "Discover Bollywood cinema with TF-IDF Vectorization and Cosine Similarity. "
        "Powered by Qt 6 Widgets and MySQL 8 for lightning-fast performance.", heroCard);
    heroSubtitle->setStyleSheet("color: #94A3B8; font-size: 13px; max-width: 700px;");
    heroSubtitle->setWordWrap(true);
    heroLayout->addWidget(heroSubtitle);

    // Search and Recommend Bar inside Hero
    QHBoxLayout *searchBarLayout = new QHBoxLayout();
    searchBarLayout->setSpacing(10);

    m_quickSearchInput = new QLineEdit(heroCard);
    m_quickSearchInput->setPlaceholderText("Search any Bollywood movie, director or actor (e.g. 3 Idiots, Sholay, Shah Rukh Khan)...");
    m_quickSearchInput->setFixedHeight(42);
    m_quickSearchInput->setStyleSheet(
        "QLineEdit {"
        "  background-color: #1A1D26;"
        "  color: #FFFFFF;"
        "  border: 1px solid #323644;"
        "  border-radius: 8px;"
        "  padding: 0 16px;"
        "  font-size: 13px;"
        "}"
        "QLineEdit:focus {"
        "  border: 1px solid #E50914;"
        "}");
    connect(m_quickSearchInput, &QLineEdit::returnPressed, this, &HomePage::onSearchClicked);
    searchBarLayout->addWidget(m_quickSearchInput, 1);

    QPushButton *searchBtn = new QPushButton("🔍 Search", heroCard);
    searchBtn->setFixedHeight(42);
    searchBtn->setCursor(Qt::PointingHandCursor);
    searchBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #262933;"
        "  color: #FFFFFF;"
        "  border: 1px solid #383D4C;"
        "  font-weight: bold;"
        "  border-radius: 8px;"
        "  padding: 0 20px;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #323644;"
        "}");
    connect(searchBtn, &QPushButton::clicked, this, &HomePage::onSearchClicked);
    searchBarLayout->addWidget(searchBtn);

    QPushButton *recBtn = new QPushButton("✨ Get Recommendations", heroCard);
    recBtn->setFixedHeight(42);
    recBtn->setCursor(Qt::PointingHandCursor);
    recBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #E50914;"
        "  color: #FFFFFF;"
        "  font-weight: bold;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 0 22px;"
        "  font-size: 13px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F40612;"
        "}");
    connect(recBtn, &QPushButton::clicked, this, &HomePage::onRecommendClicked);
    searchBarLayout->addWidget(recBtn);

    heroLayout->addLayout(searchBarLayout);
    mainLayout->addWidget(heroCard);

    // ==========================================
    // 2. STATS OVERVIEW STRIP
    // ==========================================
    QHBoxLayout *statsLayout = new QHBoxLayout();
    statsLayout->setSpacing(16);

    auto createStatCard = [](const QString &title, const QString &val, QLabel **valLabelPtr) -> QFrame* {
        QFrame *card = new QFrame();
        card->setStyleSheet(
            "QFrame {"
            "  background-color: #181A20;"
            "  border: 1px solid #2B2F3A;"
            "  border-radius: 10px;"
            "  padding: 14px;"
            "}");
        QVBoxLayout *l = new QVBoxLayout(card);
        l->setContentsMargins(12, 12, 12, 12);
        l->setSpacing(4);

        QLabel *t = new QLabel(title, card);
        t->setStyleSheet("color: #7B8191; font-size: 11px; font-weight: bold; letter-spacing: 0.5px;");
        l->addWidget(t);

        QLabel *v = new QLabel(val, card);
        QFont vf;
        vf.setPointSize(15);
        vf.setBold(true);
        v->setFont(vf);
        v->setStyleSheet("color: #FFFFFF; font-weight: bold;");
        l->addWidget(v);

        if (valLabelPtr) {
            *valLabelPtr = v;
        }
        return card;
    };

    statsLayout->addWidget(createStatCard("DATABASE MOVIES", "2,199 Titles", &m_totalMoviesLabel));
    statsLayout->addWidget(createStatCard("ALGORITHM ENGINE", "TF-IDF + Cosine Sim", &m_engineStatusLabel));
    statsLayout->addWidget(createStatCard("BACKEND STORAGE", "MySQL 8 / InnoDB", &m_dbStatusLabel));

    mainLayout->addLayout(statsLayout);

    // ==========================================
    // 3. FEATURED BOLLYWOOD MOVIES SECTION
    // ==========================================
    QLabel *featuredHeading = new QLabel("⭐ Featured Bollywood Blockbusters", container);
    QFont sectionFont;
    sectionFont.setPointSize(14);
    sectionFont.setBold(true);
    featuredHeading->setFont(sectionFont);
    featuredHeading->setStyleSheet("color: #FFFFFF; font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(featuredHeading);

    QScrollArea *featuredScroll = new QScrollArea(container);
    featuredScroll->setWidgetResizable(true);
    featuredScroll->setFixedHeight(330);
    featuredScroll->setFrameShape(QFrame::NoFrame);
    featuredScroll->setStyleSheet("background: transparent; border: none;");

    QWidget *featuredContainer = new QWidget(featuredScroll);
    m_featuredLayout = new QHBoxLayout(featuredContainer);
    m_featuredLayout->setContentsMargins(0, 0, 0, 0);
    m_featuredLayout->setSpacing(14);
    m_featuredLayout->addStretch();
    featuredScroll->setWidget(featuredContainer);
    mainLayout->addWidget(featuredScroll);

    // ==========================================
    // 4. RECENT RELEASES SECTION
    // ==========================================
    QLabel *recentHeading = new QLabel("🔥 Recent Releases", container);
    recentHeading->setFont(sectionFont);
    recentHeading->setStyleSheet("color: #FFFFFF; font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(recentHeading);

    QScrollArea *recentScroll = new QScrollArea(container);
    recentScroll->setWidgetResizable(true);
    recentScroll->setFixedHeight(330);
    recentScroll->setFrameShape(QFrame::NoFrame);
    recentScroll->setStyleSheet("background: transparent; border: none;");

    QWidget *recentContainer = new QWidget(recentScroll);
    m_recentLayout = new QHBoxLayout(recentContainer);
    m_recentLayout->setContentsMargins(0, 0, 0, 0);
    m_recentLayout->setSpacing(14);
    m_recentLayout->addStretch();
    recentScroll->setWidget(recentContainer);
    mainLayout->addWidget(recentScroll);

    mainLayout->addStretch();
    scrollArea->setWidget(container);
    outerLayout->addWidget(scrollArea);

    refreshData();
}

void HomePage::refreshData() {
    updateStats();
    populateFeatured();
    populateRecent();
}

void HomePage::updateStats() {
    int count = DatabaseManager::instance().getMovieCount();
    if (m_totalMoviesLabel) {
        m_totalMoviesLabel->setText(count > 0 ? QString("%1 Titles").arg(count) : "0 Titles (Load Dataset)");
    }
    if (m_dbStatusLabel) {
        QString driver = DatabaseManager::instance().activeDriverName();
        bool conn = DatabaseManager::instance().isConnected();
        m_dbStatusLabel->setText(conn ? QString("%1 (Active)").arg(driver) : "Disconnected");
        m_dbStatusLabel->setStyleSheet(conn ? "color: #46D369;" : "color: #FF6B6B;");
    }
}

void HomePage::populateFeatured() {
    // Clear existing cards
    while (m_featuredLayout->count() > 1) {
        QLayoutItem *item = m_featuredLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    QList<Movie> movies = DatabaseManager::instance().getFeaturedMovies(10);
    for (const auto &m : movies) {
        MovieCard *card = new MovieCard(m, -1.0, this);
        connect(card, &MovieCard::clicked, this, &HomePage::movieDetailsRequested);
        m_featuredLayout->insertWidget(m_featuredLayout->count() - 1, card);
    }
}

void HomePage::populateRecent() {
    while (m_recentLayout->count() > 1) {
        QLayoutItem *item = m_recentLayout->takeAt(0);
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    QList<Movie> movies = DatabaseManager::instance().getRecentMovies(10);
    for (const auto &m : movies) {
        MovieCard *card = new MovieCard(m, -1.0, this);
        connect(card, &MovieCard::clicked, this, &HomePage::movieDetailsRequested);
        m_recentLayout->insertWidget(m_recentLayout->count() - 1, card);
    }
}

void HomePage::onSearchClicked() {
    QString q = m_quickSearchInput->text().trimmed();
    if (!q.isEmpty()) {
        emit searchRequested(q);
    }
}

void HomePage::onRecommendClicked() {
    QString q = m_quickSearchInput->text().trimmed();
    emit recommendRequested(q);
}
