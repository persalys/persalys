//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow to define a coupling physical model
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/CouplingModelWindow.hxx"

#include "persalys/DifferentiationTableModel.hxx"
#include "persalys/CopyableTableView.hxx"
#include "persalys/SpinBoxDelegate.hxx"
#include "persalys/CollapsibleGroupBox.hxx"
#include "persalys/StudyTreeViewModel.hxx"
#include "persalys/EditButtonDelegate.hxx"
#include "persalys/ModelEvaluation.hxx"

#include <QGroupBox>
#include <QGridLayout>
#include <QLineEdit>
#include <QTableView>
#include <QHeaderView>
#include <QScrollArea>
#include <QCheckBox>

using namespace OT;

namespace PERSALYS
{
CouplingModelWindow::CouplingModelWindow(PhysicalModelItem *item, QWidget *parent)
  : SubWindow(item, parent)
{
  model_ = dynamic_cast<CouplingPhysicalModel*>(item->getPhysicalModel().getImplementation().get());

  connect(item, &PhysicalModelItem::codeChanged, [=](){ updateStepTabWidget(item); });

  QGridLayout * mainLayout = new QGridLayout(this);
  mainLayout->addWidget(new TitleLabel(tr("Coupling model")));

  QTabWidget * mainTabWidget = new QTabWidget;
  mainLayout->addWidget(mainTabWidget, 1, 0);

  QWidget * tab = new QWidget;
  QGridLayout * tabLayout = new QGridLayout(tab);
  mainTabWidget->addTab(tab, tr("Definition"));

  stepTabWidget_ = new DynamicTabWidget;
  tabLayout->addWidget(stepTabWidget_, 0, 0);
  connect(stepTabWidget_, &DynamicTabWidget::newTabRequested,
          [=](){
                CouplingStepCollection csColl(model_->getSteps());
                csColl.add(CouplingStep());
                model_->blockNotification("PhysicalModelDefinitionItem");
                model_->setSteps(csColl);
                model_->blockNotification();
                CouplingStepWidget * csWidget = new CouplingStepWidget(item, model_, csColl.getSize()-1);
                stepTabWidget_->addTab(csWidget, tr("Command") + " " + QString::number(stepTabWidget_->count()));
               });
  connect(stepTabWidget_, &DynamicTabWidget::removeTabRequested,
          [=](int index){
                          CouplingStepCollection csColl(model_->getSteps());
                          csColl.erase(csColl.begin() + index);
                          model_->blockNotification("PhysicalModelDefinitionItem");
                          model_->setSteps(csColl);
                          model_->blockNotification();
                          updateStepTabWidget(item);
                        });

  updateStepTabWidget(item);

  CollapsibleGroupBox * advancedGroupBox = new CollapsibleGroupBox(tr("Advanced parameters"));
  QGridLayout * advancedGroupBoxLayout = new QGridLayout(advancedGroupBox);
  tabLayout->addWidget(advancedGroupBox, 1, 0);

  advancedGroupBoxLayout->addWidget(new QLabel(tr("Cache input file")), 0, 0);
  advancedGroupBoxLayout->addWidget(new QLabel(tr("Cache output file")), 1, 0);

  FilePathWidget * filePath = new FilePathWidget(QString::fromUtf8(model_->getCacheInputFile().c_str()));
  advancedGroupBoxLayout->addWidget(filePath, 0, 1);
  connect(filePath, &FilePathWidget::pathChanged, [=](const QString& text) { model_->setCacheFiles(text.toUtf8().constData(), model_->getCacheOutputFile()); });

  filePath = new FilePathWidget(QString::fromUtf8(model_->getCacheOutputFile().c_str()));
  advancedGroupBoxLayout->addWidget(filePath, 1, 1);
  connect(filePath, &FilePathWidget::pathChanged, [=](const QString& text) { model_->setCacheFiles(model_->getCacheInputFile(), text.toUtf8().constData()); });

  QPushButton * clearButton = new QPushButton(tr("Clear cache"));
  advancedGroupBoxLayout->addWidget(clearButton, 2, 1, Qt::AlignRight);
  connect(clearButton, &QPushButton::clicked,
    [=] () {
            if (!model_->getCacheInputFile().empty())
            {
              Sample input(0, model_->getInputDimension());
              input.setDescription(model_->getInputNames());
              input.exportToCSVFile(model_->getCacheInputFile());
            }
            if (!model_->getCacheOutputFile().empty())
            {
              Sample output(0, model_->getOutputDimension());
              output.setDescription(model_->getOutputNames());
              output.exportToCSVFile(model_->getCacheOutputFile());
            }
           });

  advancedGroupBoxLayout->addWidget(new QLabel(tr("Working directory")), 3, 0);
  filePath = new FilePathWidget(QString::fromUtf8(model_->getWorkDir().c_str()), QFileDialog::Directory);
  advancedGroupBoxLayout->addWidget(filePath, 3, 1);
  connect(filePath, &FilePathWidget::pathChanged, [=](const QString& text) {
    model_->setWorkDir(text.toUtf8().constData()); });

  QCheckBox * keepCheckBox = new QCheckBox(tr("Keep working directory"));
  keepCheckBox->setChecked(!model_->getCleanupWorkDirectory());
  advancedGroupBoxLayout->addWidget(keepCheckBox, 4, 0);
  connect(keepCheckBox, &QCheckBox::toggled,
	  [=](bool toggled){model_->setCleanupWorkDirectory(!toggled);
	  });

  QPushButton * evaluateOutputsButton = new QPushButton(QIcon(":/images/system-run.png"), tr("Check model"));
  evaluateOutputsButton->setToolTip(tr("Evaluate the outputs"));
  connect(evaluateOutputsButton, SIGNAL(clicked(bool)), this, SLOT(evaluateOutputs()));
  tabLayout->addWidget(evaluateOutputsButton, 2, 0, Qt::AlignLeft);

  // - error message label
  errorMessageLabel_ = new TemporaryLabel;
  tabLayout->addWidget(errorMessageLabel_, 3, 0);

  // Tab : Finite difference step definition
  tab = new QWidget;
  tabLayout = new QGridLayout(tab);

  QLabel * label = new QLabel(tr("Finite difference step definition"));
  label->setStyleSheet("QLabel {font: bold;}");
  tabLayout->addWidget(label);

  CopyableTableView * differentiationTableView = new CopyableTableView;
  differentiationTableView->horizontalHeader()->setStretchLastSection(true);

  SpinBoxDelegate * spinBoxDelegate = new SpinBoxDelegate(differentiationTableView);
  spinBoxDelegate->setSpinBoxType(SpinBoxDelegate::differentiationStep);
  differentiationTableView->setItemDelegateForColumn(1, spinBoxDelegate);
  differentiationTableView->setEditTriggers(QTableView::AllEditTriggers);

  DifferentiationTableModel * differentiationTableModel  = new DifferentiationTableModel(item->getPhysicalModel(), differentiationTableView);
  differentiationTableView->setModel(differentiationTableModel);

  // connections
  connect(item, SIGNAL(inputListDifferentiationChanged()), differentiationTableModel, SLOT(updateData()));
  connect(spinBoxDelegate, SIGNAL(applyToAllRequested(double)), differentiationTableModel, SLOT(applyValueToAll(double)));

  tabLayout->addWidget(differentiationTableView);

  mainTabWidget->addTab(tab, tr("Differentiation"));
}

void CouplingModelWindow::updateStepTabWidget(PhysicalModelItem *item)
{
  stepTabWidget_->clear();

  for (UnsignedInteger i = 0; i < model_->getSteps().getSize(); ++i)
  {
    CouplingStepWidget * csWidget = new CouplingStepWidget(item, model_, i);
    stepTabWidget_->addTab(csWidget, tr("Command") + " " + QString::number(i + 1));
  }

  if (stepTabWidget_->count() < 2)
    stepTabWidget_->newTabRequested();

  item->update(0, "inputStepChanged");
  stepTabWidget_->setCurrentIndex(0);
}

void CouplingModelWindow::evaluateOutputs()
{
  ModelEvaluation eval("anEval", model_->clone());
  try
  {
    eval.run();
  }
  catch (std::exception& ex)
  {
    // do nothing
  }

  // get result
  Sample outputSample(eval.getResult().getDesignOfExperiment().getOutputSample());

  // check
  errorMessageLabel_->reset();
  if (!eval.getErrorMessage().empty())
  {
    errorMessageLabel_->setErrorMessage(eval.getErrorMessage().c_str());
    return;
  }
  if (!outputSample.getSize())
  {
    errorMessageLabel_->setErrorMessage(tr("Not possible to evaluate the outputs"));
    return;
  }

  // set output value
  for (UnsignedInteger i = 0; i < outputSample.getDimension(); ++ i)
    model_->setOutputValue(outputSample.getDescription()[i], outputSample(0, i));

  // TODO: clear errorMessageLabel_ if model modification after an error message
}


// table model to list inputs of a step

InTableModel::InTableModel(CouplingPhysicalModel *model, const int indStep, const int indFile, QWidget *parent)
  : QAbstractTableModel(parent)
  , model_(model)
  , indStep_(indStep)
  , indFile_(indFile)
{
}

CouplingInputFile InTableModel::getInputFile() const
{
  return model_->getSteps()[indStep_].getInputFiles()[indFile_];
}

int InTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return getInputFile().getVariableNames().getSize();
}

int InTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 4;
}

Qt::ItemFlags InTableModel::flags(const QModelIndex & index) const
{
  // TODO value of output variable not editable?
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant InTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        return tr("Token");
      case 3:
        return tr("Value");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant InTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    const String inName(getInputFile().getVariableNames()[index.row()]);
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(inName.c_str());
      case 1:
      {
        const bool isInput = model_->hasInputNamed(inName);
        if (isInput)
          return QString::fromUtf8(model_->getInputByName(inName).getDescription().c_str());
        else
          return QString::fromUtf8(model_->getOutputByName(inName).getDescription().c_str());
      }
      case 2:
        return QString::fromUtf8(getInputFile().getTokens()[index.row()].c_str());
      case 3:
      {
        const bool isInput = model_->hasInputNamed(inName);
        if (isInput)
          return QString::number(model_->getInputByName(inName).getValue(), 'g', StudyTreeViewModel::DefaultSignificantDigits);
        else
        {
          Output output(model_->getOutputByName(inName));
          if (!output.hasBeenComputed())
            return QString("?");
          return QString::number(output.getValue(), 'g', StudyTreeViewModel::DefaultSignificantDigits);
        }
      }
    }
  }
  return QVariant();
}

bool InTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid() || role != Qt::EditRole)
    return false;

  CouplingInputFile inFile(getInputFile());
  const String inName(inFile.getVariableNames()[index.row()]);

  switch (index.column())
  {
    case 0:
    {
      String newname = value.toString().toUtf8().constData();
      if (inName == newname)
        return true;
      if (value.toString().isEmpty())
        return false;

      const bool isInput = model_->hasInputNamed(inName);

      const String description(isInput ? model_->getInputByName(inName).getDescription() : "");
      const double varValue = isInput ? model_->getInputByName(inName).getValue() : 0.;

      Description names(inFile.getVariableNames());
      names[index.row()] = newname;
      inFile.setVariables(names, inFile.getTokens());
      updateModel(inFile);

      if (model_->hasInputNamed(newname))
      {
        model_->blockNotification("PhysicalModelDefinitionItem");
        model_->setInputDescription(newname, description);
        model_->setInputValue(newname, varValue);
        model_->blockNotification();
      }

      break;
    }
    case 1:
    {
      // the variable can be an output of another command
      const bool isInput = model_->hasInputNamed(inName);
      const String description(isInput ? model_->getInputByName(inName).getDescription() : model_->getOutputByName(inName).getDescription());
      if (description == value.toString().toUtf8().constData())
        return true;
      model_->blockNotification("PhysicalModelDefinitionItem");
      if (isInput)
        model_->setInputDescription(inName, value.toString().toUtf8().constData());
      else
        model_->setOutputDescription(inName, value.toString().toUtf8().constData());
      model_->blockNotification();
      break;
    }
    case 2:
    {
      String newtoken = value.toString().toUtf8().constData();
      if (inFile.getTokens()[index.row()] == newtoken)
        return true;
      if (value.toString().isEmpty())
        return false;

      Description tokens(inFile.getTokens());
      tokens[index.row()] = newtoken;
      inFile.setVariables(inFile.getVariableNames(), tokens);
      updateModel(inFile);
      break;
    }
    case 3:
    {
      const bool isInput = model_->hasInputNamed(inName);
      const double varValue(isInput ? model_->getInputByName(inName).getValue() : model_->getOutputByName(inName).getValue());
      if (varValue == value.toDouble())
        return true;

      const String name(isInput ? model_->getInputByName(inName).getName() : model_->getOutputByName(inName).getName());
      model_->blockNotification("PhysicalModelDefinitionItem");
      if (isInput)
        model_->setInputValue(name, value.toDouble());
      else
        model_->setOutputValue(name, value.toDouble());
      model_->blockNotification();
      break;
    }
  }
  emit dataChanged(index, QAbstractTableModel::index(index.row(), 3));
  return true;
}

