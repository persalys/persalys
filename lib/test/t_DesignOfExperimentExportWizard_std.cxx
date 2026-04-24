#include "persalys/ProbabilisticDesignOfExperiment.hxx"
#include "persalys/GridDesignOfExperiment.hxx"
#include "persalys/DesignOfExperimentExportWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/Study.hxx"
#include "persalys/StudyItem.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>

using namespace OT;

namespace PERSALYS
{
class TestDesignOfExperimentExportWizard : public QObject
{
  Q_OBJECT

public:
  TestDesignOfExperimentExportWizard()
  {
    // create the model
    Input Q("Q", 10200, Normal(10200, 100), "Primary energy");
    Input E("E", 3000, Normal(3000, 15), "Produced electric energy");
    Input C("C", 4000, Normal(4000, 60), "Valued thermal energy");
    Output Ep("Ep", "Primary energy savings");
    InputCollection inputCollection(3);
    inputCollection[0] = Q;
    inputCollection[1] = E;
    inputCollection[2] = C;
    OutputCollection outputCollection(1, Ep);

    Description formula(1, "1-(Q/((E/((1-0.05)*0.54))+(C/0.8)))");
    model_ = SymbolicPhysicalModel("model", inputCollection, outputCollection, formula);
  }

private:
  SymbolicPhysicalModel model_;

private slots:
  void TestUnevaluatedNotVisible() const
  {
    // A DOE that has not been run should not appear in the combobox
    Study aStudy;
    PhysicalModel aModel(model_);
    aStudy.add(aModel);

    // create an unevaluated MC DOE
    ProbabilisticDesignOfExperiment mcDoe("mcNotRun", aModel, 50, "MONTE_CARLO");
    Analysis mcAnalysis(mcDoe);
    aStudy.add(mcAnalysis);

    // create an evaluated MC DOE
    ProbabilisticDesignOfExperiment mcDoeRun("mcRun", aModel, 50, "MONTE_CARLO");
    mcDoeRun.run();
    Analysis mcRunAnalysis(mcDoeRun);
    aStudy.add(mcRunAnalysis);

    // create the wizard via StudyItem
    StudyItem studyItem(aStudy);
    DesignOfExperimentExportWizard wizard(&studyItem);
    wizard.show();

    // Only the evaluated DOE should be in the combobox
    QVERIFY2(wizard.doeComboBox_->count() == 1, "Only evaluated DOEs should appear in the combobox");
    QVERIFY2(wizard.doeComboBox_->itemText(0) == "mcRun", "The evaluated DOE must be named: mcRun");
  }


  void TestSelectMultipleDOEs() const
  {
    // Check that several evaluated DOEs can be selected one by one
    Study aStudy;
    PhysicalModel aModel(model_);
    aStudy.add(aModel);

    // create and run a MC DOE
    ProbabilisticDesignOfExperiment mcDoe("mcDoe", aModel, 60, "MONTE_CARLO");
    mcDoe.run();
    Analysis mcAnalysis(mcDoe);
    aStudy.add(mcAnalysis);

    // create and run a Grid DOE
    GridDesignOfExperiment gridDoe("gridDoe", aModel);
    gridDoe.run();
    Analysis gridAnalysis(gridDoe);
    aStudy.add(gridAnalysis);

    // create the wizard
    StudyItem studyItem(aStudy);
    DesignOfExperimentExportWizard wizard(&studyItem);
    wizard.show();

    // Both evaluated DOEs must be in the combobox
    QVERIFY2(wizard.doeComboBox_->count() == 2, "Both evaluated DOEs should appear in the combobox");

    // Select the first DOE (MC) and verify
    wizard.doeComboBox_->setCurrentIndex(0);
    QVERIFY2(wizard.getAnalysis().getName() == "mcDoe", "First DOE must be named: mcDoe");

    // Select the second DOE (Grid) and verify
    wizard.doeComboBox_->setCurrentIndex(1);
    QVERIFY2(wizard.getAnalysis().getName() == "gridDoe", "Second DOE must be named: gridDoe");
  }


