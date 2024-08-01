#include "persalys/MainWidget.hxx"
#include "persalys/MainWindow.hxx"
#include "persalys/MenuBar.hxx"
#include "persalys/SettingsDialog.hxx"
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


  void TestPythonConsole()
  {
    // get widgets
    MenuBar * menuBar = mainWindow_->findChild<MenuBar*>();
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

  void TestSettings()
  {
    // get widgets
    MenuBar * menuBar = mainWindow_->findChild<MenuBar*>();
    QAction * settingsAction = menuBar->settingsDisplayAction();
      settingsAction->trigger();
    SettingsDialog * settingsDialog = menuBar->findChild<SettingsDialog*>();
    std::cout << settingsDialog->getProcessNumber()<< std::endl;
    QVERIFY(settingsDialog->getProcessNumber()
            == OT::ResourceMap::GetAsUnsignedInteger("PythonPhysicalModel-DefaultProcessNumber"));
    const UnsignedInteger nProc = 2;
    settingsDialog->setProcessNumber(2);
    settingsDialog->accept();
    QVERIFY(OT::ResourceMap::GetAsUnsignedInteger("PythonPhysicalModel-DefaultProcessNumber") == nProc);
  }


  void TestActions()
  {
    // get widgets
    MenuBar * menuBar = mainWindow_->findChild<MenuBar*>();
    QList<QAction*> actions = menuBar->actions();
    QAction * fileAction = findAction(actions, "&File");
    QList<QAction*> fileMenuActions = fileAction->menu()->actions();

    // test: Ctrl+S without any study
    QTest::keyClicks(mainWindow_, "s", Qt::ControlModifier);

    // test new study action
    mainWidget_->getActions()->newAction()->trigger();
    StudyItem * studyItem = static_cast<StudyItem*>(treeViewModel_->item(0));
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
    QAction * recentAction = findAction(fileMenuActions, "Open Recent");
    QList<QAction*> recentMenuActions = recentAction->menu()->actions();
    if (recentMenuActions.size() && !recentMenuActions[0]->text().isEmpty()) // does not work on Windows
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
    files << "test_probabilistic_analyses.xml";
    files << "test_field_analyses.xml";

    for (int i = 0; i < files.size(); ++i)
    {
      // open the study
      PERSALYS::Study theStudy(PERSALYS::Study::Open(files[i].toStdString()));
      PERSALYS::Study::Add(theStudy);
      QVERIFY(treeViewModel_->rowCount() == 1);

      // check the result windows type
      for (int j = 0; j < stackedWidget_->count(); ++ j)
      {
        SubWindow * subWindow = dynamic_cast<SubWindow*>(stackedWidget_->widget(j));
        if (subWindow)
        {
          AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(subWindow->getItem());
          DesignOfExperimentDefinitionItem * doeItem = dynamic_cast<DesignOfExperimentDefinitionItem*>(subWindow->getItem());
          if (analysisItem && !doeItem)
          {
            if (analysisItem->getAnalysis().hasValidResult())
            {
              std::cout << "Analysis= " << analysisItem->getAnalysis().getImplementation()->getClassName()
                        << " - Window= " << subWindow->metaObject()->className() << std::endl;
              QVERIFY(dynamic_cast<ResultWindow*>(subWindow) != 0);
            }
            else
            {
              QVERIFY(dynamic_cast<AnalysisWindow*>(subWindow) != 0);
            }
            QAction * modifyAction = findAction(analysisItem->getActions(), "Modify");
            if (modifyAction)
            {
              QTimer::singleShot(150, [ = ]()
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


  void TestExportImportStudy()
  {
    // open a study
    PERSALYS::Study theStudy(PERSALYS::Study::Open("test_field_analyses.xml"));
    PERSALYS::Study::Add(theStudy);
    QVERIFY(treeViewModel_->rowCount() == 1);

    int analysesCounter = 0;
    const QString scriptFileName("test_field_analyses_exported.py");

    // test: export
    StudyItem * studyItem = static_cast<StudyItem*>(treeViewModel_->item(0));
    studyItem->exportPythonScript(scriptFileName);

    mainWidget_->getActions()->closeAction()->trigger();
    QVERIFY(treeViewModel_->rowCount() == 0);

    // test: import python script
    QVERIFY(QFile(scriptFileName).exists());
    manager_->importPythonScript(scriptFileName);

    QDockWidget * consoleDockWidget = mainWindow_->findChild<QDockWidget*>();
    QTextEdit * console = consoleDockWidget->findChild<QTextEdit*>();
    std::cout << "Python console Message = " << console->toPlainText().toStdString() << std::endl;
    QVERIFY2(console->toPlainText().toStdString() == ">>> __file__ = \"test_field_analyses_exported.py\"; exec(open(u\"test_field_analyses_exported.py\", encoding=\"utf-8\").read())\n>>> ", "wrong message");
    QVERIFY(treeViewModel_->rowCount() == 1);

    studyItem = static_cast<StudyItem*>(treeViewModel_->item(0));
    theStudy = studyItem->getStudy();

    // check window
    QList<AnalysisWindow *> analysisWindows = stackedWidget_->findChildren<AnalysisWindow *>();
    QVERIFY(analysisWindows.size() == 3);
    for (int i = 0; i < analysisWindows.size(); ++i)
    {
      AnalysisItem * item = dynamic_cast<AnalysisItem*>(analysisWindows[i]->getItem());
      QVERIFY(item != 0);
      ++analysesCounter;
      // launch evaluations
      if (dynamic_cast<ModelEvaluation*>(item->getAnalysis().getImplementation().get()))
      {
        QPushButton * runButton = analysisWindows[i]->findChild<QPushButton*>("runbutton");
        analysisWindows[i] = 0;
        QVERIFY(runButton != 0);
        QSignalSpy spy(item, SIGNAL(windowRequested(AnalysisItem*, bool)));
        runButton->click();
        // Wait 2 sec for result
        spy.wait(2000);
        // Check if we had a timeout or actually signal was raised
        QCOMPARE(spy.count(), 1);
      }
    }
    // check analyses window
    int oneEvalFailed = 0;
    int oneEvalFinished = 0;
    for (UnsignedInteger i = 0; i < theStudy.getAnalyses().getSize(); ++i)
    {
      Item * item = dynamic_cast<Item*>(theStudy.getAnalyses()[i].getImplementation().get()->getObserver("AnalysisItem"));
      QVERIFY(item != 0);
      for (int j = 0; j < stackedWidget_->count(); ++j)
      {
        SubWindow * window = dynamic_cast<SubWindow*>(stackedWidget_->widget(j));
        if (window && window->getItem() == item)
        {
          if (theStudy.getAnalyses()[i].hasValidResult())
          {
            QVERIFY(dynamic_cast<ResultWindow*>(window) != 0);
            ++oneEvalFinished;
          }
          else
          {
            QVERIFY(dynamic_cast<AnalysisWindow*>(window) != 0);
            QLabel * messageLabel = window->findChild<QLabel*>();
            QVERIFY(messageLabel != 0);
            if (!theStudy.getAnalyses()[i].getErrorMessage().empty())
            {
              QVERIFY(messageLabel->text().contains(theStudy.getAnalyses()[i].getErrorMessage().c_str()));
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
