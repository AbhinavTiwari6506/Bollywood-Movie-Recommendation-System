#ifndef ABOUTPAGE_H
#define ABOUTPAGE_H

#include <QWidget>

class AboutPage : public QWidget {
    Q_OBJECT

public:
    explicit AboutPage(QWidget *parent = nullptr);

private:
    void setupUI();
};

#endif // ABOUTPAGE_H
