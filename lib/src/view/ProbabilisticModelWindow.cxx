// ProbabilisticModelWindow.cxx

#include "otgui/ProbabilisticModelWindow.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"

#include "Normal.hxx"
#include "TruncatedDistribution.hxx"
#include "TruncatedNormal.hxx"

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
  rightFrame_ = new QFrame;
  QVBoxLayout * rightLayout_ = new QVBoxLayout(rightFrame_);

  //  PDF and CDF graphs
  QGridLayout * plotLayout = new QGridLayout;
  pdfPlot_ = new PlotWidget;
  plotLayout->addWidget(pdfPlot_->getPlotLabel(), 1, 0);
  cdfPlot_ = new PlotWidget;
  plotLayout->addWidget(cdfPlot_->getPlotLabel(), 1, 1);
  
  rightLayout_->addLayout(plotLayout);

  //  parameters
  parameterLayout_ = new QVBoxLayout;
  parameterLayout_->addWidget(new QWidget);
  parameterLayout_->addStretch();
  rightLayout_->addLayout(parameterLayout_);

  // advanced group: truncation parameters
  advancedGroup_ = new QGroupBox(tr("Truncation parameters"));
  QVBoxLayout * advancedGroupLayout = new QVBoxLayout(advancedGroup_);
  advancedGroup_->setCheckable(true);
  advancedGroup_->setStyleSheet("QGroupBox::indicator::unchecked {image: url(:/images/down_arrow.png);}\
                                QGroupBox::indicator::checked {image: url(:/images/up_arrow.png);}");

  connect(advancedGroup_, SIGNAL(toggled(bool)), this, SLOT(showHideAdvancedWidgets(bool)));

  advancedWidgets_ = new QWidget;
  QGridLayout * advancedWidgetsLayout = new QGridLayout(advancedWidgets_);

  lowerBoundCheckBox_ = new QCheckBox(tr("Lower bound"));
  advancedWidgetsLayout->addWidget(lowerBoundCheckBox_, 0, 0);
  upperBoundCheckBox_ = new QCheckBox(tr("Upper bound"));
  advancedWidgetsLayout->addWidget(upperBoundCheckBox_, 1, 0);

  lowerBoundLineEdit_ = new QLineEdit;
  advancedWidgetsLayout->addWidget(lowerBoundLineEdit_, 0, 1);
  upperBoundLineEdit_ = new QLineEdit;
  advancedWidgetsLayout->addWidget(upperBoundLineEdit_, 1, 1);

  connect(lowerBoundCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(truncationParametersStateChanged()));
  connect(upperBoundCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(truncationParametersStateChanged()));
  connect(lowerBoundLineEdit_, SIGNAL(editingFinished()), this, SLOT(truncationParametersChanged()));
  connect(upperBoundLineEdit_, SIGNAL(editingFinished()), this, SLOT(truncationParametersChanged()));

  advancedGroupLayout->addWidget(advancedWidgets_);
  rightLayout_->addWidget(advancedGroup_);

  rightLayout_->addStretch();
  rightScrollArea->setWidget(rightFrame_);
  horizontalSplitter->addWidget(rightScrollArea);

  if (inputTableModel_->rowCount())
  {
    inputTableView_->selectRow(0);
    updateDistributionWidgets(inputTableView_->currentIndex());
  }
  else
    updateDistributionWidgets(inputTableModel_->index(-1, 0));

  tabLayout->addWidget(horizontalSplitter);
  rootTab->addTab(tab, tr("Marginals"));

  setWidget(rootTab);
}

