#include "MovieCard.h"
#include <QPainter>
#include <QPainterPath>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QCursor>

MovieCard::MovieCard(const Movie &movie, double similarityScore, QWidget *parent)
    : QFrame(parent)
    , m_movie(movie)
    , m_similarityScore(similarityScore)
    , m_posterLabel(nullptr)
    , m_titleLabel(nullptr)
    , m_yearLabel(nullptr)
    , m_genreLabel(nullptr)
    , m_ratingLabel(nullptr)
    , m_matchBadge(nullptr)
{
    setupUI();
}

void MovieCard::setupUI() {
    setFixedSize(200, 310);
    setCursor(Qt::PointingHandCursor);
    setObjectName("MovieCard");

    // Container layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(6);

    // Poster Container (with match badge if similarity score is available)
    QFrame *posterContainer = new QFrame(this);
    posterContainer->setFixedSize(180, 190);
    posterContainer->setStyleSheet("background-color: #262933; border-radius: 8px;");

    QVBoxLayout *posterLayout = new QVBoxLayout(posterContainer);
    posterLayout->setContentsMargins(6, 6, 6, 6);

    // Top overlay row for badges
    QHBoxLayout *overlayLayout = new QHBoxLayout();
    overlayLayout->setContentsMargins(0, 0, 0, 0);

    if (m_similarityScore >= 0.0) {
        int matchPct = static_cast<int>(m_similarityScore * 100.0 + 0.5);
        m_matchBadge = new QLabel(QString("%1% Match").arg(matchPct), posterContainer);
        m_matchBadge->setStyleSheet(
            "background-color: #46D369; color: #0D0F14; font-weight: bold; "
            "font-size: 11px; padding: 2px 6px; border-radius: 4px;");
        overlayLayout->addWidget(m_matchBadge, 0, Qt::AlignLeft);
    } else {
        overlayLayout->addStretch();
    }

    // Rating badge (IMDb style)
    if (m_movie.getRating() > 0.0) {
        m_ratingLabel = new QLabel(QString("★ %1").arg(m_movie.getDisplayRating()), posterContainer);
        m_ratingLabel->setStyleSheet(
            "background-color: rgba(245, 197, 24, 0.9); color: #000000; font-weight: bold; "
            "font-size: 11px; padding: 2px 6px; border-radius: 4px;");
        overlayLayout->addWidget(m_ratingLabel, 0, Qt::AlignRight);
    }

    posterLayout->addLayout(overlayLayout);

    // Center Poster Placeholder Graphic
    m_posterLabel = new QLabel(posterContainer);
    m_posterLabel->setAlignment(Qt::AlignCenter);
    updatePosterPlaceholder();
    posterLayout->addWidget(m_posterLabel, 1);

    mainLayout->addWidget(posterContainer);

    // Movie Title (Bold, elided if long)
    QFont titleFont;
    titleFont.setPointSize(10);
    titleFont.setBold(true);

    m_titleLabel = new QLabel(this);
    m_titleLabel->setFont(titleFont);
    m_titleLabel->setStyleSheet("color: #FFFFFF; font-weight: bold;");
    
    QFontMetrics fm(titleFont);
    QString elidedTitle = fm.elidedText(m_movie.getTitle(), Qt::ElideRight, 175);
    m_titleLabel->setText(elidedTitle);
    m_titleLabel->setToolTip(m_movie.getTitle());
    mainLayout->addWidget(m_titleLabel);

    // Metadata Row: Year and Genre
    QHBoxLayout *metaLayout = new QHBoxLayout();
    metaLayout->setSpacing(6);

    m_yearLabel = new QLabel(m_movie.getFormattedYear(), this);
    m_yearLabel->setStyleSheet("color: #A0A5B5; font-size: 11px; font-weight: 500;");
    metaLayout->addWidget(m_yearLabel);

    QLabel *dotLabel = new QLabel("•", this);
    dotLabel->setStyleSheet("color: #606675; font-size: 10px;");
    metaLayout->addWidget(dotLabel);

    QString firstGenre = m_movie.getGenreList().isEmpty() ? "Drama" : m_movie.getGenreList().first();
    m_genreLabel = new QLabel(firstGenre, this);
    m_genreLabel->setStyleSheet("color: #E50914; font-size: 11px; font-weight: bold;");
    metaLayout->addWidget(m_genreLabel);

    metaLayout->addStretch();
    mainLayout->addLayout(metaLayout);

    // Director / Cast subtext
    QString directorText = m_movie.getDirector().isEmpty() ? "" : QString("Dir: %1").arg(m_movie.getDirector());
    QLabel *dirLabel = new QLabel(fm.elidedText(directorText, Qt::ElideRight, 175), this);
    dirLabel->setStyleSheet("color: #7B8191; font-size: 10px;");
    dirLabel->setToolTip(m_movie.getDirector());
    mainLayout->addWidget(dirLabel);

    mainLayout->addStretch();

    // Default card style
    setStyleSheet(
        "#MovieCard {"
        "  background-color: #181A20;"
        "  border: 1px solid #2B2F3A;"
        "  border-radius: 10px;"
        "}"
        "#MovieCard:hover {"
        "  border: 1px solid #E50914;"
        "  background-color: #20242D;"
        "}");
}

