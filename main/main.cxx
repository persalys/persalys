
#include <QApplication>

#include "otgui/MainWindow.hxx"

using namespace OTGUI;

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow window;

    window.resize(640, 480);
    window.show();
    window.launchInitialMessageBox();

    return app.exec();
}