//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a FMI physical model
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include "persalys/FMIPhysicalModelWindow.hxx"

#include "persalys/FMIPhysicalModel.hxx"
#include "persalys/CollapsibleGroupBox.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/FileTools.hxx"
#include "persalys/ModelEvaluation.hxx"
#include "persalys/FMUInfo.hxx"
#include "persalys/ParametersTableView.hxx"
#include "persalys/TitledComboBox.hxx"
#include "persalys/SpinBoxDelegate.hxx"
#include "persalys/DifferentiationTableModel.hxx"
#include "persalys/StudyTreeViewModel.hxx"
#include "persalys/FloatDelegate.hxx"


#include <QFileDialog>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QMessageBox>
#include <QApplication>
#include <QTreeView>
#include <QGroupBox>
#include <QScrollArea>
#include <QSet>

using namespace OT;

namespace PERSALYS
{

FMIPhysicalModelWindow::FMIPhysicalModelWindow(PhysicalModelItem * item, QWidget * parent)
  : SubWindow(item, parent)
  , physicalModel_(item->getPhysicalModel())
  , variablesTableModel_(0)
  , errorWidget_(0)
{
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  QVBoxLayout * widgetLayout = new QVBoxLayout(this);

  widgetLayout->addWidget(new TitleLabel(tr("FMI model")));

  tabWidget_ = new QTabWidget;

  QWidget * propertiesWidget = new QWidget;
  scrollArea->setWidget(propertiesWidget);
  QVBoxLayout * propertiesLayout = new QVBoxLayout(propertiesWidget);

  // Widgets to load XML file
  QHBoxLayout * fieldsLayout = new QHBoxLayout;

  fieldsLayout->addWidget(new QLabel(tr("FMU file")));

  FMUfileNameEdit_ = new QLineEdit;
  FMUfileNameEdit_->setReadOnly(true);
  FMUfileNameEdit_->setText(QString::fromUtf8(getFMIPhysicalModel()->getFMUFileName().c_str()));
  fieldsLayout->addWidget(FMUfileNameEdit_);

  QToolButton * selectFileButton = new QToolButton;
  selectFileButton->setText("...");
  selectFileButton->setToolTip(tr("Search file"));
  connect(selectFileButton, SIGNAL(clicked()), this, SLOT(selectImportFileDialogRequested()));
  fieldsLayout->addWidget(selectFileButton);

  propertiesLayout->addLayout(fieldsLayout);

  QStringList namesList;
  namesList << tr("Identifier");
  namesList << tr("FMI version");
  namesList << tr("Tool");
  namesList << tr("Model type");
  namesList << tr("Platform");
  namesList << tr("Author");
  namesList << tr("Version");
  namesList << tr("Copyright");
  namesList << tr("Date/Time");
  namesList << tr("GUID");
  namesList << tr("Number of variables");
  namesList << tr("Causality");
  QStringList valuesList;
  for (int i = 0; i < namesList.size(); ++ i)
    valuesList << QString(128, ' ');

  propertiesTable_ = new ParametersTableView(namesList, valuesList, true, true, false);
  propertiesTable_->hide();
  propertiesLayout->addWidget(propertiesTable_);

  CollapsibleGroupBox * advancedGroupBox = new CollapsibleGroupBox(tr("Advanced parameters"));
  QHBoxLayout * typeLayout = new QHBoxLayout(advancedGroupBox);
  QLabel *fmuTypeLabel = new QLabel(tr("FMU type"));
  typeLayout->addWidget(fmuTypeLabel);
  FMUTypeCombobox_ = new QComboBox;
  FMUTypeCombobox_->addItems({ "auto", "ME", "CS" });
  FMUTypeCombobox_->setCurrentText(getFMIPhysicalModel()->getFMUType().c_str());
  typeLayout->addWidget(FMUTypeCombobox_);
  typeLayout->addStretch();
  propertiesLayout->addWidget(advancedGroupBox);

  propertiesLayout->addStretch();

  tabWidget_->addTab(scrollArea, tr("Properties"));

  QWidget * variablesWidget = new QWidget;

  QVBoxLayout * variablesLayout = new QVBoxLayout(variablesWidget);

  // FMI scheme parameters
  QGroupBox * filterGroupBox = new QGroupBox;
  filterGroupBox->setTitle(tr("Filters"));
  filterGroupBox->setAlignment(Qt::AlignLeft);
  QGridLayout * filterLayout = new QGridLayout(filterGroupBox);

  // search name/description
  searchFieldComboBox_ = new QComboBox;
  searchFieldComboBox_->addItems(QStringList() << tr("Name") << tr("Description"));
  connect(searchFieldComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFilters()));
  filterLayout->addWidget(searchFieldComboBox_, 0, 0);

