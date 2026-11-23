#ifndef HOMEPAGE_H
#define HOMEPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QHBoxLayout>
#include <QGridLayout>
#include "../models/Movie.h"

class HomePage : public QWidget {
    Q_OBJECT

public:
    explicit HomePage(QWidget *parent = nullptr);
    void refreshData();

signals:
    void searchRequested(const QString &query);
    void recommendRequested(const QString &movieTitle);
    void movieDetailsRequested(const Movie &movie);

private slots:
    void onSearchClicked();
    void onRecommendClicked();

private:
    void setupUI();
    void populateFeatured();
    void populateRecent();
    void updateStats();

    QLineEdit *m_quickSearchInput;
    QLabel *m_totalMoviesLabel;
    QLabel *m_engineStatusLabel;
    QLabel *m_dbStatusLabel;

    QHBoxLayout *m_featuredLayout;
    QHBoxLayout *m_recentLayout;
};

#endif // HOMEPAGE_H
