#include "persalys/DesignOfExperimentWindow.hxx"
#include "persalys/DesignOfExperimentDefinitionItem.hxx"
#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/FixedDesignOfExperiment.hxx"
#include "persalys/DataAnalysisResult.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/PythonEnvironment.hxx"
#include "persalys/MomentsEstimatesTableGroupBox.hxx"
#include "persalys/CopyableTableView.hxx"

#include <openturns/OTDistribution.hxx>
#include "openturns/OTtestcode.hxx"

#include <QtTest/QtTest>
#include <QTabWidget>

#ifdef PERSALYS_HAVE_PARAVIEW
#include <pqPVApplicationCore.h>
#endif

using namespace OT;
using namespace OT::Test;
using namespace PERSALYS;

namespace PERSALYS
{
class TestDesignOfExperimentWindow : public QObject
{
  Q_OBJECT

private slots:
  void TestNotEvaluatedDesignOfExperiment() const
  {
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
    SymbolicPhysicalModel model("model", inputCollection, outputCollection, formula);

    // create a design of experiments which contains input points but is not evaluated
    const UnsignedInteger size = 4;
    Sample inSample(size, 3);
    inSample.setDescription(model.getInputNames());
    for (UnsignedInteger i = 0; i < size; ++i)
    {
      inSample(i, 0) = 10000. + i;
      inSample(i, 1) = 3000. + i;
      inSample(i, 2) = 4000. + i;
    }
    DesignOfExperiment doe("doe", model);
    doe.setInputSample(inSample);

    DataAnalysisResult result(doe);
    QVERIFY2(result.getDesignOfExperiment().getSample().getSize() == size, "wrong design of experiments sample size");
    // the design of experiments is not evaluated: no PDF/CDF nor box plots data
    QVERIFY2(result.getPDF().getSize() == 0, "the design of experiments must not be evaluated");
    QVERIFY2(result.getMedian().getSize() == 0, "the design of experiments must not be evaluated");

    DesignOfExperimentEvaluation analysis("doe", model, result);

    // create the item and the window
    DesignOfExperimentDefinitionItem item(analysis);
    DesignOfExperimentWindow window(&item);

    // the PDF/CDF and box plots tabs need evaluation results: they must not appear
    QStringList tabTexts;
    for (int i = 0; i < window.tabWidget_->count(); ++i)
      tabTexts << window.tabWidget_->tabText(i);

    QVERIFY2(!tabTexts.contains("PDF/CDF"), "the PDF/CDF tab must not be present for a non evaluated design of experiments");
    QVERIFY2(!tabTexts.contains("Box plots"), "the Box plots tab must not be present for a non evaluated design of experiments");
  }

  void TestEvaluatedMonteCarloDesignOfExperiment() const
  {
    // create the model
    Input X0("X0", 2.0, OT::Normal(), "X0");
    Input X1("X1", 2.0, OT::Normal(), "X1");
    Output Y0("Y0", "Y0");
    OT::Description formula(1, "sin(X0)+8*X1");
    SymbolicPhysicalModel model("model", {X0, X1}, {Y0}, formula);

    // Monte Carlo design: confidence intervals are meaningful, the
    // moments estimates table must have a CI column
    const UnsignedInteger size = 100;
    OT::RandomGenerator::SetSeed(0);
    FixedDesignOfExperiment doeEval("doeEval", model);
    Sample inSample = OT::Normal(2).getSample(size);
    inSample.setDescription(model.getInputNames());
    doeEval.setOriginalInputSample(inSample, DataModel::MC);
    doeEval.run();

    QVERIFY2(doeEval.getResult().getMeanConfidenceInterval().getFiniteLowerBound()[0], "the mean CI of a MC design of experiments must be finite");

    DesignOfExperimentDefinitionItem item(doeEval);
    DesignOfExperimentWindow window(&item);

    // count the columns of the moments estimates table of the first variable
    const MomentsEstimatesTableGroupBox * momentsGroupBox = window.findChild<MomentsEstimatesTableGroupBox *>();
    QVERIFY2(momentsGroupBox != nullptr, "no moments estimates table found");
    const ResizableStackedWidget * stackedWidget = momentsGroupBox->findChild<ResizableStackedWidget *>();
    QVERIFY2(stackedWidget != nullptr, "no stacked widget found in the moments estimates table");
    const auto * tableView = dynamic_cast<CopyableTableView *>(stackedWidget->currentWidget());
    QVERIFY2(tableView != nullptr, "no moments estimates table view found");
    QVERIFY2(tableView->model()->columnCount() == 3, "the moments estimates table of a MC design of experiments must contain a confidence interval column");
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
  PERSALYS::TestDesignOfExperimentWindow tc;
  return QTest::qExec(&tc, argc, argv);
}

#include "t_DesignOfExperimentWindow_std.moc"
