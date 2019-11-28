#include "persalys/MonteCarloResultWindow.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/StudyTreeViewModel.hxx"
#include "persalys/MonteCarloAnalysis.hxx"
#include "persalys/MomentsEstimatesTableGroupBox.hxx"
#include "persalys/MinMaxTableGroupBox.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/PythonEnvironment.hxx"
#include "persalys/SubWindow.hxx"
#include "persalys/MainWindow.hxx"
#include "persalys/MainWidget.hxx"

#include <openturns/OTDistribution.hxx>
#include "openturns/OTtestcode.hxx"

#include <QtTest/QtTest>
#include <QTableView>

#ifdef PERSALYS_HAVE_PARAVIEW
#include <pqPVApplicationCore.h>
#endif

using namespace OT;
using namespace OT::Test;
using namespace PERSALYS;

namespace PERSALYS
{
class TestMonteCarloResultWindow : public QObject
{
  Q_OBJECT

private:
  StudyTreeViewModel * treeModel;
  MonteCarloAnalysis analysis;
  MainWindow * mainWindow;

private slots:
  void initTestCase()
  {
    mainWindow = new MainWindow;

    // create the model
    Input Q("Q", 10200, OT::Normal(10200, 100), "Primary energy");
    Input E("E", 3000, OT::Normal(3000, 15), "Produced electric energy");
    Input C("C", 4000, OT::Normal(4000, 60), "Valued thermal energy");
    Output Ep("Ep", "Primary energy savings");
    InputCollection inputCollection(3);
    inputCollection[0] = Q;
    inputCollection[1] = E;
    inputCollection[2] = C;
    OutputCollection outputCollection(1, Ep);

    OT::Description formula(1, "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    SymbolicPhysicalModel * model = new SymbolicPhysicalModel("model", inputCollection, outputCollection, formula);

    Study aStudy;
    aStudy.add(model);

    // create the analysis
    analysis = MonteCarloAnalysis("monte_carlo", model);
    analysis.setMaximumCoefficientOfVariation(-1);
    analysis.setMaximumElapsedTime(1);
    analysis.run();
    aStudy.add(analysis);

    MonteCarloAnalysis analysis2("monte_carlo2", model);
    analysis2.setMaximumCalls(1);
    analysis2.setIsConfidenceIntervalRequired(false);
    analysis2.run();
    aStudy.add(analysis2);

    MonteCarloAnalysis analysis3("monte_carlo3", model);
    analysis3.setMaximumCoefficientOfVariation(0.02);
    analysis3.setMaximumElapsedTime(10000);
    analysis3.run();
    aStudy.add(analysis3);

    treeModel = new StudyTreeViewModel;
    treeModel->appendItem(aStudy);
  }

  void TestBehaviorForAnalysis()
  {
    // find the analysis item
    QModelIndexList listIndexes = treeModel->match(treeModel->index(0, 0), Qt::DisplayRole, "monte_carlo", 1, Qt::MatchRecursive);
    AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(treeModel->itemFromIndex(listIndexes[0]));
    // create the window
    MonteCarloResultWindow window(analysisItem, mainWindow);
    window.show();

    // get widgets
    MomentsEstimatesTableGroupBox * momentsGroupBox = window.findChild<MomentsEstimatesTableGroupBox*>();
    ResizableStackedWidget * momentStackWidget = momentsGroupBox->findChild<ResizableStackedWidget*>();
    MinMaxTableGroupBox * minMaxGroupBox = window.findChild<MinMaxTableGroupBox*>();
    ResizableStackedWidget * minMaxStackWidget = minMaxGroupBox->findChild<ResizableStackedWidget*>();

    // checks

    QVERIFY2(window.tabWidget_->count() == 10, "wrong number of tabs");
    QVERIFY2(window.analysisStopCriteriaMessage_ == "Maximum elapsed time reached", "Wrong stop message");

    Description varNames(analysis.getPhysicalModel().getOutputNames());
    varNames.add(analysis.getPhysicalModel().getInputNames());
    QVERIFY2(window.variablesListWidget_->count() == int(varNames.getSize()), "wrong number of lines");

    // change QListWidget row
    for (UnsignedInteger i = 0; i < varNames.getSize(); ++i)
    {
      // check list widget
      QVERIFY2(window.variablesListWidget_->item(i)->text() == varNames[i].c_str(), "wrong item text");
      window.variablesListWidget_->setCurrentRow(i);

      // check moments table
      QVERIFY2(momentStackWidget->currentIndex() == (int)i, "wrong current moments widget");
      CopyableTableView * tableView_i = dynamic_cast<CopyableTableView*>(momentStackWidget->currentWidget());
      QVERIFY2(tableView_i->model()->rowCount() == 8, "wrong number of lines");
      QVERIFY2(tableView_i->model()->columnCount() == 3, "wrong number of columns");

      // check min/max table
      QVERIFY2(minMaxStackWidget->currentIndex() == (int)i, "wrong current min/max widget");
      tableView_i = dynamic_cast<CopyableTableView*>(minMaxStackWidget->currentWidget());
      if (i < analysis.getPhysicalModel().getOutputDimension())
        QVERIFY2(tableView_i->model()->rowCount() == int(analysis.getPhysicalModel().getInputDimension()) + 1, "wrong number of lines");
      else
        QVERIFY2(tableView_i->model()->rowCount() == 1, "wrong number of lines");
    }
    // change QTabWidget tab
    for (int i = 0; i < window.tabWidget_->count(); ++i)
    {
      window.tabWidget_->setCurrentIndex(i);
      bool isHidden = window.variablesGroupBox_->isHidden();
      QVERIFY2((i > 1)? isHidden : !isHidden, "wrong list widget visibility");
    }
  }

  void TestBehaviorForAnalysis2()
  {
    // find the analysis item
    QModelIndexList listIndexes = treeModel->match(treeModel->index(0, 0), Qt::DisplayRole, "monte_carlo2", 1, Qt::MatchRecursive);
    AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(treeModel->itemFromIndex(listIndexes[0]));
    // create the window
    MonteCarloResultWindow window(analysisItem, mainWindow);
    window.show();

    // get widgets
    MomentsEstimatesTableGroupBox * momentsGroupBox = window.findChild<MomentsEstimatesTableGroupBox*>();
    ResizableStackedWidget * momentStackWidget = momentsGroupBox->findChild<ResizableStackedWidget*>();

    // checks

    QVERIFY2(window.tabWidget_->count() == 4, "wrong number of tabs");
    QVERIFY2(window.analysisStopCriteriaMessage_ == "Maximum calls reached", "Wrong stop message");

    // change QListWidget row
    for (int i = 0; i < window.variablesListWidget_->count(); ++i)
    {
      window.variablesListWidget_->setCurrentRow(i);

      // check moments table
      QVERIFY2(momentStackWidget->currentIndex() == i, "wrong current moments widget");
      CopyableTableView * tableView_i = dynamic_cast<CopyableTableView*>(momentStackWidget->currentWidget());
      QVERIFY2(tableView_i->model()->rowCount() == 8, "wrong number of lines");
      QVERIFY2(tableView_i->model()->columnCount() == 2, "wrong number of columns");
      QVERIFY2(tableView_i->model()->data(tableView_i->model()->index(2, 1)).toString() == "0", "wrong text in moments table");
      QVERIFY2(tableView_i->model()->data(tableView_i->model()->index(3, 1)).toString() == "0", "wrong text in moments table");
      QVERIFY2(tableView_i->model()->data(tableView_i->model()->index(4, 1)).toString() == "-", "wrong text in moments table");
      QVERIFY2(tableView_i->model()->data(tableView_i->model()->index(5, 1)).toString() == "-", "wrong text in moments table");
    }
  }

  void TestBehaviorForAnalysis3()
  {
    // find the analysis item
    QModelIndexList listIndexes = treeModel->match(treeModel->index(0, 0), Qt::DisplayRole, "monte_carlo3", 1, Qt::MatchRecursive);
    AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(treeModel->itemFromIndex(listIndexes[0]));
    // create the window
    MonteCarloResultWindow window(analysisItem, mainWindow);
    window.show();

    // check
    QVERIFY2(window.analysisStopCriteriaMessage_ == "Maximum coefficient of variation reached", "Wrong stop message");
  }
};
}

int main(int argc, char **argv)
{
  TESTPREAMBLE;
  OStream fullprint(std::cout);

  QApplication app(argc, argv);
#ifdef PERSALYS_HAVE_PARAVIEW
  pqPVApplicationCore appPV(argc, argv);
  QApplication::instance()->installEventFilter(&appPV);
#endif
  PERSALYS::TestMonteCarloResultWindow tc;
  return QTest::qExec(&tc, argc, argv);
}

#include "t_MonteCarloResultWindow_std.moc"
