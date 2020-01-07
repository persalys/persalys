#include "persalys/GridDesignOfExperiment.hxx"
#include "persalys/ImportedDesignOfExperiment.hxx"
#include "persalys/ProbabilisticDesignOfExperiment.hxx"
#include "persalys/DesignOfExperimentWizard.hxx"
#include "persalys/SymbolicPhysicalModel.hxx"
#include "persalys/SampleTableModel.hxx"

#include <openturns/OTtypes.hxx>
#include <openturns/Normal.hxx>

#include <QtTest/QtTest>
#include <QAbstractButton>

using namespace OT;

namespace PERSALYS
{
class TestDesignOfExperimentWizard : public QObject
{
  Q_OBJECT

public:
  TestDesignOfExperimentWizard()
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
  void TestDefaultDeterministic()
  {
    // create the analysis
    GridDesignOfExperiment doe("analysis", model_);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == DesignOfExperimentIntroPage::Deterministic, "Checked button must be Deterministic");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.gridPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    ExperimentTableModel * doeModel = wizard.gridPage_->findChild<ExperimentTableModel*>();
    QVERIFY2(doeModel->data(doeModel->index(0, 0), Qt::CheckStateRole).toInt() == Qt::Unchecked, "Header must be unchecked");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==doe.getParameters();
    QVERIFY2(analysisEquality, "The two GridDesignOfExperiment must be equal");
  }


  void TestGridDefinitionTable()
  {
    // create the analysis
    const Interval bounds(GridDesignOfExperiment::GetDefaultBounds(model_));
    Collection<Point> values;
    Point val1(2);
    val1[0] = bounds.getLowerBound()[0] + 100;
    val1[1] = bounds.getUpperBound()[0] + 100;
    values.add(val1);
    Point val2(3);
    val2[0] = bounds.getLowerBound()[1] + 100;
    val2[2] = bounds.getUpperBound()[1] + 300;
    val2[1] = (val2[2] + val2[0])*0.5;
    values.add(val2);
    values.add(Point(1, 4500));
    GridDesignOfExperiment doe("analysis", model_, values);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    ExperimentTableModel * doeModel = wizard.gridPage_->findChild<ExperimentTableModel*>();
    TemporaryLabel * errorMessageLabel = wizard.gridPage_->findChild<TemporaryLabel*>();
    QLabel * label = wizard.gridPage_->findChild<QLabel*>("DOESizeLabel");

    // checks second page

    wizard.next();
    QVERIFY2(wizard.currentId() == 1, "Current page ID must be 1");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
    QVERIFY2(label->text() == "6", "wrong DOE size Label");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    // check table behavior
    QVERIFY2(doeModel->data(doeModel->index(0, 0), Qt::CheckStateRole) == Qt::Unchecked, "Header must be unchecked");
    for (int i = 1; i < 3; ++i)
      QVERIFY2(doeModel->data(doeModel->index(i, 0), Qt::CheckStateRole)== Qt::Checked, "Rows must be checked");
    QVERIFY2(doeModel->data(doeModel->index(3, 0), Qt::CheckStateRole) == Qt::Unchecked, "Last row must be unchecked");
    QVERIFY2(doeModel->getDesignOfExperiment().getOriginalInputSample().getSize() == 6, "Wrong points number");
    QVERIFY2(doeModel->data(doeModel->index(1, 0)) == "Q", "wrong name");
    QVERIFY2(doeModel->data(doeModel->index(1, 1)) == "Primary energy", "wrong description");
    QVERIFY2(doeModel->data(doeModel->index(1, 2)) == "10200", "wrong value");
    QVERIFY2(doeModel->data(doeModel->index(1, 3)) == QString::number(values[0][0], 'g', 12), "wrong lower bound value");
    QVERIFY2(doeModel->data(doeModel->index(1, 4)) == QString::number(values[0][1], 'g', 12), "wrong upper bound value");
    QVERIFY2(doeModel->data(doeModel->index(1, 6)) == "2", "wrong level");
    QVERIFY2(doeModel->data(doeModel->index(3, 2)) == "4500", "wrong value");
    QVERIFY2(doeModel->data(doeModel->index(3, 3)) == QString::number(bounds.getLowerBound()[2], 'g', 12), "wrong lower bound value");
    QVERIFY2(doeModel->data(doeModel->index(3, 4)) == QString::number(bounds.getUpperBound()[2], 'g', 12), "wrong upper bound value");
    QVERIFY2(doeModel->data(doeModel->index(3, 6)) == "1", "wrong level");

    // check line
    doeModel->setData(doeModel->index(0, 0), Qt::Checked, Qt::CheckStateRole);
    for (int i = 0; i < doeModel->rowCount(); ++i)
      QVERIFY2(doeModel->data(doeModel->index(i, 0), Qt::CheckStateRole) == Qt::Checked, "Rows must be checked");
    QVERIFY2(doeModel->data(doeModel->index(3, 6)) == "2", "wrong level");
    QVERIFY2(doeModel->getDesignOfExperiment().getOriginalInputSample().getSize() == 12, "Wrong points number");
    QVERIFY2(label->text() == "12", "wrong DOE size Label");

    // change bounds
    doeModel->setData(doeModel->index(3, 3), bounds.getUpperBound()[2], Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(3, 0), Qt::ForegroundRole) == QColor(Qt::red), "wrong color");
    QVERIFY2(!doeModel->data(doeModel->index(3, 0), Qt::ToolTipRole).toString().isEmpty(), "wrong tooltip");

