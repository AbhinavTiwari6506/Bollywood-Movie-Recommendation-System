#ifndef SEARCHPAGE_H
#define SEARCHPAGE_H

#include <QWidget>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QGridLayout>
#include <QScrollArea>
#include "../models/Movie.h"

class SearchPage : public QWidget {
    Q_OBJECT

public:
    explicit SearchPage(QWidget *parent = nullptr);
    void setSearchQuery(const QString &query);
    void reloadFilters();
    void executeSearch();

signals:
    void movieDetailsRequested(const Movie &movie);
    void recommendRequested(const QString &movieTitle);

private slots:
    void onSearchTriggered();
    void onClearFilters();

private:
    void setupUI();
    void clearGrid();
    void displayResults(const QList<Movie> &movies);

    QLineEdit *m_searchInput;
    QComboBox *m_genreCombo;
    QComboBox *m_yearCombo;
    QLabel *m_resultCountLabel;
    QLabel *m_emptyStateLabel;

    QWidget *m_gridContainer;
    QGridLayout *m_gridLayout;
};

#endif // SEARCHPAGE_H