void ProbabilisticModelWindow::showHideAdvancedWidgets(bool show)
{
  if (show)
    advancedWidgets_->show();
  else
    advancedWidgets_->hide();
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
    deterministicInputsComboBox_->addItem(physicalModel_.getInputs()[inputIndex].getName().c_str(), inputIndex);

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
      deterministicInputsComboBox_->addItem(physicalModel_.getInputs()[i].getName().c_str(), i);
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

  if (!index.isValid())
  {
    rightFrame_->hide();
    return;
  }
  rightFrame_->show();
  advancedGroup_->setChecked(false);

  if (index.row() != inputTableView_->currentIndex().row())
    inputTableView_->selectRow(index.row());

  int inputIndex = inputTableModel_->data(index, Qt::UserRole).toInt();
  Distribution inputDistribution = physicalModel_.getInputs()[inputIndex].getDistribution();
  std::string distributionName = inputDistribution.getImplementation()->getClassName();

  const NumericalPointWithDescription parameters = inputDistribution.getParametersCollection()[0];
  int nbParameters = parameters.getSize();

  lowerBoundCheckBox_->blockSignals(true);
  upperBoundCheckBox_->blockSignals(true);
  lowerBoundLineEdit_->blockSignals(true);
  upperBoundLineEdit_->blockSignals(true);

  // clear truncation parameters
  lowerBoundLineEdit_->clear();
  lowerBoundLineEdit_->setEnabled(false);
  upperBoundLineEdit_->clear();
  upperBoundLineEdit_->setEnabled(false);
  lowerBoundCheckBox_->setChecked(false);
  upperBoundCheckBox_->setChecked(false);

  if (distributionName == "TruncatedDistribution")
  {
    TruncatedDistribution * dist = dynamic_cast<TruncatedDistribution*>(&*inputDistribution.getImplementation());

    lowerBoundCheckBox_->setChecked(dist->getFiniteLowerBound());
    upperBoundCheckBox_->setChecked(dist->getFiniteUpperBound());
    if (dist->getFiniteLowerBound())
    {
      lowerBoundLineEdit_->setText(QString::number(dist->getLowerBound()));
      lowerBoundLineEdit_->setEnabled(true);
    }
    if (dist->getFiniteUpperBound())
    {
      upperBoundLineEdit_->setText(QString::number(dist->getUpperBound()));
      upperBoundLineEdit_->setEnabled(true);
    }
    advancedGroup_->setChecked(true);

    nbParameters = dist->getDistribution().getParametersCollection()[0].getSize();
  }
  else if (distributionName == "TruncatedNormal")
  {
    TruncatedNormal * dist = dynamic_cast<TruncatedNormal*>(&*inputDistribution.getImplementation());

    lowerBoundCheckBox_->setChecked(true);
    upperBoundCheckBox_->setChecked(true);
    lowerBoundLineEdit_->setText(QString::number(dist->getA()));
    lowerBoundLineEdit_->setEnabled(true);
    upperBoundLineEdit_->setText(QString::number(dist->getB()));
    upperBoundLineEdit_->setEnabled(true);
    advancedGroup_->setChecked(true);

    nbParameters = 2;
  }
  lowerBoundCheckBox_->blockSignals(false);
  upperBoundCheckBox_->blockSignals(false);
  lowerBoundLineEdit_->blockSignals(false);
  upperBoundLineEdit_->blockSignals(false);

  if (nbParameters)
  {
    if (nbParameters >= 5)
      std::cout << "Number of parameters invalid :"<<nbParameters<<std::endl;
  
    Description parametersName = parameters.getDescription();

    paramEditor_ = new QGroupBox(tr("Parameters"));
    QGridLayout * lay = new QGridLayout(paramEditor_);
    for (int i=0; i<nbParameters; ++i)
    {
      parameterValuesLabel_[i] = new QLabel(parametersName[i].c_str());
      parameterValuesEdit_[i] = new QLineEdit(QString::number(parameters[i]));
      connect(parameterValuesEdit_[i], SIGNAL(editingFinished()), this, SLOT(updateDistribution()));
      parameterValuesLabel_[i]->setBuddy(parameterValuesEdit_[i]);
      lay->addWidget(parameterValuesLabel_[i], i, 0);
      lay->addWidget(parameterValuesEdit_[i], i, 1);
    }
    parameterLayout_->insertWidget(0, paramEditor_);
  }

  // update plots
  updatePlots(inputDistribution);
}