    doeModel->setData(doeModel->index(3, 4), bounds.getLowerBound()[2], Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(3, 0), Qt::ForegroundRole) == QColor(Qt::red), "wrong color");
    QVERIFY2(!doeModel->data(doeModel->index(3, 0), Qt::ToolTipRole).toString().isEmpty(), "wrong tooltip");

    doeModel->setData(doeModel->index(3, 5), "Deltas =", Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(3, 6)) == "-", "wrong delta");

    doeModel->setData(doeModel->index(3, 4), bounds.getUpperBound()[2]*2, Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(3, 6)) == QString::number(bounds.getUpperBound()[2], 'g', 12), "wrong delta");
    QVERIFY2(doeModel->data(doeModel->index(3, 0), Qt::ForegroundRole) != QColor(Qt::red), "wrong color");
    QVERIFY2(doeModel->data(doeModel->index(3, 0), Qt::ToolTipRole).toString().isEmpty(), "wrong tooltip");

    // change setting
    doeModel->setData(doeModel->index(1, 5), "Deltas =", Qt::EditRole);
    const Scalar delta = values[0][1] - values[0][0];
    QVERIFY2(doeModel->data(doeModel->index(1, 6)) == QString::number(delta, 'g', 12), "wrong delta");
    QVERIFY2(label->text() == "12", "wrong DOE size Label");

    doeModel->setData(doeModel->index(2, 5), "Deltas =", Qt::EditRole);
    const Scalar delta2 = values[1][1] - values[1][0];
    QVERIFY2(doeModel->data(doeModel->index(2, 6)) == QString::number(delta2, 'g', 12), "wrong delta");

    // change Delta
    doeModel->setData(doeModel->index(1, 6), delta*2, Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(1, 6)) == QString::number(delta, 'g', 12), "wrong delta");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");
    QVERIFY2(label->text() == "12", "wrong DOE size Label");

    doeModel->setData(doeModel->index(1, 6), delta*0.5, Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(1, 6)) == QString::number(delta*0.5, 'g', 12), "wrong delta");
    doeModel->setData(doeModel->index(1, 5), "Levels =", Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(1, 6)) == "3", "wrong level");
    QVERIFY2(label->text() == "18", "wrong DOE size Label");

    // change Levels
    doeModel->setData(doeModel->index(1, 6), 2, Qt::EditRole);
    QVERIFY2(label->text() == "12", "wrong DOE size Label");

    // simulate Edit values
    Point editValue(3);
    editValue[0] = 4502;
    editValue[1] = 4660;
    editValue[2] = 5020;
    doeModel->setData(doeModel->index(3, 7), QVariant::fromValue(editValue), Qt::UserRole);
    QVERIFY2(doeModel->data(doeModel->index(3, 3)) == QString::number(4502, 'g', 12), "wrong lower bound value");
    QVERIFY2(doeModel->data(doeModel->index(3, 4)) == QString::number(5020, 'g', 12), "wrong upper bound value");
    doeModel->setData(doeModel->index(3, 5), "Deltas =", Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(3, 6)) == "-", "wrong delta");
    doeModel->setData(doeModel->index(3, 5), "Levels =", Qt::EditRole);
    QVERIFY2(doeModel->data(doeModel->index(3, 6)) == "3", "wrong level");

    // redo same DOE
    doeModel->setData(doeModel->index(3, 0), Qt::Unchecked, Qt::CheckStateRole);
    QVERIFY2(doeModel->data(doeModel->index(0, 0), Qt::CheckStateRole) == Qt::Unchecked, "Header must be unchecked");
    doeModel->setData(doeModel->index(3, 2), 4600, Qt::EditRole);
    QVERIFY2(doeModel->getDesignOfExperiment().getValues()[2] == Point(1, 4600), "wrong values");
    doeModel->setData(doeModel->index(3, 2), 4500, Qt::EditRole);

    bool analysisEquality = wizard.getAnalysis().getParameters()==doe.getParameters();
    QVERIFY2(analysisEquality, "The two GridDesignOfExperiment must be equal");
  }


  void TestProbabilistic()
  {
    // create the analysis
    ProbabilisticDesignOfExperiment doe("analysis", model_);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == DesignOfExperimentIntroPage::Probabilistic, "Checked button must be Probabilistic");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    QVERIFY2(wizard.currentId() == 2, "Current page ID must be 2");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");

    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==doe.getParameters();
    QVERIFY2(analysisEquality, "The two ProbabilisticDesignOfExperiment must be equal");
  }


  void TestImportEmpty()
  {
    // create the analysis
    ImportedDesignOfExperiment doe("analysis", model_);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == DesignOfExperimentIntroPage::Import, "Checked button must be Import");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    TemporaryLabel * errorMessageLabel = wizard.importPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 3, "Current page ID must be 3");
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");
    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    bool analysisEquality = wizard.getAnalysis().getParameters()==doe.getParameters();
    QVERIFY2(analysisEquality, "The two ImportedDesignOfExperiment must be equal");
  }


  void TestImport()
  {
    // create the analysis
    const String filename = "normal.csv";
    Normal(5).getSample(10).exportToCSVFile(filename);
    Indices ind(3);
    ind[0] = 4;
    ind[1] = 1;
    ind[2] = 3;
    ImportedDesignOfExperiment doe("analysis", model_, filename, ind);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    QVERIFY2(buttonGroup->checkedId() == DesignOfExperimentIntroPage::Import, "Checked button must be Import");
    QVERIFY2(wizard.currentId() == 0, "Current page ID must be 0");

    // - second page
    wizard.next();
    QLineEdit * fileLineEdit = wizard.importPage_->findChild<QLineEdit*>();
    TemporaryLabel * errorMessageLabel = wizard.importPage_->findChild<TemporaryLabel*>();
    QVERIFY2(wizard.currentId() == 3, "Current page ID must be 3");
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");
    QVERIFY2(fileLineEdit->text() == filename.c_str(), "wrong filename");
    QVERIFY2(wizard.nextId() == -1, "Next page ID must be -1");

    Description sampleDescription(5);
    sampleDescription[1] = "E";
    sampleDescription[3] = "C";
    sampleDescription[4] = "Q";

    SampleTableModel * model = wizard.importPage_->findChild<SampleTableModel*>();
    Description tableDescription;
    for (int i = 0; i < model->columnCount(); ++i)
      tableDescription.add(model->headerData(i, Qt::Horizontal).toString().toStdString());
    QVERIFY2(sampleDescription == tableDescription, "Wrong table description");

    model->setHeaderData(0, Qt::Horizontal, "E", Qt::DisplayRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    model->setHeaderData(0, Qt::Horizontal, "", Qt::DisplayRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    model->setHeaderData(1, Qt::Horizontal, "", Qt::DisplayRole);
    QVERIFY2(!wizard.validateCurrentPage(), "Page must be not valid");
    QVERIFY2(!errorMessageLabel->text().isEmpty(), "Label must be not empty");

    model->setHeaderData(1, Qt::Horizontal, "E", Qt::DisplayRole);
    QVERIFY2(wizard.validateCurrentPage(), "Page must be valid");
    QVERIFY2(errorMessageLabel->text().isEmpty(), "Label must be empty");

    bool analysisEquality = wizard.getAnalysis().getParameters()==doe.getParameters();
    QVERIFY2(analysisEquality, "The two ImportedDesignOfExperiment must be equal");

    remove("normal.csv");
  }


  void TestAnalysisModification()
  {
    // create the analysis
    GridDesignOfExperiment doe("analysis", model_);

    // create the wizard
    DesignOfExperimentWizard wizard(doe);
    wizard.show();

    // checks

    // - first page
    QButtonGroup * buttonGroup = wizard.introPage_->findChild<QButtonGroup*>();
    buttonGroup->button(DesignOfExperimentIntroPage::Probabilistic)->click();
    QVERIFY2(wizard.nextId() == 2, "Next page ID must be 2");
    bool analysisEquality = wizard.getAnalysis().getParameters()==ProbabilisticDesignOfExperiment("analysis", model_).getParameters();
    QVERIFY2(analysisEquality, "The two ProbabilisticDesignOfExperiment must be equal");

    buttonGroup->button(DesignOfExperimentIntroPage::Import)->click();
    QVERIFY2(wizard.nextId() == 3, "Next page ID must be 3");
    analysisEquality = wizard.getAnalysis().getParameters()==ImportedDesignOfExperiment("analysis", model_).getParameters();
    QVERIFY2(analysisEquality, "The two ImportedDesignOfExperiment must be equal");
  }
};
}

QTEST_MAIN(PERSALYS::TestDesignOfExperimentWizard)
#include "t_DesignOfExperimentWizard_std.moc"