void InTableModel::updateData()
{
  beginResetModel();
  endResetModel();
  emit dataChanged(QModelIndex(), QModelIndex());
}

void InTableModel::updateModel(const CouplingInputFile &file)
{
  CouplingStepCollection csColl(model_->getSteps());
  CouplingInputFileCollection cinColl(csColl[indStep_].getInputFiles());
  cinColl[indFile_] = file;
  csColl[indStep_].setInputFiles(cinColl);
  model_->blockNotification("PhysicalModelDefinitionItem");
  model_->setSteps(csColl);
  model_->blockNotification();
}

void InTableModel::addLine()
{
  int i = 0;
  while (model_->hasInputNamed('X' + (OSS() << i).str()))
    ++i;

  CouplingInputFile inFile(getInputFile());
  Description names(inFile.getVariableNames());
  Description tokens(inFile.getTokens());
  names.add('X' + (OSS() << i).str());
  tokens.add("@X" + (OSS() << i).str());

  inFile.setVariables(names, tokens);
  updateModel(inFile);

  updateData();
}


void InTableModel::removeLine()
{
  QTableView * tableView = static_cast<QTableView*>(parent());
  if (!tableView || !tableView->selectionModel() || !tableView->selectionModel()->hasSelection())
    return;

  QModelIndex index = tableView->selectionModel()->currentIndex();

  CouplingInputFile inFile(getInputFile());
  Description names(inFile.getVariableNames());
  Description tokens(inFile.getTokens());
  names.erase(names.begin() + index.row());
  tokens.erase(tokens.begin() + index.row());

  inFile.setVariables(names, tokens);
  updateModel(inFile);

  updateData();
}

// table model to list outputs of a step

OutTableModel::OutTableModel(CouplingPhysicalModel *model, const int indStep, const int indFile, QWidget *parent)
  : QAbstractTableModel(parent)
  , model_(model)
  , indStep_(indStep)
  , indFile_(indFile)
{
}

CouplingOutputFile OutTableModel::getOutputFile() const
{
  return model_->getSteps()[indStep_].getOutputFiles()[indFile_];
}

int OutTableModel::rowCount(const QModelIndex & /*parent*/) const
{
  return getOutputFile().getVariableNames().getSize();
}

int OutTableModel::columnCount(const QModelIndex & /*parent*/) const
{
  return 6;
}

