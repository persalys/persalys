// PhysicalModelWindow.cxx

#include "otgui/PhysicalModelWindow.hxx"

#include "otgui/AnalyticalPhysicalModel.hxx"
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
  , inputTableModel_(0)
  , outputTableModel_(0)
{
  connect(this, SIGNAL(physicalModelChanged(const PhysicalModel&)), item, SLOT(updatePhysicalModel(const PhysicalModel&)));
  connect(item, SIGNAL(outputChanged()), this, SLOT(updateOutputTableModel()));
  connect(item, SIGNAL(inputChanged()), this, SLOT(updateInputTableModel()));
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

  if (physicalModel_.getImplementation()->getClassName() == "AnalyticalPhysicalModel")
    comboBox->setCurrentIndex(0);
  else if(physicalModel_.getImplementation()->getClassName() == "PythonPhysicalModel")
    comboBox->setCurrentIndex(1);
#ifdef OTGUI_HAVE_YACS
  else if(physicalModel_.getImplementation()->getClassName() == "YACSPhysicalModel")
    comboBox->setCurrentIndex(2);
#endif

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

  XMLfileNameEdit_ = new QLineEdit;
  if(physicalModel_.getImplementation()->getClassName() == "YACSPhysicalModel")
    XMLfileNameEdit_->setText(dynamic_cast<YACSPhysicalModel*>(&*physicalModel_.getImplementation())->getXMLFileName().c_str());
  fieldsLayout->addWidget(XMLfileNameEdit_, 0, 1);

  QPushButton * selectFileButton = new QPushButton(tr("Search file"));
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectImportFileDialogRequested()));
  fieldsLayout->addWidget(selectFileButton, 0, 2);

  loadButton_ = new QPushButton(tr("Load Data"));
  connect(loadButton_, SIGNAL(clicked()), this, SLOT(loadXML()));
  fieldsLayout->addWidget(loadButton_, 1, 2);
  loadButton_->setEnabled(false);

  mainLayout->addWidget(loadXMLFileBox_);
#endif

  // Define Inputs
  QGroupBox * inputsBox = new QGroupBox(tr("Inputs"));
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  inputTableView_ = new QTableView;
  inputTableView_->setEditTriggers(QTableView::SelectedClicked);
  inputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  inputsLayout->addWidget(inputTableView_);

  addInputLineButton_ = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
  addInputLineButton_->setToolTip(tr("Add an input"));
  connect(addInputLineButton_, SIGNAL(clicked(bool)), this, SLOT(addInputLine()));

  removeInputLineButton_ = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeInputLineButton_->setToolTip(tr("Remove the selected input"));
  connect(removeInputLineButton_, SIGNAL(clicked(bool)), this, SLOT(removeInputLine()));

  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(addInputLineButton_);
  buttonsLayout->addWidget(removeInputLineButton_);
  inputsLayout->addLayout(buttonsLayout);

  mainLayout->addWidget(inputsBox);

  // Define Outputs
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  outputTableView_ = new QTableView;
  outputTableView_->setEditTriggers(QTableView::SelectedClicked);
  outputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  outputsLayout->addWidget(outputTableView_);

  addOutputLineButton_ = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
  addOutputLineButton_->setToolTip(tr("Add an output"));
  connect(addOutputLineButton_, SIGNAL(clicked(bool)), this, SLOT(addOutputLine()));

  removeOutputLineButton_ = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeOutputLineButton_->setToolTip(tr("Remove the selected output"));
  connect(removeOutputLineButton_, SIGNAL(clicked(bool)), this, SLOT(removeOutputLine()));

  buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(addOutputLineButton_);
  buttonsLayout->addWidget(removeOutputLineButton_);
  outputsLayout->addLayout(buttonsLayout);

  mainLayout->addWidget(outputsBox);

  updateMethodWidgets(comboBox->currentIndex());
  ////////////////
  setWidget(mainWidget);
}


void PhysicalModelWindow::selectImportFileDialogRequested()
{
  QString xmlPath;
  QString fileName = QFileDialog::getOpenFileName(this, tr("Data to import..."),
                     xmlPath,
                     tr("Data files (*.xml);;"));

  if (!fileName.isEmpty())
  {
    XMLfileNameEdit_->setText(fileName);
    loadButton_->setEnabled(true);
  }
}


void PhysicalModelWindow::loadXML()
{
#ifdef OTGUI_HAVE_YACS
  try
  {
    physicalModel_ = YACSPhysicalModel(physicalModel_.getName(), XMLfileNameEdit_->text().toStdString());
    emit physicalModelChanged(physicalModel_);
    updateInputTableModel();
    updateOutputTableModel();
  }
  catch(std::exception)
  {
    std::cerr<<"Impossible to load data from the file "<< XMLfileNameEdit_->text().toStdString()<<std::endl;
  }
#endif
}


void PhysicalModelWindow::updateInputTableModel()
{
  if (inputTableModel_)
    delete inputTableModel_;
  inputTableModel_ = new InputTableModel(physicalModel_);
  inputTableView_->setModel(inputTableModel_);
}


void PhysicalModelWindow::updateOutputTableModel()
{
  if (outputTableModel_)
    delete outputTableModel_;
  outputTableModel_ = new OutputTableModel(physicalModel_);
  outputTableView_->setModel(outputTableModel_);
}

void PhysicalModelWindow::addInputLine()
{
  inputTableModel_->addLine();
  inputTableView_->selectRow(inputTableModel_->rowCount(QModelIndex())-1);
}


void PhysicalModelWindow::addOutputLine()
{
  outputTableModel_->addLine();
}


void PhysicalModelWindow::updateMethodWidgets(int method)
{
  switch(method)
  {
    case 0:
    case 1:
    {
#ifdef OTGUI_HAVE_YACS
      loadXMLFileBox_->hide();
#endif
      addInputLineButton_->show();
      removeInputLineButton_->show();
      addOutputLineButton_->show();
      removeOutputLineButton_->show();
      break;
    }
#ifdef OTGUI_HAVE_YACS
    case 2:
    {
      loadXMLFileBox_->show();
      addInputLineButton_->hide();
      removeInputLineButton_->hide();
      addOutputLineButton_->hide();
      removeOutputLineButton_->hide();
      break;
    }
#endif
  }
  updateInputTableModel();
  updateOutputTableModel();
}


void PhysicalModelWindow::methodChanged(int method)
{
  switch(method)
  {
    case 0:
    case 1:
    {
      physicalModel_ = AnalyticalPhysicalModel(physicalModel_.getName());
      emit physicalModelChanged(physicalModel_);
      break;
    }
#ifdef OTGUI_HAVE_YACS
    case 2:
    {
      physicalModel_ = PhysicalModel(physicalModel_.getName());
      emit physicalModelChanged(physicalModel_);
      break;
    }
#endif
  }
  updateMethodWidgets(method);
}


void PhysicalModelWindow::removeInputLine()
{
  if (inputTableView_->selectionModel()->hasSelection())
  {
    QModelIndex index = inputTableView_->selectionModel()->currentIndex();
    inputTableModel_->removeLine(index);
    int lastRow = inputTableModel_->rowCount(QModelIndex())-1;

    if (lastRow+1)
      inputTableView_->selectRow(lastRow);
  }
}


void PhysicalModelWindow::removeOutputLine()
{
  if (outputTableView_->selectionModel()->hasSelection())
  {
    QModelIndex index = outputTableView_->selectionModel()->currentIndex();
    outputTableModel_->removeLine(index);
  }
}
}