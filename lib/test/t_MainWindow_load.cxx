#include "persalys/MainWidget.hxx"
#include "persalys/MainWindow.hxx"
#include "persalys/MenuBar.hxx"
#include "persalys/PythonEnvironment.hxx"
#include "persalys/AnalysisWindow.hxx"
#include "persalys/ResultWindow.hxx"
#include "persalys/DesignOfExperimentInputWindow.hxx"
#include "persalys/Actions.hxx"
#include "persalys/ModelEvaluation.hxx"

#include "openturns/OTtestcode.hxx"

#include <QtTest/QtTest>
#include <QApplication>
#include <QStandardItemModel>
#include <QTextEdit>

#ifdef PERSALYS_HAVE_PARAVIEW
#include <pqPVApplicationCore.h>
#endif

using namespace OT;
using namespace OT::Test;

using namespace PERSALYS;

namespace PERSALYS
{

QAction* findAction(QList<QAction*> actions, QString text)
{
  for (int i = 0; i < actions.size(); ++i)
  {
    if (actions[i]->text() == text)
      return actions[i];
  }
  return 0;
}


class TestMainWindow : public QObject
{
  Q_OBJECT

private:
  MainWindow * mainWindow_;
  MainWidget * mainWidget_;
  StudyManager * manager_;
  QStandardItemModel * treeViewModel_;
  SubWindowsStackedWidget * stackedWidget_;

private slots:
  void initTestCase()
  {
    mainWindow_ = new MainWindow;
    mainWindow_->show();

    mainWidget_ = mainWindow_->findChild<MainWidget*>();
    manager_ = mainWindow_->findChild<StudyManager*>();
    treeViewModel_ = static_cast<QStandardItemModel*>(mainWidget_->getStudyTree()->model());
    stackedWidget_ = mainWidget_->findChild<SubWindowsStackedWidget*>();
  }

  void TestOpenStudy() const
  {
    QVERIFY(stackedWidget_->count() == 1);

    QStringList files;
    files << "test_probabilistic_analyses.xml";

    for (int i = 0; i < files.size(); ++i)
    {
      // open the study
      PERSALYS::Study theStudy(PERSALYS::Study::Open(files[i].toStdString()));
      PERSALYS::Study::Add(theStudy);
      QVERIFY(treeViewModel_->rowCount() == 1);

      // check the result windows type
      for (int j = 0; j < stackedWidget_->count(); ++ j)
      {
        auto * subWindow = dynamic_cast<SubWindow*>(stackedWidget_->widget(j));
        if (subWindow)
        {
          auto * analysisItem = dynamic_cast<AnalysisItem*>(subWindow->getItem());
          const auto * doeItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(subWindow->getItem());
          if (analysisItem && !doeItem)
          {
            if (analysisItem->getAnalysis().hasValidResult())
            {
              std::cout << "Analysis= " << analysisItem->getAnalysis().getImplementation()->getClassName()
                        << " - Window= " << subWindow->metaObject()->className() << std::endl;
              QVERIFY(dynamic_cast<ResultWindow*>(subWindow) != nullptr);
            }
            else
            {
              QVERIFY(dynamic_cast<AnalysisWindow*>(subWindow) != nullptr);
            }
            QAction * modifyAction = findAction(analysisItem->getActions(), "Modify");
            if (modifyAction)
            {
              QTimer::singleShot(150, [analysisItem]()
              {
                QWidget * widget = QApplication::activeModalWidget();
                std::cout << "Analysis= " << analysisItem->getAnalysis().getImplementation()->getClassName()
                          << " - Wizard= " << widget->metaObject()->className() << std::endl;
                widget->close();
              });
              modifyAction->trigger();
            }
          }
        }
      }
      mainWidget_->getActions()->closeAction()->trigger();
      QVERIFY(treeViewModel_->rowCount() == 0);
      QVERIFY(stackedWidget_->count() == 1);
    }
  }
};
}


int main(int argc, char **argv)
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  // Python Environment
  PythonEnvironment env;
  {
    QApplication app(argc, argv);
#ifdef PERSALYS_HAVE_PARAVIEW
    pqPVApplicationCore appPV(argc, argv);
    QApplication::instance()->installEventFilter(&appPV);
#endif
    PERSALYS::TestMainWindow tc;
    return QTest::qExec(&tc, argc, argv);
  }
}

#include "t_MainWindow_load.moc"