Qt::ItemFlags OutTableModel::flags(const QModelIndex & index) const
{
  // TODO value column not editable
  return Qt::ItemIsEditable | QAbstractTableModel::flags(index);
}

QVariant OutTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        return tr("Token");
      case 3:
        return tr("Skip Line");
      case 4:
        return tr("Skip Column");
      case 5:
        return tr("Value");
    }
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}

QVariant OutTableModel::data(const QModelIndex & index, int role) const
{
  if (!index.isValid())
    return QVariant();

  if (role == Qt::DisplayRole || role == Qt::EditRole)
  {
    const String outName(getOutputFile().getVariableNames()[index.row()]);
    switch (index.column())
    {
      case 0:
        return QString::fromUtf8(outName.c_str());
      case 1:
        return QString::fromUtf8(model_->getOutputByName(outName).getDescription().c_str());
      case 2:
        return QString::fromUtf8(getOutputFile().getTokens()[index.row()].c_str());
      case 3:
        return QString::number(getOutputFile().getSkipLines()[index.row()], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      case 4:
        return QString::number(getOutputFile().getSkipColumns()[index.row()], 'g', StudyTreeViewModel::DefaultSignificantDigits);
      case 5:
      {
        Output output(model_->getOutputByName(outName));
        if (!output.hasBeenComputed())
          return QString("?");
        return QString::number(output.getValue(), 'g', StudyTreeViewModel::DefaultSignificantDigits);
      }
    }
  }
  return QVariant();
}

bool OutTableModel::setData(const QModelIndex & index, const QVariant & value, int role)
{
  if (!index.isValid() || role != Qt::EditRole)
    return false;

  CouplingOutputFile outFile(getOutputFile());
  const String outName(outFile.getVariableNames()[index.row()]);
  Output output(model_->getOutputByName(outName));
  switch (index.column())
  {
    case 0:
    {
      String newname = value.toString().toUtf8().constData();
      if (outName == newname)
        return true;
      if (value.toString().isEmpty())
        return false;

      const String description(output.getDescription());
      const double varValue = output.getValue();

      Description names(outFile.getVariableNames());
      names[index.row()] = newname;
      outFile.setVariables(names, outFile.getTokens(), outFile.getSkipLines(), outFile.getSkipColumns());
      updateModel(outFile);

      model_->blockNotification("PhysicalModelDefinitionItem");
      model_->setOutputDescription(newname, description);
      model_->setOutputValue(newname, varValue);
      model_->blockNotification();

//     TODO?  emit errorMessageChanged("");
      break;
    }
    case 1:
    {
      if (output.getDescription() == value.toString().toUtf8().constData())
        return true;
//     TODO?  emit errorMessageChanged("");
      model_->blockNotification("PhysicalModelDefinitionItem");
      model_->setOutputDescription(output.getName(), value.toString().toUtf8().constData());
      model_->blockNotification();
      break;
    }
    case 2:
    {
      String newtoken = value.toString().toUtf8().constData();
      if (outFile.getTokens()[index.row()] == newtoken)
        return true;
      if (value.toString().isEmpty())
        return false;

      Description tokens(outFile.getTokens());
      tokens[index.row()] = newtoken;
      outFile.setVariables(outFile.getVariableNames(), tokens, outFile.getSkipLines(), outFile.getSkipColumns());
      updateModel(outFile);
      break;
    }
    case 3:
    {
      if (outFile.getSkipLines()[index.row()] == value.toDouble())
        return true;

      Point skipLines(outFile.getSkipLines());
      skipLines[index.row()] = value.toDouble();
      outFile.setVariables(outFile.getVariableNames(), outFile.getTokens(), skipLines, outFile.getSkipColumns());
      updateModel(outFile);
      break;
    }
    case 4:
    {
      if (outFile.getSkipColumns()[index.row()] == value.toDouble())
        return true;

      Point skipColumns(outFile.getSkipColumns());
      skipColumns[index.row()] = value.toDouble();
      outFile.setVariables(outFile.getVariableNames(), outFile.getTokens(), outFile.getSkipLines(), skipColumns);
      updateModel(outFile);
      break;
    }
  }
  emit dataChanged(index, index);
  return true;
}

void OutTableModel::updateData()
{
  beginResetModel();
  endResetModel();
  emit dataChanged(QModelIndex(), QModelIndex());
}

void OutTableModel::updateModel(const CouplingOutputFile &file)
{
  CouplingStepCollection csColl(model_->getSteps());
  CouplingOutputFileCollection coutColl(csColl[indStep_].getOutputFiles());
  coutColl[indFile_] = file;
  csColl[indStep_].setOutputFiles(coutColl);
  model_->blockNotification("PhysicalModelDefinitionItem");
  model_->setSteps(csColl);
  model_->blockNotification();
}

void OutTableModel::addLine()
{
  int i = 0;
  while (model_->hasOutputNamed('Y' + (OSS() << i).str()))
    ++i;

  CouplingOutputFile outFile(getOutputFile());
  Description names(outFile.getVariableNames());
  Description tokens(outFile.getTokens());
  Point skipLine(outFile.getSkipLines());
  Point skipCol(outFile.getSkipColumns());
  names.add('Y' + (OSS() << i).str());
  tokens.add('Y' + (OSS() << i).str() + '=');
  skipLine.add(0.);
  skipCol.add(0.);

  outFile.setVariables(names, tokens, skipLine, skipCol);
  updateModel(outFile);

  updateData();
}

void OutTableModel::removeLine()
{
  QTableView * tableView = static_cast<QTableView*>(parent());
  if (!tableView || !tableView->selectionModel() || !tableView->selectionModel()->hasSelection())
    return;

  QModelIndex index = tableView->selectionModel()->currentIndex();

  CouplingOutputFile outFile(getOutputFile());
  Description names(outFile.getVariableNames());
  Description tokens(outFile.getTokens());
  Point skipLine(outFile.getSkipLines());
  Point skipCol(outFile.getSkipColumns());
  names.erase(names.begin() + index.row());
  tokens.erase(tokens.begin() + index.row());
  skipLine.erase(skipLine.begin() + index.row());
  skipCol.erase(skipCol.begin() + index.row());

  outFile.setVariables(names, tokens, skipLine, skipCol);
  updateModel(outFile);

  updateData();
}

// Widget for Coupling Input file

CouplingInputFileWidget::CouplingInputFileWidget(PhysicalModelItem *item, CouplingPhysicalModel *model, const int indStep, const int indFile, QWidget *parent)
  : QWidget(parent)
  , indStep_(indStep)
  , indFile_(indFile)
{
  QGridLayout * layout = new QGridLayout(this);

  int row = -1;

  // template file path
  QLabel * templateFileLabel = new QLabel(tr("Template file"));
  layout->addWidget(templateFileLabel, ++row, 0);

  FilePathWidget * templateFileLineEdit = new FilePathWidget(model->getSteps()[indStep].getInputFiles()[indFile].getPath().c_str());
  layout->addWidget(templateFileLineEdit, row, 1);

  // input model file path
  QLabel * fileLabel = new QLabel(tr("Configured file"));
  layout->addWidget(fileLabel, ++row, 0);

  QLineEdit * fileLineEdit = new QLineEdit;
  fileLineEdit->setText(model->getSteps()[indStep].getInputFiles()[indFile].getConfiguredPath().c_str());
  layout->addWidget(fileLineEdit, row, 1);

  // input variable table
  QTableView * inTableView = new QTableView;
  inTableView->horizontalHeader()->setStretchLastSection(true);
  layout->addWidget(inTableView, ++row, 0, 1, 3);
  AddRemoveWidget * addRemoveWidget = new AddRemoveWidget;
  layout->addWidget(addRemoveWidget, ++row, 1, 1, 2, Qt::AlignRight);

  InTableModel * inTableModel = new InTableModel(model, indStep, indFile, inTableView);
  inTableView->setModel(inTableModel);
  connect(addRemoveWidget, &AddRemoveWidget::addRequested, inTableModel, &InTableModel::addLine);
  connect(addRemoveWidget, &AddRemoveWidget::removeRequested, inTableModel, &InTableModel::removeLine);
  connect(inTableModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), item, SIGNAL(inputListDifferentiationChanged()));
  connect(item, &PhysicalModelItem::outputChanged, inTableModel, &InTableModel::updateData);

  inTableView->setDisabled(templateFileLineEdit->text().isEmpty() || fileLineEdit->text().isEmpty());
  addRemoveWidget->setDisabled(templateFileLineEdit->text().isEmpty() || fileLineEdit->text().isEmpty());

  connect(templateFileLineEdit, &FilePathWidget::pathChanged,
          [=](const QString& text){
                inTableView->setDisabled(text.isEmpty() || fileLineEdit->text().isEmpty());
                addRemoveWidget->setDisabled(text.isEmpty() || fileLineEdit->text().isEmpty());
                // update model
                CouplingStepCollection csColl(model->getSteps());
                CouplingStep cs(csColl[indStep]);
                CouplingInputFileCollection inColl(cs.getInputFiles());

                inColl[indFile].setPath(text.toUtf8().constData());
                cs.setInputFiles(inColl);
                csColl[indStep] = cs;
                model->blockNotification("PhysicalModelDefinitionItem");
                model->setSteps(csColl);
                model->blockNotification();

                // refresh configured path in case it was unitialized
                fileLineEdit->setText(inColl[indFile].getConfiguredPath().c_str());
               });
  connect(fileLineEdit, &QLineEdit::editingFinished,
          [=](){
                if (QFileInfo(fileLineEdit->text()).isAbsolute())
                  fileLineEdit->setText(QFileInfo(fileLineEdit->text()).fileName());

                inTableView->setDisabled(templateFileLineEdit->text().isEmpty() || fileLineEdit->text().isEmpty());
                addRemoveWidget->setDisabled(templateFileLineEdit->text().isEmpty() || fileLineEdit->text().isEmpty());
                // update model
                CouplingStepCollection csColl(model->getSteps());
                CouplingStep cs(csColl[indStep]);
                CouplingInputFileCollection inColl(cs.getInputFiles());

                inColl[indFile].setConfiguredPath(fileLineEdit->text().toUtf8().constData());
                cs.setInputFiles(inColl);
                csColl[indStep] = cs;
                model->blockNotification("PhysicalModelDefinitionItem");
                model->setSteps(csColl);
                model->blockNotification();
               });
}

