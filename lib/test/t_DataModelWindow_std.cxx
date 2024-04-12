#include "persalys/DataModelWindow.hxx"
#include "persalys/DataModel.hxx"
#include "persalys/DataModelDefinitionItem.hxx"
#include "persalys/StudyTreeViewModel.hxx"
#include "persalys/CheckableHeaderView.hxx"
#include "persalys/QtTools.hxx"

#include <openturns/OTDistribution.hxx>

#include <QtTest/QtTest>
#include <QTableView>

using namespace OT;

namespace PERSALYS
{
class TestDataModelWindow : public QObject
{
  Q_OBJECT

private:
  DataModelDefinitionItem * modelItem;
  DataModel * model;

private slots:
  void initTestCase()
  {
    Sample sample(Normal(5).getSample(20));
    sample.exportToCSVFile("DataModelSample.csv");
    Indices inCol(2);
    inCol[0] = 1;
    inCol[1] = 3;
    Indices outCol(2);
    outCol[0] = 2;
    outCol[1] = 0;
    Description inDesc(2);
    inDesc[0] = "in1";
    inDesc[1] = "in2";
    Description outDesc(2);
    outDesc[0] = "out1";
    outDesc[1] = "out2";
    model = new DataModel("model", "DataModelSample.csv", inCol, outCol, inDesc, outDesc);

    Study aStudy;
    aStudy.add(model);

    StudyTreeViewModel * treeModel = new StudyTreeViewModel;
    treeModel->appendItem(aStudy);

    // Find the ProbabilisticModelItem
    QModelIndexList listIndexes = treeModel->match(treeModel->index(0, 0), Qt::DisplayRole, "Definition", 1, Qt::MatchRecursive);
    modelItem = dynamic_cast<DataModelDefinitionItem*>(treeModel->itemFromIndex(listIndexes[0]));
  }


  void TestCreationFromFullModel()
  {
    // create the window
    DataModelWindow window(modelItem);
    window.show();

    // get widgets
    QTableView * variableTable = window.findChild<QTableView*>("variableTable");
    DataModelTableModel * variableModel = static_cast<DataModelTableModel*>(variableTable->model());
    CheckableHeaderView * headerView = dynamic_cast<CheckableHeaderView*>(variableTable->verticalHeader());
    QTableView * rowIDTable = window.findChild<QTableView*>("rowIDTable");
    QTableView * sampleTable = window.findChild<QTableView*>("sampleTable");

    // check
    QVERIFY2(variableModel->rowCount() == 2, "one line in the variable table");
    QVERIFY2(variableModel->columnCount() == 5, "5 columns in the variable table");
    QVERIFY2(rowIDTable->model()->columnCount() == 6, "6 columns in the rowIDTable table");
    QVERIFY2(sampleTable->model()->columnCount() == 6, "6 columns in the sample table");
    QVERIFY2(sampleTable->model()->rowCount() == (int)model->getSample().getSize(), "wrong line number in the sample table");
    QVERIFY2(headerView->isChecked() == false, "wrong header check state");
    for (int i = 0; i < variableModel->columnCount(); ++i)
    {
      if (model->getInputColumns().contains(i) || model->getOutputColumns().contains(i))
      {
        QVERIFY2(variableModel->data(variableModel->index(0, i), Qt::CheckStateRole) == Qt::Checked, "wrong check state in Table");
        if (model->getInputColumns().contains(i))
          QVERIFY2(variableModel->data(variableModel->index(1, i)) == "Input", "wrong variable type in Table");
        if (model->getOutputColumns().contains(i))
          QVERIFY2(variableModel->data(variableModel->index(1, i)) == "Output", "wrong variable type in Table");
      }
      else
      {
        QVERIFY2(variableModel->data(variableModel->index(0, i), Qt::CheckStateRole) == Qt::Unchecked, "wrong check state in Table");
        QVERIFY2(variableModel->data(variableModel->index(1, i)).toString().isEmpty() == true, "wrong variable type in Table");
      }
    }
    Indices col(model->getInputColumns());
    for (UnsignedInteger i = 0; i < col.getSize(); ++i)
    {
      QString name_i(model->getInputNames()[i].c_str());
      QVERIFY2(variableModel->data(variableModel->index(0, col[i])) == name_i, "wrong name in variable Table");
      QVERIFY2(sampleTable->model()->headerData(col[i] + 1, Qt::Horizontal) == name_i, "wrong name in sample Table");
    }
    col = model->getOutputColumns();
    for (UnsignedInteger i = 0; i < col.getSize(); ++i)
    {
      QString name_i(model->getOutputNames()[i].c_str());
      QVERIFY2(variableModel->data(variableModel->index(0, col[i])) == name_i, "wrong name in variable Table");
      QVERIFY2(sampleTable->model()->headerData(col[i] + 1, Qt::Horizontal) == name_i, "wrong name in sample Table");
    }

    // resize column
    const int sectionSize = sampleTable->horizontalHeader()->sectionSize(2);
    sampleTable->horizontalHeader()->resizeSection(2, sectionSize + 10);
    QVERIFY2(variableTable->horizontalHeader()->sectionSize(1) == sectionSize + 10, "wrong section size");
    variableTable->horizontalHeader()->resizeSection(2, sectionSize + 20);
    QVERIFY2(sampleTable->horizontalHeader()->sectionSize(3) == sectionSize + 20, "wrong section size");
  }


