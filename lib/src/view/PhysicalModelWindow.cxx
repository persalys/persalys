// PhysicalModelWindow.cxx

#include "PhysicalModelWindow.hxx"

#include "ComboBoxDelegate.hxx"

#include <QRadioButton>
#include <QButtonGroup>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QSplitter>
#include <QMdiArea>
#include <QScrollArea>
#include <QLabel>
#include <QComboBox>

#include <iostream>
#include <string>

namespace OTGUI {

PhysicalModelWindow::PhysicalModelWindow(PhysicalModelItem * item)
  : OTguiSubWindow(item)
  , item_(item)
{
  paramEditor_ = 0;
  buildInterface();
}


PhysicalModelWindow::~PhysicalModelWindow()
{
}


void PhysicalModelWindow::buildInterface()
{
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout;

  // Choose method to define Inputs and Outputs
  QGroupBox * methodBox = new QGroupBox(tr(""));
  QHBoxLayout * methodLayout = new QHBoxLayout;

  QComboBox * comboBox = new QComboBox;
  comboBox->addItems(QStringList()<<QString(tr("Analytical"))<<QString(tr("Python"))<<QString(tr("XML")));

  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(methodChanged(int)));
  methodLayout->addWidget(comboBox);
  methodLayout->addStretch();
  methodBox->setLayout(methodLayout);
  mainLayout->addWidget(methodBox);


  // Widgets to load XML file
  loadXMLFileBox_ = new QGroupBox(tr(""));
  QGridLayout * fieldsLayout = new QGridLayout;

  QLabel * label = new QLabel("Data file");
  fieldsLayout->addWidget(label, 0, 0);

  XMLfileEdit_ = new QLineEdit;
  fieldsLayout->addWidget(XMLfileEdit_, 0, 1);

  QPushButton * selectFileButton = new QPushButton(tr("search file"));
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectImportFileDialogRequested()));
  fieldsLayout->addWidget(selectFileButton, 0, 2);

  loadButton_ = new QPushButton(tr("Load Data"));
  connect(loadButton_, SIGNAL(clicked()), this, SLOT(loadDataWithYACS()));
  fieldsLayout->addWidget(loadButton_, 1, 2);
  loadButton_->setEnabled(false);

  loadXMLFileBox_->setLayout(fieldsLayout);
  loadXMLFileBox_->hide();
  mainLayout->addWidget(loadXMLFileBox_);

  // Define Inputs
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QGridLayout * inputsLayout = new QGridLayout;

  inputTableView_ = new QTableView;
  inputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  connect(inputTableView_, SIGNAL(clicked(const QModelIndex &)), this, SLOT(variableChanged(const QModelIndex &)));

  inputTableModel_ = new InputTableModel(item_->getPhysicalModel().getInputs());
  inputTableView_->setModel(inputTableModel_);
  inputsLayout->addWidget(inputTableView_, 0, 0, 2, 1);

  ComboBoxDelegate * delegate = new ComboBoxDelegate;
  inputTableView_->setItemDelegateForColumn(3, delegate);

  QPushButton * addLineButton = new QPushButton(tr("Add"));
  addLineButton->setToolTip(tr("Add an input"));
  connect(addLineButton, SIGNAL(clicked(bool)), this, SLOT(addInputLine()));
  inputsLayout->addWidget(addLineButton, 0, 1);

  QPushButton * removeLineButton = new QPushButton(tr("Remove"));
  removeLineButton->setToolTip(tr("Remove the selected input"));
  connect(removeLineButton, SIGNAL(clicked(bool)), this, SLOT(removeInputLine()));
  inputsLayout->addWidget(removeLineButton, 1, 1);

  inputsBox->setLayout(inputsLayout);
  mainLayout->addWidget(inputsBox);

  connect(inputTableModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(inputDataChanged(QModelIndex,QModelIndex)));
  connect(inputTableModel_, SIGNAL(distributionChanged(OT::NumericalPointWithDescription)), this, SLOT(updateParameters(OT::NumericalPointWithDescription)));
  connect(inputTableModel_, SIGNAL(distributionChanged(OT::NumericalPointWithDescription)), this, SLOT(updatePlot()));

  connect(item_, SIGNAL(inputChanged(InputCollection)), this, SLOT(updateInputData(InputCollection)));

  // Distribution editor
  QGroupBox * distributionBox = new QGroupBox(tr("Distribution Editor"));
  QHBoxLayout * distributionLayout = new QHBoxLayout();

  QGridLayout * plotLayout = new QGridLayout();

  // pdf
  label = new QLabel("PDF");
  plotLayout->addWidget(label, 0, 0, Qt::AlignHCenter);
  pdfPlot_ = new PlotWidget;
  plotLayout->addWidget(pdfPlot_, 1, 0, Qt::AlignHCenter);
  updatePlot();

  distributionLayout->addLayout(plotLayout);

  parameterLayout_ = new QHBoxLayout;
  parameterLayout_->addWidget(new QWidget);
  parameterLayout_->addStretch();

  distributionLayout->addLayout(parameterLayout_);
  distributionLayout->addStretch();

  distributionBox->setLayout(distributionLayout);
  mainLayout->addWidget(distributionBox);

  // Define Outputs
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QGridLayout * outputsLayout = new QGridLayout;

  outputTableView_ = new QTableView;
  outputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  outputTableModel_ = new OutputTableModel(item_->getPhysicalModel().getOutputs());
  outputTableView_->setModel(outputTableModel_);
  outputsLayout->addWidget(outputTableView_, 0, 0, 2, 1);

  addLineButton = new QPushButton(tr("Add"));
  addLineButton->setToolTip(tr("Add an output"));
  connect(addLineButton, SIGNAL(clicked(bool)), this, SLOT(addOutputLine()));
  outputsLayout->addWidget(addLineButton, 0, 1);

  removeLineButton = new QPushButton(tr("Remove"));
  removeLineButton->setToolTip(tr("Remove the selected output"));
  connect(removeLineButton, SIGNAL(clicked(bool)), this, SLOT(removeOutputLine()));
  outputsLayout->addWidget(removeLineButton, 1, 1);

  outputsBox->setLayout(outputsLayout);
  mainLayout->addWidget(outputsBox);
  connect(outputTableModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(outputDataChanged(QModelIndex,QModelIndex)));
  connect(item_, SIGNAL(outputChanged(OutputCollection)), this, SLOT(updateOutputData(OutputCollection)));

  ////////////////
  mainWidget->setLayout(mainLayout);
  setWidget(mainWidget);
}