// Widget for Coupling Ressource file ( <=> Coupling input file without template path )

CouplingResourceFileWidget::CouplingResourceFileWidget(CouplingPhysicalModel *model, const int indStep, QWidget *parent)
  : QWidget(parent)
  , model_(model)
  , indStep_(indStep)
{
  QGridLayout * resGroupBoxLayout = new QGridLayout(this);
  resGroupBoxLayout->setContentsMargins(0, 0, 0, 0);

  tableWidget_ = new QTableWidget(0, 2);
  QHeaderView * headerView = new QHeaderView(Qt::Horizontal, tableWidget_);
  tableWidget_->setHorizontalHeader(headerView);
  headerView->setSectionResizeMode(0, QHeaderView::Stretch);
  headerView->setSectionResizeMode(1, QHeaderView::Fixed);
  headerView->hide();
  resGroupBoxLayout->addWidget(tableWidget_, 0, 0);
  AddRemoveWidget * addRemoveWidget = new AddRemoveWidget;
  resGroupBoxLayout->addWidget(addRemoveWidget, 1, 0, Qt::AlignRight);

  updateTable();
  connect(tableWidget_, &QTableWidget::itemChanged,
          [=](QTableWidgetItem * item) {
                 CouplingStepCollection csColl(model->getSteps());
                 CouplingStep cs(csColl[indStep]);
                 CouplingResourceFileCollection inColl(cs.getResourceFiles());

                 inColl[item->data(Qt::UserRole).toInt()].setPath(item->data(Qt::DisplayRole).toString().toUtf8().constData());
                 cs.setResourceFiles(inColl);
                 csColl[indStep] = cs;
                 model->blockNotification("PhysicalModelDefinitionItem");
                 model->setSteps(csColl);
                 model->blockNotification();
                });

  connect(addRemoveWidget, &AddRemoveWidget::addRequested,
          [=]() {
                 CouplingStepCollection csColl(model->getSteps());
                 CouplingStep cs(csColl[indStep]);
                 CouplingResourceFileCollection inColl(cs.getResourceFiles());

                 inColl.add(CouplingResourceFile());
                 cs.setResourceFiles(inColl);
                 csColl[indStep] = cs;
                 model->blockNotification("PhysicalModelDefinitionItem");
                 model->setSteps(csColl);
                 model->blockNotification();

                 const int row = tableWidget_->rowCount();
                 tableWidget_->setRowCount(row+1);

                 QTableWidgetItem * newItem = new QTableWidgetItem;
                 newItem->setData(Qt::UserRole, (int)inColl.getSize()-1);
                 tableWidget_->setItem(row, 0, newItem);

                 QToolButton * tb = new QToolButton;
                 tb->setText("...");
                 tableWidget_->setCellWidget(row, 1, tb);
                 tableWidget_->horizontalHeader()->resizeSection(1, tb->width());
                 connect(tb, &QToolButton::clicked,
                         [=](){
                                QString fileName = QFileDialog::getOpenFileName(this, tr("Search file"), FileTools::GetCurrentDir());
                                if (fileName.isEmpty())
                                  return;

                                FileTools::SetCurrentDir(fileName);
                                newItem->setData(Qt::DisplayRole, fileName);

                                CouplingStepCollection csColl(model->getSteps());
                                CouplingStep cs(csColl[indStep]);
                                CouplingResourceFileCollection inColl(cs.getResourceFiles());

                                inColl[newItem->data(Qt::UserRole).toInt()].setPath(newItem->data(Qt::DisplayRole).toString().toUtf8().constData());
                                cs.setResourceFiles(inColl);
                                csColl[indStep] = cs;
                                model->blockNotification("PhysicalModelDefinitionItem");
                                model->setSteps(csColl);
                                model->blockNotification();
                              });
                });
  connect(addRemoveWidget, &AddRemoveWidget::removeRequested,
    [=]() {
           CouplingStepCollection csColl(model->getSteps());
           CouplingStep cs(csColl[indStep]);
           CouplingResourceFileCollection inColl(cs.getResourceFiles());

           inColl.erase(inColl.begin() + tableWidget_->selectionModel()->currentIndex().data(Qt::UserRole).toInt());
           cs.setResourceFiles(inColl);
           csColl[indStep] = cs;
           model->blockNotification("PhysicalModelDefinitionItem");
           model->setSteps(csColl);
           model->blockNotification();
           // emit signal to the parent widget in order to rebuild widgets
           // (because we want to update the input file index in all widgets)
           emit couplingResourceCollectionModified();
          });
}