  void TestModificationFromWidgets()
  {
    // create the window
    DataModelWindow window(modelItem);
    window.show();

    // get widgets
    QTableView * variableTable = window.findChild<QTableView*>("variableTable");
    DataModelTableModel * variableModel = static_cast<DataModelTableModel*>(variableTable->model());
    CheckableHeaderView * headerView = dynamic_cast<CheckableHeaderView*>(variableTable->verticalHeader());

    // check

    // change number of variables
    QTest::mousePress(headerView->viewport(), Qt::LeftButton, Qt::NoModifier, headerView->viewport()->rect().topLeft() + QPoint(5, 2));
    QVERIFY2(model->getInputColumns().contains(4) == true, "wrong input columns");

    variableModel->setData(variableModel->index(0, 4), Qt::Unchecked, Qt::CheckStateRole);
    QVERIFY2(headerView->isChecked() == false, "wrong header check state");
    QVERIFY2(model->getInputColumns().contains(4) == false, "wrong input columns");

    variableModel->setData(variableModel->index(0, 2), Qt::Unchecked, Qt::CheckStateRole);
    QVERIFY2(model->getOutputColumns().contains(2) == false, "wrong output columns");

    variableModel->setData(variableModel->index(0, 2), Qt::Checked, Qt::CheckStateRole);
    QVERIFY2(model->getInputColumns().contains(2) == true, "wrong input columns");

    // change variable type
    variableModel->setData(variableModel->index(1, 2), "Output");
    QVERIFY2(model->getOutputColumns().contains(2) == true, "wrong output columns");
    QVERIFY2(model->getInputColumns().contains(2) == false, "wrong input columns");

    // change variable name
    variableModel->setData(variableModel->index(0, 1), "myVariable");
    QVERIFY2(QtOT::DescriptionToStringList(model->getInputNames()).contains("myVariable") == true, "wrong input names");
    variableModel->setData(variableModel->index(0, 2), "myVariable2");
    QVERIFY2(QtOT::DescriptionToStringList(model->getOutputNames()).contains("myVariable2") == true, "wrong output names");
  }