  // search text
  filterTextEdit_ = new QLineEdit;
  connect(filterTextEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateFilters()));
  filterLayout->addWidget(filterTextEdit_, 0, 1);

  // case sensitivity
  matchCaseCheckBox_ = new QCheckBox(tr("Match case"));
  connect(matchCaseCheckBox_, SIGNAL(stateChanged(int)), this, SLOT(updateFilters()));
  filterLayout->addWidget(matchCaseCheckBox_, 0, 2);

  // variability filter
  QStringList variabilityFields;
  variabilityFields << tr("Constant") << tr("Fixed") << tr("Tunable") << tr("Discrete") << tr("Continuous") << tr("Unknown");
  variabilityField_ = new ListWidgetWithCheckBox(tr("Variability") + QString("     "), variabilityFields);
  TitledComboBox * variabilityFieldComboBox = new TitledComboBox(tr("Variability") + QString("     "));
  variabilityFieldComboBox->setModel(variabilityField_->model());
  variabilityFieldComboBox->setView(variabilityField_);
  filterLayout->addWidget(variabilityFieldComboBox, 0, 3);
  connect(variabilityField_, SIGNAL(checkedItemsChanged(QStringList)), this, SLOT(updateFilters()));

  // causality filter
  QStringList causalityFields;
  causalityFields << tr("Parameter") << tr("Calculated") << tr("Input") << tr("Output") << tr("Local") << tr("Independent") << tr("Unknown");
  causalityField_ = new ListWidgetWithCheckBox(tr("Causality") + QString("         "), causalityFields);
  TitledComboBox * causalityFieldComboBox = new TitledComboBox(tr("Causality") + QString("         "));
  causalityFieldComboBox->setModel(causalityField_->model());
  causalityFieldComboBox->setView(causalityField_);
  filterLayout->addWidget(causalityFieldComboBox, 0, 4);
  connect(causalityField_, SIGNAL(checkedItemsChanged(QStringList)), this, SLOT(updateFilters()));

  // I/O filter
  QStringList ioFields;
  ioFields << tr("Disabled") << tr("Input") << tr("Output");
  ioField_ = new ListWidgetWithCheckBox(tr("I/O") + QString("            "), ioFields);
  TitledComboBox * ioFieldComboBox = new TitledComboBox(tr("I/O") + QString("            "));
  ioFieldComboBox->setModel(ioField_->model());
  ioFieldComboBox->setView(ioField_);
  filterLayout->addWidget(ioFieldComboBox, 0, 5);
  connect(ioField_, SIGNAL(checkedItemsChanged(QStringList)), this, SLOT(updateFilters()));

  // clear filters
  QPushButton * clearButton = new QPushButton(tr("Clear"));
  connect(clearButton, SIGNAL(clicked(bool)), this, SLOT(clearFilters()));
  filterLayout->addWidget(clearButton, 0, 6);

  filterLayout->setColumnStretch(7, 1);

  variablesLayout->addWidget(filterGroupBox);

  tree_model_ = new TreeModel("", this);

  QTreeView * view = new DeselectableTreeView;
  view->setModel(tree_model_);
  connect(view->selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex)), this, SLOT(treeSelect(QModelIndex, QModelIndex)));
  variablesLayout->addWidget(view);

  // variables tables
  variablesTableView_ = new CopyableTableView;
  variablesTableView_->verticalHeader ()->hide();
  variablesTableView_->setEditTriggers(QTableView::AllEditTriggers);
  variablesLayout->addWidget(variablesTableView_);

  // button Evaluate outputs
  ioCountLabel_ = new QLabel;
  QPushButton * evaluateOutputsButton = new QPushButton(QIcon(":/images/system-run.png"), tr("Check model"));
  evaluateOutputsButton->setToolTip(tr("Evaluate the outputs"));
  QLabel * timeInfo = new QLabel();
  connect(evaluateOutputsButton, &QPushButton::clicked,  [ = ] (bool)
  {
    timeInfo->clear();
    evaluateOutputs();
    if(physicalModel_.getEvalTime() > 0)
      timeInfo->setText(tr("Elapsed time") + ": " + QtOT::FormatDuration(physicalModel_.getEvalTime()));
  });
  QGridLayout * evaluationLayout = new QGridLayout;
  evaluationLayout->addWidget(ioCountLabel_, 0, 0);
  evaluationLayout->setColumnStretch(0, 2);
  evaluationLayout->addWidget(timeInfo, 1, 0);
  evaluationLayout->addWidget(evaluateOutputsButton, 0, 1);
  variablesLayout->addLayout(evaluationLayout);

  // error message
  errorWidget_ = new ErrorWidget;
  errorWidget_->setHeight(80); // fixed height with scrollbar because evaluation errors can be long (e.g. FMU tracebacks)
  variablesLayout->addWidget(errorWidget_);

  tabWidget_->addTab(variablesWidget, tr("Variables"));

  // differentiation
  QWidget * diff_tab = new QWidget;
  QVBoxLayout * vbox = new QVBoxLayout(diff_tab);

  QLabel * label = new QLabel(tr("Finite difference step definition"));
  label->setStyleSheet("QLabel {font: bold;}");
  vbox->addWidget(label);

  CopyableTableView * differentiationTableView = new CopyableTableView;
  differentiationTableView->horizontalHeader()->setStretchLastSection(true);

  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(differentiationTableView);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::differentiationStep);
  differentiationTableView->setItemDelegateForColumn(1, spinBoxDelegate);
  differentiationTableView->setEditTriggers(QTableView::AllEditTriggers);

  DifferentiationTableModel * differentiationTableModel  = new DifferentiationTableModel(physicalModel_, differentiationTableView);
  connect(spinBoxDelegate, SIGNAL(applyToAllRequested(double)), differentiationTableModel, SLOT(applyValueToAll(double)));
  connect(item, SIGNAL(numberInputsChanged()), differentiationTableModel, SLOT(updateData()));
  connect(item, SIGNAL(inputListDefinitionChanged()), differentiationTableModel, SLOT(updateData()));
  connect(item, SIGNAL(inputListDifferentiationChanged()), differentiationTableModel, SLOT(updateData()));
  differentiationTableView->setModel(differentiationTableModel);

  vbox->addWidget(differentiationTableView);

  tabWidget_->addTab(diff_tab, tr("Differentiation"));

  ////////////////
  widgetLayout->addWidget(tabWidget_);

  updateVariablesTableModel();

  FMIPhysicalModel* fmiModel = getFMIPhysicalModel();
  if (!fmiModel->getFMUInfo().getFileName().empty())
  {
    loadModel(fmiModel->getFMUInfo());
  }
}