void CouplingResourceFileWidget::updateTable()
{
  // remove all rows
  tableWidget_->clear();
  tableWidget_->setRowCount(0);

  // fill in the table
  CouplingStep cs(model_->getSteps()[indStep_]);
  if (cs.getResourceFiles().getSize())
  {
    int row = -1;
    for (UnsignedInteger i = 0; i < cs.getResourceFiles().getSize(); ++i)
    {
      if (!cs.getResourceFiles()[i].getPath().empty())
      {
        ++row;
        tableWidget_->setRowCount(row+1);

        QTableWidgetItem * newItem = new QTableWidgetItem(cs.getResourceFiles()[i].getPath().c_str());
        newItem->setData(Qt::UserRole, int(i));
        tableWidget_->setItem(row, 0, newItem);

        QToolButton * tb = new QToolButton;
        tb->setText("...");
        tableWidget_->setCellWidget(row, 1, tb);
      }
    }
  }
}

// Widget for Coupling Output file

CouplingOutputFileWidget::CouplingOutputFileWidget(PhysicalModelItem *item, CouplingPhysicalModel *model, const int indStep, const int indFile, QWidget *parent)
  : QWidget(parent)
{
  QGridLayout * layout = new QGridLayout(this);
  int row = -1;

  // output file path
  QLabel * outFileLabel = new QLabel(tr("Output file"));
  layout->addWidget(outFileLabel, ++row, 0);

  QLineEdit * outFileLineEdit = new QLineEdit;
  layout->addWidget(outFileLineEdit, row, 1);
  outFileLineEdit->setText(model->getSteps()[indStep].getOutputFiles()[indFile].getPath().c_str());

  // input variables table
  QTableView * outTableView = new QTableView;
  outTableView->horizontalHeader()->setStretchLastSection(true);
  layout->addWidget(outTableView, ++row, 0, 1, 2);
  AddRemoveWidget * addRemoveWidget = new AddRemoveWidget;
  layout->addWidget(addRemoveWidget, ++row, 1, Qt::AlignRight);

  OutTableModel * outTableModel = new OutTableModel(model, indStep, indFile, outTableView);
  outTableView->setModel(outTableModel);
  connect(addRemoveWidget, &AddRemoveWidget::addRequested, outTableModel, &OutTableModel::addLine);
  connect(addRemoveWidget, &AddRemoveWidget::removeRequested, outTableModel, &OutTableModel::removeLine);
  connect(outTableModel, SIGNAL(dataChanged(QModelIndex, QModelIndex)), item, SIGNAL(inputListDifferentiationChanged()));
  connect(item, &PhysicalModelItem::outputChanged, outTableModel, &OutTableModel::updateData);

  outTableView->setDisabled(outFileLineEdit->text().isEmpty());
  addRemoveWidget->setDisabled(outFileLineEdit->text().isEmpty());

  connect(outFileLineEdit, &QLineEdit::editingFinished,
        [=](){
              if (QFileInfo(outFileLineEdit->text()).isAbsolute())
                outFileLineEdit->setText(QFileInfo(outFileLineEdit->text()).fileName());

              outTableView->setDisabled(outFileLineEdit->text().isEmpty());
              addRemoveWidget->setDisabled(outFileLineEdit->text().isEmpty());

              CouplingStepCollection csColl(model->getSteps());
              CouplingStep cs(csColl[indStep]);
              CouplingOutputFileCollection outColl(cs.getOutputFiles());

              outColl[indFile].setPath(outFileLineEdit->text().toUtf8().constData());
              cs.setOutputFiles(outColl);
              csColl[indStep] = cs;
              model->blockNotification("PhysicalModelDefinitionItem");
              model->setSteps(csColl);
              model->blockNotification();
             });
}

