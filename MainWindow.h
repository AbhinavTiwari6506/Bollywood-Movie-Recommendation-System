#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QButtonGroup>
#include <memory>

#include "../recommender/RecommendationEngine.h"
#include "../models/Movie.h"
#include "HomePage.h"
#include "SearchPage.h"
#include "RecommendationPage.h"
#include "AllMoviesPage.h"
#include "AboutPage.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onNavButtonClicked(int id);
    void onDatabaseStatusChanged(bool connected, const QString &message);
    void openDbConfigDialog();
    void showMovieDetails(const Movie &movie);
    void navigateToRecommend(const QString &movieTitle);
    void navigateToSearch(const QString &query);
    void bootstrapApplicationData();

private:
    void setupUI();
    void createTopBar();
    void createSidebar();
    void createPages();
    void updateDbStatusBadge();

    std::unique_ptr<RecommendationEngine> m_engine;

    // UI Widgets
    QWidget *m_sidebar;
    QButtonGroup *m_navButtonGroup;
    QStackedWidget *m_pagesStack;

    // Top Bar Widgets
    QLabel *m_dbStatusBadge;
    QPushButton *m_dbConfigBtn;

    // Pages
    HomePage *m_homePage;
    SearchPage *m_searchPage;
    RecommendationPage *m_recPage;
    AllMoviesPage *m_allMoviesPage;
    AboutPage *m_aboutPage;
};

#endif // MAINWINDOW_H
