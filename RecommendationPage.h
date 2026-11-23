#ifndef RECOMMENDATIONPAGE_H
#define RECOMMENDATIONPAGE_H

#include <QWidget>
#include <QComboBox>
#include <QLineEdit>
#include <QSlider>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>
#include "../models/Movie.h"
#include "../recommender/RecommendationEngine.h"

class RecommendationPage : public QWidget {
    Q_OBJECT

public:
    explicit RecommendationPage(RecommendationEngine *engine, QWidget *parent = nullptr);
    void setMovieToRecommend(const QString &movieTitle);
    void refreshMovieTitles();

signals:
    void movieDetailsRequested(const Movie &movie);

private slots:
    void onRecommendClicked();
    void onMovieSelected(int index);
    void onSliderValueChanged(int value);

private:
    void setupUI();
    void updateSelectedMoviePreview(const Movie &movie);
    void clearRecommendationsGrid();
    void displayRecommendations(const QList<MovieRecommendation> &recs, const QString &sourceTitle);

    RecommendationEngine *m_engine;

    QComboBox *m_movieCombo;
    QSlider *m_nRecsSlider;
    QLabel *m_sliderValueLabel;
    QPushButton *m_recommendBtn;

    // Selected Movie Preview Banner
    QFrame *m_previewCard;
    QLabel *m_previewTitle;
    QLabel *m_previewMeta;
    QLabel *m_previewOverview;

    // Output recommendations area
    QLabel *m_resultsHeading;
    QWidget *m_gridContainer;
    QGridLayout *m_gridLayout;
    QLabel *m_statusMessage;
};

#endif // RECOMMENDATIONPAGE_H
