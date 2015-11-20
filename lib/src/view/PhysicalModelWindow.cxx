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
  QVBoxLayout * inputsLayout = new QVBoxLayout(inputsBox);

  inputTableView_ = new QTableView;
  inputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);

  inputTableModel_ = new InputTableModel(physicalModel_);
  inputTableView_->setModel(inputTableModel_);
  inputsLayout->addWidget(inputTableView_);

  QPushButton * addLineButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
  addLineButton->setToolTip(tr("Add an input"));
  connect(addLineButton, SIGNAL(clicked(bool)), this, SLOT(addInputLine()));

  QPushButton * removeLineButton = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeLineButton->setToolTip(tr("Remove the selected input"));
  connect(removeLineButton, SIGNAL(clicked(bool)), this, SLOT(removeInputLine()));

  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(addLineButton);
  buttonsLayout->addWidget(removeLineButton);
  inputsLayout->addLayout(buttonsLayout);

  mainLayout->addWidget(inputsBox);

  // Define Outputs
  QGroupBox * outputsBox = new QGroupBox(tr("Outputs"));
  QVBoxLayout * outputsLayout = new QVBoxLayout(outputsBox);

  outputTableView_ = new QTableView;
  outputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  outputTableModel_ = new OutputTableModel(physicalModel_);
  outputTableView_->setModel(outputTableModel_);
  outputsLayout->addWidget(outputTableView_);

  addLineButton = new QPushButton(QIcon(":/images/list-add.png"), tr("Add"));
  addLineButton->setToolTip(tr("Add an output"));
  connect(addLineButton, SIGNAL(clicked(bool)), this, SLOT(addOutputLine()));

  removeLineButton = new QPushButton(QIcon(":/images/list-remove.png"), tr("Remove"));
  removeLineButton->setToolTip(tr("Remove the selected output"));
  connect(removeLineButton, SIGNAL(clicked(bool)), this, SLOT(removeOutputLine()));

  buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(addLineButton);
  buttonsLayout->addWidget(removeLineButton);
  outputsLayout->addLayout(buttonsLayout);

  mainLayout->addWidget(outputsBox);

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


void PhysicalModelWindow::updateInputTableModel()
{
  delete inputTableModel_;
  inputTableModel_ = new InputTableModel(physicalModel_);
  inputTableView_->setModel(inputTableModel_);
}


void PhysicalModelWindow::updateOutputTableModel()
{
  delete outputTableModel_;
  outputTableModel_ = new OutputTableModel(physicalModel_);
  outputTableView_->setModel(outputTableModel_);
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
  updateInputTableModel();
  updateOutputTableModel();
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
