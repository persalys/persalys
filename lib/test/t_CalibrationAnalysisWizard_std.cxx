#include "persalys/CalibrationAnalysisWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/Observations.hxx"
#include "persalys/CheckableHeaderView.hxx"
#include "persalys/QtTools.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>
#include <openturns/LogNormalMuSigma.hxx>
#include <openturns/Uniform.hxx>
#include <openturns/ParametricFunction.hxx>
#include <openturns/NormalCopula.hxx>
#include <openturns/JointDistribution.hxx>

#include <QtTest/QtTest>

using namespace OT;

namespace PERSALYS
{
class TestCalibrationAnalysisWizard : public QObject
{
  Q_OBJECT

public:
  TestCalibrationAnalysisWizard()
  {
    // create the model
    Input R("R", 750e6, LogNormalMuSigma(750e6, 11e6).getDistribution(), "Parameter R");
    Input C("C", 2750e6, Normal(2750e6, 250e6), "Parameter C");
    Input gamma("gam", 10., Normal(10, 2), "Parameter gamma");
    Input eps("epsilon", 0., Uniform(0, 0.07), "Strain");

    Output sigma("sigma", "stress (Pa)");
    Output sigma2("sigma2", "stress (Pa)");

    InputCollection inputCollection(4);
    inputCollection[0] = R;
    inputCollection[1] = C;
    inputCollection[2] = gamma;
    inputCollection[3] = eps;
    OutputCollection outputCollection(2);
    outputCollection[0] = sigma;
    outputCollection[1] = sigma2;

    Description formula(2, "R + C * (1 - exp(-gam * epsilon))");
    SymbolicPhysicalModel model("modelChaboche", inputCollection, outputCollection, formula);

    // create the observations
    const int sampleSize = 100;
    Sample strainSample(eps.getDistribution().getSample(sampleSize));
    strainSample.setDescription(Description(1, "epsilon"));
    Indices ind(3);
    ind.fill();
    Point values(3);
    values[0] = 750e6;
    values[1] = 2750e6;
    values[2] = 10.;
    Function f(model.getFunction("sigma"));
    Sample outputStress(ParametricFunction(f, ind, values)(strainSample));
    int stressObservationNoiseSigma = 40.e6; // (Pa)
    Normal noiseSigma(0., stressObservationNoiseSigma);
    Sample sampleNoise(noiseSigma.getSample(sampleSize));
    Sample sigmaSample(outputStress + sampleNoise);

    observations = Observations("observations", model, strainSample, sigmaSample);
  }

private:
  Observations observations;

private slots:
  void TestWizardInitialization()
  {
    // create the analysis
    CalibrationAnalysis analysis("analysis", observations);
    analysis.setMethodName("GaussianNonlinear");
    Description calibratedIn(2);
    calibratedIn[0] = "R";
    calibratedIn[1] = "C";
    JointDistribution::DistributionCollection coll(2, Normal(5., 0.2));
    CorrelationMatrix R(2);
    R(0, 0) = 0.2;
    analysis.setCalibratedInputs(calibratedIn, JointDistribution(coll, NormalCopula(R)), Description(1, "gam"), Point(1, 7.5));
    analysis.setConfidenceIntervalLength(0.99);
    analysis.setBootStrapSize(50);
    CorrelationMatrix R2(1);
    R2(0, 0) = 0.2;
    analysis.setErrorCovariance(R2);
    OptimizationAlgorithm optimAlgo(analysis.getOptimizationAlgorithm());
    optimAlgo.setMaximumEvaluationNumber(75);
    optimAlgo.setMaximumAbsoluteError(1e-4);
    analysis.setOptimizationAlgorithm(optimAlgo);

    // create the wizard
    CalibrationAnalysisWizard wizard(analysis);
    wizard.show();

    // checks
    QVERIFY2(wizard.getAnalysis().getParameters() == analysis.getParameters(), "The two CalibrationAnalysis must be equal");
  }


  void TestMethodModification()
  {
    // create the analysis
    CalibrationAnalysis analysis("analysis", observations);

    // create the wizard
    CalibrationAnalysisWizard wizard(analysis);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == CalibrationIntroPage::LinearLeastSquares, "Checked button must be LinearLeastSquares");
    bool analysisEquality = wizard.getAnalysis().getParameters() == analysis.getParameters();
    QVERIFY2(analysisEquality, "The two CalibrationAnalysis must be equal");

    wizard.next();
    // - next page
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.nextId() == CalibrationAnalysisWizard::Page_Param, "Next page ID must be Page_Param");

    buttonGroup->button(CalibrationIntroPage::NonlinearLeastSquares)->click();
    QVERIFY2(wizard.nextId() == CalibrationAnalysisWizard::Page_Param, "Next page ID must be Page_Param");
    analysis.setMethodName("LeastSquaresNonlinear");
    QVERIFY2(wizard.getAnalysis().getParameters() == analysis.getParameters(), "The two CalibrationAnalysis must be equal");

    buttonGroup->button(CalibrationIntroPage::LinearGaussian)->click();
    QVERIFY2(wizard.nextId() == CalibrationAnalysisWizard::Page_PriorDist, "Next page ID must be Page_PriorDist");
    analysis.setMethodName("GaussianLinear");
    QVERIFY2(wizard.getAnalysis().getParameters() == analysis.getParameters(), "The two CalibrationAnalysis must be equal");

