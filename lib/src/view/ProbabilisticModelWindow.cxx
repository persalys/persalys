//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a probabilistic model
 *
 *  Copyright 2015-2016 EDF
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/ProbabilisticModelWindow.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/SpinBoxDelegate.hxx"
#include "otgui/InputTableProbabilisticModel.hxx"
#include "otgui/DistributionDictionary.hxx"
#include "otgui/CorrelationTableModel.hxx"
#include "otgui/CustomDoubleValidator.hxx"

#include "Normal.hxx"
#include "TruncatedDistribution.hxx"
#include "TruncatedNormal.hxx"

#include <QSplitter>
#include <QHeaderView>
#include <QScrollArea>
#include <QPushButton>
#include <QStackedLayout>

using namespace OT;

namespace OTGUI {

ProbabilisticModelWindow::ProbabilisticModelWindow(ProbabilisticModelItem * item)
  : OTguiSubWindow(item)
  , physicalModel_(item->getPhysicalModel())
  , paramEditor_(0)
{
  connect(item, SIGNAL(inputChanged()), this, SLOT(updateProbabilisticModel()));
  connect(item, SIGNAL(physicalModelChanged(const PhysicalModel&)), this, SLOT(updatePhysicalModel(const PhysicalModel&)));
  buildInterface();
  connect(this, SIGNAL(windowStateChanged(Qt::WindowStates, Qt::WindowStates)), this, SLOT(showHideGraphConfigurationWidget(Qt::WindowStates, Qt::WindowStates)));
}


void ProbabilisticModelWindow::buildInterface()
{
  QTabWidget * rootTab = new QTabWidget;
  
  // First Tab: marginals
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QSplitter * horizontalSplitter = new QSplitter;

  // Inputs table
  inputTableView_ = new QTableView;
  QStringList items;
  Description listDistributions = DistributionDictionary::GetAvailableDistributions();
  for (UnsignedInteger i=0; i<listDistributions.getSize(); ++i)
    items << tr(listDistributions[i].c_str());

  ComboBoxDelegate * delegate = new ComboBoxDelegate(items);
  inputTableView_->setItemDelegateForColumn(1, delegate);
  inputTableView_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  inputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);

  inputTableModel_ = new InputTableProbabilisticModel(physicalModel_);
  inputTableView_->setModel(inputTableModel_);
  inputTableView_->resizeColumnToContents(0);

  for (int i = 0; i < inputTableModel_->rowCount(); ++i)
    inputTableView_->openPersistentEditor(inputTableModel_->index(i, 1));