FMIPhysicalModel * FMIPhysicalModelWindow::getFMIPhysicalModel() const
{
  return dynamic_cast<FMIPhysicalModel *>(physicalModel_.getImplementation().get());
}


void FMIPhysicalModelWindow::evaluateOutputs()
{
  if (!physicalModel_.getInputDimension())
  {
    errorWidget_->setFramelessErrorMessage(tr("No inputs"));
    return;
  }

  // if no outputs do nothing
  if (!physicalModel_.getSelectedOutputsNames().getSize())
  {
    errorWidget_->setFramelessErrorMessage(tr("No outputs"));
    return;
  }

  // evaluate
  ModelEvaluation eval("anEval", physicalModel_);
  try
  {
    eval.run();
  }
  catch (const std::exception& ex)
  {
    // do nothing
  }

  // get result
  Sample outputSample(eval.getResult().getDesignOfExperiment().getOutputSample());

  // check
  if (!eval.getErrorMessage().empty())
  {
    errorWidget_->setFramelessErrorMessage(eval.getErrorMessage().c_str());
    physicalModel_.setEvalTime(0);
    return;
  }
  if (!outputSample.getSize())
  {
    errorWidget_->setFramelessErrorMessage(tr("Not possible to evaluate the outputs"));
    physicalModel_.setEvalTime(0);
    return;
  }

  // set output value
  for (UnsignedInteger i = 0; i < outputSample.getDimension(); ++ i)
  {
    physicalModel_.setOutputValue(outputSample.getDescription()[i], outputSample(0, i));
  }
  errorWidget_->reset();
}


void FMIPhysicalModelWindow::selectImportFileDialogRequested()
{
  QString fileName = QFileDialog::getOpenFileName(this, tr("Model to import..."),
                     FileTools::GetCurrentDir(),
                     tr("FMU files (*.fmu);;"));

  if (!fileName.isEmpty())
  {
    QFile file(fileName);
    FileTools::SetCurrentDir(fileName);

    // check
    if (!file.open(QFile::ReadOnly))
    {
      QMessageBox::warning(this, tr("Warning"),
                           tr("Cannot read file %1:\n%2").arg(fileName).arg(file.errorString()));
    }
    else
    {
      FMUfileNameEdit_->setText(fileName);
      FMIPhysicalModel* fmiModel = getFMIPhysicalModel();
      QApplication::setOverrideCursor(Qt::WaitCursor);
      try
      {
        fmiModel->setFMUType(FMUTypeCombobox_->currentText().toStdString());
        fmiModel->setFMUFileName(fileName.toUtf8().data());
        errorWidget_->reset();
        loadModel(fmiModel->getFMUInfo());
      }
      catch (const std::exception & ex)
      {
        errorWidget_->setFramelessErrorMessage(ex.what());
      }
      QApplication::restoreOverrideCursor();
    }
  }
}


void FMIPhysicalModelWindow::loadModel(const FMUInfo & info)
{
  variablesTableModel_->loadData(info);

  // causality labels and I/O rules depend on the FMI version
  causalityField_->resetItemNames(variablesTableModel_->getCausalityNames());
  if (ioDelegate_)
    ioDelegate_->setFMIVersion(QString::fromStdString(variablesTableModel_->getFMIVersion()));

  Description properties(variablesTableModel_->getProperties());
  for(int i = 0; i < propertiesTable_->model()->rowCount(); ++ i)
  {
    propertiesTable_->model()->setData(propertiesTable_->model()->index(i, 1), QString::fromStdString(properties[i]));
  }
  propertiesTable_->show();
  tree_model_->setVariableNames(variablesTableModel_->getVariableNames());
  updateIOCount();
  filterTextEdit_->setText("");
  tabWidget_->setCurrentIndex(1);
}


