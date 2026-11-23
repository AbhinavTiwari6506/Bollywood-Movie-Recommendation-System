#include "RecommendationPage.h"
#include "MovieCard.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QCompleter>
#include <QFrame>
#include <QMessageBox>

RecommendationPage::RecommendationPage(RecommendationEngine *engine, QWidget *parent)
    : QWidget(parent)
    , m_engine(engine)
    , m_movieCombo(nullptr)
    , m_nRecsSlider(nullptr)
    , m_sliderValueLabel(nullptr)
    , m_recommendBtn(nullptr)
    , m_previewCard(nullptr)
    , m_previewTitle(nullptr)
    , m_previewMeta(nullptr)
    , m_previewOverview(nullptr)
    , m_resultsHeading(nullptr)
    , m_gridContainer(nullptr)
    , m_gridLayout(nullptr)
    , m_statusMessage(nullptr)
{
    setupUI();
    refreshMovieTitles();
}

void RecommendationPage::setupUI() {
    QVBoxLayout *outerLayout = new QVBoxLayout(this);
    outerLayout->setContentsMargins(0, 0, 0, 0);

    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent; border: none;");

    QWidget *container = new QWidget(scrollArea);
    QVBoxLayout *mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(30, 24, 30, 24);
    mainLayout->setSpacing(22);

    // Page Title
    QLabel *heading = new QLabel("🎬 Bollywood Movie Recommender", container);
    QFont hFont;
    hFont.setPointSize(20);
    hFont.setBold(true);
    heading->setFont(hFont);
    heading->setStyleSheet("color: #FFFFFF; font-weight: bold;");
    mainLayout->addWidget(heading);

    QLabel *subheading = new QLabel(
        "Select any Bollywood movie to compute similar recommendations using TF-IDF feature extraction "
        "and Cosine Similarity based on Genre, Overview, Director, and Cast.", container);
    subheading->setStyleSheet("color: #94A3B8; font-size: 13px;");
    mainLayout->addWidget(subheading);

    // ==========================================
    // CONTROL PANEL (Movie Selection + Top-N Slider + Recommend Button)
    // ==========================================
    QFrame *controlCard = new QFrame(container);
    controlCard->setObjectName("ControlCard");
    controlCard->setStyleSheet(
        "#ControlCard {"
        "  background-color: #181A20;"
        "  border: 1px solid #2B2F3A;"
        "  border-radius: 12px;"
        "  padding: 20px;"
        "}");

    QVBoxLayout *controlLayout = new QVBoxLayout(controlCard);
    controlLayout->setContentsMargins(20, 20, 20, 20);
    controlLayout->setSpacing(16);

    // Row 1: Movie Selection Dropdown with Search Completer
    QLabel *selectLabel = new QLabel("CHOOSE A MOVIE TO RECOMMEND SIMILAR TITLES:", controlCard);
    selectLabel->setStyleSheet("color: #A0A5B5; font-size: 11px; font-weight: bold; letter-spacing: 0.5px;");
    controlLayout->addWidget(selectLabel);

    m_movieCombo = new QComboBox(controlCard);
    m_movieCombo->setEditable(true);
    m_movieCombo->setInsertPolicy(QComboBox::NoInsert);
    m_movieCombo->setFixedHeight(44);
    m_movieCombo->setStyleSheet(
        "QComboBox {"
        "  background-color: #20242D;"
        "  color: #FFFFFF;"
        "  border: 1px solid #363C4A;"
        "  border-radius: 8px;"
        "  padding: 0 16px;"
        "  font-size: 14px;"
        "}"
        "QComboBox:focus {"
        "  border: 1px solid #E50914;"
        "}"
        "QComboBox QAbstractItemView {"
        "  background-color: #20242D;"
        "  color: #FFFFFF;"
        "  selection-background-color: #E50914;"
        "  selection-color: #FFFFFF;"
        "  border: 1px solid #363C4A;"
        "  padding: 4px;"
        "}");
    m_movieCombo->completer()->setCaseSensitivity(Qt::CaseInsensitive);
    m_movieCombo->completer()->setFilterMode(Qt::MatchContains);
    connect(m_movieCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &RecommendationPage::onMovieSelected);
    controlLayout->addWidget(m_movieCombo);

    // Row 2: Top N Slider and Button
    QHBoxLayout *sliderRow = new QHBoxLayout();
    sliderRow->setSpacing(16);

    QLabel *sliderTitle = new QLabel("Number of recommendations:", controlCard);
    sliderTitle->setStyleSheet("color: #E2E8F0; font-size: 13px; font-weight: 500;");
    sliderRow->addWidget(sliderTitle);

    m_nRecsSlider = new QSlider(Qt::Horizontal, controlCard);
    m_nRecsSlider->setRange(5, 20);
    m_nRecsSlider->setValue(10);
    m_nRecsSlider->setFixedWidth(200);
    m_nRecsSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "  height: 6px;"
        "  background: #2D323E;"
        "  border-radius: 3px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "  background: #E50914;"
        "  border-radius: 3px;"
        "}"
        "QSlider::handle:horizontal {"
        "  background: #FFFFFF;"
        "  border: 2px solid #E50914;"
        "  width: 16px;"
        "  margin-top: -5px;"
        "  margin-bottom: -5px;"
        "  border-radius: 8px;"
        "}");
    connect(m_nRecsSlider, &QSlider::valueChanged, this, &RecommendationPage::onSliderValueChanged);
    sliderRow->addWidget(m_nRecsSlider);

    m_sliderValueLabel = new QLabel("10 movies", controlCard);
    m_sliderValueLabel->setStyleSheet("color: #E50914; font-weight: bold; font-size: 13px; min-width: 70px;");
    sliderRow->addWidget(m_sliderValueLabel);

    sliderRow->addStretch();

    m_recommendBtn = new QPushButton("✨ Generate Recommendations", controlCard);
    m_recommendBtn->setFixedHeight(44);
    m_recommendBtn->setCursor(Qt::PointingHandCursor);
    m_recommendBtn->setStyleSheet(
        "QPushButton {"
        "  background-color: #E50914;"
        "  color: #FFFFFF;"
        "  font-weight: bold;"
        "  font-size: 14px;"
        "  border: none;"
        "  border-radius: 8px;"
        "  padding: 0 24px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #F40612;"
        "}");
    connect(m_recommendBtn, &QPushButton::clicked, this, &RecommendationPage::onRecommendClicked);
    sliderRow->addWidget(m_recommendBtn);

    controlLayout->addLayout(sliderRow);
    mainLayout->addWidget(controlCard);

    // ==========================================
    // SELECTED MOVIE PREVIEW CARD
    // ==========================================
    m_previewCard = new QFrame(container);
    m_previewCard->setStyleSheet(
        "QFrame {"
        "  background-color: rgba(229, 9, 20, 0.08);"
        "  border: 1px solid rgba(229, 9, 20, 0.3);"
        "  border-radius: 10px;"
        "  padding: 16px;"
        "}");
    QVBoxLayout *prevLayout = new QVBoxLayout(m_previewCard);
    prevLayout->setSpacing(6);

    QLabel *selectedTag = new QLabel("TARGET MOVIE SELECTED FOR SIMILARITY MATCHING", m_previewCard);
    selectedTag->setStyleSheet("color: #FF6B74; font-size: 10px; font-weight: bold; letter-spacing: 1px;");
    prevLayout->addWidget(selectedTag);

    m_previewTitle = new QLabel(m_previewCard);
    QFont ptFont;
    ptFont.setPointSize(14);
    ptFont.setBold(true);
    m_previewTitle->setFont(ptFont);
    m_previewTitle->setStyleSheet("color: #FFFFFF; font-weight: bold;");
    prevLayout->addWidget(m_previewTitle);

    m_previewMeta = new QLabel(m_previewCard);
    m_previewMeta->setStyleSheet("color: #A0A5B5; font-size: 12px;");
    prevLayout->addWidget(m_previewMeta);

    m_previewOverview = new QLabel(m_previewCard);
    m_previewOverview->setStyleSheet("color: #CBD5E1; font-size: 12px; line-height: 1.4;");
    m_previewOverview->setWordWrap(true);
    prevLayout->addWidget(m_previewOverview);

    mainLayout->addWidget(m_previewCard);
    m_previewCard->hide();

    // ==========================================
    // RECOMMENDATIONS RESULTS GRID
    // ==========================================
    m_resultsHeading = new QLabel("Recommended Similar Movies", container);
    QFont resFont;
    resFont.setPointSize(15);
    resFont.setBold(true);
    m_resultsHeading->setFont(resFont);
    m_resultsHeading->setStyleSheet("color: #FFFFFF; font-weight: bold; margin-top: 10px;");
    mainLayout->addWidget(m_resultsHeading);

    m_statusMessage = new QLabel("Select a movie and click 'Generate Recommendations' to compute similar titles.", container);
    m_statusMessage->setStyleSheet("color: #7B8191; font-size: 13px;");
    mainLayout->addWidget(m_statusMessage);

    m_gridContainer = new QWidget(container);
    m_gridLayout = new QGridLayout(m_gridContainer);
    m_gridLayout->setContentsMargins(0, 0, 0, 0);
    m_gridLayout->setHorizontalSpacing(16);
    m_gridLayout->setVerticalSpacing(18);
    mainLayout->addWidget(m_gridContainer);

    mainLayout->addStretch();
    scrollArea->setWidget(container);
    outerLayout->addWidget(scrollArea);
}

