#include "persalys/DataSensitivityAnalysis.hxx"
#include "persalys/DataSensitivityAnalysisWizard.hxx"
#include "persalys/DataModel.hxx"
#include "persalys/ErrorWidget.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>
#include <openturns/RandomGenerator.hxx>
#include <openturns/SquaredExponential.hxx>

#include <QtTest/QtTest>
#include <QCheckBox>
#include <QTableView>

using namespace OT;

namespace PERSALYS
{
class TestDataSensitivityAnalysisWizard : public QObject
{
  Q_OBJECT

public:
  TestDataSensitivityAnalysisWizard()
  {
    // Create sample data: 3 inputs, 1 output
    RandomGenerator::SetSeed(0);
    Sample inSample = Normal(3).getSample(100);
    inSample.setDescription({"X0", "X1", "X2"});

    Sample outSample(inSample.getSize(), 1);
    for (UnsignedInteger i = 0; i < inSample.getSize(); ++i)
      outSample(i, 0) = inSample(i, 0) + inSample(i, 1) * inSample(i, 1) + inSample(i, 2);
    outSample.setDescription({"Y0"});

    model_ = DataModel("model", inSample, outSample);
    model_.setType(DataModel::MC);
  }

private:
  DataModel model_;

private slots:
  void TestDefaultWizard() const
  {
    // Create analysis with defaults (RankSobol | SRC)
    DataSensitivityAnalysis analysis("analysis", model_);

    // Create wizard
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // Check intro page
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::Intro, "Should start on intro page");

    // Validate page - should be valid with default settings
    QVERIFY2(wizard.validateCurrentPage(), "Intro page must be valid with defaults");

    // With no HSIC checked, next should be -1 (finish)
    QVERIFY2(wizard.nextId() == -1, "Next page should be -1 when HSIC is not checked");

    // Verify getAnalysis returns valid analysis
    Analysis result = wizard.getAnalysis();
    QVERIFY2(result.getParameters().getSize() > 0, "Analysis should have parameters");
  }


  void TestOutputsSelection() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    const auto * outputsGroupBox = wizard.introPage_->findChild<OutputsSelectionGroupBox*>();
    const auto * errorWidget = wizard.introPage_->findChild<ErrorWidget*>();
    auto * comboBox = outputsGroupBox->findChild<TitledComboBox*>();
    const auto * listWidget = outputsGroupBox->findChild<ListWidgetWithCheckBox*>();

    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid initially");
    QVERIFY2(errorWidget->toPlainText().isEmpty(), "Error widget must be empty initially");

    // Deselect all outputs
    QTest::mouseClick(comboBox, Qt::LeftButton);
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must not be valid without outputs");
    QVERIFY2(!errorWidget->toPlainText().isEmpty(), "Error widget must show message");

    // Re-select outputs
    QTest::mouseClick(listWidget->viewport(), Qt::LeftButton);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid after re-selecting outputs");
    QVERIFY2(errorWidget->toPlainText().isEmpty(), "Error widget must be empty after fix");
  }


  void TestMethodDeselection() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    const auto * errorWidget = wizard.introPage_->findChild<ErrorWidget*>();

    // Get method checkboxes
    auto buttons = methodGroup->buttons();
    //QVERIFY2(buttons.size() == 3, "Should have 3 method checkboxes");

    // Deselect all methods
    for (auto * btn : buttons)
    {
      auto * cb = qobject_cast<QCheckBox*>(btn);
      if (cb && cb->isChecked())
        cb->setChecked(false);
    }
    QVERIFY2(!wizard.validateCurrentPage(), "Page must not be valid without any method");
    QVERIFY2(!errorWidget->toPlainText().isEmpty(), "Error widget must show message about methods");
  }


  void TestHSICPageNavigation() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();

    // Find HSIC checkbox and enable it
    auto * hsicButton = methodGroup->button(DataSensitivityAnalysisResult::GlobalHSIC);
    QVERIFY2(hsicButton != nullptr, "HSIC button should exist");

    auto * hsicCB = qobject_cast<QCheckBox*>(hsicButton);
    hsicCB->setChecked(true);

    // Next should now lead to HSIC parameters page
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::GlobalHSICParameters,
             "Next page should be HSIC parameters page when HSIC is checked");

    // Navigate to HSIC page
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::GlobalHSICParameters,
             "Current page should be HSIC parameters page");

    // Validate HSIC parameters page
    QVERIFY2(wizard.validateCurrentPage(), "HSIC parameters page must be valid");

    // Next from HSIC page should be -1 (finish)
    QVERIFY2(wizard.nextId() == -1, "Next page after HSIC parameters should be -1");
  }


  void TestHSICParametersPage() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // Enable HSIC and navigate to its page
    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    auto * hsicCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::GlobalHSIC));
    hsicCB->setChecked(true);
    wizard.next();

    // Check widgets exist on HSIC page
    const auto * asymCB = wizard.globalHSICParametersPage_->findChild<QCheckBox*>();
    QVERIFY2(asymCB != nullptr, "Should find checkboxes on HSIC page");

    const  auto * comboBox = wizard.globalHSICParametersPage_->findChild<QComboBox*>();
    QVERIFY2(comboBox != nullptr, "Should find U/V statistic combo box");

    const auto * tableView = wizard.globalHSICParametersPage_->findChild<QTableView*>();
    QVERIFY2(tableView != nullptr, "Should find covariance models table");

    // Verify covariance table has correct number of rows (3 inputs + 1 output = 4)
    QVERIFY2(tableView->model()->rowCount() == 4, "Covariance table should have 4 rows");
  }


  void TestGetAnalysisWithHSIC() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // Enable HSIC
    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    auto * hsicCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::GlobalHSIC));
    hsicCB->setChecked(true);
    wizard.next();

    // Get the analysis from wizard
    Analysis result = wizard.getAnalysis();
    auto params = result.getParameters();
    QVERIFY2(params.getSize() > 0, "Analysis with HSIC should have parameters");

    // Verify the analysis parameters contain HSIC marker
    bool foundHSIC = false;
    for (UnsignedInteger i = 0; i < params.getSize(); ++i)
    {
      auto entry = params[i];
      if (entry.first.find("HSIC") != std::string::npos ||
          entry.second.find("HSIC") != std::string::npos)
      {
        foundHSIC = true;
        break;
      }
    }
    QVERIFY2(foundHSIC, "Analysis parameters should mention HSIC");
  }


  void TestToggleHSICOnOff() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    auto * hsicCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::GlobalHSIC));

    // Initially HSIC is off
    QVERIFY2(!hsicCB->isChecked(), "HSIC should not be checked initially");
    QVERIFY2(wizard.nextId() == -1, "Should finish when HSIC is off");

    // Turn HSIC on
    hsicCB->setChecked(true);
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::GlobalHSICParameters,
             "Should navigate to HSIC page when enabled");

    // Turn HSIC off again
    hsicCB->setChecked(false);
    QVERIFY2(wizard.nextId() == -1, "Should finish again when HSIC is off");
  }
};
}

QTEST_MAIN(PERSALYS::TestDataSensitivityAnalysisWizard)
#include "t_DataSensitivityAnalysisWizard_std.moc"
