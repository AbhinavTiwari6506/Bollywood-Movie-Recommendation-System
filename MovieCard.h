#ifndef MOVIECARD_H
#define MOVIECARD_H

#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "../models/Movie.h"

/**
 * @brief Modern card widget for displaying a movie item in grids and carousels.
 */
class MovieCard : public QFrame {
    Q_OBJECT

public:
    explicit MovieCard(const Movie &movie,
                       double similarityScore = -1.0,
                       QWidget *parent = nullptr);

    Movie getMovie() const { return m_movie; }
    void setSimilarityScore(double score);

signals:
    void clicked(const Movie &movie);
    void recommendRequested(const Movie &movie);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    void setupUI();
    void updatePosterPlaceholder();

    Movie m_movie;
    double m_similarityScore;

    QLabel *m_posterLabel;
    QLabel *m_titleLabel;
    QLabel *m_yearLabel;
    QLabel *m_genreLabel;
    QLabel *m_ratingLabel;
    QLabel *m_matchBadge;
};

#endif // MOVIECARD_H