void ProbabilisticModelWindow::updatePlots(Distribution inputDistribution)
{
  pdfPlot_->clear();
  cdfPlot_->clear();
  pdfPlot_->plotPDFCurve(inputDistribution);
  cdfPlot_->plotCDFCurve(inputDistribution);
}


void ProbabilisticModelWindow::updateDistribution()
{
  QModelIndex index = inputTableView_->currentIndex();
  Input input = Input(physicalModel_.getInputs()[index.data(Qt::UserRole).toInt()]);
  Distribution inputDistribution = input.getDistribution();
  std::string distributionName = inputDistribution.getImplementation()->getClassName();

  if (distributionName == "TruncatedDistribution")
  {
    TruncatedDistribution truncatedDistribution = *dynamic_cast<TruncatedDistribution*>(&*inputDistribution.getImplementation());
    Distribution distribution = truncatedDistribution.getDistribution();
    int nbParameters = distribution.getParametersCollection()[0].getSize();
    NumericalPoint newParameters(nbParameters);
    for (int i=0; i<nbParameters; ++i)
      newParameters[i] = parameterValuesEdit_[i]->text().toDouble();
    try
    {
      distribution.setParametersCollection(newParameters);
      truncatedDistribution.setDistribution(distribution);
      physicalModel_.blockNotification(true, "updateProbabilisticModelWindow");
      physicalModel_.updateInputDistribution(input.getName(), truncatedDistribution);
      physicalModel_.blockNotification(false);
      updateDistributionWidgets(index);
    }
    catch(Exception)
    {
      std::cerr << "ProbabilisticModelWindow::updateDistribution invalid params:"<<newParameters<<" for distribution:"<<distributionName<<std::endl;
    }
  }
  else
  {
    NumericalPointWithDescription parameters = inputDistribution.getParametersCollection()[0];
    int nbParameters = parameters.getSize();
    if (distributionName == "TruncatedNormal")
      nbParameters = 2;

    for (int i=0; i<nbParameters; ++i)
      parameters[i] = parameterValuesEdit_[i]->text().toDouble();

    try
    {
      inputDistribution.setParametersCollection(parameters);
      physicalModel_.blockNotification(true, "updateProbabilisticModelWindow");
      physicalModel_.updateInputDistribution(input.getName(), inputDistribution);
      physicalModel_.blockNotification(false);
      updateDistributionWidgets(index);
    }
    catch(Exception)
    {
      std::cerr << "ProbabilisticModelWindow::updateDistribution invalid params:"<<parameters<<" for distribution:"<<distributionName<<std::endl;
    }
  }
}


void ProbabilisticModelWindow::truncationParametersChanged()
{
  QModelIndex index = inputTableView_->currentIndex();
  Input input = Input(physicalModel_.getInputs()[index.data(Qt::UserRole).toInt()]);
  Distribution inputDistribution = input.getDistribution();
  
  TruncatedDistribution truncatedDistribution = TruncatedDistribution(*dynamic_cast<TruncatedDistribution*>(&*inputDistribution.getImplementation()));

  if (lowerBoundCheckBox_->isChecked())
    truncatedDistribution.setLowerBound(lowerBoundLineEdit_->text().toDouble());
  if (upperBoundCheckBox_->isChecked())
    truncatedDistribution.setUpperBound(upperBoundLineEdit_->text().toDouble());

  physicalModel_.blockNotification(true);
  physicalModel_.updateInputDistribution(input.getName(), truncatedDistribution);
  physicalModel_.blockNotification(false);

  updatePlots(truncatedDistribution);
}


