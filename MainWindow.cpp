#include "MainWindow.h"
#include "MovieDetailsDialog.h"
#include "DbConfigDialog.h"
#include "../database/DatabaseManager.h"
#include "../utils/CSVReader.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QMessageBox>
#include <QApplication>
#include <QTimer>
#include <QFile>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_engine(std::make_unique<RecommendationEngine>())
    , m_sidebar(nullptr)
    , m_navButtonGroup(nullptr)
    , m_pagesStack(nullptr)
    , m_dbStatusBadge(nullptr)
    , m_dbConfigBtn(nullptr)
    , m_homePage(nullptr)
    , m_searchPage(nullptr)
    , m_recPage(nullptr)
    , m_allMoviesPage(nullptr)
    , m_aboutPage(nullptr)
{
    setupUI();

    // Listen to database connection status changes
    connect(&DatabaseManager::instance(), &DatabaseManager::connectionStatusChanged,
            this, &MainWindow::onDatabaseStatusChanged);

    // Bootstrap data after event loop starts
    QTimer::singleShot(100, this, &MainWindow::bootstrapApplicationData);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    setWindowTitle("Bollywood Movie Recommendation System (Qt 6 + MySQL 8)");
    resize(1280, 820);
    setMinimumSize(1024, 680);

    // Global Window Stylesheet
    setStyleSheet(
        "QMainWindow {"
        "  background-color: #0F1117;"
        "}"
        "QWidget#CentralWidget {"
        "  background-color: #0F1117;"
        "}"
        "QScrollBar:vertical {"
        "  border: none;"
        "  background: #14161D;"
        "  width: 10px;"
        "  margin: 0px;"
        "}"
        "QScrollBar::handle:vertical {"
        "  background: #2D323E;"
        "  min-height: 25px;"
        "  border-radius: 5px;"
        "}"
        "QScrollBar::handle:vertical:hover {"
        "  background: #E50914;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "  height: 0px;"
        "}");

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setObjectName("CentralWidget");
    setCentralWidget(centralWidget);

    QVBoxLayout *rootLayout = new QVBoxLayout(centralWidget);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // 1. Top Header Bar
    createTopBar();
    rootLayout->addWidget(m_dbStatusBadge->parentWidget());

    // 2. Main Body (Sidebar + QStackedWidget Pages)
    QHBoxLayout *bodyLayout = new QHBoxLayout();
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);

    createSidebar();
    bodyLayout->addWidget(m_sidebar);

    createPages();
    bodyLayout->addWidget(m_pagesStack, 1);

    rootLayout->addLayout(bodyLayout, 1);
}

void MainWindow::createTopBar() {
    QFrame *topBar = new QFrame(this);
    topBar->setFixedHeight(60);
    topBar->setStyleSheet(
        "QFrame {"
        "  background-color: #14161D;"
        "  border-bottom: 1px solid #232733;"
        "}");

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(24, 0, 24, 0);
    topLayout->setSpacing(16);

    // App Logo & Brand
    QLabel *logoIcon = new QLabel("🎬", topBar);
    logoIcon->setStyleSheet("font-size: 22px;");
    topLayout->addWidget(logoIcon);

    QLabel *logoText = new QLabel("BOLLYWOOD RECOMMENDER", topBar);
    QFont logoFont;
    logoFont.setPointSize(13);
    logoFont.setBold(true);
    logoFont.setLetterSpacing(QFont::AbsoluteSpacing, 1.5);
    logoText->setFont(logoFont);
    logoText->setStyleSheet("color: #FFFFFF; font-weight: 800;");
    topLayout->addWidget(logoText);

    QLabel *versionTag = new QLabel("Qt 6 • C++17 • MySQL 8", topBar);
    versionTag->setStyleSheet("color: #64748B; font-size: 11px; margin-left: 6px;");
    topLayout->addWidget(versionTag);

    topLayout->addStretch();

    // DB Connection Status Pill
    m_dbStatusBadge = new QLabel("Connecting...", topBar);
    m_dbStatusBadge->setStyleSheet(
        "background-color: #262933; color: #F5C518; font-weight: bold; "
        "font-size: 11px; padding: 5px 12px; border-radius: 12px;");
    topLayout->addWidget(m_dbStatusBadge);

    // DB Settings Button
    m_dbConfigBtn = new QPushButton("⚙️ Database Settings", topBar);
    m_dbConfigBtn->setFixedHeight(34);
    m_dbConfigBtn->setCursor(Qt::PointingHandCursor);
    m_dbConfigBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #1F222A;"
        "  color: #E2E8F0;"
        "  border: 1px solid #323644;"
        "  border-radius: 6px;"
        "  padding: 0 14px;"
        "  font-size: 12px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover {"
        "  background-color: #2D323E;"
        "  color: #FFFFFF;"
        "  border-color: #E50914;"
        "}");
    connect(m_dbConfigBtn, &QPushButton::clicked, this, &MainWindow::openDbConfigDialog);
    topLayout->addWidget(m_dbConfigBtn);
}

