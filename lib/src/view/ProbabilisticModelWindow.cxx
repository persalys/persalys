// ProbabilisticModelWindow.cxx

#include "otgui/ProbabilisticModelWindow.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"

#include "Normal.hxx"

#include <QSplitter>
#include <QHeaderView>
#include <QScrollArea>
#include <QPushButton>

using namespace OT;

namespace OTGUI {

ProbabilisticModelWindow::ProbabilisticModelWindow(ProbabilisticModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
  , paramEditor_(0)
{
  connect(item, SIGNAL(inputChanged()), this, SLOT(updateProbabilisticModel()));
  buildInterface();
}


void ProbabilisticModelWindow::buildInterface()
{
  QTabWidget * rootTab = new QTabWidget;
  
  // First Tab: marginals
  QWidget * tab = new QWidget;
  QHBoxLayout * tabLayout = new QHBoxLayout(tab);

  QSplitter * horizontalSplitter = new QSplitter;

  // Inputs
  QWidget * leftSideOfSplitter = new QWidget;
  QVBoxLayout * leftSideOfSplitterLayout = new QVBoxLayout(leftSideOfSplitter);

  //   stochastic inputs table
  inputTableView_ = new QTableView;
  ComboBoxDelegate * delegate = new ComboBoxDelegate;
  inputTableView_->setItemDelegateForColumn(1, delegate);
  inputTableView_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  inputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);

  inputTableModel_ = new InputTableProbabilisticModel(physicalModel_);
  inputTableView_->setModel(inputTableModel_);

  for (int i = 0; i < inputTableModel_->rowCount(); ++i)
    inputTableView_->openPersistentEditor(inputTableModel_->index(i, 1));

  connect(inputTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));
  connect(inputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));

  leftSideOfSplitterLayout->addWidget(inputTableView_);

  //   deterministic inputs list
  deterministicInputsComboBox_ = new QComboBox;
  populateDeterministicInputsComboBox();
  connect(deterministicInputsComboBox_, SIGNAL(activated(int)), this, SLOT(addInputRequested(int)));

  QPushButton * removeButton = new QPushButton(QIcon(":/images/list-remove.png"), "Remove");
  connect(removeButton, SIGNAL(clicked(bool)), this, SLOT(removeInputRequested()));

  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  buttonsLayout->addWidget(deterministicInputsComboBox_);
  buttonsLayout->addWidget(removeButton);
  leftSideOfSplitterLayout->addLayout(buttonsLayout);

  horizontalSplitter->addWidget(leftSideOfSplitter);

  // Distribution edition
  QScrollArea * rightScrollArea = new QScrollArea;
  rightScrollArea->setWidgetResizable(true);
  QFrame * rightFrame_ = new QFrame;
  QVBoxLayout * rightLayout_ = new QVBoxLayout(rightFrame_);

  //  PDF and CDF graphs
  QGridLayout * plotLayout = new QGridLayout;
  pdfPlot_ = new PlotWidget;
  plotLayout->addWidget(pdfPlot_->getPlotLabel(), 1, 0);
  cdfPlot_ = new PlotWidget;
  plotLayout->addWidget(cdfPlot_->getPlotLabel(), 1, 1);
  
  rightLayout_->addLayout(plotLayout);

  //  parameters
  parameterLayout_ = new QHBoxLayout;
  parameterLayout_->addWidget(new QWidget);
  parameterLayout_->addStretch();
  rightLayout_->addLayout(parameterLayout_);

  if (inputTableModel_->rowCount())
  {
    inputTableView_->selectRow(0);
    updateDistributionWidgets(inputTableView_->currentIndex());
  }
  else
    updateDistributionWidgets(inputTableModel_->index(-1, 0));

  rightLayout_->addStretch();
  rightScrollArea->setWidget(rightFrame_);
  horizontalSplitter->addWidget(rightScrollArea);

  tabLayout->addWidget(horizontalSplitter);
  rootTab->addTab(tab, tr("Marginals"));

  setWidget(rootTab);
}


void ProbabilisticModelWindow::addInputRequested(int comboIndex)
{
  if (comboIndex)
  {
    int ith_input = deterministicInputsComboBox_->itemData(deterministicInputsComboBox_->currentIndex(), Qt::UserRole).toInt();
    deterministicInputsComboBox_->setCurrentIndex(0);
    deterministicInputsComboBox_->removeItem(comboIndex);

    // had to do this before adding the row to avoid bug with comboboxes' display
    for (int i = 0; i < inputTableModel_->rowCount(); ++i)
      inputTableView_->closePersistentEditor(inputTableModel_->index(i, 1));

    inputTableModel_->addLine(ith_input);

    for (int i = 0; i < inputTableModel_->rowCount(); ++i)
      inputTableView_->openPersistentEditor(inputTableModel_->index(i, 1));

    inputTableView_->selectRow(inputTableModel_->rowCount()-1);
  }
}


void ProbabilisticModelWindow::removeInputRequested()
{
  if (inputTableView_->selectionModel()->hasSelection())
  {
    QModelIndex index = inputTableView_->currentIndex();
    int inputIndex = inputTableModel_->data(index, Qt::UserRole).toInt();
    deterministicInputsComboBox_->addItem(QString(physicalModel_.getInputs()[inputIndex].getName().c_str()), inputIndex);

    inputTableModel_->removeLine(index);

    if (inputTableModel_->rowCount())
    {
      inputTableView_->selectRow(0);
      updateDistributionWidgets(inputTableView_->currentIndex());
    }
    else
      updateDistributionWidgets(inputTableModel_->index(-1, 0));
  }
}