void ProbabilisticModelWindow::truncationParametersStateChanged()
{
  QModelIndex index = inputTableView_->currentIndex();
  Input input = Input(physicalModel_.getInputs()[index.data(Qt::UserRole).toInt()]);
  Distribution inputDistribution = input.getDistribution();
  std::string distributionName = inputDistribution.getImplementation()->getClassName();

  if (!lowerBoundCheckBox_->isChecked() && !upperBoundCheckBox_->isChecked())
  {
    lowerBoundLineEdit_->clear();
    upperBoundLineEdit_->clear();
    lowerBoundLineEdit_->setEnabled(false);
    upperBoundLineEdit_->setEnabled(false);
    if (distributionName == "TruncatedDistribution")
    {
      TruncatedDistribution * dist = dynamic_cast<TruncatedDistribution*>(&*inputDistribution.getImplementation());
      inputDistribution = dist->getDistribution();
    }
    else if (distributionName == "TruncatedNormal")
    {
      TruncatedNormal * dist = dynamic_cast<TruncatedNormal*>(&*inputDistribution.getImplementation());
      inputDistribution = Normal(dist->getMu(), dist->getSigma());
    }
    else
      throw;

    physicalModel_.blockNotification(true);
    physicalModel_.updateInputDistribution(input.getName(), inputDistribution);
    physicalModel_.blockNotification(false);
    updatePlots(inputDistribution);
  }
  else
  {
    TruncatedDistribution truncatedDistribution;
    if (distributionName == "TruncatedDistribution")
      truncatedDistribution = TruncatedDistribution(*dynamic_cast<TruncatedDistribution*>(&*inputDistribution.getImplementation()));
    else if (distributionName == "TruncatedNormal")
    {
      TruncatedNormal dist = TruncatedNormal(*dynamic_cast<TruncatedNormal*>(&*inputDistribution.getImplementation()));
      truncatedDistribution = TruncatedDistribution(Normal(dist.getMu(), dist.getSigma()), dist.getA(), dist.getB());
    }
    else
    {
      double mean = inputDistribution.getMean()[0];
      double std = inputDistribution.getStandardDeviation()[0];
      truncatedDistribution = TruncatedDistribution(inputDistribution, mean - std, mean + std);
    }

    QCheckBox * checkbox = qobject_cast<QCheckBox*>(sender());
    if (checkbox == lowerBoundCheckBox_)
    {
      if (lowerBoundCheckBox_->isChecked())
      {
          double distMean = truncatedDistribution.getMean()[0];
          double distStd = truncatedDistribution.getStandardDeviation()[0];
          truncatedDistribution.setLowerBound(distMean - distStd);
          lowerBoundLineEdit_->blockSignals(true);
          lowerBoundLineEdit_->setText(QString::number(distMean - distStd));
          lowerBoundLineEdit_->blockSignals(false);
          lowerBoundLineEdit_->setEnabled(true);
          truncatedDistribution.setFiniteLowerBound(true);
      }
      else
      {
        lowerBoundLineEdit_->clear();
        lowerBoundLineEdit_->setEnabled(false);
        truncatedDistribution.setFiniteLowerBound(false);
      }
    }
    else if (checkbox == upperBoundCheckBox_)
    {
      if (upperBoundCheckBox_->isChecked())
      {
        double distMean = truncatedDistribution.getMean()[0];
        double distStd = truncatedDistribution.getStandardDeviation()[0];
        truncatedDistribution.setUpperBound(distMean + distStd);
        upperBoundLineEdit_->blockSignals(true);
        upperBoundLineEdit_->setText(QString::number(distMean + distStd));
        upperBoundLineEdit_->blockSignals(false);
        upperBoundLineEdit_->setEnabled(true);
        truncatedDistribution.setFiniteUpperBound(true);
      }
      else
      {
        upperBoundLineEdit_->clear();
        upperBoundLineEdit_->setEnabled(false);
        truncatedDistribution.setFiniteUpperBound(false);
      }
    }
    physicalModel_.blockNotification(true);
    physicalModel_.updateInputDistribution(input.getName(), truncatedDistribution);
    physicalModel_.blockNotification(false);

    updatePlots(truncatedDistribution);
  }
}
}