void PhysicalModelWindow::selectImportFileDialogRequested()
{
  QString xmlPath;
  QString fileName = QFileDialog::getOpenFileName(this, tr("Data to import..."),
                     xmlPath,
                     tr("Data files (*.xml);;"));

  if (!fileName.isNull())
  {
    XMLfileEdit_->setText(fileName);
    loadButton_->setEnabled(true);
  }
}


void PhysicalModelWindow::variableChanged(const QModelIndex & index)
{
  updateParameters(inputTableModel_->getParameters(index.row()));
  updatePlot();
}


void PhysicalModelWindow::updateParameters(const OT::NumericalPointWithDescription & parameters)
{
  if (paramEditor_)
  {
//     disconnect(paramEditor_->getDistributionDescription().data(), SIGNAL(modified()), this, SLOT(update()));
    parameterLayout_->removeWidget(paramEditor_);
    paramEditor_->disconnect();
    paramEditor_->deleteLater();
    paramEditor_ = 0;
  }

  int nbParameters = parameters.getDescription().getSize();
  if (nbParameters)
  {
    OT::Description parametersName = parameters.getDescription();

    paramEditor_ = new QGroupBox(tr("Parameters"));
    QVBoxLayout * editorLayout = new QVBoxLayout;
    QGridLayout * lay = new QGridLayout();
    for (int i = 0; i < nbParameters; ++ i)
    {
      parameterValuesLabel_[i] = new QLabel(QString(parametersName[i].c_str()));
      parameterValuesEdit_[i] = new QLineEdit(QString::number(parameters[i]));
      connect(parameterValuesEdit_[i], SIGNAL(textChanged(QString)), this, SLOT(updateDistribution()));
      parameterValuesLabel_[i]->setBuddy(parameterValuesEdit_[i]);
      lay->addWidget(parameterValuesLabel_[i], i, 0);
      lay->addWidget(parameterValuesEdit_[i], i, 1);
    }
    editorLayout->addLayout(lay);
    paramEditor_->setLayout(editorLayout);

    parameterLayout_->insertWidget(0, paramEditor_);
  }
}