void FMIPhysicalModelWindow::updateFilters()
{
  QRegularExpression::PatternOptions options = matchCaseCheckBox_->isChecked() ? QRegularExpression::NoPatternOption : QRegularExpression::CaseInsensitiveOption;
  QRegularExpression regExp(QRegularExpression::escape(filterTextEdit_->text()), options);
  proxyModel_->setFilterRegularExpression(regExp);
  proxyModel_->setFilterKeyColumn(searchFieldComboBox_->currentIndex());

  // filter variability
  {
    QStringList names(variabilityField_->getItemNames());
    const QStringList checkedNames(variabilityField_->getCheckedItemNames());
    const QSet<QString> checked(checkedNames.begin(), checkedNames.end());
    QList<int> indices;
    for (int i = 0; i < names.size(); ++i)
      if (checked.contains(names[i]))
        indices.append(i);
    proxyModel_->setVariabilityFilter(indices);
  }

  // filter causality
  {
    QStringList names(causalityField_->getItemNames());
    const QStringList checkedNames(causalityField_->getCheckedItemNames());
    const QSet<QString> checked(checkedNames.begin(), checkedNames.end());
    QList<int> indices;
    for (int i = 0; i < names.size(); ++i)
      if (checked.contains(names[i]))
        indices.append(i);
    proxyModel_->setCausalityFilter(indices);
  }

  // filter I/O
  {
    QStringList names(ioField_->getItemNames());
    const QStringList checkedNames(ioField_->getCheckedItemNames());
    const QSet<QString> checked(checkedNames.begin(), checkedNames.end());
    QList<int> indices;
    for (int i = 0; i < names.size(); ++i)
      if (checked.contains(names[i]))
        indices.append(i);
    proxyModel_->setIOFilter(indices);
  }
}


void FMIPhysicalModelWindow::updateIOCount()
{
  int inVar = 0;
  int outVar = 0;
  Indices io(variablesTableModel_->getIO());
  for (UnsignedInteger i = 0; i < io.getSize(); ++ i)
  {
    switch(io[i])
    {
      case 1:
        ++ inVar;
        break;
      case 2:
        ++ outVar;
        break;
      default:
        break;
    }
  }
  ioCountLabel_->setText(QString(tr("Selected variables: input:%1, output:%2")).arg(inVar).arg(outVar));
}

void FMIPhysicalModelWindow::clearFilters()
{
  searchFieldComboBox_->setCurrentIndex(0);// by name
  filterTextEdit_->clear();
  variabilityField_->setCheckedNames(variabilityField_->getItemNames());
  causalityField_->setCheckedNames(causalityField_->getItemNames());
  ioField_->setCheckedNames(ioField_->getItemNames());
}


void FMIPhysicalModelWindow::treeSelect(const QModelIndex & current, const QModelIndex & /*previous*/)
{
  QModelIndex index(current);
  if (!index.isValid())
  {
    filterTextEdit_->setText("");
    return;
  }

  QString prefix(index.data().toString());
  while (index.parent().isValid())
  {
    index = index.parent();
    prefix.insert(0, '.').insert(0, index.data().toString());
  }
  searchFieldComboBox_->setCurrentIndex(0);// by name
//   matchCaseCheckBox_->setChecked(true);
  filterTextEdit_->setText(prefix);
}



void FMIPhysicalModelWindow::updateVariablesTableModel()
{
  variablesTableModel_ = new DataTableModel(physicalModel_, variablesTableView_);
  connect(variablesTableModel_, SIGNAL(ioCountChanged()), this, SLOT(updateIOCount()));
  connect(dynamic_cast<PhysicalModelItem*>(getItem()), SIGNAL(outputChanged()), variablesTableModel_, SLOT(updateOutputValues()));

  proxyModel_ = new DataFilterProxyModel(this);
  proxyModel_->setSourceModel(variablesTableModel_);
  proxyModel_->setDynamicSortFilter(true);

  variablesTableView_->setModel(proxyModel_);
  ioDelegate_ = new EnumDelegate(QStringList() << tr("Disabled") << tr("Input") << tr("Output"), variablesTableView_);
  variablesTableView_->setItemDelegateForColumn(4, ioDelegate_);

  QItemDelegate * fdelegate = new FloatDelegate(variablesTableView_);
  variablesTableView_->setItemDelegateForColumn(5, fdelegate);

  variablesTableView_->horizontalHeader()->setStretchLastSection(true);

  int width = variablesTableView_->horizontalHeader()->width();
  variablesTableView_->horizontalHeader()->resizeSection(0, width * 1 / 8);
  variablesTableView_->horizontalHeader()->resizeSection(1, width * 3 / 8); //description
  variablesTableView_->horizontalHeader()->resizeSection(2, width * 1 / 8);
  variablesTableView_->horizontalHeader()->resizeSection(3, width * 1 / 8);
  variablesTableView_->horizontalHeader()->resizeSection(4, width * 1 / 8);
  variablesTableView_->horizontalHeader()->resizeSection(5, width * 1 / 8);
}




DataTableModel::DataTableModel(const PhysicalModel & physicalModel, QObject * parent)
  : QAbstractTableModel(parent)
  , physicalModel_(physicalModel)
{
}


int DataTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 6;
}


int DataTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return variableNames_.getSize();
}