    buttonGroup->button(CalibrationIntroPage::NonlinearGaussian)->click();
    QVERIFY2(wizard.nextId() == CalibrationAnalysisWizard::Page_PriorDist, "Next page ID must be Page_PriorDist");
    analysis.setMethodName("GaussianNonlinear");
    QVERIFY2(wizard.getAnalysis().getParameters() == analysis.getParameters(), "The two CalibrationAnalysis must be equal");
  }


  void TestObservationsModification()
  {
    Study aStudy;
    PhysicalModel aModel(observations.getPhysicalModel());
    aStudy.add(aModel);

    Sample outS(Normal(0, 0.1).getSample(10));
    outS.setDescription(Description(1, "sigma"));

    // - observations1
    Sample inS1(Normal(0, 0.1).getSample(10));
    inS1.setDescription(Description(1, "epsilon"));
    DesignOfExperiment observations1(Observations("obs1", aModel, inS1, outS));
    aStudy.add(observations1);

    QString text1(tr("Observed variables") + " : " );
    text1 += observations1.getSample().getDescription().__str__().c_str();
    text1 += "\n" + tr("Number of observations") + " : " + QString::number(observations1.getSample().getSize());

    // - observations2
    Sample outS2(outS);
    outS2.setDescription(Description(1, "sigma2"));
    Sample inS2(Normal(0, 0.1).getSample(10));
    inS2.setDescription(Description(1, "R"));
    DesignOfExperiment observations2(Observations("obs2", aModel, inS2, outS2));
    aStudy.add(observations2);

    QString text2(tr("Observed variables") + " : " );
    text2 += observations2.getSample().getDescription().__str__().c_str();
    text2 += "\n" + tr("Number of observations") + " : " + QString::number(observations2.getSample().getSize());

    // create an analysis
    CalibrationAnalysis analysis("analysis", observations1);
    aStudy.add(analysis);

    // create the wizard
    CalibrationAnalysisWizard wizard(analysis, true);
    wizard.show();

    // checks
    QComboBox * comboBox = wizard.introPage_->findChild<QComboBox*>();
    QLabel * label = wizard.introPage_->findChild<QLabel*>();
    QVERIFY2(comboBox->count() == 2, "The combobox must have two items");
    QVERIFY2(label->text() == text1, "Wrong observations label");
    QVERIFY2(wizard.introPage_->getObservations().getName() == "obs1", "The observations must be named : obs1");
    QVERIFY2(wizard.obsErrorDistPage_->getDistribution().getDescription()[0] == "sigma", "The observed output must be named : sigma");
    QVERIFY2(QtOT::DescriptionToStringList(wizard.priorDistPage_->getDistribution().getDescription()).join(",").toStdString() == "R,C,gam", "wrong calibrated input names");

    comboBox->setCurrentIndex(1);
    QVERIFY2(label->text() == text2, "Wrong observations label");
    QVERIFY2(wizard.introPage_->getObservations().getName() == "obs2", "The observations must be named : obs2");
    QVERIFY2(wizard.obsErrorDistPage_->getDistribution().getDescription()[0] == "sigma2", "The observed output must be named : sigma2");
    QVERIFY2(QtOT::DescriptionToStringList(wizard.priorDistPage_->getDistribution().getDescription()).join(",").toStdString() == "C,gam,epsilon", "wrong calibrated input names");

    QVERIFY2(wizard.nextId() == 1, "Next page ID must be 1");
  }


  void TestTableModification()
  {
    // create the analysis
    CalibrationAnalysis analysis("analysis", observations);

    // create the wizard
    CalibrationAnalysisWizard wizard(analysis);
    wizard.show();

    CheckableHeaderView * headerView = dynamic_cast<CheckableHeaderView*>(wizard.refPointPage_->tableView_->horizontalHeader());

    // checks
    wizard.next();

    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(wizard.refPointPage_->errorMessageLabel_->text().isEmpty(), "Label must be empty");

    QTest::mousePress(headerView->viewport(), Qt::LeftButton, Qt::NoModifier, headerView->viewport()->rect().bottomLeft() + QPoint(5, 2));
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!wizard.refPointPage_->errorMessageLabel_->text().isEmpty(), "Label must be not empty");
    QVERIFY2(QtOT::DescriptionToStringList(wizard.refPointPage_->getFixedValues().getDescription()).join(",").toStdString() == "R,C,gam", "wrong fixed input names");
    QVERIFY2(wizard.priorDistPage_->getDistribution() == Distribution(), "wrong calibrated input number");

    QAbstractItemModel * model = wizard.refPointPage_->tableView_->model();
    model->setData(model->index(0, 0), Qt::Checked, Qt::CheckStateRole);
    QVERIFY2(headerView->isChecked() == false, "Header must be not checked");
    QVERIFY2(QtOT::DescriptionToStringList(wizard.refPointPage_->getFixedValues().getDescription()).join(",").toStdString() == "C,gam", "wrong fixed input names");
    QVERIFY2(wizard.priorDistPage_->getDistribution().getDescription() == Description(1, "R"), "wrong calibrated input names");

    QTest::mousePress(headerView->viewport(), Qt::LeftButton, Qt::NoModifier, headerView->viewport()->rect().bottomLeft() + QPoint(5, 2));
    bool analysisEquality = wizard.getAnalysis().getParameters() == analysis.getParameters();
    QVERIFY2(analysisEquality, "The two CalibrationAnalysis must be equal");
  }
};
}

QTEST_MAIN(PERSALYS::TestCalibrationAnalysisWizard)
#include "t_CalibrationAnalysisWizard_std.moc"
