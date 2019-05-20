#include "persalys/MainWidget.hxx"

#include <QtTest/QtTest>

using namespace PERSALYS;

class TestGui : public QObject
{
  Q_OBJECT
private slots:
  void testGui()
  {
    MainWidget window;
    QTest::keyClicks(&window, "hello world");
  }
};


QTEST_MAIN(TestGui)
#include "t_WelcomeWindow_std.moc"