QVariant DataTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
  {
    switch (section)
    {
      case 0:
        return tr("Name");
      case 1:
        return tr("Description");
      case 2:
        return tr("Variability");
      case 3:
        return tr("Causality");
      case 4:
        return tr("I/O");
      case 5:
        return tr("Value");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}


QVariant DataTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    switch (index.column())
    {
      case 0://name
        return QString::fromUtf8(variableNames_[index.row()].c_str());
      case 1://description
        // cached: data() must stay O(1) without physical model lookups
        return QString::fromUtf8(descriptions_[index.row()].c_str());
      case 2://variability
      {
        if (role == Qt::DisplayRole)
        {
          switch(variability_[index.row()])
          {
            case 0:
              return tr("Constant");
              break;
            case 1:
              return tr("Fixed");
              break;
            case 2:
              return tr("Tunable");
              break;
            case 3:
              return tr("Discrete");
              break;
            case 4:
              return tr("Continuous");
              break;
            case 5:
              return tr("Unknown");
              break;
            default:
              break;
          }
        }
        return static_cast<int>(variability_[index.row()]);
      }
      case 3://causality
      {
        if (role == Qt::DisplayRole)
          return getCausalityName(causality_[index.row()]);
        return static_cast<int>(causality_[index.row()]);
      }
      case 4://input/output
      {
        if (role == Qt::DisplayRole)
        {
          switch(inputOutput_[index.row()])
          {
            case 0:
              return tr("Disabled");
              break;
            case 1:
              return tr("Input");
              break;
            case 2:
              return tr("Output");
              break;
            default:
              break;
          }
        }
        return static_cast<int>(inputOutput_[index.row()]);
      }
      case 5://value
        // cached: data() must stay O(1) without physical model lookups
        return QString::number(values_[index.row()], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      default:
        break;
    }
  }
  return QVariant();
}


bool DataTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid())
    return false;

  if (role != Qt::EditRole) return false;

  switch (index.column())
  {
    case 1:// description
    {
      String description(value.toString().toUtf8().constData());
      if (description == descriptions_[index.row()]) return false;
      descriptions_[index.row()] = description;
      const String varName(variableNames_[index.row()]);
      physicalModel_.blockNotification("PhysicalModelDefinitionItem");
      const OT::UnsignedInteger io = inputOutput_[index.row()];
      if (io == 1)
        physicalModel_.setInputDescription(varName, description);
      else if (io == 2)
        physicalModel_.setOutputDescription(varName, description);
      physicalModel_.blockNotification();
      break;
    }
    case 4://input/output
    {
      int inputOutput = value.toInt();
      int oldInputOutput = static_cast<int>(inputOutput_[index.row()]);
      if (inputOutput == oldInputOutput) return false;
      const int causality = causality_[index.row()];
      if ((inputOutput < 0) || (inputOutput > 2)) return false;
      // causality codes depend on the FMI version, otfmi accepts inputs with
      // input/parameter causality and outputs with output/local causality
      const UnsignedInteger inputCausality = getInputCausality();
      const UnsignedInteger outputCausality = getOutputCausality();
      if ((inputOutput == 0) && (causality == static_cast<int>(inputCausality))) return false;// cannot disable an input
      if (inputOutput == 1)
      {
        if (causality == static_cast<int>(outputCausality)) return false;// cannot turn an output to input
        if (fmiVersion_ == "3.0")
        {
          if (causality == 0) return false;// cannot turn a structural parameter to input
          if (causality == 2) return false;// cannot turn a calculated parameter to input
          if (causality == 5) return false;// cannot turn a local to input
        }
        else if (fmiVersion_ == "1.0")
        {
          if (causality != static_cast<int>(inputCausality)) return false;// only inputs can be inputs
        }
        else
        {
          if (causality == 1) return false;// cannot turn a calculated parameter to input
          if (causality == 4) return false;// cannot turn a local to input
        }
      }
      if (inputOutput == 2)
      {
        if (causality == static_cast<int>(inputCausality)) return false;// cannot turn an input to output
        if (fmiVersion_ == "3.0")
        {
          if (causality == 0) return false;// cannot turn a structural parameter to output
          if (causality == 1) return false;// cannot turn a parameter to output
        }
        else if (fmiVersion_ == "1.0")
        {
          if (causality != static_cast<int>(outputCausality)) return false;// only outputs can be outputs
        }
        else
        {
          if (causality == 0) return false;// cannot turn a parameter to output
        }
      }

      inputOutput_[index.row()] = inputOutput;

      const String varName(variableNames_[index.row()]);

      physicalModel_.blockNotification("PhysicalModelDefinitionItem");
      dynamic_cast<FMIPhysicalModel*>(physicalModel_.getImplementation().get())->reassignVariables(getInputVariableNames(), getOutputVariableNames());
      // reassignVariables preserves values/descriptions of surviving variables
      // (including dotted names); only the toggled row may need its FMU start
      // value when it becomes newly enabled, or its display value reset when disabled
      {
        const auto it = startValuesMap_.find(varName);
        const double startValue = (it != startValuesMap_.end() ? it->second : 0.0);
        if (inputOutput == 1)
        {
          if (it != startValuesMap_.end())
            physicalModel_.getInputByName(varName).setValue(it->second);
          values_[index.row()] = physicalModel_.getInputByName(varName).getValue();
        }
        else if (inputOutput == 2)
        {
          if (it != startValuesMap_.end())
            physicalModel_.getOutputByName(varName).setValue(it->second);
          values_[index.row()] = physicalModel_.getOutputByName(varName).getValue();
        }
        else
          values_[index.row()] = startValue;
      }
      physicalModel_.blockNotification();

      emit ioCountChanged();

      break;
    }
    case 5://value
    {
      const double dvalue = value.toDouble();
      if (dvalue == values_[index.row()]) return false;
      values_[index.row()] = dvalue;
      const String varName(variableNames_[index.row()]);
      physicalModel_.blockNotification("PhysicalModelDefinitionItem");
      const OT::UnsignedInteger io = inputOutput_[index.row()];
      if (io == 1)
        physicalModel_.getInputByName(varName).setValue(dvalue);
      else if (io == 2)
        physicalModel_.getOutputByName(varName).setValue(dvalue);
      else
        startValuesMap_[varName] = dvalue;
      physicalModel_.blockNotification();
      break;
    }
  }
  emit dataChanged(index, index);
  return true;

}