void PhysicalModelWindow::updatePlot()
{
  QModelIndex index = inputTableView_->selectionModel()->currentIndex();
  if (index.isValid())
  {
    pdfPlot_->clear();
    pdfPlot_->plotPDFCurve(inputTableModel_->getData()[index.row()].getDistribution());
  }
}


void PhysicalModelWindow::updateDistribution()
{
  QModelIndex index = inputTableView_->selectionModel()->currentIndex();
  OT::NumericalPoint parameters(2);
  parameters[0] = parameterValuesEdit_[0]->text().toDouble();
  parameters[1] = parameterValuesEdit_[1]->text().toDouble();
  inputTableModel_->updateDistributionParameters(index, parameters);
  updatePlot();
}


void PhysicalModelWindow::loadDataWithYACS()
{
  item_->loadDataWithYACS(XMLfileEdit_->text());
}


void PhysicalModelWindow::inputDataChanged(const QModelIndex&, const QModelIndex&)
{
  //if tableOut && tableIn is valid
  item_->setPhysicalModelInputs(inputTableModel_->getData());
}


void PhysicalModelWindow::updateInputData(const InputCollection & inputs)
{
  delete inputTableModel_;
  inputTableModel_ = new InputTableModel(inputs);
  inputTableView_->setModel(inputTableModel_);
  connect(inputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(inputDataChanged(const QModelIndex&, const QModelIndex&)));
}


void PhysicalModelWindow::outputDataChanged(const QModelIndex&, const QModelIndex&)
{
  //if tableOut && tableIn is valid
  item_->setPhysicalModelOutputs(outputTableModel_->getData());
}


void PhysicalModelWindow::updateOutputData(const OutputCollection & outputs)
{
  delete outputTableModel_;
  outputTableModel_ = new OutputTableModel(outputs);
  outputTableView_->setModel(outputTableModel_);
  connect(outputTableModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(outputDataChanged(QModelIndex,QModelIndex)));
}

void PhysicalModelWindow::addInputLine()
{
  inputTableModel_->addLine();
  inputTableView_->openPersistentEditor(inputTableModel_->index(-1, 3));
  inputTableView_->selectRow(inputTableModel_->rowCount(QModelIndex())-1);
  updatePlot();
}


void PhysicalModelWindow::addOutputLine()
{
  outputTableModel_->addLine();
}


void PhysicalModelWindow::methodChanged(int method)
{
//   disconnect(inputTableModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(inputDataChanged(QModelIndex,QModelIndex)));
//   disconnect(outputTableModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(outputDataChanged(QModelIndex,QModelIndex)));
//   inputTableModel_->deleteLater();
//   inputTableModel_ = 0;
//   outputTableModel_->deleteLater();
//   outputTableModel_ = 0;

  updateParameters();
  pdfPlot_->clear();

  delete inputTableModel_;
  delete outputTableModel_;
  inputTableModel_ = new InputTableModel();
  inputTableView_->setModel(inputTableModel_);
  outputTableModel_ = new OutputTableModel();
  outputTableView_->setModel(outputTableModel_);
  connect(inputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(inputDataChanged(const QModelIndex&,const QModelIndex&)));
  connect(outputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(outputDataChanged(const QModelIndex&,const QModelIndex&)));

  switch(method)
  {
    case 0:
    case 1:
    {
      loadXMLFileBox_->hide();
      break;
    }
    case 2:
    {
      loadXMLFileBox_->show();
      break;
    }
  }
}


void PhysicalModelWindow::removeInputLine()
{
  QModelIndex index = inputTableView_->selectionModel()->currentIndex();
  inputTableModel_->removeLine(index);
  int lastRow = inputTableModel_->rowCount(QModelIndex())-1;

  if (lastRow+1)
  {
    inputTableView_->selectRow(lastRow);
    variableChanged(inputTableModel_->index(lastRow, 0));
  }
  else
  {
    updateParameters();
    pdfPlot_->clear();
  }
}


void PhysicalModelWindow::removeOutputLine()
{
  QModelIndex index = outputTableView_->selectionModel()->currentIndex();
  outputTableModel_->removeLine(index);
}


}