  void TestModificationFromModel()
  {
    // create the window
    DataModelWindow window(modelItem);
    window.show();

    // get widgets
    QTableView * variableTable = window.findChild<QTableView*>("variableTable");
    DataModelTableModel * variableModel = static_cast<DataModelTableModel*>(variableTable->model());
    QTableView * sampleTable = window.findChild<QTableView*>("sampleTable");

    // checks

    // change variables
    Indices inCol(2);
    inCol[0] = 2;
    inCol[1] = 0;
    Indices outCol(2);
    outCol[0] = 1;
    outCol[1] = 3;
    Description inDesc(2);
    inDesc[0] = "inp1";
    inDesc[1] = "inp2";
    Description outDesc(2);
    outDesc[0] = "outp1";
    outDesc[1] = "outp2";
    model->setColumns(inCol, inDesc, outCol, outDesc);
    for (int i = 0; i < variableModel->columnCount(); ++i)
    {
      if (inCol.contains(i) || outCol.contains(i))
      {
        QVERIFY2(variableModel->data(variableModel->index(0, i), Qt::CheckStateRole) == Qt::Checked, "wrong check state in Table");
        if (inCol.contains(i))
          QVERIFY2(variableModel->data(variableModel->index(1, i)) == "Input", "wrong variable type in Table");
        if (outCol.contains(i))
          QVERIFY2(variableModel->data(variableModel->index(1, i)) == "Output", "wrong variable type in Table");
      }
      else
      {
        QVERIFY2(variableModel->data(variableModel->index(0, i), Qt::CheckStateRole) == Qt::Unchecked, "wrong check state in Table");
        QVERIFY2(variableModel->data(variableModel->index(1, i)).toString().isEmpty() == true, "wrong variable type in Table");
      }
    }
    for (UnsignedInteger i = 0; i < inCol.getSize(); ++i)
    {
      QString name_i(inDesc[i].c_str());
      QVERIFY2(variableModel->data(variableModel->index(0, inCol[i])) == name_i, "wrong name in variable Table");
      QVERIFY2(sampleTable->model()->headerData(inCol[i] + 1, Qt::Horizontal) == name_i, "wrong name in sample Table");
    }
    for (UnsignedInteger i = 0; i < outCol.getSize(); ++i)
    {
      QString name_i(outDesc[i].c_str());
      QVERIFY2(variableModel->data(variableModel->index(0, outCol[i])) == name_i, "wrong name in variable Table");
      QVERIFY2(sampleTable->model()->headerData(outCol[i] + 1, Qt::Horizontal) == name_i, "wrong name in sample Table");
    }

    // change file name
    Sample sample(Normal(2).getSample(10));
    sample.exportToCSVFile("DataModelSample2.csv");
    model->setFileName("DataModelSample2.csv");
    for (int i = 0; i < variableModel->columnCount(); ++i)
    {
      if (model->getInputColumns().contains(i) || model->getOutputColumns().contains(i))
      {
        QVERIFY2(variableModel->data(variableModel->index(0, i), Qt::CheckStateRole) == Qt::Checked, "wrong check state in Table");
        if (model->getInputColumns().contains(i))
          QVERIFY2(variableModel->data(variableModel->index(1, i)) == "Input", "wrong variable type in Table");
        if (model->getOutputColumns().contains(i))
          QVERIFY2(variableModel->data(variableModel->index(1, i)) == "Output", "wrong variable type in Table");
      }
      else
      {
        QVERIFY2(variableModel->data(variableModel->index(0, i), Qt::CheckStateRole) == Qt::Unchecked, "wrong check state in Table");
        QVERIFY2(variableModel->data(variableModel->index(1, i)).toString().isEmpty() == true, "wrong variable type in Table");
      }
    }
    Indices col(model->getInputColumns());
    for (UnsignedInteger i = 0; i < col.getSize(); ++i)
    {
      QString name_i(model->getInputNames()[i].c_str());
      QVERIFY2(variableModel->data(variableModel->index(0, col[i])) == name_i, "wrong name in variable Table");
      QVERIFY2(sampleTable->model()->headerData(col[i] + 1, Qt::Horizontal) == name_i, "wrong name in sample Table");
    }
    col = model->getOutputColumns();
    for (UnsignedInteger i = 0; i < col.getSize(); ++i)
    {
      QString name_i(model->getOutputNames()[i].c_str());
      QVERIFY2(variableModel->data(variableModel->index(0, col[i])) == name_i, "wrong name in variable Table");
      QVERIFY2(sampleTable->model()->headerData(col[i] + 1, Qt::Horizontal) == name_i, "wrong name in sample Table");
    }
  }
};
}

QTEST_MAIN(PERSALYS::TestDataModelWindow)
#include "t_DataModelWindow_std.moc"