void DataTableModel::updateOutputValues()
{
  // resync cached values for outputs in O(nIO) via row lookup
  const OutputCollection outputs(physicalModel_.getOutputs());
  for (OT::UnsignedInteger i = 0; i < outputs.getSize(); ++ i)
  {
    const auto it = rowByName_.find(outputs[i].getName());
    if (it != rowByName_.end())
      values_[it->second] = outputs[i].getValue();
  }
  // refresh value column
  QModelIndex topleft = createIndex(0, 5);
  QModelIndex bottomright = createIndex(rowCount(), 5);
  emit dataChanged(topleft, bottomright);
}



Qt::ItemFlags DataTableModel::flags(const QModelIndex & index) const
{
  if ((index.column() == 1) || (index.column() == 4) || (index.column() == 5))
    return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
  return QAbstractTableModel::flags(index);
}



void DataTableModel::loadData(const FMUInfo & info)
{
  beginResetModel();
  variableNames_ = info.getVariableNames();
  descriptions_ = Description(variableNames_.getSize());
  values_ = Point(variableNames_.getSize(), 0.0);
  causality_ = info.getCausality();
  variability_ = info.getVariability();
  fmiVersion_ = info.getFMIVersion();
  inputOutput_ = Indices(causality_.getSize());
  startKeys_ = info.getStartKeys();
  startValues_ = info.getStartValues();
  startValuesMap_.clear();
  for (UnsignedInteger i = 0; i < startKeys_.getSize(); ++ i)
    startValuesMap_[startKeys_[i]] = startValues_[i];
  rowByName_.clear();
  for (UnsignedInteger i = 0; i < variableNames_.getSize(); ++ i)
    rowByName_[variableNames_[i]] = i;
  // Build IO lookups once: per-row hasInputNamed()/getInputByName()
  // scans made loadData O(N * nIO). Hoist collections to avoid
  // per-iteration copies.
  const InputCollection inputs(physicalModel_.getInputs());
  const OutputCollection outputs(physicalModel_.getOutputs());
  std::unordered_map<std::string, std::string> inputDescriptions;
  std::unordered_map<std::string, double> inputValues;
  for (UnsignedInteger i = 0; i < inputs.getSize(); ++ i)
  {
    inputDescriptions[inputs[i].getName()] = inputs[i].getDescription();
    inputValues[inputs[i].getName()] = inputs[i].getValue();
  }
  std::unordered_map<std::string, std::string> outputDescriptions;
  std::unordered_map<std::string, double> outputValues;
  for (UnsignedInteger i = 0; i < outputs.getSize(); ++ i)
  {
    outputDescriptions[outputs[i].getName()] = outputs[i].getDescription();
    outputValues[outputs[i].getName()] = outputs[i].getValue();
  }
  Indices causalityCount(8);
  for (UnsignedInteger i = 0; i < variableNames_.getSize(); ++ i)
  {
    int causality = causality_[i];
    if (causality > 7) throw InternalException(HERE) << "Invalid causality value: " << causality;
    ++ causalityCount[causality];

    const auto itIn = inputDescriptions.find(variableNames_[i]);
    if (itIn != inputDescriptions.end())
    {
      inputOutput_[i] = 1;
      descriptions_[i] = itIn->second;
      values_[i] = inputValues[variableNames_[i]];
      continue;
    }
    const auto itOut = outputDescriptions.find(variableNames_[i]);
    if (itOut != outputDescriptions.end())
    {
      inputOutput_[i] = 2;
      descriptions_[i] = itOut->second;
      values_[i] = outputValues[variableNames_[i]];
      continue;
    }
    const auto itStart = startValuesMap_.find(variableNames_[i]);
    if (itStart != startValuesMap_.end())
      values_[i] = itStart->second;
  }
  properties_.clear();
  properties_.add(info.getIdentifier());
  properties_.add(info.getFMIVersion());
  properties_.add(info.getTool());
  properties_.add(info.getModelType());
  properties_.add(info.getPlatform());
  properties_.add(info.getAuthor());
  properties_.add(info.getVersion());
  properties_.add(info.getCopyright());
  properties_.add(info.getDateTime());
  properties_.add(info.getGUID());
  properties_.add(OSS() << variableNames_.getSize());
  if (fmiVersion_ == "1.0")
  {
    properties_.add(QString(tr("input:%1, output:%2")).arg(causalityCount[getInputCausality()]).arg(causalityCount[getOutputCausality()]).toStdString());
  }
  else
  {
    // FMI 3.0 structural parameters counted with parameters; local is 4 for FMI 2.0, 5 for FMI 3.0
    const UnsignedInteger parameterCount = (fmiVersion_ == "3.0" ? causalityCount[0] + causalityCount[1] : causalityCount[0]);
    const UnsignedInteger localCausality = (fmiVersion_ == "3.0" ? 5 : 4);
    properties_.add(QString(tr("parameter:%1, input:%2, output:%3, local:%4")).arg(parameterCount).arg(causalityCount[getInputCausality()]).arg(causalityCount[getOutputCausality()]).arg(causalityCount[localCausality]).toStdString());
  }
  endResetModel();
}


