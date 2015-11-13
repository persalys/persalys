// PhysicalModelWindow.cxx

#include "otgui/PhysicalModelWindow.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#ifdef OTGUI_HAVE_YACS
# include "otgui/YACSPhysicalModel.hxx"
#endif

#include <QFileDialog>
#include <QComboBox>

using namespace OT;

namespace OTGUI {

PhysicalModelWindow::PhysicalModelWindow(PhysicalModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
{
  connect(this, SIGNAL(physicalModelChanged(PhysicalModel)), item, SLOT(updatePhysicalModel(PhysicalModel)));
  connect(item, SIGNAL(outputChanged(OutputCollection)), this, SLOT(updateOutputData(OutputCollection)));
  connect(item, SIGNAL(inputChanged(InputCollection)), this, SLOT(updateInputData(InputCollection)));
  buildInterface();
}


void PhysicalModelWindow::buildInterface()
{
  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);

  // Choose method to define Inputs and Outputs
  QGroupBox * methodBox = new QGroupBox(tr(""));
  QHBoxLayout * methodLayout = new QHBoxLayout(methodBox);

  QComboBox * comboBox = new QComboBox;
  QStringList items = QStringList()<<tr("Analytical")<<tr("Python");
#ifdef OTGUI_HAVE_YACS
  items<<tr("XML");
#endif
  comboBox->addItems(items);

  connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(methodChanged(int)));
  methodLayout->addWidget(comboBox);
  methodLayout->addStretch();

  mainLayout->addWidget(methodBox);

#ifdef OTGUI_HAVE_YACS
  // Widgets to load XML file
  loadXMLFileBox_ = new QGroupBox(tr(""));
  QGridLayout * fieldsLayout = new QGridLayout(loadXMLFileBox_);

  QLabel * labelDataFile = new QLabel("Data file");
  fieldsLayout->addWidget(labelDataFile, 0, 0);

  XMLfileEdit_ = new QLineEdit;
  fieldsLayout->addWidget(XMLfileEdit_, 0, 1);

  QPushButton * selectFileButton = new QPushButton(tr("Search file"));
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectImportFileDialogRequested()));
  fieldsLayout->addWidget(selectFileButton, 0, 2);

  loadButton_ = new QPushButton(tr("Load Data"));
  connect(loadButton_, SIGNAL(clicked()), this, SLOT(loadXML()));
  fieldsLayout->addWidget(loadButton_, 1, 2);
  loadButton_->setEnabled(false);

  loadXMLFileBox_->hide();
  mainLayout->addWidget(loadXMLFileBox_);
#endif

  // Define Inputs
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QGridLayout * inputsLayout = new QGridLayout(inputsBox);

  inputTableView_ = new QTableView;
  inputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);

  inputTableModel_ = new InputTableModel(physicalModel_.getInputs());
  inputTableView_->setModel(inputTableModel_);
  inputsLayout->addWidget(inputTableView_, 0, 0, 2, 1);
  connect(inputTableModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(inputDataChanged()));

  QPushButton * addLineButton = new QPushButton(tr("Add"));
  addLineButton->setToolTip(tr("Add an input"));
  connect(addLineButton, SIGNAL(clicked(bool)), this, SLOT(addInputLine()));
  inputsLayout->addWidget(addLineButton, 0, 1);

  QPushButton * removeLineButton = new QPushButton(tr("Remove"));
  removeLineButton->setToolTip(tr("Remove the selected input"));
  connect(removeLineButton, SIGNAL(clicked(bool)), this, SLOT(removeInputLine()));
  inputsLayout->addWidget(removeLineButton, 1, 1);

  mainLayout->addWidget(inputsBox);

  // Define Outputs
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QGridLayout * outputsLayout = new QGridLayout(outputsBox);

  outputTableView_ = new QTableView;
  outputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  outputTableModel_ = new OutputTableModel(physicalModel_.getOutputs());
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

  mainLayout->addWidget(outputsBox);
  connect(outputTableModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(outputDataChanged()));

  ////////////////
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


void PhysicalModelWindow::loadXML()
{
#ifdef OTGUI_HAVE_YACS
  YACSPhysicalModel *ymodel = dynamic_cast<YACSPhysicalModel*>(physicalModel_.getImplementation().get());
  if (ymodel)
    ymodel->loadDataWithYACS(XMLfileEdit_->text().toStdString());
#endif
}


void PhysicalModelWindow::inputDataChanged()
{
  //TODO: if tableOut && tableIn is valid
  bool inputsAreValid = physicalModel_.updateInputs(inputTableModel_->getData());
}


void PhysicalModelWindow::updateInputData(const InputCollection & inputs)
{
  delete inputTableModel_;
  inputTableModel_ = new InputTableModel(inputs);
  inputTableView_->setModel(inputTableModel_);
  connect(inputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(inputDataChanged()));
}


void PhysicalModelWindow::outputDataChanged()
{
  //TODO: if tableOut && tableIn is valid
  bool outputsAreValid = physicalModel_.updateOutputs(outputTableModel_->getData());
}


void PhysicalModelWindow::updateOutputData(const OutputCollection & outputs)
{
  delete outputTableModel_;
  outputTableModel_ = new OutputTableModel(outputs);
  outputTableView_->setModel(outputTableModel_);
  connect(outputTableModel_, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(outputDataChanged()));
}

void PhysicalModelWindow::addInputLine()
{
  inputTableModel_->addLine();
  inputTableView_->openPersistentEditor(inputTableModel_->index(-1, 3));
  inputTableView_->selectRow(inputTableModel_->rowCount(QModelIndex())-1);
}


void PhysicalModelWindow::addOutputLine()
{
  outputTableModel_->addLine();
}


void PhysicalModelWindow::methodChanged(int method)
{
  delete inputTableModel_;
  delete outputTableModel_;
  inputTableModel_ = new InputTableModel();
  inputTableView_->setModel(inputTableModel_);
  outputTableModel_ = new OutputTableModel();
  outputTableView_->setModel(outputTableModel_);
  connect(inputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(inputDataChanged()));
  connect(outputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(outputDataChanged()));

  switch(method)
  {
    case 0:
    case 1:
    {
      physicalModel_ = PhysicalModel(physicalModel_.getName());
      emit physicalModelChanged(physicalModel_);
#ifdef OTGUI_HAVE_YACS
      loadXMLFileBox_->hide();
#endif
      break;
    }
#ifdef OTGUI_HAVE_YACS
    case 2:
    {
      physicalModel_ = YACSPhysicalModel(physicalModel_.getName());
      emit physicalModelChanged(physicalModel_);
      loadXMLFileBox_->show();
      break;
    }
#endif
  }
}


void PhysicalModelWindow::removeInputLine()
{
  QModelIndex index = inputTableView_->selectionModel()->currentIndex();
  inputTableModel_->removeLine(index);
  int lastRow = inputTableModel_->rowCount(QModelIndex())-1;

  if (lastRow+1)
    inputTableView_->selectRow(lastRow);
}


void PhysicalModelWindow::removeOutputLine()
{
  QModelIndex index = outputTableView_->selectionModel()->currentIndex();
  outputTableModel_->removeLine(index);
}


}
