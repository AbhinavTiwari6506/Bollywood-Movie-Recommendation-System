#ifndef MOVIEDETAILSDIALOG_H
#define MOVIEDETAILSDIALOG_H

#include <QDialog>
#include "../models/Movie.h"

class MovieDetailsDialog : public QDialog {
    Q_OBJECT

public:
    explicit MovieDetailsDialog(const Movie &movie, QWidget *parent = nullptr);

signals:
    void recommendRequested(const QString &movieTitle);

private:
    void setupUI();

    Movie m_movie;
};

#endif // MOVIEDETAILSDIALOG_H