void MovieCard::updatePosterPlaceholder() {
    // Generate an attractive visual poster placeholder
    QPixmap pixmap(140, 110);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);

    // Background gradient for placeholder
    QLinearGradient grad(0, 0, 140, 110);
    grad.setColorAt(0, QColor(38, 43, 56));
    grad.setColorAt(1, QColor(24, 28, 36));
    painter.setBrush(grad);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(0, 0, 140, 110, 8, 8);

    // Film clapper / projector icon representation
    painter.setPen(QPen(QColor(229, 9, 20), 2));
    painter.drawRect(45, 25, 50, 40);
    painter.fillRect(45, 25, 50, 12, QColor(229, 9, 20, 180));
    painter.fillRect(52, 25, 8, 12, QColor(255, 255, 255, 200));
    painter.fillRect(68, 25, 8, 12, QColor(255, 255, 255, 200));
    painter.fillRect(84, 25, 8, 12, QColor(255, 255, 255, 200));

    // Play triangle
    QPolygon triangle;
    triangle << QPoint(65, 45) << QPoint(65, 57) << QPoint(77, 51);
    painter.setBrush(QColor(255, 255, 255, 220));
    painter.setPen(Qt::NoPen);
    painter.drawPolygon(triangle);

    // Subtitle text: Bollywood
    painter.setPen(QColor(160, 165, 181));
    QFont f = painter.font();
    f.setPointSize(8);
    f.setBold(true);
    painter.setFont(f);
    painter.drawText(QRect(0, 75, 140, 25), Qt::AlignCenter, "BOLLYWOOD");

    painter.end();
    m_posterLabel->setPixmap(pixmap);
}

void MovieCard::setSimilarityScore(double score) {
    m_similarityScore = score;
    if (m_matchBadge && score >= 0.0) {
        m_matchBadge->setText(QString("%1% Match").arg(static_cast<int>(score * 100.0 + 0.5)));
        m_matchBadge->show();
    }
}

void MovieCard::mousePressEvent(QMouseEvent *event) {
    Q_UNUSED(event);
    emit clicked(m_movie);
}

void MovieCard::enterEvent(QEnterEvent *event) {
    Q_UNUSED(event);
    setStyleSheet(
        "#MovieCard {"
        "  background-color: #20242D;"
        "  border: 1px solid #E50914;"
        "  border-radius: 10px;"
        "}");
}

void MovieCard::leaveEvent(QEvent *event) {
    Q_UNUSED(event);
    setStyleSheet(
        "#MovieCard {"
        "  background-color: #181A20;"
        "  border: 1px solid #2B2F3A;"
        "  border-radius: 10px;"
        "}");
}