void ProbabilisticModelWindow::updateDeterministicInputsComboBox()
{
  deterministicInputsComboBox_->clear();
  delete deterministicInputsComboBoxModel_;
  populateDeterministicInputsComboBox();
}


void ProbabilisticModelWindow::populateDeterministicInputsComboBox()
{
  deterministicInputsComboBoxModel_ = new QStandardItemModel(1, 1);
  QStandardItem * firstItem = new QStandardItem(QIcon(":/images/list-add.png"), "Add");
  firstItem->setBackground(QBrush(QColor(200, 200, 200)));
  deterministicInputsComboBoxModel_->setItem(0, 0, firstItem);
  deterministicInputsComboBox_->setModel(deterministicInputsComboBoxModel_);

  for (int i=0; i<physicalModel_.getInputs().getSize(); ++i)
    if (physicalModel_.getInputs()[i].getDistribution().getImplementation()->getClassName() == "Dirac")
      deterministicInputsComboBox_->addItem(QString(physicalModel_.getInputs()[i].getName().c_str()), i);
}


void ProbabilisticModelWindow::updateProbabilisticModel()
{
  // update stochastic inputs table
  delete inputTableModel_;
  inputTableModel_ = new InputTableProbabilisticModel(physicalModel_);
  inputTableView_->setModel(inputTableModel_);
  for (int i = 0; i < inputTableModel_->rowCount(); ++i)
    inputTableView_->openPersistentEditor(inputTableModel_->index(i, 1));
  connect(inputTableModel_, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));

  // update deterministic inputs list
  updateDeterministicInputsComboBox();
}


void ProbabilisticModelWindow::updateDistributionWidgets(const QModelIndex & index)
{
  // update parameters widgets
  if (paramEditor_)
  {
    parameterLayout_->removeWidget(paramEditor_);
    paramEditor_->disconnect();
    paramEditor_->deleteLater();
    paramEditor_ = 0;
  }

  pdfPlot_->clear();
  cdfPlot_->clear();

  if (!index.isValid())
  {
    pdfPlot_->getPlotLabel()->hide();
    cdfPlot_->getPlotLabel()->hide();
    return;
  }

  if (pdfPlot_->getPlotLabel()->isHidden())
  {
    pdfPlot_->getPlotLabel()->show();
    cdfPlot_->getPlotLabel()->show();
  }

  if (index.row() != inputTableView_->currentIndex().row())
    inputTableView_->selectRow(index.row());

  int inputIndex = inputTableModel_->data(index, Qt::UserRole).toInt();
  Distribution inputDistribution = physicalModel_.getInputs()[inputIndex].getDistribution();

  const NumericalPointWithDescription parameters = inputDistribution.getParametersCollection()[0];
  Description parametersName = parameters.getDescription();

  if (parametersName.getSize())
  {
    if (parametersName.getSize() >= 5)
      std::cout << "Number of parameters invalid :"<<parametersName.getSize()<<std::endl;

    paramEditor_ = new QGroupBox(tr("Parameters"));
    QGridLayout * lay = new QGridLayout(paramEditor_);
    for (int i=0; i<parametersName.getSize(); ++i)
    {
      parameterValuesLabel_[i] = new QLabel(QString(parametersName[i].c_str()));
      parameterValuesEdit_[i] = new QLineEdit(QString::number(parameters[i]));
      connect(parameterValuesEdit_[i], SIGNAL(textChanged(QString)), this, SLOT(updateDistribution()));
      parameterValuesLabel_[i]->setBuddy(parameterValuesEdit_[i]);
      lay->addWidget(parameterValuesLabel_[i], i, 0);
      lay->addWidget(parameterValuesEdit_[i], i, 1);
    }
    parameterLayout_->insertWidget(0, paramEditor_);
  }

  // update plots
  pdfPlot_->plotPDFCurve(inputDistribution);
  cdfPlot_->plotCDFCurve(inputDistribution);
}


void ProbabilisticModelWindow::updateDistribution()
{
  QModelIndex index = inputTableView_->currentIndex();
  int inputIndex = index.data(Qt::UserRole).toInt();
  Distribution inputDistribution = physicalModel_.getInputs()[inputIndex].getDistribution();

  NumericalPoint parameters(2);
  parameters[0] = parameterValuesEdit_[0]->text().toDouble();
  parameters[1] = parameterValuesEdit_[1]->text().toDouble();

  try
  {
    inputDistribution.setParametersCollection(parameters);
    Input input = Input(physicalModel_.getInputs()[inputIndex]);
    input.setDistribution(inputDistribution);
    physicalModel_.updateInput(inputIndex, input, false);
    updateDistributionWidgets(index);
  }
  catch(Exception)
  {
    std::cerr << "InputTableProbabilisticModel::updateDistributionParameters invalid params:"<<parameters<<" for distribution:"<<inputDistribution.getImplementation()->getName()<<std::endl;
  }
}
}