void RecommendationPage::refreshMovieTitles() {
    if (!m_engine) return;

    QString current = m_movieCombo->currentText();
    m_movieCombo->blockSignals(true);
    m_movieCombo->clear();

    QStringList titles = m_engine->getAllTitles();
    titles.sort(Qt::CaseInsensitive);

    m_movieCombo->addItems(titles);

    if (!current.isEmpty()) {
        int idx = m_movieCombo->findText(current);
        if (idx >= 0) {
            m_movieCombo->setCurrentIndex(idx);
        }
    }
    m_movieCombo->blockSignals(false);

    if (m_movieCombo->count() > 0) {
        onMovieSelected(m_movieCombo->currentIndex());
    }
}

void RecommendationPage::setMovieToRecommend(const QString &movieTitle) {
    if (movieTitle.isEmpty()) return;

    int idx = m_movieCombo->findText(movieTitle, Qt::MatchFixedString | Qt::MatchCaseSensitive);
    if (idx < 0) {
        idx = m_movieCombo->findText(movieTitle, Qt::MatchContains);
    }

    if (idx >= 0) {
        m_movieCombo->setCurrentIndex(idx);
        onRecommendClicked();
    }
}

void RecommendationPage::onSliderValueChanged(int value) {
    m_sliderValueLabel->setText(QString("%1 movies").arg(value));
}

