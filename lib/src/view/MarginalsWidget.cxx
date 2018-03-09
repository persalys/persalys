//                                               -*- C++ -*-
/**
 *  @brief QStackedWidget to define marginals parameters
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/MarginalsWidget.hxx"

#include "otgui/ComboBoxDelegate.hxx"
#include "otgui/DistributionDictionary.hxx"
#include "otgui/CollapsibleGroupBox.hxx"
#include "otgui/QtTools.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/WidgetBoundToDockWidget.hxx"
#include "otgui/TranslationManager.hxx"
#include "otgui/CheckableHeaderView.hxx"
#include "otgui/InferenceResultWizard.hxx"
#include "otgui/OTStudyItem.hxx"

#ifdef OTGUI_HAVE_OTMORRIS
#include "otgui/ScreeningResultWizard.hxx"
#endif

#include <openturns/Normal.hxx>
#include <openturns/TruncatedDistribution.hxx>
#include <openturns/TruncatedNormal.hxx>
#include <openturns/Dirac.hxx>

#include <QSplitter>
#include <QScrollArea>
#include <QPushButton>
#include <QMessageBox>
#include <QToolButton>
#include <QDesktopServices>
#include <QUrl> // for qt4

using namespace OT;

namespace OTGUI
{

MarginalsWidget::MarginalsWidget(ProbabilisticModelItem * item, QWidget * parent)
  : QWidget(parent)
  , study_(item->getParentOTStudyItem()->getOTStudy())
  , physicalModel_(item->getPhysicalModel())
  , paramEditor_(0)
{
  connect(item, SIGNAL(stochasticInputListChanged()), this, SLOT(updateProbabilisticModel()));
  connect(item, SIGNAL(inputListDefinitionChanged()), this, SLOT(updateCurrentVariableDistributionWidgets()));

  buildInterface();
}


void MarginalsWidget::buildInterface()
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  QSplitter * hSplitter = new QSplitter;

  QWidget * leftSideWidget = new QWidget;
  QVBoxLayout * leftSideLayout = new QVBoxLayout(leftSideWidget);

  // Inputs table
  inputTableView_ = new QTableView;
  inputTableView_->setSelectionMode(QAbstractItemView::SingleSelection);
  inputTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);

  // - model
  inputTableModel_ = new InputTableProbabilisticModel(physicalModel_, inputTableView_);
  inputTableView_->setModel(inputTableModel_);

  // - header view
  CheckableHeaderView * inputTableHeaderView = new CheckableHeaderView;
  inputTableHeaderView->setStretchLastSection(true);
  inputTableView_->setHorizontalHeader(inputTableHeaderView);

  // - delegate for distributions list
  ComboBoxDelegate * delegate = new ComboBoxDelegate(inputTableView_);
  delegate->setNoWheelEvent(true);
  inputTableView_->setItemDelegateForColumn(1, delegate);

  // - connections
  connect(inputTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));
  connect(inputTableModel_, SIGNAL(distributionChanged(const QModelIndex&)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));
  connect(inputTableModel_, SIGNAL(distributionsChanged()), this, SIGNAL(updateDependenciesRequested()));
  connect(inputTableModel_, SIGNAL(distributionsChanged()), this, SLOT(updateCurrentVariableDistributionWidgets()));
  connect(inputTableModel_, SIGNAL(inferenceResultRequested(const QModelIndex&)), this, SLOT(openWizardToChooseInferenceResult(const QModelIndex&)));

  leftSideLayout->addWidget(inputTableView_);

#ifdef OTGUI_HAVE_OTMORRIS
  // import Morris result button
  QPushButton * importButton = new QPushButton(tr("Import Morris result"));
  connect(importButton, SIGNAL(clicked(bool)), this, SLOT(openWizardToChooseScreeningResult()));
  leftSideLayout->addWidget(importButton);
#endif

  hSplitter->addWidget(leftSideWidget);

  // Distribution edition
  rightSideOfSplitterStackedWidget_ = new ResizableStackedWidget;

  // 1- If physical model is not valid: use a dummy widget
  rightSideOfSplitterStackedWidget_->addWidget(new QWidget);

  // 2- If the selected variable is deterministic
  QWidget * rightWidget = new QWidget;
  QVBoxLayout * rightWidgetLayout = new QVBoxLayout(rightWidget);
  QScrollArea * rightScrollArea = new QScrollArea;
  rightScrollArea->setWidgetResizable(true);
  QFrame * rightFrame = new QFrame;
  QVBoxLayout * rightFrameLayout = new QVBoxLayout(rightFrame);

  QGroupBox * valueGroupBox = new QGroupBox(tr("Parameters"));
  QGridLayout * valueGroupBoxLayout = new QGridLayout(valueGroupBox);
  valueGroupBoxLayout->addWidget(new QLabel(tr("Value")), 0, 0);
  valueForDeterministicVariable_ = new ValueLineEdit;
  valueForDeterministicVariable_->setEnabled(false);
  valueGroupBoxLayout->addWidget(valueForDeterministicVariable_, 0, 1);
  valueGroupBoxLayout->setSizeConstraint(QLayout::SetMaximumSize);

  rightFrameLayout->addWidget(valueGroupBox, 0, Qt::AlignTop);
  rightScrollArea->setWidget(rightFrame);
  rightWidgetLayout->addWidget(rightScrollArea);
  rightSideOfSplitterStackedWidget_->addWidget(rightWidget);

  // 3- If the selected variable is stochastic
  rightWidget = new QWidget;
  rightWidgetLayout = new QVBoxLayout(rightWidget);
  rightScrollArea = new QScrollArea;
  rightScrollArea->setWidgetResizable(true);
  rightFrame = new QFrame;
  rightFrameLayout = new QVBoxLayout(rightFrame);

  //  PDF and CDF graphs
  WidgetBoundToDockWidget * plotWidget = new WidgetBoundToDockWidget(this);
  QVBoxLayout * plotWidgetLayout = new QVBoxLayout(plotWidget);

  ResizableStackedWidget * plotStackedWidget = new ResizableStackedWidget;
  QVector<PlotWidget*> listPlotWidgets;

  pdfPlot_ = new PlotWidget(tr("distributionPDF"));
  plotStackedWidget->addWidget(pdfPlot_);
  listPlotWidgets.append(pdfPlot_);

  cdfPlot_ = new PlotWidget(tr("distributionCDF"));
  plotStackedWidget->addWidget(cdfPlot_);
  listPlotWidgets.append(cdfPlot_);

  plotWidgetLayout->addWidget(plotStackedWidget);

  GraphConfigurationWidget * plotsSettingWidget = new GraphConfigurationWidget(listPlotWidgets,
      QStringList(),
      QStringList(),
      GraphConfigurationWidget::PDF,
      this);
  plotWidget->setDockWidget(plotsSettingWidget);
  connect(plotsSettingWidget, SIGNAL(currentPlotChanged(int)), plotStackedWidget, SLOT(setCurrentIndex(int)));
  rightFrameLayout->addWidget(plotWidget, 1);

  // button to open the OT documentation
  QToolButton * infoButton = new QToolButton;
  infoButton->setIcon(QIcon(":/images/documentinfo.png"));
  infoButton->setToolTip(tr("Open the OpenTURNS documentation"));
  connect(infoButton, SIGNAL(clicked()), this, SLOT(openUrl()));
  rightFrameLayout->addWidget(infoButton);

  // parameters
  parameterLayout_ = new QVBoxLayout;
  rightFrameLayout->addLayout(parameterLayout_);

  // truncation parameters
  truncationParamGroupBox_ = new CollapsibleGroupBox;
  truncationParamGroupBox_->setTitle(tr("Truncation parameters"));
  QGridLayout * truncationParamGroupBoxLayout = new QGridLayout(truncationParamGroupBox_);

  lowerBoundCheckBox_ = new QCheckBox(tr("Lower bound"));
  truncationParamGroupBoxLayout->addWidget(lowerBoundCheckBox_, 0, 0);
  upperBoundCheckBox_ = new QCheckBox(tr("Upper bound"));
  truncationParamGroupBoxLayout->addWidget(upperBoundCheckBox_, 1, 0);

  lowerBoundLineEdit_ = new ValueLineEdit;
  truncationParamGroupBoxLayout->addWidget(lowerBoundLineEdit_, 0, 1);
  upperBoundLineEdit_ = new ValueLineEdit;
  truncationParamGroupBoxLayout->addWidget(upperBoundLineEdit_, 1, 1);

  connect(lowerBoundCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(truncationParametersStateChanged()));
  connect(upperBoundCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(truncationParametersStateChanged()));
  connect(lowerBoundLineEdit_, SIGNAL(editingFinished()), this, SLOT(truncationParametersChanged()));
  connect(upperBoundLineEdit_, SIGNAL(editingFinished()), this, SLOT(truncationParametersChanged()));

  rightFrameLayout->addWidget(truncationParamGroupBox_);
  rightFrameLayout->addStretch();

  rightScrollArea->setWidget(rightFrame);
  rightWidgetLayout->addWidget(rightScrollArea);
  rightSideOfSplitterStackedWidget_->addWidget(rightWidget);

  hSplitter->addWidget(rightSideOfSplitterStackedWidget_);
  hSplitter->setStretchFactor(1, 3);

  mainLayout->addWidget(hSplitter, 1);

  // QLabel for temporary error message
  errorMessageLabel_ = new TemporaryLabel;
  mainLayout->addWidget(errorMessageLabel_, 0);

  updateProbabilisticModel();
}


void MarginalsWidget::openUrl()
{
  if (!inputTableView_->currentIndex().isValid())
    return;
  const int currentRow = inputTableView_->currentIndex().row();
  if (currentRow > (int)physicalModel_.getInputDimension() || currentRow < 0)
    return;

  // get current distribution
  const Input input = physicalModel_.getInputs()[currentRow];
  const String distName = input.getDistribution().getImplementation()->getClassName();

  // open url
  const QString link = "http://openturns.github.io/openturns/master/user_manual/_generated/openturns." + QString(distName.c_str()) + ".html";
  QDesktopServices::openUrl(QUrl(link));
}


void MarginalsWidget::updateProbabilisticModel()
{
  if (!inputTableModel_)
    return;

  // update stochastic inputs table
  inputTableModel_->updateData();

  // - show combo box
  for (int i = 0; i < inputTableModel_->rowCount(); ++i)
    inputTableView_->openPersistentEditor(inputTableModel_->index(i, 1));

  // update plots and truncation widgets
  if (inputTableModel_->rowCount())
  {
    inputTableView_->selectRow(0);
    updateDistributionWidgets(inputTableView_->currentIndex());
  }
  else
    updateDistributionWidgets(inputTableView_->model()->index(-1, 0));
}


void MarginalsWidget::updateDistributionWidgets(const QModelIndex & index)
{
  if (!index.isValid())
  {
    rightSideOfSplitterStackedWidget_->setCurrentIndex(0);
    return;
  }

  if (index.row() != inputTableView_->currentIndex().row())
    inputTableView_->selectRow(index.row());

  const Input input = physicalModel_.getInputs()[index.row()];

  // If the selected variable is deterministic
  if (!input.isStochastic())
  {
    rightSideOfSplitterStackedWidget_->setCurrentIndex(1);
    valueForDeterministicVariable_->setValue(input.getValue(), false);
  }
  // If the selected variable is stochastic
  else
  {
    rightSideOfSplitterStackedWidget_->setCurrentIndex(2);

    // fill truncation parameters widgets
    updateTruncationParametersWidgets(index);

    // update parameters widgets
    updateDistributionParametersWidgets(index);

    // update plots
    updatePlots();
  }
}


void MarginalsWidget::updateCurrentVariableDistributionWidgets()
{
  updateDistributionWidgets(inputTableView_->currentIndex());
}


void MarginalsWidget::updateDistributionParametersWidgets(const QModelIndex& index)
{
  if (!index.isValid())
    return;

  // update parameters widgets
  if (paramEditor_)
  {
    parameterLayout_->removeWidget(paramEditor_);
    paramEditor_->disconnect();
    paramEditor_->deleteLater();
    paramEditor_ = 0;
  }

  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();
  Distribution::PointWithDescriptionCollection parameters = DistributionDictionary::GetParametersCollection(inputDist);
  const UnsignedInteger parametersType = input.getDistributionParametersType();
  UnsignedInteger nbParameters = parameters[parametersType].getSize();

  const String distName = inputDist.getImplementation()->getClassName();
  if (distName == "TruncatedDistribution")
  {
    TruncatedDistribution dist(*dynamic_cast<TruncatedDistribution*>(inputDist.getImplementation().get()));
    parameters = DistributionDictionary::GetParametersCollection(dist.getDistribution().getImplementation());
    nbParameters = parameters[parametersType].getSize();
  }
  else if (distName == "TruncatedNormal")
  {
    nbParameters = 2;
  }

  // check nbParameters value
  if (!nbParameters)
    return;

  if (nbParameters >= 5)
    throw InternalException(HERE) << "Error: Number of parameters invalid :" << nbParameters;

  // fill parameters widgets
  paramEditor_ = new QGroupBox(tr("Parameters"));
  QGridLayout * lay = new QGridLayout(paramEditor_);

  // combobox to choose the type of distribution configuration
  lay->addWidget(new QLabel(tr("Type")), 0, 0);

  QComboBox * selectParametersTypeCombo = new QComboBox;
  for (UnsignedInteger i = 0; i < parameters.getSize(); ++i)
  {
    QStringList parametersNamesList;
    for (UnsignedInteger j = 0; j < parameters[i].getDescription().getSize(); ++j)
      parametersNamesList << TranslationManager::GetTranslatedDistributionParameterName(parameters[i].getDescription()[j]);

    selectParametersTypeCombo->addItem(parametersNamesList.join(", "));
  }
  selectParametersTypeCombo->setCurrentIndex(parametersType);
  connect(selectParametersTypeCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(typeDistributionParametersChanged(int)));
  lay->addWidget(selectParametersTypeCombo, 0, 1);

  // lineEdits to set the distribution parameters
  const Description parametersName = parameters[parametersType].getDescription();
  for (UnsignedInteger i = 0; i < nbParameters; ++i)
  {
    parameterValuesLabel_[i] = new QLabel(TranslationManager::GetTranslatedDistributionParameterName(parametersName[i]));
    parameterValuesEdit_[i] = new ValueLineEdit(parameters[parametersType][i]);
    connect(parameterValuesEdit_[i], SIGNAL(editingFinished()), this, SLOT(distributionParametersChanged()));
    parameterValuesLabel_[i]->setBuddy(parameterValuesEdit_[i]);
    lay->addWidget(parameterValuesLabel_[i], i + 1, 0);
    lay->addWidget(parameterValuesEdit_[i], i + 1, 1);
  }
  parameterLayout_->insertWidget(0, paramEditor_);
}


void MarginalsWidget::updateTruncationParametersWidgets(const QModelIndex & index)
{
  if (!index.isValid())
    return;

  SignalBlocker lowerBoundCheckBoxBlocker(lowerBoundCheckBox_);
  SignalBlocker upperBoundCheckBoxBlocker(upperBoundCheckBox_);
  SignalBlocker lowerBoundLineEditBlocker(lowerBoundLineEdit_);
  SignalBlocker upperBoundLineEditBlocker(upperBoundLineEdit_);

  // fill default truncation parameters widgets
  lowerBoundLineEdit_->deactivate();
  upperBoundLineEdit_->deactivate();
  lowerBoundCheckBox_->setChecked(false);
  upperBoundCheckBox_->setChecked(false);
  truncationParamGroupBox_->setExpanded(false);

  const Input input(physicalModel_.getInputs()[index.row()]);
  const String distName = input.getDistribution().getImplementation()->getClassName();
  if (distName != "TruncatedDistribution" && distName != "TruncatedNormal")
    return;

  // fill truncation parameters widgets
  const Interval range(input.getDistribution().getRange());

  lowerBoundCheckBox_->setChecked(range.getFiniteLowerBound()[0]);
  upperBoundCheckBox_->setChecked(range.getFiniteUpperBound()[0]);
  if (range.getFiniteLowerBound()[0])
    lowerBoundLineEdit_->setValue(range.getLowerBound()[0]);
  if (range.getFiniteUpperBound()[0])
    upperBoundLineEdit_->setValue(range.getUpperBound()[0]);

  truncationParamGroupBox_->setExpanded(range.getFiniteLowerBound()[0] || range.getFiniteUpperBound()[0]);
}


void MarginalsWidget::updatePlots()
{
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  pdfPlot_->clear();
  cdfPlot_->clear();
  pdfPlot_->plotPDFCurve(input.getDistribution());
  pdfPlot_->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(input.getName().c_str()));
  cdfPlot_->plotCDFCurve(input.getDistribution());
  cdfPlot_->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(input.getName().c_str()));
}


void MarginalsWidget::distributionParametersChanged()
{
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();
  const String distName = inputDist.getImplementation()->getClassName();
  const UnsignedInteger parametersType = input.getDistributionParametersType();

  if (distName == "TruncatedDistribution")
  {
    TruncatedDistribution truncatedDistribution(*dynamic_cast<TruncatedDistribution*>(inputDist.getImplementation().get()));
    Distribution distribution = truncatedDistribution.getDistribution();
    PointWithDescription parameters = DistributionDictionary::GetParametersCollection(distribution)[parametersType];
    try
    {
      for (UnsignedInteger i = 0; i < parameters.getSize(); ++i)
        parameters[i] = parameterValuesEdit_[i]->value();

      if (parameters == DistributionDictionary::GetParametersCollection(distribution)[parametersType])
        return;

      DistributionDictionary::UpdateDistribution(distribution, parameters, parametersType);

      // create a new TruncatedDistribution (that fixes a bug...)
      TruncatedDistribution newTruncatedDistribution;
      newTruncatedDistribution.setDistribution(distribution);
      newTruncatedDistribution.setFiniteLowerBound(false);
      newTruncatedDistribution.setFiniteUpperBound(false);

      if (truncatedDistribution.getFiniteLowerBound())
        newTruncatedDistribution.setLowerBound(truncatedDistribution.getLowerBound()); // FiniteLowerBound = true

      if (truncatedDistribution.getFiniteUpperBound())
        newTruncatedDistribution.setUpperBound(truncatedDistribution.getUpperBound()); // FiniteUpperBound = true

      // update input distribution
      physicalModel_.blockNotification("ProbabilisticModel");
      physicalModel_.setDistribution(input.getName(), newTruncatedDistribution);
      physicalModel_.blockNotification();
      updatePlots();
    }
    catch(std::exception & ex)
    {
      physicalModel_.blockNotification();
      qDebug() << "MarginalsWidget::distributionParametersChanged invalid parameters:"
               << parameters.__str__().data() << " for distribution:" << distName.data();
      updateDistributionParametersWidgets(index);
      errorMessageLabel_->setTemporaryErrorMessage(ex.what());
    }
  }
  else
  {
    PointWithDescription parameters = DistributionDictionary::GetParametersCollection(inputDist)[parametersType];
    UnsignedInteger nbParameters = parameters.getSize();
    if (distName == "TruncatedNormal")
      nbParameters = 2; // the 2 truncation parameters are updated in truncationParametersChanged

    try
    {
      for (UnsignedInteger i = 0; i < nbParameters; ++i)
        parameters[i] = parameterValuesEdit_[i]->value();

      if (parameters == DistributionDictionary::GetParametersCollection(inputDist)[parametersType])
        return;

      DistributionDictionary::UpdateDistribution(inputDist, parameters, parametersType);
      physicalModel_.blockNotification("ProbabilisticModel");
      physicalModel_.setDistribution(input.getName(), inputDist);
      physicalModel_.blockNotification();
      updatePlots();
    }
    catch(std::exception & ex)
    {
      physicalModel_.blockNotification();
      qDebug() << "MarginalsWidget::distributionParametersChanged invalid parameters:"
               << parameters.__str__().data() << " for distribution:" << distName.data();
      updateDistributionParametersWidgets(index);
      errorMessageLabel_->setTemporaryErrorMessage(ex.what());
    }
  }
}


void MarginalsWidget::typeDistributionParametersChanged(int comboIndex)
{
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  physicalModel_.setDistributionParametersType(input.getName(), comboIndex);
  updateDistributionParametersWidgets(index);
}


void MarginalsWidget::truncationParametersChanged()
{
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();

  try
  {
    physicalModel_.blockNotification("ProbabilisticModel");
    // TruncatedNormal
    if (inputDist.getImplementation()->getClassName() == "TruncatedNormal")
    {
      TruncatedNormal dist(*dynamic_cast<TruncatedNormal*>(inputDist.getImplementation().get()));
      if (lowerBoundCheckBox_->isChecked())
        dist.setA(lowerBoundLineEdit_->value());
      if (upperBoundCheckBox_->isChecked())
        dist.setB(upperBoundLineEdit_->value());

      physicalModel_.setDistribution(input.getName(), dist);
    }
    // TruncatedDistribution
    else
    {
      TruncatedDistribution truncatedDistribution(*dynamic_cast<TruncatedDistribution*>(inputDist.getImplementation().get()));
      if (lowerBoundCheckBox_->isChecked())
        truncatedDistribution.setLowerBound(lowerBoundLineEdit_->value());
      if (upperBoundCheckBox_->isChecked())
        truncatedDistribution.setUpperBound(upperBoundLineEdit_->value());

      physicalModel_.setDistribution(input.getName(), truncatedDistribution);
    }
    physicalModel_.blockNotification();

    updatePlots();
  }
  catch (std::exception & ex)
  {
    physicalModel_.blockNotification();
    qDebug() << "Error: MarginalsWidget::truncationParametersChanged\n";
    updateTruncationParametersWidgets(index);
    errorMessageLabel_->setTemporaryErrorMessage(ex.what());
  }
}


void MarginalsWidget::truncationParametersStateChanged()
{
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();
  String distName = inputDist.getImplementation()->getClassName();

  // update lineEdit widgets
  if (!lowerBoundCheckBox_->isChecked())
    lowerBoundLineEdit_->deactivate();
  if (!upperBoundCheckBox_->isChecked())
    upperBoundLineEdit_->deactivate();

  // find the not truncated distribution
  Interval truncatureInterval(inputDist.getRange());
  if (distName == "TruncatedDistribution")
  {
    TruncatedDistribution dist(*dynamic_cast<TruncatedDistribution*>(inputDist.getImplementation().get()));
    inputDist = dist.getDistribution();
  }
  else if (distName == "TruncatedNormal")
  {
    TruncatedNormal dist(*dynamic_cast<TruncatedNormal*>(inputDist.getImplementation().get()));
    inputDist = Normal(dist.getMu(), dist.getSigma());
  }

  try
  {
    // If the two checkboxes are unchecked : the distribution is not truncated
    if (!lowerBoundCheckBox_->isChecked() && !upperBoundCheckBox_->isChecked() &&
        distName != "TruncatedDistribution" && distName != "TruncatedNormal")
    {
      qDebug() << "Error: MarginalsWidget::truncationParametersStateChanged the distribution was not truncated!\n";
      throw;
    }
    // if at least one checkbox is checked : the distribution is truncated
    else
    {
      // initialize distribution
      distName = inputDist.getImplementation()->getClassName();
      Distribution newDist = DistributionDictionary::BuildDistribution(distName, 0.);
      newDist.setParametersCollection(inputDist.getParametersCollection());
      const double distMean = newDist.getMean()[0];
      const double distStd = newDist.getStandardDeviation()[0];

      // update truncatureInterval according to the checkboxes state
      QCheckBox * checkbox = qobject_cast<QCheckBox*>(sender());

      // lower bound
      if (checkbox == lowerBoundCheckBox_)
      {
        // - checked
        if (lowerBoundCheckBox_->isChecked())
        {
          double lowerBound = distMean - distStd;
          if (truncatureInterval.getFiniteUpperBound()[0] && lowerBound >= truncatureInterval.getUpperBound()[0])
            lowerBound = truncatureInterval.getUpperBound()[0] - 0.1 * distStd;
          truncatureInterval.setLowerBound(Point(1, lowerBound));
          truncatureInterval.setFiniteLowerBound(Interval::BoolCollection(1, true));
          SignalBlocker blocker(lowerBoundLineEdit_);
          lowerBoundLineEdit_->setValue(lowerBound);
        }
        // - unchecked
        else
        {
          truncatureInterval.setLowerBound(Point(1, newDist.getRange().getLowerBound()[0]));
          truncatureInterval.setFiniteLowerBound(Interval::BoolCollection(1, false));
        }
      }
      // upper bound
      else if (checkbox == upperBoundCheckBox_)
      {
        // - checked
        if (upperBoundCheckBox_->isChecked())
        {
          double upperBound = distMean + distStd;
          if (truncatureInterval.getFiniteLowerBound()[0] && upperBound <= truncatureInterval.getLowerBound()[0])
            upperBound = truncatureInterval.getLowerBound()[0] + 0.1 * distStd;
          truncatureInterval.setUpperBound(Point(1, upperBound));
          truncatureInterval.setFiniteUpperBound(Interval::BoolCollection(1, true));
          SignalBlocker blocker(upperBoundLineEdit_);
          upperBoundLineEdit_->setValue(upperBound);
        }
        // - unchecked
        else
        {
          truncatureInterval.setUpperBound(Point(1, newDist.getRange().getUpperBound()[0]));
          truncatureInterval.setFiniteUpperBound(Interval::BoolCollection(1, false));
        }
      }

      // update Distribution
      inputDist = TruncatedDistribution(newDist, truncatureInterval);
    }

    physicalModel_.blockNotification("ProbabilisticModel");
    physicalModel_.setDistribution(input.getName(), inputDist);

    // update plots
    updatePlots();
  }
  catch (std::exception & ex)
  {
    qDebug() << "Error: MarginalsWidget::truncationParametersStateChanged\n";
    updateTruncationParametersWidgets(index);
    errorMessageLabel_->setTemporaryErrorMessage(ex.what());
  }
  physicalModel_.blockNotification();
}


void MarginalsWidget::openWizardToChooseInferenceResult(const QModelIndex& inputIndex)
{
  bool studyHasInferenceResults = false;
  // we need at least one inference analysis result to open the wizard
  for (UnsignedInteger i = 0; i < study_.getAnalyses().getSize(); ++i)
  {
    if (study_.getAnalyses()[i].getImplementation()->getClassName() == "InferenceAnalysis"
        && study_.getAnalyses()[i].hasValidResult())
    {
      studyHasInferenceResults = true;
      break;
    }
  }
  // error message if no inference analyses
  if (!studyHasInferenceResults)
  {
    QMessageBox::critical(this, tr("Error"), tr("The current study has not inference analyses results."));
    return;
  }
  // open a wizard to choose a result
  InferenceResultWizard wizard(study_, this);
  if (wizard.exec())
  {
    // update the input
    const Input input(physicalModel_.getInputs()[inputIndex.row()]);
    physicalModel_.blockNotification("ProbabilisticModel");
    physicalModel_.setDistribution(input.getName(), wizard.getDistribution());
    physicalModel_.setDistributionParametersType(input.getName(), 0);
    physicalModel_.blockNotification();
    updateDistributionWidgets(inputIndex);
  }
}


#ifdef OTGUI_HAVE_OTMORRIS
void MarginalsWidget::openWizardToChooseScreeningResult()
{
  bool studyHasScreeningResults = false;
  // we need at least one screening analysis result to open the wizard
  for (UnsignedInteger i = 0; i < study_.getAnalyses().getSize(); ++i)
  {
    if (study_.getAnalyses()[i].getImplementation()->getClassName() == "MorrisAnalysis"
        && study_.getAnalyses()[i].hasValidResult())
    {
      studyHasScreeningResults = true;
      break;
    }
  }
  // error message if no screening analyses
  if (!studyHasScreeningResults)
  {
    QMessageBox::critical(this, tr("Error"), tr("The current study has not screening analyses results."));
    return;
  }
  // open a wizard to choose a result
  ScreeningResultWizard wizard(study_, physicalModel_, this);
  if (wizard.exec())
  {
    // update the inputs
    const Indices selectedInputs(wizard.getInputsSelection());
    Q_ASSERT(selectedInputs.getSize() == physicalModel_.getInputDimension());
    physicalModel_.blockNotification("ProbabilisticModel");
    for (UnsignedInteger i = 0; i < physicalModel_.getInputDimension(); ++i)
    {
      const Input input(physicalModel_.getInputs()[i]);
      if (selectedInputs[i] == 0 && input.isStochastic())
      {
        physicalModel_.setDistribution(input.getName(), Dirac(input.getValue()));
      }
    }
    physicalModel_.blockNotification();
    updateProbabilisticModel();
  }
}
#endif
}