void MainWindow::createSidebar() {
    m_sidebar = new QWidget(this);
    m_sidebar->setFixedWidth(220);
    m_sidebar->setStyleSheet(
        "QWidget {"
        "  background-color: #14161D;"
        "  border-right: 1px solid #232733;"
        "}"
        "QPushButton {"
        "  text-align: left;"
        "  padding: 12px 20px;"
        "  font-size: 13px;"
        "  font-weight: 600;"
        "  color: #94A3B8;"
        "  background-color: transparent;"
        "  border: none;"
        "  border-left: 4px solid transparent;"
        "}"
        "QPushButton:hover {"
        "  color: #FFFFFF;"
        "  background-color: #1A1D26;"
        "}"
        "QPushButton:checked {"
        "  color: #FFFFFF;"
        "  background-color: #1F222C;"
        "  border-left: 4px solid #E50914;"
        "  font-weight: bold;"
        "}");

    QVBoxLayout *sideLayout = new QVBoxLayout(m_sidebar);
    sideLayout->setContentsMargins(0, 16, 0, 16);
    sideLayout->setSpacing(4);

    m_navButtonGroup = new QButtonGroup(this);
    m_navButtonGroup->setExclusive(true);

    auto addNavItem = [this, sideLayout](int id, const QString &text, bool checked = false) {
        QPushButton *btn = new QPushButton(text, m_sidebar);
        btn->setCheckable(true);
        btn->setChecked(checked);
        btn->setCursor(Qt::PointingHandCursor);
        m_navButtonGroup->addButton(btn, id);
        sideLayout->addWidget(btn);
    };

    addNavItem(0, "🏠  Home", true);
    addNavItem(1, "🔍  Search Movies");
    addNavItem(2, "✨  Recommend Movies");
    addNavItem(3, "📑  All Movies");
    addNavItem(4, "ℹ️  About Project");

    connect(m_navButtonGroup, &QButtonGroup::idClicked, this, &MainWindow::onNavButtonClicked);

    sideLayout->addStretch();

    // Footer info inside sidebar
    QLabel *footer = new QLabel("Antigravity\nC++ Desktop App", m_sidebar);
    footer->setAlignment(Qt::AlignCenter);
    footer->setStyleSheet("color: #475569; font-size: 11px; padding: 10px;");
    sideLayout->addWidget(footer);
}

void MainWindow::createPages() {
    m_pagesStack = new QStackedWidget(this);

    m_homePage = new HomePage(m_pagesStack);
    connect(m_homePage, &HomePage::searchRequested, this, &MainWindow::navigateToSearch);
    connect(m_homePage, &HomePage::recommendRequested, this, &MainWindow::navigateToRecommend);
    connect(m_homePage, &HomePage::movieDetailsRequested, this, &MainWindow::showMovieDetails);
    m_pagesStack->addWidget(m_homePage);

    m_searchPage = new SearchPage(m_pagesStack);
    connect(m_searchPage, &SearchPage::movieDetailsRequested, this, &MainWindow::showMovieDetails);
    connect(m_searchPage, &SearchPage::recommendRequested, this, &MainWindow::navigateToRecommend);
    m_pagesStack->addWidget(m_searchPage);

    m_recPage = new RecommendationPage(m_engine.get(), m_pagesStack);
    connect(m_recPage, &RecommendationPage::movieDetailsRequested, this, &MainWindow::showMovieDetails);
    m_pagesStack->addWidget(m_recPage);

    m_allMoviesPage = new AllMoviesPage(m_pagesStack);
    connect(m_allMoviesPage, &AllMoviesPage::movieDetailsRequested, this, &MainWindow::showMovieDetails);
    connect(m_allMoviesPage, &AllMoviesPage::recommendRequested, this, &MainWindow::navigateToRecommend);
    m_pagesStack->addWidget(m_allMoviesPage);

    m_aboutPage = new AboutPage(m_pagesStack);
    m_pagesStack->addWidget(m_aboutPage);
}

void MainWindow::onNavButtonClicked(int id) {
    if (id >= 0 && id < m_pagesStack->count()) {
        m_pagesStack->setCurrentIndex(id);
    }
}