void RecommendationPage::onMovieSelected(int index) {
    if (!m_engine || index < 0) return;
    QString title = m_movieCombo->itemText(index);
    int movieIdx = m_engine->findMovieIndex(title);
    if (movieIdx >= 0) {
        Movie m = m_engine->getMovie(movieIdx);
        updateSelectedMoviePreview(m);
    }
}

void RecommendationPage::updateSelectedMoviePreview(const Movie &movie) {
    m_previewTitle->setText(QString("%1 (%2)").arg(movie.getTitle(), movie.getFormattedYear()));
    
    QString meta = QString("Genre: %1  |  Director: %2  |  Cast: %3")
                       .arg(movie.getGenre().isEmpty() ? "N/A" : movie.getGenre())
                       .arg(movie.getDirector().isEmpty() ? "N/A" : movie.getDirector())
                       .arg(movie.getCast().isEmpty() ? "N/A" : movie.getCast());
    m_previewMeta->setText(meta);
    m_previewOverview->setText(movie.getOverview());
    m_previewCard->show();
}

void RecommendationPage::clearRecommendationsGrid() {
    QLayoutItem *item;
    while ((item = m_gridLayout->takeAt(0)) != nullptr) {
        if (item->widget()) {
            item->widget()->deleteLater();
        }
        delete item;
    }
}

void RecommendationPage::onRecommendClicked() {
    QString title = m_movieCombo->currentText().trimmed();
    if (title.isEmpty()) {
        QMessageBox::information(this, "Select Movie", "Please select or type a Bollywood movie name first.");
        return;
    }

    if (!m_engine || !m_engine->isTrained()) {
        QMessageBox::warning(this, "Model Not Ready", "Recommendation model is not trained yet. Please make sure dataset is loaded.");
        return;
    }

    int topN = m_nRecsSlider->value();
    QList<MovieRecommendation> recs = m_engine->getRecommendations(title, topN);

    if (recs.isEmpty()) {
        m_statusMessage->setText(QString("No similar recommendations could be found for '%1'. Try another movie.").arg(title));
        m_statusMessage->setStyleSheet("color: #FF6B6B; font-size: 13px; font-weight: bold;");
        clearRecommendationsGrid();
    } else {
        displayRecommendations(recs, title);
    }
}

void RecommendationPage::displayRecommendations(const QList<MovieRecommendation> &recs, const QString &sourceTitle) {
    clearRecommendationsGrid();

    m_resultsHeading->setText(QString("✨ Top %1 Movies Similar to: \"%2\"").arg(recs.size()).arg(sourceTitle));
    m_statusMessage->setText(QString("Calculated in real-time via TF-IDF Vectorization and Cosine Similarity."));
    m_statusMessage->setStyleSheet("color: #46D369; font-size: 13px;");

    const int cols = 4;
    for (int i = 0; i < recs.size(); ++i) {
        const MovieRecommendation &rec = recs[i];
        MovieCard *card = new MovieCard(rec.movie, rec.similarityScore, m_gridContainer);
        connect(card, &MovieCard::clicked, this, &RecommendationPage::movieDetailsRequested);
        connect(card, &MovieCard::recommendRequested, this, [this](const Movie &m) {
            setMovieToRecommend(m.getTitle());
        });

        int row = i / cols;
        int col = i % cols;
        m_gridLayout->addWidget(card, row, col);
    }
}
