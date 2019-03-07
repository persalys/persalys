#include "otgui/MeshDefinitionWizard.hxx"
#include "otgui/ImportedMeshModel.hxx"
#include "otgui/GridMeshModel.hxx"

#include <openturns/OTtypes.hxx>

#include <QtTest/QtTest>
#include <QAbstractButton>

namespace OTGUI
{
class TestMeshDefinitionWizard : public QObject
{
  Q_OBJECT

public:
  TestMeshDefinitionWizard()
  {
  }

private slots:
  void TestGridMeshModel()
  {
    // create the mesh
    GridMeshModel meshModel(OT::Interval(0., 12.), OT::Indices(1, 5));

    // create the wizard
    MeshDefinitionWizard wizard(meshModel);
    wizard.show();

    // checks
    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");
    QVERIFY2(wizard.methodGroup_->checkedId() == MeshDefinitionWizard::Grid, "Checked button must be Grid");
    QVERIFY2(!wizard.dataPreviewTableView_->isEnabled(), "Table view must be not enabled");
    QVERIFY2(static_cast<QWidget*>(wizard.tableModel_->parent())->isEnabled(), "Table view must be enabled");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(wizard.errorMessageLabel_->text().isEmpty(), "Label must be empty");

    bool analysisEquality = wizard.getMesh().getPythonScript()==meshModel.getPythonScript();
    QVERIFY2(analysisEquality, "The two GridMeshModel must be equal");

    wizard.tableModel_->setData(wizard.tableModel_->index(0, 2), 14., Qt::EditRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!wizard.errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    wizard.tableModel_->setData(wizard.tableModel_->index(0, 2), 0., Qt::EditRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(wizard.errorMessageLabel_->text().isEmpty(), "Label must be empty");

    wizard.tableModel_->setData(wizard.tableModel_->index(0, 4), 1, Qt::EditRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!wizard.errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    wizard.tableModel_->setData(wizard.tableModel_->index(0, 4), 5, Qt::EditRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(wizard.errorMessageLabel_->text().isEmpty(), "Label must be empty");

    analysisEquality = wizard.getMesh().getPythonScript()==meshModel.getPythonScript();
    QVERIFY2(analysisEquality, "The two GridMeshModel must be equal");
  }


  void TestImportedMeshModel()
  {
    QTemporaryFile file;
    QVERIFY2(file.open(), "Can not open the file");

    OT::Sample(2, OT::Point(1, 2.5)).exportToCSVFile(file.fileName().toStdString());

    // create the mesh
    ImportedMeshModel meshModel(file.fileName().toStdString());

    // create the wizard
    MeshDefinitionWizard wizard(meshModel);
    wizard.show();

    // checks
    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");
    QVERIFY2(!static_cast<QWidget*>(wizard.tableModel_->parent())->isEnabled(), "Table view must be not enabled");
    QVERIFY2(wizard.dataPreviewTableView_->isEnabled(), "Table view must be enabled");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(wizard.errorMessageLabel_->text().isEmpty(), "Label must be empty");

    OT::Sample(1, OT::Point(1, 2.5)).exportToCSVFile(file.fileName().toStdString());
    wizard.setData(file.fileName());
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!wizard.errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    OT::Sample(2, OT::Point(2, 2.5)).exportToCSVFile(file.fileName().toStdString());
    wizard.setData(file.fileName());
    wizard.dataPreviewTableView_->model()->setHeaderData(0, Qt::Horizontal, "", Qt::DisplayRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!wizard.errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    wizard.dataPreviewTableView_->model()->setHeaderData(1, Qt::Horizontal, "t", Qt::DisplayRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(wizard.errorMessageLabel_->text().isEmpty(), "Label must be empty");

    wizard.dataPreviewTableView_->model()->setHeaderData(0, Qt::Horizontal, "t", Qt::DisplayRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!wizard.errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    wizard.dataPreviewTableView_->model()->setHeaderData(1, Qt::Horizontal, "", Qt::DisplayRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(wizard.errorMessageLabel_->text().isEmpty(), "Label must be empty");

    bool analysisEquality = wizard.getMesh().getPythonScript()==meshModel.getPythonScript();
    QVERIFY2(analysisEquality, "The two ImportedMeshModel must be equal");
  }


  void TestMeshModification()
  {
    // create the mesh
    GridMeshModel meshModel(OT::Interval(0., 12.), OT::Indices(1, 5));

    // create the wizard
    MeshDefinitionWizard wizard(meshModel);
    wizard.show();

    // checks
    wizard.methodGroup_->button(MeshDefinitionWizard::Import)->click();
    QVERIFY2(wizard.dataPreviewTableView_->isEnabled(), "Table view must be enabled");

    QTemporaryFile file;
    QVERIFY2(file.open(), "Can not open the file");
    wizard.setData(file.fileName());
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!wizard.errorMessageLabel_->text().isEmpty(), "Label must be not empty");

    OT::Sample(2, OT::Point(1, 2.5)).exportToCSVFile(file.fileName().toStdString());
    wizard.setData(file.fileName());
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(wizard.errorMessageLabel_->text().isEmpty(), "Label must be empty");

    bool analysisEquality = wizard.getMesh().getPythonScript()==ImportedMeshModel(file.fileName().toStdString()).getPythonScript();
    QVERIFY2(analysisEquality, "The two ImportedMeshModel must be equal");
  }
};
}

QTEST_MAIN(OTGUI::TestMeshDefinitionWizard)
#include "t_MeshDefinitionWizard_std.moc"
