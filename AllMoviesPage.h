#ifndef ALLMOVIESPAGE_H
#define ALLMOVIESPAGE_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QScrollArea>
#include "../models/Movie.h"

class AllMoviesPage : public QWidget {
    Q_OBJECT

public:
    explicit AllMoviesPage(QWidget *parent = nullptr);
    void reloadMovies();

signals:
    void movieDetailsRequested(const Movie &movie);
    void recommendRequested(const QString &movieTitle);

private slots:
    void onNextPage();
    void onPrevPage();

private:
    void setupUI();
    void updateGrid();

    int m_currentPage;
    int m_pageSize;
    int m_totalCount;

    QLabel *m_pageInfoLabel;
    QPushButton *m_prevBtn;
    QPushButton *m_nextBtn;

    QWidget *m_gridContainer;
    QGridLayout *m_gridLayout;
    QScrollArea *m_scrollArea;
};

#endif // ALLMOVIESPAGE_H
