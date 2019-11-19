//                                               -*- C++ -*-
/**
 *  @brief QStackedWidget to define marginals parameters
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "persalys/MarginalsWidget.hxx"

#include "persalys/ComboBoxDelegate.hxx"
#include "persalys/DistributionDictionary.hxx"
#include "persalys/CollapsibleGroupBox.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/GraphConfigurationWidget.hxx"
#include "persalys/WidgetBoundToDockWidget.hxx"
#include "persalys/TranslationManager.hxx"
#include "persalys/CheckableHeaderView.hxx"
#include "persalys/InferenceResultWizard.hxx"
#include "persalys/StudyItem.hxx"
#include "persalys/DocumentationToolButton.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/EditValuesWizard.hxx"

#ifdef PERSALYS_HAVE_OTMORRIS
#include "persalys/ScreeningResultWizard.hxx"
#endif

#include <openturns/Normal.hxx>
#include <openturns/TruncatedDistribution.hxx>
#include <openturns/TruncatedNormal.hxx>

#include <QSplitter>
#include <QScrollArea>
#include <QPushButton>
#include <QMessageBox>
#include <QDesktopServices>
#include <openturns/UserDefined.hxx>

using namespace OT;

namespace PERSALYS
{

MarginalsWidget::MarginalsWidget(PhysicalModelItem * item, QWidget * parent)
  : QWidget(parent)
  , failSoftMode_(false)
  , study_(item->getParentStudyItem()->getStudy())
  , physicalModel_(item->getPhysicalModel())
{
  for (UnsignedInteger i = 0; i < 5; ++i)
  {
    parameterValuesLabel_[i] = 0;
    parameterValuesEdit_[i] = 0;
  }
  connect(item, SIGNAL(stochasticInputListChanged()), this, SLOT(updateProbabilisticModel()));
  connect(item, SIGNAL(inputListDefinitionChanged()), this, SLOT(updateCurrentVariableDistributionWidgets()));

  buildInterface();
}


MarginalsWidget::MarginalsWidget(const PhysicalModel& model, QWidget * parent)
  : QWidget(parent)
  , failSoftMode_(true)
  , study_()
  , physicalModel_(model)
{
  for (UnsignedInteger i = 0; i < 5; ++i)
  {
    parameterValuesLabel_[i] = 0;
    parameterValuesEdit_[i] = 0;
  }
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
  inputTableView_->setStyleSheet("QTableView::item:selected{background-color: " + ApplicationColor["lightColor"] + ";color: doubledarkgray;}");

  // - model
  inputTableModel_ = new InputTableProbabilisticModel(physicalModel_, failSoftMode_, inputTableView_);
  inputTableView_->setModel(inputTableModel_);

  if (!failSoftMode_)
  {
    // - header view
    CheckableHeaderView * inputTableHeaderView = new CheckableHeaderView;
    inputTableView_->setHorizontalHeader(inputTableHeaderView);

    // - delegate for distributions list
    ComboBoxDelegate * delegate = new ComboBoxDelegate(inputTableView_);
    delegate->setNoWheelEvent(true);
    const int continuousDistNb = DistributionDictionary::ContinuousDistributions_.size();
    delegate->addSeparatorIndex(0, tr("Continuous"));
    delegate->addSeparatorIndex(continuousDistNb + 1, tr("Discrete"));
    delegate->addSeparatorIndex(continuousDistNb + DistributionDictionary::DiscreteDistributions_.size() + 2, tr("Use result"));
    inputTableView_->setItemDelegateForColumn(1, delegate);
    inputTableView_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  }
  inputTableView_->horizontalHeader()->setStretchLastSection(true);

  // - connections
  connect(inputTableView_->selectionModel(), SIGNAL(currentRowChanged(QModelIndex, QModelIndex)), this, SLOT(updateDistributionWidgets(QModelIndex, QModelIndex)));

  connect(inputTableModel_, SIGNAL(distributionChanged(const QModelIndex&)), this, SLOT(updateDistributionWidgets(const QModelIndex&)));
  connect(inputTableModel_, SIGNAL(distributionsChanged()), this, SIGNAL(updateDependenciesRequested()));
  connect(inputTableModel_, SIGNAL(distributionsChanged()), this, SLOT(updateCurrentVariableDistributionWidgets()));
  connect(inputTableModel_, SIGNAL(inferenceResultRequested(const QModelIndex&)), this, SLOT(openWizardToChooseInferenceResult(const QModelIndex&)));

  leftSideLayout->addWidget(inputTableView_);

#ifdef PERSALYS_HAVE_OTMORRIS
  if (!failSoftMode_ && !physicalModel_.hasMesh())
  {
    // import Morris result button
    QPushButton * importButton = new QPushButton(tr("Import Morris result"));
    connect(importButton, SIGNAL(clicked(bool)), this, SLOT(openWizardToChooseScreeningResult()));
    leftSideLayout->addWidget(importButton);
  }
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
  valueForDeterministicVariable_->setObjectName("valueDeterministicVar");
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
  ResizableStackedWidget * plotStackedWidget = new ResizableStackedWidget;
  QVector<PlotWidget*> listPlotWidgets;

  pdfPlot_ = new PlotWidget(tr("distributionPDF"));
  plotStackedWidget->addWidget(pdfPlot_);
  listPlotWidgets.append(pdfPlot_);

  cdfPlot_ = new PlotWidget(tr("distributionCDF"));
  plotStackedWidget->addWidget(cdfPlot_);
  listPlotWidgets.append(cdfPlot_);

  quantilePlot_ = new PlotWidget(tr("distributionQuantile"));
  plotStackedWidget->addWidget(quantilePlot_);
  listPlotWidgets.append(quantilePlot_);

  survivalPlot_ = new PlotWidget(tr("distributionSurvivalFunction"));
  plotStackedWidget->addWidget(survivalPlot_);
  listPlotWidgets.append(survivalPlot_);

  PDFGraphSetting * plotsSettingWidget = new PDFGraphSetting(listPlotWidgets, PDFGraphSetting::Distribution, this);
  connect(plotsSettingWidget, SIGNAL(currentPlotChanged(int)), plotStackedWidget, SLOT(setCurrentIndex(int)));

  rightFrameLayout->addWidget(new WidgetBoundToDockWidget(plotStackedWidget, plotsSettingWidget, this), 1);

  if (!failSoftMode_)
  {
    // button to open the OT documentation
    DocumentationToolButton * infoButton = new DocumentationToolButton("", FileTools::docOT);
    connect(infoButton, SIGNAL(clicked()), this, SLOT(openUrl()));
    rightFrameLayout->addWidget(infoButton);
  }

  // parameters
  QGroupBox * paramEditor = new QGroupBox(tr("Parameters"));
  QGridLayout * lay = new QGridLayout(paramEditor);

  // combobox to choose the type of distribution configuration
  QLabel * typeLabel = new QLabel(tr("Type"));
  lay->addWidget(typeLabel, 0, 0);

  selectParametersTypeCombo_ = new QComboBox;
  selectParametersTypeCombo_->setObjectName("paramTypeCombo");
  connect(selectParametersTypeCombo_, SIGNAL(currentIndexChanged(int)), this, SLOT(typeDistributionParametersChanged(int)));
  lay->addWidget(selectParametersTypeCombo_, 0, 1, 1, 2);
  if (failSoftMode_)
  {
    typeLabel->hide();
    selectParametersTypeCombo_->hide();
  }

  // lineEdits to set the distribution parameters
  for (UnsignedInteger i = 0; i < 5; ++i)
  {
    parameterValuesLabel_[i] = new QLabel;
    parameterValuesEdit_[i] = new ValueLineEdit;
    parameterValuesEdit_[i]->setObjectName(QString("paramValueEdit_%1").arg(i));
    connect(parameterValuesEdit_[i], SIGNAL(editingFinished()), this, SLOT(distributionParametersChanged()));
    parameterValuesLabel_[i]->setBuddy(parameterValuesEdit_[i]);
    lay->addWidget(parameterValuesLabel_[i], i + 1, 0);
    lay->addWidget(parameterValuesEdit_[i], i + 1, 1);
  }
  editButton_ = new QToolButton;
  editButton_->setText("...");
  lay->addWidget(editButton_, 1, 2);
  connect(editButton_, SIGNAL(pressed()), this, SLOT(openValuesDefinitionWizard()));

  rightFrameLayout->addWidget(paramEditor);

  // truncation parameters
  if (!failSoftMode_)
  {
    truncationParamGroupBox_ = new CollapsibleGroupBox;
    truncationParamGroupBox_->setTitle(tr("Truncation parameters"));
    QGridLayout * truncationParamGroupBoxLayout = new QGridLayout(truncationParamGroupBox_);

    lowerBoundCheckBox_ = new QCheckBox(tr("Lower bound"));
    truncationParamGroupBoxLayout->addWidget(lowerBoundCheckBox_, 0, 0);
    upperBoundCheckBox_ = new QCheckBox(tr("Upper bound"));
    truncationParamGroupBoxLayout->addWidget(upperBoundCheckBox_, 1, 0);

    lowerBoundLineEdit_ = new ValueLineEdit;
    lowerBoundLineEdit_->setObjectName("lowerBound");
    truncationParamGroupBoxLayout->addWidget(lowerBoundLineEdit_, 0, 1);
    upperBoundLineEdit_ = new ValueLineEdit;
    upperBoundLineEdit_->setObjectName("upperBound");
    truncationParamGroupBoxLayout->addWidget(upperBoundLineEdit_, 1, 1);

    connect(lowerBoundCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(truncationParametersStateChanged()));
    connect(upperBoundCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(truncationParametersStateChanged()));
    connect(lowerBoundLineEdit_, SIGNAL(editingFinished()), this, SLOT(truncationParametersChanged()));
    connect(upperBoundLineEdit_, SIGNAL(editingFinished()), this, SLOT(truncationParametersChanged()));

    rightFrameLayout->addWidget(truncationParamGroupBox_);
  }
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
  const QString link = FileTools::OpenTURNSUrlLink + "user_manual/_generated/openturns." + QString(distName.c_str()) + ".html";
  QDesktopServices::openUrl(QUrl(link));
}


void MarginalsWidget::updateProbabilisticModel()
{
  if (!inputTableModel_)
    return;

  // update stochastic inputs table
  inputTableModel_->updateData();

  // - show combo box
  if (!failSoftMode_)
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


void MarginalsWidget::updateDistributionWidgets(const QModelIndex & index, const QModelIndex & /*prevIndex*/)
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

  // block signals
  SignalBlocker comboBlocker(selectParametersTypeCombo_);
  for (UnsignedInteger i = 0; i < 5; ++i)
  {
    SignalBlocker blocker(parameterValuesEdit_[i]);
  }

  // get parameters
  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();
  Distribution::PointWithDescriptionCollection parameters = DistributionDictionary::GetParametersCollection(inputDist);
  const UnsignedInteger parametersType = input.getDistributionParametersType();
  UnsignedInteger nbParameters = parametersType < parameters.getSize() ? parameters[parametersType].getSize() : 0;

  const String distName = inputDist.getImplementation()->getClassName();
  if (distName == "TruncatedDistribution")
  {
    TruncatedDistribution dist(*dynamic_cast<TruncatedDistribution*>(inputDist.getImplementation().get()));
    parameters = DistributionDictionary::GetParametersCollection(dist.getDistribution().getImplementation());
    nbParameters = parametersType < parameters.getSize() ? parameters[parametersType].getSize() : 0;
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
  selectParametersTypeCombo_->clear();
  for (UnsignedInteger i = 0; i < parameters.getSize(); ++i)
  {
    QStringList parametersNamesList;
    for (UnsignedInteger j = 0; j < parameters[i].getDescription().getSize(); ++j)
      parametersNamesList << TranslationManager::GetTranslatedDistributionParameterName(parameters[i].getDescription()[j]);

    selectParametersTypeCombo_->addItem(parametersNamesList.join(", "));
  }
  selectParametersTypeCombo_->setCurrentIndex(parametersType);

  // lineEdits to set the distribution parameters
  const Description parametersName = parameters[parametersType].getDescription();
  for (UnsignedInteger i = 0; i < 5; ++i)
  {
    if (i < nbParameters)
    {
      parameterValuesLabel_[i]->setText(TranslationManager::GetTranslatedDistributionParameterName(parametersName[i]));
      parameterValuesLabel_[i]->show();
      editButton_->hide();
      if (distName != "UserDefined")
      {
        parameterValuesEdit_[i]->setValue(parameters[parametersType][i]);
        parameterValuesEdit_[i]->setReadOnly(false);
      }
      else
      {
        QString text;
        const int nbValues = inputDist.getParametersCollection()[0].getSize();
        for (int j = 0; j < nbValues; ++j)
        {
          text += QString::number(inputDist.getParametersCollection()[i][j], 'g', StudyTreeViewModel::DefaultSignificantDigits);
          if (j < nbValues-1)
            text += ", ";
        }
        parameterValuesEdit_[i]->setText(text);
        editButton_->show();
        parameterValuesEdit_[i]->setReadOnly(true);
      }
      parameterValuesEdit_[i]->show();
    }
    else
    {
      parameterValuesLabel_[i]->hide();
      parameterValuesEdit_[i]->hide();
    }
  }
  if (failSoftMode_)
    parameterValuesEdit_[0]->setDisabled(true);
}