  connect(inputTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));
  connect(inputTableModel_, SIGNAL(distributionChanged(const QModelIndex&)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));
  connect(inputTableModel_, SIGNAL(correlationToChange()), this, SLOT(updateCorrelationTable()));

  horizontalSplitter->addWidget(inputTableView_);

  // Distribution edition
  QWidget * rightSideOfSplitterWidget = new QWidget;
  rightSideOfSplitterStackedLayout_ = new QStackedLayout(rightSideOfSplitterWidget);

  // If physical model is not valid: use a dummy widget
  rightSideOfSplitterStackedLayout_->addWidget(new QWidget);

  // If the selected variable is deterministic
  QScrollArea * scrollAreaForDeterministic = new QScrollArea;
  scrollAreaForDeterministic->setWidgetResizable(true);
  QGroupBox * groupBoxParameters = new QGroupBox(tr("Parameters"));
  QGridLayout * groupBoxParametersLayout = new QGridLayout(groupBoxParameters);
  QLabel * valueLabel = new QLabel(tr("Value"));
  groupBoxParametersLayout->addWidget(valueLabel, 0, 0);
  valueForDeterministicVariable_ = new QLineEdit;
  valueForDeterministicVariable_->setEnabled(false);
  groupBoxParametersLayout->addWidget(valueForDeterministicVariable_, 0, 1);
  groupBoxParametersLayout->setRowStretch(1, 1);
  scrollAreaForDeterministic->setWidget(groupBoxParameters);
  rightSideOfSplitterStackedLayout_->addWidget(scrollAreaForDeterministic);

  // If the selected variable is stochastic
  QScrollArea * rightScrollArea = new QScrollArea;
  rightScrollArea->setWidgetResizable(true);
  QFrame * rightFrame = new QFrame;
  QVBoxLayout * rightFrameLayout = new QVBoxLayout(rightFrame);

  //  PDF and CDF graphs
  QStackedLayout * plotLayout = new QStackedLayout(rightFrameLayout);

  QVector<PlotWidget*> listPlotWidgets;
  QWidget * widget = new QWidget;
  QVBoxLayout * vBox = new QVBoxLayout(widget);
  pdfPlot_ = new PlotWidget;
  pdfPlot_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  vBox->addWidget(pdfPlot_, 0, Qt::AlignHCenter|Qt::AlignTop);
  plotLayout->addWidget(widget);
  listPlotWidgets.append(pdfPlot_);

  widget = new QWidget;
  vBox = new QVBoxLayout(widget);
  cdfPlot_ = new PlotWidget;
  cdfPlot_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
  vBox->addWidget(cdfPlot_, 0, Qt::AlignHCenter|Qt::AlignTop);
  plotLayout->addWidget(widget);
  listPlotWidgets.append(cdfPlot_);

  pdf_cdfPlotsConfigurationWidget_ = new GraphConfigurationWidget(listPlotWidgets, QStringList(), QStringList(), GraphConfigurationWidget::PDF);
  connect(pdf_cdfPlotsConfigurationWidget_, SIGNAL(currentPlotChanged(int)), plotLayout, SLOT(setCurrentIndex(int)));
  connect(rootTab, SIGNAL(currentChanged(int)), this, SLOT(showHideGraphConfigurationWidget(int)));

  //  parameters
  parameterLayout_ = new QVBoxLayout;
  parameterLayout_->addWidget(new QWidget);
  rightFrameLayout->addLayout(parameterLayout_);

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
  advancedGroupLayout->addStretch();
  rightFrameLayout->addWidget(advancedGroup_);

  errorMessageLabel_ = new QLabel;
  errorMessageLabel_->setWordWrap(true);
  rightFrameLayout->addWidget(errorMessageLabel_);

  rightScrollArea->setWidget(rightFrame);
  rightSideOfSplitterStackedLayout_->addWidget(rightScrollArea);
  horizontalSplitter->addWidget(rightSideOfSplitterWidget);

  if (inputTableModel_->rowCount())
  {
    inputTableView_->selectRow(0);
    updateDistributionWidgets(inputTableView_->currentIndex());
  }
  else
    updateDistributionWidgets(inputTableModel_->index(-1, 0));

  tabLayout->addWidget(horizontalSplitter);
  rootTab->addTab(tab, tr("Marginals"));

  // Second Tab: correlation
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  QGroupBox * groupBox = new QGroupBox(tr("Spearman's rank (Gaussian Copula)"));
  QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);
  correlationTableView_ = new QTableView;
  SpinBoxDelegate * correlationDelegate = new SpinBoxDelegate;
  correlationTableView_->setItemDelegate(correlationDelegate);
  correlationTableModel_ = new CorrelationTableModel(physicalModel_);
  correlationTableView_->setModel(correlationTableModel_);
  correlationTableView_->setEditTriggers(QAbstractItemView::AllEditTriggers);
  groupBoxLayout->addWidget(correlationTableView_);
  tabLayout->addWidget(groupBox);

  correlationErrorMessage_ = new QLabel;
  correlationErrorMessage_->setWordWrap(true);
  tabLayout->addWidget(correlationErrorMessage_);
  connect(correlationTableModel_, SIGNAL(errorMessageChanged(QString)), this, SLOT(setCorrelationTabErrorMessage(QString)));

  rootTab->addTab(tab, tr("Correlation"));

  currentIndexTab_ = rootTab->currentIndex();
  setWidget(rootTab);
}


void ProbabilisticModelWindow::showHideAdvancedWidgets(bool show)
{
  if (show)
    advancedWidgets_->show();
  else
    advancedWidgets_->hide();
}


void ProbabilisticModelWindow::updatePhysicalModel(const PhysicalModel & physicalModel)
{
  physicalModel_ = physicalModel;
  updateProbabilisticModel();
}