void MainWindow::navigateToSearch(const QString &query) {
    m_navButtonGroup->button(1)->setChecked(true);
    m_pagesStack->setCurrentIndex(1);
    m_searchPage->setSearchQuery(query);
}

void MainWindow::navigateToRecommend(const QString &movieTitle) {
    m_navButtonGroup->button(2)->setChecked(true);
    m_pagesStack->setCurrentIndex(2);
    if (!movieTitle.isEmpty()) {
        m_recPage->setMovieToRecommend(movieTitle);
    }
}

void MainWindow::showMovieDetails(const Movie &movie) {
    MovieDetailsDialog dlg(movie, this);
    connect(&dlg, &MovieDetailsDialog::recommendRequested, this, &MainWindow::navigateToRecommend);
    dlg.exec();
}

void MainWindow::openDbConfigDialog() {
    DbConfigDialog dlg(this);
    connect(&dlg, &DbConfigDialog::databaseConfigured, this, [this]() {
        bootstrapApplicationData();
    });
    dlg.exec();
}

void MainWindow::onDatabaseStatusChanged(bool connected, const QString &message) {
    Q_UNUSED(message);
    updateDbStatusBadge();
}

void MainWindow::updateDbStatusBadge() {
    if (!m_dbStatusBadge) return;

    if (DatabaseManager::instance().isConnected()) {
        QString drv = DatabaseManager::instance().activeDriverName();
        m_dbStatusBadge->setText(QString("🟢 %1 Connected").arg(drv));
        m_dbStatusBadge->setStyleSheet(
            "background-color: rgba(70, 211, 105, 0.15); color: #46D369; font-weight: bold; "
            "font-size: 11px; padding: 5px 12px; border-radius: 12px; border: 1px solid rgba(70, 211, 105, 0.4);");
    } else {
        m_dbStatusBadge->setText("🔴 DB Disconnected");
        m_dbStatusBadge->setStyleSheet(
            "background-color: rgba(229, 9, 20, 0.15); color: #FF4D58; font-weight: bold; "
            "font-size: 11px; padding: 5px 12px; border-radius: 12px; border: 1px solid rgba(229, 9, 20, 0.4);");
    }
}

void MainWindow::bootstrapApplicationData() {
    QString errMsg;
    
    // 1. Attempt connection with configured settings
    bool connected = DatabaseManager::instance().connectWithCurrentConfig(&errMsg);
    
    // If MySQL connection failed, attempt SQLite fallback so app runs out-of-the-box
    if (!connected) {
        DbConfig fallback = DatabaseManager::instance().currentConfig();
        fallback.driver = "QSQLITE";
        fallback.databaseName = "bollywood_recommender.db";
        connected = DatabaseManager::instance().connectToDatabase(fallback, &errMsg);
    }

    updateDbStatusBadge();

    // 2. Check if database has movies; if not, auto-import from CSV dataset
    int count = DatabaseManager::instance().getMovieCount();
    if (count == 0) {
        QString csvPath = DatabaseManager::instance().currentConfig().datasetPath;
        if (!QFile::exists(csvPath)) {
            // Check alternative relative paths
            if (QFile::exists("IMDB-Movie-Dataset(2023-1951).csv")) {
                csvPath = "IMDB-Movie-Dataset(2023-1951).csv";
            } else if (QFile::exists("data/IMDB-Movie-Dataset(2023-1951).csv")) {
                csvPath = "data/IMDB-Movie-Dataset(2023-1951).csv";
            } else if (QFile::exists("../data/IMDB-Movie-Dataset(2023-1951).csv")) {
                csvPath = "../data/IMDB-Movie-Dataset(2023-1951).csv";
            }
        }

        if (QFile::exists(csvPath)) {
            DatabaseManager::instance().importDatasetCSV(csvPath, nullptr, nullptr);
            count = DatabaseManager::instance().getMovieCount();
        }
    }

    // 3. Train the Recommendation Engine with all movies
    QList<Movie> allMovies = DatabaseManager::instance().getAllMovies(3000, 0);
    if (!allMovies.isEmpty()) {
        m_engine->train(allMovies);
    }

    // 4. Refresh all UI pages
    if (m_homePage) m_homePage->refreshData();
    if (m_searchPage) {
        m_searchPage->reloadFilters();
        m_searchPage->executeSearch();
    }
    if (m_recPage) m_recPage->refreshMovieTitles();
    if (m_allMoviesPage) m_allMoviesPage->reloadMovies();
}
