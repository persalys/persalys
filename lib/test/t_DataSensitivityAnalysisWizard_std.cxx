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


  void TestTargetHSICPageNavigation() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();

    // Enable Target HSIC only
    auto * targetCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::TargetHSIC));
    QVERIFY2(targetCB != nullptr, "Target HSIC button should exist");
    targetCB->setChecked(true);

    // Next from intro should go to Critical Domain page first
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "Next page should be Critical Domain when Target HSIC is checked");

    // Navigate to Critical Domain page
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "Current page should be Critical Domain page");

    // Validate Critical Domain page
    QVERIFY2(wizard.validateCurrentPage(), "Critical Domain page must be valid");

    // Next from Critical Domain should go to Target HSIC parameters page
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::TargetHSICParameters,
             "Next page should be Target HSIC parameters from Critical Domain");

    // Navigate to Target HSIC page
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::TargetHSICParameters,
             "Current page should be Target HSIC parameters page");

    // Validate Target HSIC parameters page
    QVERIFY2(wizard.validateCurrentPage(), "Target HSIC parameters page must be valid");

    // Next from Target HSIC page should be -1 (finish, no Conditional)
    QVERIFY2(wizard.nextId() == -1, "Next page after Target HSIC parameters should be -1");
  }


  void TestConditionalHSICPageNavigation() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();

    // Enable Conditional HSIC only
    auto * condCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::ConditionalHSIC));
    QVERIFY2(condCB != nullptr, "Conditional HSIC button should exist");
    condCB->setChecked(true);

    // Next from intro should go to Critical Domain page first
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "Next page should be Critical Domain when Conditional HSIC is checked");

    // Navigate to Critical Domain page
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "Current page should be Critical Domain page");

    // Validate Critical Domain page
    QVERIFY2(wizard.validateCurrentPage(), "Critical Domain page must be valid");

    // Next from Critical Domain should go to Conditional HSIC parameters page
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::ConditionalHSICParameters,
             "Next page should be Conditional HSIC parameters from Critical Domain");

    // Navigate to Conditional HSIC page
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::ConditionalHSICParameters,
             "Current page should be Conditional HSIC parameters page");

    // Validate Conditional HSIC parameters page
    QVERIFY2(wizard.validateCurrentPage(), "Conditional HSIC parameters page must be valid");

    // Next from Conditional HSIC page should always be -1 (it's the last)
    QVERIFY2(wizard.nextId() == -1, "Next page after Conditional HSIC parameters should be -1");
  }


  void TestMultiHSICPageNavigation() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();

    // Enable all three HSIC types
    auto * globalCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::GlobalHSIC));
    auto * targetCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::TargetHSIC));
    auto * condCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::ConditionalHSIC));

    globalCB->setChecked(true);
    targetCB->setChecked(true);
    condCB->setChecked(true);

    // From intro, should go to Global HSIC first
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::GlobalHSICParameters,
             "First HSIC page should be Global");

    // Navigate: Intro -> Global HSIC
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::GlobalHSICParameters,
             "Should be on Global HSIC page");

    // From Global, should go to Critical Domain
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "From Global HSIC, next should be Critical Domain");

    // Navigate: Global HSIC -> Critical Domain
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "Should be on Critical Domain page");

    // From Critical Domain, should go to Target HSIC
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::TargetHSICParameters,
             "From Critical Domain, next should be Target HSIC");

    // Navigate: Critical Domain -> Target HSIC
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::TargetHSICParameters,
             "Should be on Target HSIC page");

    // From Target, should go to Conditional
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::ConditionalHSICParameters,
             "From Target HSIC, next should be Conditional HSIC");

    // Navigate: Target HSIC -> Conditional HSIC
    wizard.next();
    QVERIFY2(wizard.currentId() == DataSensitivityAnalysisWizard::Page::ConditionalHSICParameters,
             "Should be on Conditional HSIC page");

    // From Conditional, should finish
    QVERIFY2(wizard.nextId() == -1, "From Conditional HSIC, should finish");
  }


  void TestTargetHSICParametersPage() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // Enable Target HSIC and navigate to its page
    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    auto * targetCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::TargetHSIC));
    targetCB->setChecked(true);
    wizard.next();

    // Check widgets exist on Target HSIC page
    const auto * asymCB = wizard.targetHSICParametersPage_->findChild<QCheckBox*>();
    QVERIFY2(asymCB != nullptr, "Should find checkboxes on Target HSIC page");

    const auto * comboBox = wizard.targetHSICParametersPage_->findChild<QComboBox*>();
    QVERIFY2(comboBox != nullptr, "Should find U/V statistic combo box on Target HSIC page");

    // Should have covariance models table
    const auto * tableView = wizard.targetHSICParametersPage_->findChild<QTableView*>();
    QVERIFY2(tableView != nullptr, "Should find table view on Target HSIC page");
  }


  void TestConditionalHSICParametersPage() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // Enable Conditional HSIC and navigate to its page
    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    auto * condCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::ConditionalHSIC));
    condCB->setChecked(true);
    wizard.next();

    // Should have permutation p-values checkbox
    const auto * permCB = wizard.conditionalHSICParametersPage_->findChild<QCheckBox*>();
    QVERIFY2(permCB != nullptr, "Should find permutation p-values checkbox on Conditional HSIC page");

    // Should have covariance models table
    const auto * tableView = wizard.conditionalHSICParametersPage_->findChild<QTableView*>();
    QVERIFY2(tableView != nullptr, "Should find table view on Conditional HSIC page");
  }


  void TestGetAnalysisWithTargetHSIC() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // Enable Target HSIC
    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    auto * targetCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::TargetHSIC));
    targetCB->setChecked(true);
    wizard.next();

    // Get the analysis from wizard
    Analysis result = wizard.getAnalysis();
    auto params = result.getParameters();
    QVERIFY2(params.getSize() > 0, "Analysis with Target HSIC should have parameters");

    // Verify the analysis parameters contain Target HSIC
    bool foundTargetHSIC = false;
    for (UnsignedInteger i = 0; i < params.getSize(); ++i)
    {
      auto [name, value] = params[i];
      if (value.find("Target HSIC") != std::string::npos)
      {
        foundTargetHSIC = true;
        break;
      }
    }
    QVERIFY2(foundTargetHSIC, "Analysis parameters should mention Target HSIC");
  }


  void TestGetAnalysisWithConditionalHSIC() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    // Enable Conditional HSIC
    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    auto * condCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::ConditionalHSIC));
    condCB->setChecked(true);
    wizard.next();

    // Get the analysis from wizard
    Analysis result = wizard.getAnalysis();
    auto params = result.getParameters();
    QVERIFY2(params.getSize() > 0, "Analysis with Conditional HSIC should have parameters");

    // Verify the analysis parameters contain Conditional HSIC
    bool foundConditionalHSIC = false;
    for (UnsignedInteger i = 0; i < params.getSize(); ++i)
    {
      auto [name, value] = params[i];
      if (value.find("Conditional HSIC") != std::string::npos)
      {
        foundConditionalHSIC = true;
        break;
      }
    }
    QVERIFY2(foundConditionalHSIC, "Analysis parameters should mention Conditional HSIC");
  }


  void TestToggleTargetAndConditionalHSIC() const
  {
    DataSensitivityAnalysis analysis("analysis", model_);
    DataSensitivityAnalysisWizard wizard(analysis);
    wizard.show();

    const auto * methodGroup = wizard.introPage_->findChild<QButtonGroup*>();
    auto * targetCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::TargetHSIC));
    auto * condCB = qobject_cast<QCheckBox*>(methodGroup->button(DataSensitivityAnalysisResult::ConditionalHSIC));

    // Initially both off
    QVERIFY2(!targetCB->isChecked(), "Target HSIC should not be checked initially");
    QVERIFY2(!condCB->isChecked(), "Conditional HSIC should not be checked initially");
    QVERIFY2(wizard.nextId() == -1, "Should finish when no HSIC is checked");

    // Turn Target on only
    targetCB->setChecked(true);
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "Should navigate to Critical Domain when only Target is enabled");

    // Turn Conditional on too
    condCB->setChecked(true);
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "Should still navigate to Critical Domain when both are enabled");

    // Turn Target off, only Conditional remains
    targetCB->setChecked(false);
    QVERIFY2(wizard.nextId() == DataSensitivityAnalysisWizard::Page::CriticalDomain,
             "Should navigate to Critical Domain when only Conditional is enabled");

    // Turn Conditional off
    condCB->setChecked(false);
    QVERIFY2(wizard.nextId() == -1, "Should finish when no HSIC is checked");
  }
};
}

QTEST_MAIN(PERSALYS::TestDataSensitivityAnalysisWizard)
#include "t_DataSensitivityAnalysisWizard_std.moc"