void ProbabilisticModelWindow::updateProbabilisticModel()
{
  // update stochastic inputs table
  updateStochasticInputsTable();

  // update plots and truncation widgets
  if (inputTableModel_->rowCount())
  {
    inputTableView_->selectRow(0);
    updateDistributionWidgets(inputTableView_->currentIndex());
  }
  else
    updateDistributionWidgets(inputTableModel_->index(-1, 0));

  // update correlation table
  updateCorrelationTable();
}


void ProbabilisticModelWindow::updateStochasticInputsTable()
{
  delete inputTableModel_;
  inputTableModel_ = new InputTableProbabilisticModel(physicalModel_);
  inputTableView_->setModel(inputTableModel_);
  for (int i=0; i<inputTableModel_->rowCount(); ++i)
    inputTableView_->openPersistentEditor(inputTableModel_->index(i, 1));
  connect(inputTableModel_, SIGNAL(distributionChanged(const QModelIndex&)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));
  connect(inputTableModel_, SIGNAL(correlationToChange()), this, SLOT(updateCorrelationTable()));
}


void ProbabilisticModelWindow::updateCorrelationTable()
{
  delete correlationTableModel_;
  correlationTableModel_ = new CorrelationTableModel(physicalModel_);
  correlationTableView_->setModel(correlationTableModel_);
  connect(correlationTableModel_, SIGNAL(errorMessageChanged(QString)), this, SLOT(setCorrelationTabErrorMessage(QString)));
}


void ProbabilisticModelWindow::setCorrelationTabErrorMessage(const QString & message)
{
  correlationErrorMessage_->setText(QString("%1%2%3").arg("<font color=red>").arg(message).arg("</font>"));
  setErrorMessage(message);
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
    rightSideOfSplitterStackedLayout_->setCurrentIndex(0);
    return;
  }

  if (index.row() != inputTableView_->currentIndex().row())
    inputTableView_->selectRow(index.row());

  Input input = physicalModel_.getInputs()[index.row()];
  Distribution inputDistribution = input.getDistribution();
  String distributionName = inputDistribution.getImplementation()->getClassName();

  // If the selected variable is deterministic
  if (distributionName == "Dirac")
  {
    rightSideOfSplitterStackedLayout_->setCurrentIndex(1);
    showHideGraphConfigurationWidget(-1);
    valueForDeterministicVariable_->setText(QString::number(input.getValue()));
    return;
  }

  // If the selected variable is stochastic
  rightSideOfSplitterStackedLayout_->setCurrentIndex(2);
  advancedGroup_->setChecked(false);

  Distribution::NumericalPointWithDescriptionCollection parameters = DistributionDictionary::GetParametersCollection(inputDistribution);
  UnsignedInteger parametersType = input.getDistributionParametersType();
  UnsignedInteger nbParameters = parameters[parametersType].getSize();

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

  // fill truncation parameters widgets
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

    parameters = DistributionDictionary::GetParametersCollection(dist->getDistribution().getImplementation());
    nbParameters = parameters[parametersType].getSize();
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

  // fill parameters widgets
  if (nbParameters)
  {
    if (nbParameters >= 5)
      std::cout << "Number of parameters invalid :"<<nbParameters<<std::endl;

    paramEditor_ = new QGroupBox(tr("Parameters"));
    QGridLayout * lay = new QGridLayout(paramEditor_);
    QLabel * label = new QLabel(tr("Type"));
    lay->addWidget(label, 0, 0);
    selectParametersTypeCombo_ = new QComboBox;
    for (UnsignedInteger i=0; i<parameters.getSize(); ++i)
    {
      QStringList paramatersNamesList;
      for (UnsignedInteger j=0; j<parameters[i].getDescription().getSize(); ++j)
      {
        paramatersNamesList << parameters[i].getDescription()[j].c_str();
      }
      selectParametersTypeCombo_->addItem(paramatersNamesList.join(", "));
    }
    selectParametersTypeCombo_->setCurrentIndex(parametersType);
    connect(selectParametersTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTypeDistributionParameters(int)));
    lay->addWidget(selectParametersTypeCombo_, 0, 1);
      
    Description parametersName = parameters[parametersType].getDescription();
    for (UnsignedInteger i=0; i<nbParameters; ++i)
    {
      parameterValuesLabel_[i] = new QLabel(parametersName[i].c_str());
      parameterValuesEdit_[i] = new QLineEdit(QString::number(parameters[parametersType][i]));
      parameterValuesEdit_[i]->setValidator(new CustomDoubleValidator);
      connect(parameterValuesEdit_[i], SIGNAL(editingFinished()), this, SLOT(updateDistributionParameters()));
      parameterValuesLabel_[i]->setBuddy(parameterValuesEdit_[i]);
      lay->addWidget(parameterValuesLabel_[i], i+1, 0);
      lay->addWidget(parameterValuesEdit_[i], i+1, 1);
    }

    parameterLayout_->insertWidget(0, paramEditor_);
  }

  // update plots
  updatePlots();
  showHideGraphConfigurationWidget(0);
}