void MarginalsWidget::updateTruncationParametersWidgets(const QModelIndex & index)
{
  if (failSoftMode_ || !index.isValid())
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
  truncationParamGroupBox_->setVisible(!input.getDistribution().isDiscrete());

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
  quantilePlot_->clear();
  survivalPlot_->clear();
  pdfPlot_->plotPDFCurve(input.getDistribution());
  pdfPlot_->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(input.getName().c_str()));
  cdfPlot_->plotCDFCurve(input.getDistribution());
  cdfPlot_->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(input.getName().c_str()));
  quantilePlot_->plotQuantileCurve(input.getDistribution());
  quantilePlot_->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(input.getName().c_str()));
  survivalPlot_->plotSurvivalCurve(input.getDistribution());
  survivalPlot_->setAxisTitle(QwtPlot::xBottom, QString::fromUtf8(input.getName().c_str()));
}


void MarginalsWidget::distributionParametersChanged()
{
  errorMessageLabel_->reset();
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();
  const String distName = inputDist.getImplementation()->getClassName();
  const UnsignedInteger parametersType = input.getDistributionParametersType();

  // do nothing if UserDefined : parameters are read only
  if (distName == "UserDefined")
    return;

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

      // set truncated Distribution bounds: workaround to fix plots creation
      // OT use the bounds to get plot points even if the bounds are not finite
      Interval distInterval(truncatedDistribution.getBounds());
      Interval truncatureInterval(distribution.getRange());

      if (!distInterval.getFiniteLowerBound()[0])
      {
        distInterval.setLowerBound(Point(1, truncatureInterval.getLowerBound()[0]));
        distInterval.setFiniteLowerBound(Interval::BoolCollection(1, false));
      }
      if (!distInterval.getFiniteUpperBound()[0])
      {
        distInterval.setUpperBound(Point(1, truncatureInterval.getUpperBound()[0]));
        distInterval.setFiniteUpperBound(Interval::BoolCollection(1, false));
      }

      // create a new TruncatedDistribution
      TruncatedDistribution newTruncatedDistribution(distribution, distInterval);

      // update input distribution
      physicalModel_.blockNotification("ProbabilisticModelItem");
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
      physicalModel_.blockNotification("ProbabilisticModelItem");
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
  errorMessageLabel_->reset();
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();

  try
  {
    physicalModel_.blockNotification("ProbabilisticModelItem");
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
      Interval bounds(truncatedDistribution.getBounds());
      if (lowerBoundCheckBox_->isChecked())
        bounds.setLowerBound(Point(1, lowerBoundLineEdit_->value()));
      if (upperBoundCheckBox_->isChecked())
        bounds.setUpperBound(Point(1, upperBoundLineEdit_->value()));
      truncatedDistribution.setBounds(bounds);

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
  errorMessageLabel_->reset();
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();
  String distName = inputDist.getImplementation()->getClassName();

  truncationParamGroupBox_->setExpanded(true);
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
    if (!lowerBoundCheckBox_->isChecked() && !upperBoundCheckBox_->isChecked())
    {
      if (distName != "TruncatedDistribution" && distName != "TruncatedNormal")
      {
        qDebug() << "Error: MarginalsWidget::truncationParametersStateChanged the distribution was not truncated!\n";
        throw;
      }
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

    physicalModel_.blockNotification("ProbabilisticModelItem");
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
    physicalModel_.blockNotification("ProbabilisticModelItem");
    physicalModel_.setDistribution(input.getName(), wizard.getDistribution());
    physicalModel_.setDistributionParametersType(input.getName(), 0);
    physicalModel_.blockNotification();
    updateDistributionWidgets(inputIndex);
  }
}


#ifdef PERSALYS_HAVE_OTMORRIS
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
    physicalModel_.blockNotification("ProbabilisticModelItem");
    for (UnsignedInteger i = 0; i < physicalModel_.getInputDimension(); ++i)
    {
      const Input input(physicalModel_.getInputs()[i]);
      if (selectedInputs[i] == 0 && input.isStochastic())
      {
        physicalModel_.setInputStochastic(input.getName(), false);
      }
    }
    physicalModel_.blockNotification();
    updateProbabilisticModel();
  }
}
#endif


void MarginalsWidget::openValuesDefinitionWizard()
{
  const QModelIndex index = inputTableView_->currentIndex();
  const Input input(physicalModel_.getInputs()[index.row()]);
  Distribution inputDist = input.getDistribution();
  String distName = inputDist.getImplementation()->getClassName();

  UserDefinedWizard wizard(inputDist.getParametersCollection(), this);
  if (wizard.exec())
  {
    // update the input
    physicalModel_.blockNotification("ProbabilisticModelItem");
    physicalModel_.setDistribution(input.getName(), wizard.getDistribution());
    physicalModel_.setDistributionParametersType(input.getName(), 0);
    physicalModel_.blockNotification();
    updateDistributionWidgets(index);
  }
}
}