Description DataTableModel::getVariableNames() const
{
  return variableNames_;
}

Indices DataTableModel::getIO() const
{
  return inputOutput_;
}

Description DataTableModel::getProperties() const
{
  return properties_;
}

String DataTableModel::getFMIVersion() const
{
  return fmiVersion_;
}

UnsignedInteger DataTableModel::getInputCausality() const
{
  if (fmiVersion_ == "3.0")
    return 3;
  if (fmiVersion_ == "1.0")
    return 0;
  return 2;
}

UnsignedInteger DataTableModel::getOutputCausality() const
{
  if (fmiVersion_ == "3.0")
    return 4;
  if (fmiVersion_ == "1.0")
    return 1;
  return 3;
}

QString DataTableModel::getCausalityName(const int causality) const
{
  if (fmiVersion_ == "3.0")
  {
    switch (causality)
    {
      case 0:
        return tr("Structural parameter");
      case 1:
        return tr("Parameter");
      case 2:
        return tr("Calculated parameter");
      case 3:
        return tr("Input");
      case 4:
        return tr("Output");
      case 5:
        return tr("Local");
      case 6:
        return tr("Independent");
      case 7:
        return tr("Unknown");
      default:
        break;
    }
  }
  else if (fmiVersion_ == "1.0")
  {
    switch (causality)
    {
      case 0:
        return tr("Input");
      case 1:
        return tr("Output");
      case 2:
        return tr("Internal");
      case 3:
        return tr("None");
      case 4:
        return tr("Unknown");
      default:
        break;
    }
  }
  else
  {
    switch (causality)
    {
      case 0:
        return tr("Parameter");
      case 1:
        return tr("Calculated parameter");
      case 2:
        return tr("Input");
      case 3:
        return tr("Output");
      case 4:
        return tr("Local");
      case 5:
        return tr("Independent");
      case 6:
        return tr("Unknown");
      default:
        break;
    }
  }
  return QString();
}

QStringList DataTableModel::getCausalityNames() const
{
  QStringList names;
  if (fmiVersion_ == "3.0")
  {
    names << tr("Structural parameter") << tr("Parameter") << tr("Calculated parameter") << tr("Input")
          << tr("Output") << tr("Local") << tr("Independent") << tr("Unknown");
  }
  else if (fmiVersion_ == "1.0")
  {
    names << tr("Input") << tr("Output") << tr("Internal") << tr("None") << tr("Unknown");
  }
  else
  {
    names << tr("Parameter") << tr("Calculated parameter") << tr("Input") << tr("Output")
          << tr("Local") << tr("Independent") << tr("Unknown");
  }
  return names;
}

Description DataTableModel::filterIOVariables(const UnsignedInteger io) const
{
  Description variableNames;
  for (UnsignedInteger i = 0; i < variableNames_.getSize(); ++ i)
  {
    if (inputOutput_[i] == io)
      variableNames.add(variableNames_[i]);
  }
  return variableNames;
}

Description DataTableModel::getInputVariableNames() const
{
  return filterIOVariables(1);
}

Description DataTableModel::getOutputVariableNames() const
{
  return filterIOVariables(2);
}

DataFilterProxyModel::DataFilterProxyModel(QObject *parent)
  : QSortFilterProxyModel(parent)
{
  // fill all
  for (int i = 0; i < 8; ++ i)
  {
    variabilityFilter_.append(i);
    causalityFilter_.append(i);
    ioFilter_.append(i);
  }
}

void DataFilterProxyModel::setVariabilityFilter(const QList<int> & variabilityFilter)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
  beginFilterChange();
  variabilityFilter_ = variabilityFilter;
  endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
  variabilityFilter_ = variabilityFilter;
  invalidateFilter();
#endif
}

void DataFilterProxyModel::setCausalityFilter(const QList<int> & causalityFilter)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
  beginFilterChange();
  causalityFilter_ = causalityFilter;
  endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
  causalityFilter_ = causalityFilter;
  invalidateFilter();
#endif
}

void DataFilterProxyModel::setIOFilter(const QList<int> & ioFilter)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
  beginFilterChange();
  ioFilter_ = ioFilter;
  endFilterChange(QSortFilterProxyModel::Direction::Rows);
#else
  ioFilter_ = ioFilter;
  invalidateFilter();
#endif
}

bool DataFilterProxyModel::filterAcceptsRow(int sourceRow,
    const QModelIndex & sourceParent) const
{
  // matches text regex
  QModelIndex index01 = sourceModel()->index(sourceRow, filterKeyColumn(), sourceParent);

  const bool textMatch = sourceModel()->data(index01).toString().contains(filterRegularExpression());

  // matches variability
  QModelIndex index2 = sourceModel()->index(sourceRow, 2, sourceParent);
  int variability = sourceModel()->data(index2, Qt::EditRole).toInt();
  const bool variabilityMatch = variabilityFilter_.contains(variability);

  // mathces causality
  QModelIndex index3 = sourceModel()->index(sourceRow, 3, sourceParent);
  int causality = sourceModel()->data(index3, Qt::EditRole).toInt();
  const bool causalityMatch = causalityFilter_.contains(causality);

  // matches i/o
  QModelIndex index4 = sourceModel()->index(sourceRow, 4, sourceParent);
  int io = sourceModel()->data(index4, Qt::EditRole).toInt();
  const bool ioMatch = ioFilter_.contains(io);

  return textMatch && variabilityMatch && causalityMatch && ioMatch;
}