void ProbabilisticModelWindow::showHideGraphConfigurationWidget(int indexTab)
{
  currentIndexTab_ = indexTab;
  switch (indexTab)
  {
    case 0:
    {
      if (rightSideOfSplitterStackedLayout_->currentIndex() == 2 && (windowState() == 4 || windowState() == 8 || windowState() == 10))
        emit graphWindowActivated(pdf_cdfPlotsConfigurationWidget_);
      else
        emit graphWindowDeactivated(pdf_cdfPlotsConfigurationWidget_);
      break;
    }
    default:
      emit graphWindowDeactivated(pdf_cdfPlotsConfigurationWidget_);
      break;
  }
}


void ProbabilisticModelWindow::showHideGraphConfigurationWidget(Qt::WindowStates oldState, Qt::WindowStates newState)
{
  if (newState == 4 || newState == 8 || newState == 10)
    showHideGraphConfigurationWidget(currentIndexTab_);
  else if (newState == 0 || newState == 1 || newState == 2 || newState == 9)
    emit graphWindowDeactivated(pdf_cdfPlotsConfigurationWidget_);
}


void ProbabilisticModelWindow::updatePlots()
{
  setErrorMessage("");
  QModelIndex index = inputTableView_->currentIndex();
  Input input = physicalModel_.getInputs()[inputTableView_->currentIndex().row()];
  pdfPlot_->clear();
  cdfPlot_->clear();
  pdfPlot_->plotPDFCurve(input.getDistribution());
  pdfPlot_->setAxisTitle(QwtPlot::xBottom, input.getName().c_str());
  cdfPlot_->plotCDFCurve(input.getDistribution());
  pdfPlot_->setAxisTitle(QwtPlot::xBottom, input.getName().c_str());
}


