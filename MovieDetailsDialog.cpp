#include "MovieDetailsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QFrame>

MovieDetailsDialog::MovieDetailsDialog(const Movie &movie, QWidget *parent)
    : QDialog(parent)
    , m_movie(movie)
{
    setupUI();
}

void MovieDetailsDialog::setupUI() {
    setWindowTitle(QString("%1 (%2) - Details").arg(m_movie.getTitle(), m_movie.getFormattedYear()));
    setFixedSize(620, 520);
    setStyleSheet(
        "QDialog {"
        "  background-color: #14161D;"
        "  color: #FFFFFF;"
        "}"
        "QLabel {"
        "  color: #D1D5DB;"
        "}"
        "QPushButton#RecommendBtn {"
        "  background-color: #E50914;"
        "  color: #FFFFFF;"
        "  font-weight: bold;"
        "  font-size: 13px;"
        "  padding: 10px 20px;"
        "  border-radius: 6px;"
        "}"
        "QPushButton#RecommendBtn:hover {"
        "  background-color: #F40612;"
        "}"
        "QPushButton#CloseBtn {"
        "  background-color: #262933;"
        "  color: #A0A5B5;"
        "  font-size: 13px;"
        "  padding: 10px 20px;"
        "  border-radius: 6px;"
        "}"
        "QPushButton#CloseBtn:hover {"
        "  background-color: #323644;"
        "  color: #FFFFFF;"
        "}");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(16);

    // Title and Year Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    
    QLabel *titleLabel = new QLabel(m_movie.getTitle(), this);
    QFont titleFont;
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setStyleSheet("color: #FFFFFF; font-weight: bold;");
    headerLayout->addWidget(titleLabel, 1);

    QLabel *yearBadge = new QLabel(QString("  %1  ").arg(m_movie.getFormattedYear()), this);
    yearBadge->setStyleSheet("background-color: #262933; color: #E50914; font-weight: bold; border-radius: 4px; padding: 4px 8px; font-size: 13px;");
    headerLayout->addWidget(yearBadge);

    if (m_movie.getRating() > 0.0) {
        QLabel *ratingBadge = new QLabel(QString("  ★ %1  ").arg(m_movie.getDisplayRating()), this);
        ratingBadge->setStyleSheet("background-color: #F5C518; color: #000000; font-weight: bold; border-radius: 4px; padding: 4px 8px; font-size: 13px;");
        headerLayout->addWidget(ratingBadge);
    }

    mainLayout->addLayout(headerLayout);

    // Genre Badges Row
    QHBoxLayout *genreLayout = new QHBoxLayout();
    genreLayout->setSpacing(8);
    for (const QString &g : m_movie.getGenreList()) {
        QLabel *pill = new QLabel(QString(" %1 ").arg(g), this);
        pill->setStyleSheet("background-color: rgba(229, 9, 20, 0.15); color: #FF4A54; border: 1px solid rgba(229, 9, 20, 0.4); border-radius: 12px; padding: 3px 10px; font-size: 11px; font-weight: 600;");
        genreLayout->addWidget(pill);
    }
    genreLayout->addStretch();
    mainLayout->addLayout(genreLayout);

    // Separator line
    QFrame *line = new QFrame(this);
    line->setFrameShape(QFrame::HLine);
    line->setStyleSheet("background-color: #262933;");
    line->setFixedHeight(1);
    mainLayout->addWidget(line);

    // Movie Metadata fields
    QScrollArea *scrollArea = new QScrollArea(this);
    scrollArea->setWidgetResizable(true);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setStyleSheet("background: transparent;");

    QWidget *scrollContent = new QWidget(scrollArea);
    QVBoxLayout *contentLayout = new QVBoxLayout(scrollContent);
    contentLayout->setContentsMargins(0, 0, 0, 0);
    contentLayout->setSpacing(14);

    // Director
    if (!m_movie.getDirector().isEmpty()) {
        QLabel *dirTitle = new QLabel("DIRECTOR", scrollContent);
        dirTitle->setStyleSheet("color: #7B8191; font-size: 11px; font-weight: bold; letter-spacing: 1px;");
        contentLayout->addWidget(dirTitle);

        QLabel *dirVal = new QLabel(m_movie.getDirector(), scrollContent);
        dirVal->setStyleSheet("color: #FFFFFF; font-size: 13px;");
        contentLayout->addWidget(dirVal);
    }

    // Cast / Actors
    if (!m_movie.getCast().isEmpty()) {
        QLabel *castTitle = new QLabel("STARRING CAST", scrollContent);
        castTitle->setStyleSheet("color: #7B8191; font-size: 11px; font-weight: bold; letter-spacing: 1px;");
        contentLayout->addWidget(castTitle);

        QLabel *castVal = new QLabel(m_movie.getCast(), scrollContent);
        castVal->setStyleSheet("color: #E2E8F0; font-size: 13px; line-height: 1.4;");
        castVal->setWordWrap(true);
        contentLayout->addWidget(castVal);
    }

    // Overview / Synopsis
    QLabel *overviewTitle = new QLabel("OVERVIEW & SYNOPSIS", scrollContent);
    overviewTitle->setStyleSheet("color: #7B8191; font-size: 11px; font-weight: bold; letter-spacing: 1px;");
    contentLayout->addWidget(overviewTitle);

    QLabel *overviewVal = new QLabel(m_movie.getOverview().isEmpty() ? "No synopsis available for this title." : m_movie.getOverview(), scrollContent);
    overviewVal->setStyleSheet("color: #CBD5E1; font-size: 13px; line-height: 1.5;");
    overviewVal->setWordWrap(true);
    contentLayout->addWidget(overviewVal);

    // IMDb ID
    if (!m_movie.getMovieId().isEmpty()) {
        QLabel *idTitle = new QLabel("IMDB IDENTIFIER", scrollContent);
        idTitle->setStyleSheet("color: #7B8191; font-size: 11px; font-weight: bold;");
        contentLayout->addWidget(idTitle);

        QLabel *idVal = new QLabel(m_movie.getMovieId(), scrollContent);
        idVal->setStyleSheet("color: #94A3B8; font-size: 12px; font-family: monospace;");
        contentLayout->addWidget(idVal);
    }

    contentLayout->addStretch();
    scrollArea->setWidget(scrollContent);
    mainLayout->addWidget(scrollArea, 1);

    // Bottom Action Row
    QHBoxLayout *actionLayout = new QHBoxLayout();
    actionLayout->setSpacing(12);

    QPushButton *recBtn = new QPushButton("✨ Find Similar Movies", this);
    recBtn->setObjectName("RecommendBtn");
    recBtn->setCursor(Qt::PointingHandCursor);
    connect(recBtn, &QPushButton::clicked, this, [this]() {
        emit recommendRequested(m_movie.getTitle());
        accept();
    });
    actionLayout->addWidget(recBtn);

    actionLayout->addStretch();

    QPushButton *closeBtn = new QPushButton("Close", this);
    closeBtn->setObjectName("CloseBtn");
    closeBtn->setCursor(Qt::PointingHandCursor);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
    actionLayout->addWidget(closeBtn);

    mainLayout->addLayout(actionLayout);
}