TreeItem::TreeItem(const QList<QVariant> &data, TreeItem *parent)
  : m_itemData(data)
  , m_parentItem(parent)
{
}

TreeItem::~TreeItem()
{
  qDeleteAll(m_childItems);
}

void TreeItem::appendChild(TreeItem *item)
{
  m_childItems.append(item);
}

TreeItem *TreeItem::child(int row)
{
  return m_childItems.value(row);
}

void TreeItem::setChild(int row, TreeItem * child)
{
  m_childItems[row] = child;
}

void TreeItem::clearChildren()
{
  m_childItems.removeAll(nullptr);
}

int TreeItem::childCount() const
{
  return m_childItems.count();
}

int TreeItem::columnCount() const
{
  return m_itemData.count();
}

QVariant TreeItem::data(int column) const
{
  return m_itemData.value(column);
}

void TreeItem::setData(const QList<QVariant> &data)
{
  m_itemData = data;
}

TreeItem *TreeItem::parentItem()
{
  return m_parentItem;
}

void TreeItem::setParentItem(TreeItem * parentItem)
{
  m_parentItem = parentItem;
}

int TreeItem::row() const
{
  if (m_parentItem)
    return m_parentItem->m_childItems.indexOf(const_cast<TreeItem*>(this));

  return 0;
}

TreeModel::TreeModel(const QString & /*file*/, QObject *parent)
  : QAbstractItemModel(parent)
{
  QList<QVariant> rootData = {tr("Variables")};
  rootItem_ = new TreeItem(rootData);
}

void TreeModel::setVariableNames(const Description & variableNames)
{
  setupModelData(variableNames);
}

TreeModel::~TreeModel()
{
  delete rootItem_;
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
  if (parent.isValid())
    return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
  else
    return rootItem_->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role != Qt::DisplayRole)
    return QVariant();

  TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

  return item->data(index.column());
}

Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
  if (!index.isValid())
    return Qt::ItemFlags();

  return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    return rootItem_->data(section);

  return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
const
{
  if (!hasIndex(row, column, parent))
    return QModelIndex();

  TreeItem *parentItem;

  if (!parent.isValid())
    parentItem = rootItem_;
  else
    parentItem = static_cast<TreeItem*>(parent.internalPointer());

  TreeItem *childItem = parentItem->child(row);
  if (childItem)
    return createIndex(row, column, childItem);
  else
    return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
  if (!index.isValid())
    return QModelIndex();

  TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
  TreeItem *parentItem = childItem->parentItem();

  if (parentItem == rootItem_)
    return QModelIndex();

  return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
  TreeItem *parentItem;
  if (parent.column() > 0)
    return 0;

  if (!parent.isValid())
    parentItem = rootItem_;
  else
    parentItem = static_cast<TreeItem*>(parent.internalPointer());

  return parentItem->childCount();
}

void TreeModel::setupModelData(const Description & variableNames)
{
  beginResetModel();

  if (rootItem_->childCount())
  {
    delete rootItem_;
    QList<QVariant> rootData = {"Variable"};
    rootItem_ = new TreeItem(rootData);
  }

  Description sortedVariables(variableNames);
  std::sort(sortedVariables.begin(), sortedVariables.end());

  for (UnsignedInteger i = 0; i < sortedVariables.getSize(); ++ i)
  {
    QString varName(QString::fromStdString(sortedVariables[i]));

    QStringList chunks(varName.split('.'));
    TreeItem * currentParent = rootItem_;
    foreach (QString chunk, chunks)
    {
      int rows = currentParent->childCount();
      TreeItem * newChild = 0;
      if ((rows > 0) && (currentParent->child(rows - 1)->data(0).toString() == chunk))
      {
        // reuse node
        newChild = currentParent->child(rows - 1);
      }
      else
      {
        // create new node
        QList<QVariant> columnData = { chunk };
        newChild = new TreeItem(columnData, currentParent);
        currentParent->appendChild(newChild);
      }
      currentParent = newChild;
    }
  }

  // simplify unnecessary depth in tree by merging nodes with 1 child
  for (int i = 0; i < rootItem_->childCount(); ++ i)
    MergeNodes(rootItem_->child(i));

  endResetModel();
}

void TreeModel::MergeNodes(TreeItem * node)
{
  while (node->childCount() == 1)
  {
    TreeItem *child = node->child(0);
    const QString mergedString = node->data(0).toString() + "." + child->data(0).toString();
    QList<QVariant> columnData = { mergedString };
    node->setData(columnData);
    // grand child 0
    if (child->childCount())
      node->setChild(0, child->child(0));
    else
    {
      node->setChild(0, nullptr);
      node->clearChildren();
    }
    // grand child 1-n
    for (int i = 1; i < child->childCount(); ++ i)
      node->appendChild(child->child(i));
    for (int i = 0; i < child->childCount(); ++ i)
    {
      child->child(i)->setParentItem(node);
      child->setChild(i, nullptr); // avoid ctor killing children
    }
    delete child;
  }
  for (int i = 0; i < node->childCount(); ++ i)
    MergeNodes(node->child(i));
}

}