void ProbabilisticModelWindow::updateDistributionParameters()
{
  QModelIndex index = inputTableView_->currentIndex();
  Input input = physicalModel_.getInputs()[inputTableView_->currentIndex().row()];
  Distribution inputDistribution = input.getDistribution();
  String distributionName = inputDistribution.getImplementation()->getClassName();
  UnsignedInteger parametersType = input.getDistributionParametersType();

  if (distributionName == "TruncatedDistribution")
  {
    TruncatedDistribution truncatedDistribution = *dynamic_cast<TruncatedDistribution*>(&*inputDistribution.getImplementation());
    Distribution distribution = truncatedDistribution.getDistribution();
    NumericalPointWithDescription parameters = DistributionDictionary::GetParametersCollection(distribution)[parametersType];
    try
    {
      for (UnsignedInteger i=0; i<parameters.getSize(); ++i)
      {
        QString value = parameterValuesEdit_[i]->text();
        if (value[0].toLower() == 'e' || value.isEmpty() || value.toLower() == "e" || value == "-")
          throw InvalidArgumentException(HERE) << "The value " << value.toStdString() << " is invalid";
        bool ok;
        parameters[i] = value.toDouble(&ok);
        if (!ok)
          throw InvalidArgumentException(HERE) << "The value " << value.toStdString() << " is invalid";
      }

      if (parameters == DistributionDictionary::GetParametersCollection(distribution)[parametersType])
        return;


      DistributionDictionary::UpdateDistribution(distribution, parameters, parametersType);
      truncatedDistribution.setDistribution(distribution);
      physicalModel_.blockNotification(true);
      physicalModel_.setInputDistribution(input.getName(), truncatedDistribution);
      physicalModel_.blockNotification(false);
      updatePlots();
    }
    catch(std::exception & ex)
    {
      std::cerr << "ProbabilisticModelWindow::updateDistribution invalid parameters:"<<parameters<<" for distribution:"<<distributionName<<std::endl;
      updateDistributionWidgets(index);
      setErrorMessage(ex.what());
    }
  }
  else
  {
    NumericalPointWithDescription parameters = DistributionDictionary::GetParametersCollection(inputDistribution)[parametersType];
    UnsignedInteger nbParameters = parameters.getSize();
    if (distributionName == "TruncatedNormal")
      nbParameters = 2;

    try
    {
      for (UnsignedInteger i=0; i<nbParameters; ++i)
      {
        QString value = parameterValuesEdit_[i]->text();
        if (value[0].toLower() == 'e' || value.isEmpty() || value.toLower() == "e" || value == "-")
          throw InvalidArgumentException(HERE) << "The value " << value.toStdString() << " is invalid";
        bool ok;
        parameters[i] = value.toDouble(&ok);
        if (!ok)
          throw InvalidArgumentException(HERE) << "The value " << value.toStdString() << " is invalid";
      }

      if (parameters == DistributionDictionary::GetParametersCollection(inputDistribution)[parametersType])
        return;

      DistributionDictionary::UpdateDistribution(inputDistribution, parameters, parametersType);
      physicalModel_.blockNotification(true);
      physicalModel_.setInputDistribution(input.getName(), inputDistribution);
      physicalModel_.blockNotification(false);
      updatePlots();
    }
    catch(std::exception & ex)
    {
      std::cerr << "ProbabilisticModelWindow::updateDistribution invalid parameters:"<<parameters<<" for distribution:"<<distributionName<<std::endl;
      updateDistributionWidgets(index);
      setErrorMessage(ex.what());
    }
  }
}


void ProbabilisticModelWindow::updateTypeDistributionParameters(int comboIndex)
{
  QModelIndex index = inputTableView_->currentIndex();
  Input input = physicalModel_.getInputs()[inputTableView_->currentIndex().row()];
  physicalModel_.setInputDistributionParametersType(input.getName(), comboIndex);
  updateDistributionWidgets(index);
}


void ProbabilisticModelWindow::truncationParametersChanged()
{
  QModelIndex index = inputTableView_->currentIndex();
  Input input = physicalModel_.getInputs()[index.row()];
  Distribution inputDistribution = input.getDistribution();

  try
  {
    TruncatedDistribution truncatedDistribution = TruncatedDistribution(*dynamic_cast<TruncatedDistribution*>(&*inputDistribution.getImplementation()));

    if (lowerBoundCheckBox_->isChecked())
      truncatedDistribution.setLowerBound(lowerBoundLineEdit_->text().toDouble());
    if (upperBoundCheckBox_->isChecked())
      truncatedDistribution.setUpperBound(upperBoundLineEdit_->text().toDouble());

    physicalModel_.blockNotification(true);
    physicalModel_.setInputDistribution(input.getName(), truncatedDistribution);
    physicalModel_.blockNotification(false);

    updatePlots();
  }
  catch (std::exception & ex)
  {
    std::cerr << "ProbabilisticModelWindow::truncationParametersChanged \n";
    updateDistributionWidgets(index);
    setErrorMessage(ex.what());
  }
}