  void TestExportedType() const
  {
    // Check that each DOE is exported with the expected type information
    Study aStudy;
    PhysicalModel aModel(model_);
    aStudy.add(aModel);

    // create and run a MC DOE
    ProbabilisticDesignOfExperiment mcDoe("mcDoe", aModel, 60, "MONTE_CARLO");
    mcDoe.run();
    Analysis mcAnalysis(mcDoe);
    aStudy.add(mcAnalysis);

    // create and run a Grid DOE
    GridDesignOfExperiment gridDoe("gridDoe", aModel);
    gridDoe.run();
    Analysis gridAnalysis(gridDoe);
    aStudy.add(gridAnalysis);

    // create the wizard
    StudyItem studyItem(aStudy);
    DesignOfExperimentExportWizard wizard(&studyItem);
    wizard.show();

    // Select MC DOE and check parameters contain "Monte Carlo"
    wizard.doeComboBox_->setCurrentIndex(0);
    {
      const Analysis analysis = wizard.getAnalysis();
      const Parameters params = analysis.getImplementation().get()->getParameters();
      const String designType = params.get("Design type");
      QVERIFY2(designType == "Monte Carlo",
               qPrintable(QString("MC DOE design type must be 'Monte Carlo', got: '%1'").arg(QString::fromStdString(designType))));
    }

    // Select Grid DOE and check parameters contain "Grid"
    wizard.doeComboBox_->setCurrentIndex(1);
    {
      const Analysis analysis = wizard.getAnalysis();
      const Parameters params = analysis.getImplementation().get()->getParameters();
      const String designType = params.get("Design type");
      QVERIFY2(designType == "Grid",
               qPrintable(QString("Grid DOE design type must be 'Grid', got: '%1'").arg(QString::fromStdString(designType))));
    }
  }


  void TestMixEvaluatedAndUnevaluated() const
  {
    // Mix of evaluated and unevaluated DOEs: only evaluated ones appear
    Study aStudy;
    PhysicalModel aModel(model_);
    aStudy.add(aModel);

    // unevaluated MC DOE
    ProbabilisticDesignOfExperiment mcNotRun("mcNotRun", aModel, 30, "MONTE_CARLO");
    Analysis mcNotRunAnalysis(mcNotRun);
    aStudy.add(mcNotRunAnalysis);

    // evaluated MC DOE
    ProbabilisticDesignOfExperiment mcRun("mcRun", aModel, 40, "MONTE_CARLO");
    mcRun.run();
    Analysis mcRunAnalysis(mcRun);
    aStudy.add(mcRunAnalysis);

    // unevaluated Grid DOE
    GridDesignOfExperiment gridNotRun("gridNotRun", aModel);
    Analysis gridNotRunAnalysis(gridNotRun);
    aStudy.add(gridNotRunAnalysis);

    // evaluated Grid DOE
    GridDesignOfExperiment gridRun("gridRun", aModel);
    gridRun.run();
    Analysis gridRunAnalysis(gridRun);
    aStudy.add(gridRunAnalysis);

    // create the wizard
    StudyItem studyItem(aStudy);
    DesignOfExperimentExportWizard wizard(&studyItem);
    wizard.show();

    // Only 2 evaluated DOEs should be visible
    QVERIFY2(wizard.doeComboBox_->count() == 2, "Only 2 evaluated DOEs should appear in the combobox");

    // Verify we can select each one and get the right analysis
    wizard.doeComboBox_->setCurrentIndex(0);
    QVERIFY2(wizard.getAnalysis().getName() == "mcRun", "First evaluated DOE must be named: mcRun");

    // Check MC type
    {
      const Parameters params = wizard.getAnalysis().getImplementation().get()->getParameters();
      QVERIFY2(params.get("Design type") == "Monte Carlo", "MC DOE must have design type 'Monte Carlo'");
    }

    wizard.doeComboBox_->setCurrentIndex(1);
    QVERIFY2(wizard.getAnalysis().getName() == "gridRun", "Second evaluated DOE must be named: gridRun");

    // Check Grid type
    {
      const Parameters params = wizard.getAnalysis().getImplementation().get()->getParameters();
      QVERIFY2(params.get("Design type") == "Grid", "Grid DOE must have design type 'Grid'");
    }
  }
};
}

QTEST_MAIN(PERSALYS::TestDesignOfExperimentExportWizard)
#include "t_DesignOfExperimentExportWizard_std.moc"