// Widget for Coupling Step

CouplingStepWidget::CouplingStepWidget(PhysicalModelItem *item, CouplingPhysicalModel *model, const int indStep, QWidget *parent)
  : QWidget(parent)
  , model_(model)
  , indStep_(indStep)
{
  QGridLayout * widgetLayout = new QGridLayout(this);

  // command definition
  QLabel * commandLabel = new QLabel(tr("Command"));
  widgetLayout->addWidget(commandLabel, 0, 0);

  QLineEdit * commandLineEdit = new QLineEdit(QString::fromUtf8(model->getSteps()[indStep].getCommand().c_str()));
  widgetLayout->addWidget(commandLineEdit, 0, 1);
  connect(commandLineEdit, &QLineEdit::editingFinished,
        [=](){
              CouplingStepCollection csColl(model->getSteps());
              CouplingStep cs(csColl[indStep]);

              cs.setCommand(commandLineEdit->text().toUtf8().constData());
              csColl[indStep] = cs;
              model->blockNotification("PhysicalModelDefinitionItem");
              model->setSteps(csColl);
              model->blockNotification();
             });

  QCheckBox * checkBox = new QCheckBox(tr("Shell command"));
  widgetLayout->addWidget(checkBox, 1, 0, 1, 2);
  checkBox->setChecked(model->getSteps()[indStep].getIsShell());
  connect(checkBox, &QCheckBox::toggled,
          [=](bool toggled){
                            CouplingStepCollection csColl(model->getSteps());
                            CouplingStep cs(csColl[indStep]);

                            cs.setIsShell(toggled);
                            csColl[indStep] = cs;
                            model->blockNotification("PhysicalModelDefinitionItem");
                            model->setSteps(csColl);
                            model->blockNotification();
                           });

  // step definition
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setWidgetResizable(true);
  widgetLayout->addWidget(scrollArea, 2, 0, 1, 2);

  QWidget * mainWidget = new QWidget;
  QVBoxLayout * mainLayout = new QVBoxLayout(mainWidget);
  scrollArea->setWidget(mainWidget);

  // input definition
  CollapsibleGroupBox * inGroupBox = new CollapsibleGroupBox(tr("Input"));
  QVBoxLayout * inGroupBoxLayout = new QVBoxLayout(inGroupBox);
  mainLayout->addWidget(inGroupBox);

  inTabWidget_ = new DynamicTabWidget;
  inGroupBoxLayout->addWidget(inTabWidget_);
  connect(inTabWidget_, &DynamicTabWidget::newTabRequested,
          [=](){
                CouplingStepCollection csColl(model->getSteps());
                CouplingStep cs(csColl[indStep]);
                CouplingInputFileCollection inColl(cs.getInputFiles());

                inColl.add(CouplingInputFile());
                cs.setInputFiles(inColl);
                csColl[indStep] = cs;
                model->blockNotification("PhysicalModelDefinitionItem");
                model->setSteps(csColl);
                model->blockNotification();
                CouplingInputFileWidget * ciFileWidget = new CouplingInputFileWidget(item, model, indStep, inColl.getSize()-1);
                inTabWidget_->addTab(ciFileWidget, tr("File"));
               });
  connect(inTabWidget_, &DynamicTabWidget::removeTabRequested,
          [=](int index){
                          CouplingStepCollection csColl(model->getSteps());
                          CouplingStep cs(csColl[indStep]);
                          CouplingInputFileCollection inColl(cs.getInputFiles());

                          inColl.erase(inColl.begin() + index);
                          cs.setInputFiles(inColl);
                          csColl[indStep] = cs;
                          model->blockNotification("PhysicalModelDefinitionItem");
                          model->setSteps(csColl);
                          model->blockNotification();
                          updateInputFileWidgets(item);
                        });

  // ressource definition
  CollapsibleGroupBox * resGroupBox = new CollapsibleGroupBox(tr("Ressource"));
  QGridLayout * resGroupBoxLayout = new QGridLayout(resGroupBox);
  mainLayout->addWidget(resGroupBox);

  ressourceFileWidget_ = new CouplingResourceFileWidget(model, indStep, resGroupBox);
  resGroupBoxLayout->addWidget(ressourceFileWidget_);
  connect(ressourceFileWidget_, &CouplingResourceFileWidget::couplingResourceCollectionModified, [=]() { updateInputFileWidgets(item); });

  updateInputFileWidgets(item);

  // output definition
  CollapsibleGroupBox * outGroupBox = new CollapsibleGroupBox(tr("Output"));
  QGridLayout * outGroupBoxLayout = new QGridLayout(outGroupBox);
  mainLayout->addWidget(outGroupBox);

  DynamicTabWidget * outTabWidget = new DynamicTabWidget;
  outGroupBoxLayout->addWidget(outTabWidget);
  connect(outTabWidget, &DynamicTabWidget::newTabRequested,
          [=](){
                CouplingStepCollection csColl(model->getSteps());
                CouplingStep cs(csColl[indStep]);
                CouplingOutputFileCollection outColl(cs.getOutputFiles());

                outColl.add(CouplingOutputFile());
                cs.setOutputFiles(outColl);
                csColl[indStep] = cs;
                model->blockNotification("PhysicalModelDefinitionItem");
                model->setSteps(csColl);
                model->blockNotification();
                CouplingOutputFileWidget * outFileWidget = new CouplingOutputFileWidget(item, model, indStep, outColl.getSize()-1);
                outTabWidget->addTab(outFileWidget, tr("File"));
               });
  connect(outTabWidget, &DynamicTabWidget::removeTabRequested,
          [=](int index){
                          CouplingStepCollection csColl(model->getSteps());
                          CouplingStep cs(csColl[indStep]);
                          CouplingOutputFileCollection outColl(cs.getOutputFiles());

                          outColl.erase(outColl.begin() + index);
                          cs.setOutputFiles(outColl);
                          csColl[indStep] = cs;
                          model->blockNotification("PhysicalModelDefinitionItem");
                          model->setSteps(csColl);
                          model->blockNotification();
                          item->update(0, "inputStepChanged");
                        });

  // - fill in the QTabWidget
  CouplingStep cs(model->getSteps()[indStep]);
  if (cs.getOutputFiles().getSize())
  {
    for (UnsignedInteger i = 0; i < cs.getOutputFiles().getSize(); ++i)
    {
      CouplingOutputFileWidget * outFileWidget = new CouplingOutputFileWidget(item, model, indStep, i);
      outTabWidget->addTab(outFileWidget, tr("File"));
    }
  }
  // - if no coupling output file : add an empty widget
  if (outTabWidget->count() < 2)
    outTabWidget->newTabRequested();

  mainLayout->addStretch();
}


void CouplingStepWidget::updateInputFileWidgets(PhysicalModelItem *item)
{
  // remove all tabs
  inTabWidget_->clear();

  // fill in the QTabWidget
  CouplingStep cs(model_->getSteps()[indStep_]);
  if (cs.getInputFiles().getSize())
  {
    for (UnsignedInteger i = 0; i < cs.getInputFiles().getSize(); ++i)
    {
      CouplingInputFileWidget * ciFileWidget = new CouplingInputFileWidget(item, model_, indStep_, i);
      inTabWidget_->addTab(ciFileWidget, tr("File"));
    }
  }
  // if no coupling input file : add an empty widget
  if (inTabWidget_->count() < 2)
    inTabWidget_->newTabRequested();

  // update the ressource widget
  ressourceFileWidget_->updateTable();

  item->update(0, "inputStepChanged");
}
}