void ProbabilisticModelWindow::truncationParametersStateChanged()
{
  QModelIndex index = inputTableView_->currentIndex();
  Input input = Input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDistribution = input.getDistribution();
  String distributionName = inputDistribution.getImplementation()->getClassName();

  try
  {
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
      physicalModel_.setInputDistribution(input.getName(), inputDistribution);
      physicalModel_.blockNotification(false);
      updatePlots();
    }
    else
    {
      // update widgets
      Interval truncatureInterval(NumericalPoint(1), NumericalPoint(1), Interval::BoolCollection(1, false), Interval::BoolCollection(1, false));
      Distribution distribution;
      TruncatedDistribution truncatedDistribution;
      if (distributionName == "TruncatedDistribution")
      {
        truncatedDistribution = TruncatedDistribution(*dynamic_cast<TruncatedDistribution*>(&*inputDistribution.getImplementation()));
        distribution = truncatedDistribution.getDistribution();
        truncatureInterval = Interval(truncatedDistribution.getLowerBound(), truncatedDistribution.getUpperBound());
        truncatureInterval.setFiniteLowerBound(Interval::BoolCollection(1, truncatedDistribution.getFiniteLowerBound()));
        truncatureInterval.setFiniteUpperBound(Interval::BoolCollection(1, truncatedDistribution.getFiniteUpperBound()));
      }
      else if (distributionName == "TruncatedNormal")
      {
        TruncatedNormal dist = TruncatedNormal(*dynamic_cast<TruncatedNormal*>(&*inputDistribution.getImplementation()));
        distribution = Normal(dist.getMu(), dist.getSigma());
        truncatureInterval = Interval(dist.getA(), dist.getB());
      }
      else
      {
        distribution = inputDistribution;
        truncatureInterval.setLowerBound(distribution.getRange().getLowerBound());
        truncatureInterval.setUpperBound(distribution.getRange().getUpperBound());
      }

      QCheckBox * checkbox = qobject_cast<QCheckBox*>(sender());
      if (checkbox == lowerBoundCheckBox_)
      {
        if (lowerBoundCheckBox_->isChecked())
        {
            double distMean = distribution.getMean()[0];
            double distStd = distribution.getStandardDeviation()[0];
            truncatureInterval.setLowerBound(NumericalPoint(1, distMean - distStd));
            lowerBoundLineEdit_->blockSignals(true);
            lowerBoundLineEdit_->setText(QString::number(distMean - distStd));
            lowerBoundLineEdit_->blockSignals(false);
            lowerBoundLineEdit_->setEnabled(true);
            truncatureInterval.setFiniteLowerBound(Interval::BoolCollection(1, true));
        }
        else
        {
          lowerBoundLineEdit_->clear();
          lowerBoundLineEdit_->setEnabled(false);
          truncatureInterval.setFiniteLowerBound(Interval::BoolCollection(1, false));
        }
      }
      else if (checkbox == upperBoundCheckBox_)
      {
        if (upperBoundCheckBox_->isChecked())
        {
          double distMean = distribution.getMean()[0];
          double distStd = distribution.getStandardDeviation()[0];
          truncatureInterval.setUpperBound(NumericalPoint(1, distMean + distStd));
          upperBoundLineEdit_->blockSignals(true);
          upperBoundLineEdit_->setText(QString::number(distMean + distStd));
          upperBoundLineEdit_->blockSignals(false);
          upperBoundLineEdit_->setEnabled(true);
          truncatureInterval.setFiniteUpperBound(Interval::BoolCollection(1, true));
        }
        else
        {
          upperBoundLineEdit_->clear();
          upperBoundLineEdit_->setEnabled(false);
          truncatureInterval.setFiniteUpperBound(Interval::BoolCollection(1, false));
        }
      }

      // update Distribution
      //TODO after fixing problem with CTR of TruncatedDistribution(Distribution, Interval) in OT do:
      // truncatedDistribution = TruncatedDistribution(distribution, truncatureInterval)
      if (!(truncatureInterval.getFiniteLowerBound()[0] && truncatureInterval.getFiniteUpperBound()[0]))
      {
        if (truncatureInterval.getFiniteLowerBound()[0])
          truncatedDistribution = TruncatedDistribution(distribution, truncatureInterval.getLowerBound()[0]);
        else
          truncatedDistribution = TruncatedDistribution(distribution, truncatureInterval.getUpperBound()[0], TruncatedDistribution::UPPER);
      }
      else
        truncatedDistribution = TruncatedDistribution(distribution, truncatureInterval.getLowerBound()[0], truncatureInterval.getUpperBound()[0]);

      physicalModel_.blockNotification(true);
      physicalModel_.setInputDistribution(input.getName(), truncatedDistribution);
      physicalModel_.blockNotification(false);

      updatePlots();
    }
  }
  catch (std::exception & ex)
  {
    std::cerr << "ProbabilisticModelWindow::truncationParametersStateChanged \n";
    updateDistributionWidgets(index);
    setErrorMessage(ex.what());
  }
}
}
