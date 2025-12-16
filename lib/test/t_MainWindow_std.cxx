#include "persalys/MainWidget.hxx"
#include "persalys/MainWindow.hxx"
#include "persalys/MenuBar.hxx"
#include "persalys/PythonEnvironment.hxx"
#include "persalys/AnalysisWindow.hxx"
#include "persalys/ResultWindow.hxx"
#include "persalys/DesignOfExperimentInputWindow.hxx"
#include "persalys/Actions.hxx"
#include "persalys/ModelEvaluation.hxx"
#include "persalys/ErrorWidget.hxx"

#include "openturns/OTtestcode.hxx"

#include <QtTest/QtTest>
#include <QApplication>
#include <QStandardItemModel>
#include <QTextEdit>
#include <QMetaType>
#include <QPointer>

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
  for (QAction * action : actions)
  {
    if (action->text() == text)
      return action;
  }
  return nullptr;
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
    qRegisterMetaType<AnalysisItem*>("AnalysisItem*");

    mainWindow_ = new MainWindow;
    mainWindow_->show();

    mainWidget_ = mainWindow_->findChild<MainWidget*>();
    manager_ = mainWindow_->findChild<StudyManager*>();
    treeViewModel_ = static_cast<QStandardItemModel*>(mainWidget_->getStudyTree()->model());
    stackedWidget_ = mainWidget_->findChild<SubWindowsStackedWidget*>();
  }


  void TestPythonConsole()
  {
    // get widgets
    const auto * menuBar = mainWindow_->findChild<MenuBar*>();
    QDockWidget * consoleDockWidget = mainWindow_->findChild<QDockWidget*>();

    // test python console visibility
    QAction * consoleAction = menuBar->pythonConsoleDisplayAction();
    if (!consoleDockWidget->isVisible())
      consoleAction->trigger();
    QVERIFY(consoleDockWidget->isVisible() == true);
    consoleDockWidget->close();
    QVERIFY(consoleDockWidget->isVisible() == false);
    consoleAction->trigger();
    QVERIFY(consoleDockWidget->isVisible() == true);
  }


  void TestActions()
  {
    // get widgets
    const MenuBar * menuBar = mainWindow_->findChild<MenuBar*>();
    QList<QAction*> actions = menuBar->actions();
    const QAction * fileAction = findAction(actions, "&File");
    QList<QAction*> fileMenuActions = fileAction->menu()->actions();

    // test: Ctrl+S without any study
    QTest::keyClicks(mainWindow_, "s", Qt::ControlModifier);

    // test new study action
    mainWidget_->getActions()->newAction()->trigger();
    auto * studyItem = static_cast<StudyItem*>(treeViewModel_->item(0));
    studyItem->getStudy().save("myStudy.xml");
    QVERIFY(treeViewModel_->rowCount() == 1);

    // - change name - save - reopen to check the study modification - close
    studyItem->setData("awesomeStudy", Qt::EditRole);
    mainWidget_->getActions()->saveAction()->trigger();
    findAction(fileMenuActions, "Close")->trigger();
    QVERIFY(treeViewModel_->rowCount() == 0);

    PERSALYS::Study theStudy(PERSALYS::Study::Open("myStudy.xml"));
    PERSALYS::Study::Add(theStudy);
    QVERIFY(treeViewModel_->rowCount() == 1);
    QVERIFY(treeViewModel_->item(0)->text() == "awesomeStudy");
    mainWidget_->getActions()->closeAction()->trigger();
    QVERIFY(treeViewModel_->rowCount() == 0);

    // test: Ctrl+N
    QTest::keyClicks(mainWindow_, "n", Qt::ControlModifier);
    QVERIFY(treeViewModel_->rowCount() == 1);
    studyItem = static_cast<StudyItem*>(treeViewModel_->item(0));
    studyItem->getStudy().save("myStudy.xml");

    // test rename study item
    studyItem->setData("awesomeStudy", Qt::EditRole);
    QVERIFY(studyItem->getStudy().getName() == "awesomeStudy");

    // test: Ctrl+S and close action of item
    QTest::keyClicks(mainWindow_, "s", Qt::ControlModifier);
    findAction(studyItem->getActions(), "Close")->trigger();
    QVERIFY(treeViewModel_->rowCount() == 0);

    // open recent study
    const QAction * recentAction = findAction(fileMenuActions, "Open Recent");
    if (QList<QAction*> recentMenuActions = recentAction->menu()->actions(); recentMenuActions.size() && !recentMenuActions[0]->text().isEmpty()) // does not work on Windows
    {
      recentMenuActions[0]->trigger();
      QVERIFY(treeViewModel_->rowCount() == 1);
      QVERIFY(treeViewModel_->item(0)->text() == "awesomeStudy");
    }

    // test: Ctrl+Q
    QTest::keyClicks(mainWindow_, "q", Qt::ControlModifier);

    mainWindow_->show();
  }


  void TestOpenStudy()
  {
    QVERIFY(stackedWidget_->count() == 1);

    QStringList files;
#ifdef PERSALYS_HAVE_OTMORRIS
    files << "test_deterministic_analyses.xml";
#endif
    files << "test_field_analyses.xml";

    for (const QString &file : files)
    {
      // open the study
      PERSALYS::Study theStudy(PERSALYS::Study::Open(file.toStdString()));
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
              QPointer<AnalysisItem> analysisItemGuard(analysisItem);
              QTimer::singleShot(150, [analysisItemGuard]()
              {
                QWidget * widget = QApplication::activeModalWidget();
                if (!widget)
                  return;
                if (analysisItemGuard)
                {
                  std::cout << "Analysis= " << analysisItemGuard->getAnalysis().getImplementation()->getClassName()
                            << " - Wizard= " << widget->metaObject()->className() << std::endl;
                }
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


  void TestExportImportStudy()
  {
    // open a study
    PERSALYS::Study theStudy(PERSALYS::Study::Open("test_field_analyses.xml"));
    PERSALYS::Study::Add(theStudy);
    QVERIFY(treeViewModel_->rowCount() == 1);

    int analysesCounter = 0;
    const QString scriptFileName("test_field_analyses_exported.py");

    // test: export
    auto * studyItem = static_cast<StudyItem*>(treeViewModel_->item(0));
    studyItem->exportPythonScript(scriptFileName);

    mainWidget_->getActions()->closeAction()->trigger();
    QVERIFY(treeViewModel_->rowCount() == 0);

    // test: import python script
    QVERIFY(QFile(scriptFileName).exists());
    manager_->importPythonScript(scriptFileName);

    const QDockWidget * consoleDockWidget = mainWindow_->findChild<QDockWidget*>();
    const QTextEdit * console = consoleDockWidget->findChild<QTextEdit*>();
    std::cout << "Python console Message = " << console->toPlainText().toStdString() << std::endl;
    QVERIFY2(console->toPlainText().toStdString() == ">>> __file__ = \"test_field_analyses_exported.py\"; exec(open(u\"test_field_analyses_exported.py\", encoding=\"utf-8\").read())\n>>> ", "wrong message");
    QVERIFY(treeViewModel_->rowCount() == 1);

    studyItem = static_cast<StudyItem*>(treeViewModel_->item(0));
    theStudy = studyItem->getStudy();

    // check window
    QList<AnalysisWindow *> analysisWindows = stackedWidget_->findChildren<AnalysisWindow *>();
    QVERIFY(analysisWindows.size() == 3);
    for (AnalysisWindow * & analysisWindow : analysisWindows)
    {
      const auto * item = dynamic_cast<AnalysisItem*>(analysisWindow->getItem());
      QVERIFY(item != nullptr);
      ++analysesCounter;
      // launch evaluations
      if (dynamic_cast<ModelEvaluation*>(item->getAnalysis().getImplementation().get()))
      {
        QPushButton * runButton = analysisWindow->findChild<QPushButton*>("runbutton");
        analysisWindow = nullptr;
        QVERIFY(runButton != nullptr);
        QSignalSpy spy(item, SIGNAL(windowRequested(AnalysisItem*, bool)));
        runButton->click();
        // Wait 2 sec for result
        spy.wait(2000);
        // Check if we had a timeout or if the signal was actually raised
        QCOMPARE(spy.count(), 1);
      }
    }
    // check analyses window
    int oneEvalFailed = 0;
    int oneEvalFinished = 0;
    for (UnsignedInteger i = 0; i < theStudy.getAnalyses().getSize(); ++i)
    {
      const auto * item = dynamic_cast<Item*>(theStudy.getAnalyses()[i].getImplementation().get()->getObserver("AnalysisItem"));
      QVERIFY(item != nullptr);
      for (int j = 0; j < stackedWidget_->count(); ++j)
      {
        auto * window = dynamic_cast<SubWindow*>(stackedWidget_->widget(j));
        if (window && window->getItem() == item)
        {
          if (theStudy.getAnalyses()[i].hasValidResult())
          {
            QVERIFY(dynamic_cast<ResultWindow*>(window) != nullptr);
            ++oneEvalFinished;
          }
          else
          {
            QVERIFY(dynamic_cast<AnalysisWindow*>(window) != nullptr);
            const auto * messageLabel = window->findChild<ErrorWidget*>();
            QVERIFY(messageLabel != nullptr);
            if (!theStudy.getAnalyses()[i].getErrorMessage().empty())
            {
              QVERIFY((messageLabel->toPlainText() + "\n").contains(theStudy.getAnalyses()[i].getErrorMessage().c_str()));
              ++oneEvalFailed;
            }
          }
        }
      }
    }
    QVERIFY(oneEvalFinished == 1);
    QVERIFY(oneEvalFailed == 1);
    QVERIFY(analysesCounter == (int)theStudy.getAnalyses().getSize());
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

#include "t_MainWindow_std.moc"
