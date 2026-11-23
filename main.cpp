#include <QApplication>
#include <QStyleFactory>
#include <QFont>
#include "ui/MainWindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    // Metadata
    QApplication::setApplicationName("Bollywood Movie Recommendation System");
    QApplication::setApplicationVersion("2.0.0");
    QApplication::setOrganizationName("BollywoodAI");

    // Clean modern typography
    QFont font("Segoe UI", 10);
    QApplication::setFont(font);

    // Apply Fusion style base for clean dark rendering
    app.setStyle(QStyleFactory::create("Fusion"));

    MainWindow window;
    window.show();

    return app.exec();
}
