//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose a copula inference result
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/CopulaInferenceResultWizard.hxx"

#include "persalys/CopulaInferenceAnalysis.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/CopulaInferenceResultWidget.hxx"
#include "persalys/ComboBoxDelegate.hxx"

#include <QGridLayout>
#include <QHeaderView>
#include <QtGlobal>

using namespace OT;

namespace PERSALYS
{

CopulaInferenceResultWizard::CopulaInferenceResultWizard(const Study &study, const Description &variables, QWidget *parent)
  : Wizard(parent)
  , study_(study)
  , variables_(variables)
  , inferenceResultsComboBox_(0)
  , variablesComboBox_(0)
  , tableView_(0)
  , inferenceResultStackWidget_(0)
  , errorMessageLabel_(0)
{
  buildInterface();
  resize(1000, 700);
}


void CopulaInferenceResultWizard::buildInterface()
{
  setWindowTitle(tr("Inference analyses results"));
  docLink_ = "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#dependenceinferenceresultwizard";

  QWizardPage * page = new QWizardPage(this);
  page->setTitle(tr("Define dependence between %1").arg(QtOT::DescriptionToStringList(variables_).join(", ")));
  QVBoxLayout * mainLayout = new QVBoxLayout(page);

  QWidget * topWidget = new QWidget;
  QGridLayout * topWidgetLayout = new QGridLayout(topWidget);

  // choose CopulaInferenceAnalysis
  inferenceResultsComboBox_ = new QComboBox;
  topWidgetLayout->addWidget(new QLabel(tr("Inference analysis")), 0, 0);
  topWidgetLayout->addWidget(inferenceResultsComboBox_ , 0, 1);

  for (UnsignedInteger i = 0; i < study_.getAnalyses().getSize(); ++i)
    if (study_.getAnalyses()[i].getImplementation()->getClassName() == "CopulaInferenceAnalysis")
      if (dynamic_cast<CopulaInferenceAnalysis*>(study_.getAnalyses()[i].getImplementation().get())->hasValidResult())
        inferenceResultsComboBox_->addItem(QString::fromUtf8(study_.getAnalyses()[i].getName().c_str()), (int)i);

  // choose variable
  variablesComboBox_ = new QComboBox;
  topWidgetLayout->addWidget(new QLabel(tr("Group of variables")), 1, 0);
  topWidgetLayout->addWidget(variablesComboBox_, 1, 1);

  if (variables_.getSize() > 2)
  {
    // table with the variables of the group
    tableView_ = new QTableView;
    tableView_->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    tableView_->verticalHeader()->hide();
    // - model
    QStandardItemModel * tableModel = new QStandardItemModel(1, variables_.getSize(), tableView_);
    tableView_->setModel(tableModel);
    for (UnsignedInteger i = 0; i < variables_.getSize(); ++i)
    {
      tableModel->setData(tableModel->index(0, i) , QtOT::DescriptionToStringList(variables_), Qt::UserRole + 1);
      tableModel->setData(tableModel->index(0, i) , QColor(Qt::white), Qt::BackgroundRole);
    }
    // - delegate
    ComboBoxDelegate * delegate = new ComboBoxDelegate(tableView_);
    tableView_->setItemDelegateForRow(0, delegate);
    for (UnsignedInteger i = 0; i < variables_.getSize(); ++i)
    {
      tableView_->openPersistentEditor(tableModel->index(0, i));
    }
    // - resize
    const int h = tableView_->verticalHeader()->length() + tableView_->horizontalHeader()->height();
    const QMargins margins(tableView_->contentsMargins());
    tableView_->setFixedHeight(h + margins.top() + margins.bottom());
    topWidgetLayout->addWidget(tableView_, 2, 0, 1, 2);

    // signal to update the table
    connect(variablesComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateVariablesTable(QString)));
  }

  mainLayout->addWidget(topWidget);

  // widget with tables
  inferenceResultStackWidget_ = new ResizableStackedWidget;
  mainLayout->addWidget(inferenceResultStackWidget_);
  connect(variablesComboBox_, SIGNAL(currentIndexChanged(int)), inferenceResultStackWidget_, SLOT(setCurrentIndex(int)));

  // error message
  errorMessageLabel_ = new TemporaryLabel;
  mainLayout->addWidget(errorMessageLabel_);

  // update tables
  connect(inferenceResultsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateVariablesComboBox(int)));
  updateVariablesComboBox(0);

  addPage(page);
}


void CopulaInferenceResultWizard::updateVariablesComboBox(int currentAnalysis)
{
  if (!(errorMessageLabel_ && variablesComboBox_ && inferenceResultsComboBox_))
    return;

  // reset
  variablesComboBox_->blockSignals(true);
  variablesComboBox_->clear();
  // - remove all widgets
  const int nbWidgets = inferenceResultStackWidget_->count();
  for (int i = 0; i < nbWidgets; ++i)
  {
    QWidget * widget = inferenceResultStackWidget_->currentWidget();
    inferenceResultStackWidget_->removeWidget(widget);
    widget->deleteLater();
  }

  // get variables names
  QStringList variablesNames;
  if (!inferenceResultsComboBox_->count())
    return;

  const int analysisIndex = inferenceResultsComboBox_->itemData(currentAnalysis).toInt();
  CopulaInferenceResult result(dynamic_cast<CopulaInferenceAnalysis*>(study_.getAnalyses()[analysisIndex].getImplementation().get())->getResult());

  const Description variablesDOE = result.getDesignOfExperiment().getSample().getDescription();

  for (UnsignedInteger i = 0; i < result.getCopulaInferenceSetResultCollection().getSize(); ++i)
  {
    const Description setVariables = result.getCopulaInferenceSetResultCollection()[i].getSetOfVariablesNames();
    if (setVariables.getSize() == variables_.getSize())
    {
      variablesNames << QString::fromUtf8(setVariables.__str__().c_str());

      Indices indices;
      for (UnsignedInteger j = 0; j < setVariables.getSize(); ++j)
        for (UnsignedInteger k = 0; k < variablesDOE.getSize(); ++k)
          if (variablesDOE[k] == setVariables[j])
            indices.add(k);

      // - create a CopulaInferenceResultWidget
      CopulaInferenceResultWidget * inferenceResultWidget_i =  new CopulaInferenceResultWidget(result.getCopulaInferenceSetResultCollection()[i],
          result.getDesignOfExperiment().getSample().getMarginal(indices),
          false,
          this);

      inferenceResultStackWidget_->addWidget(inferenceResultWidget_i);
    }
  }
  // if no copula with a dim of variables_.getSize()
  if (!inferenceResultStackWidget_->count())
  {
    variablesNames << " ";
    QWidget * aWidget = new QWidget;
    QVBoxLayout * aWidgetLayout = new QVBoxLayout(aWidget);
    QLabel * label = new QLabel(tr("The result does not contain infered copula of dimension equal to %1").arg(variables_.getSize()));
    label->setWordWrap(true);
    aWidgetLayout->addWidget(label, 0, Qt::AlignTop);
    inferenceResultStackWidget_->addWidget(aWidget);
    if (tableView_)
      tableView_->hide();
  }
  else
  {
    if (tableView_)
      tableView_->show();

    variablesComboBox_->addItems(variablesNames);
    variablesComboBox_->blockSignals(false);
    variablesComboBox_->setCurrentIndex(0);
    inferenceResultStackWidget_->setCurrentIndex(0);
    updateVariablesTable(variablesComboBox_->currentText());
  }
}


void CopulaInferenceResultWizard::updateVariablesTable(const QString &text)
{
  if (!tableView_ || text.isEmpty())
    return;

#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
  const QStringList variablesNames(text.split(QRegularExpression("\\W+"), Qt::SkipEmptyParts));
#else
  const QStringList variablesNames(text.split(QRegularExpression("\\W+"), QString::SkipEmptyParts));
#endif
  QStringList variablesNamesCopy(variablesNames);
  variablesNamesCopy.sort();
  QStringList variablesList(QtOT::DescriptionToStringList(variables_));
  variablesList.sort();
  for (UnsignedInteger i = 0; i < variables_.getSize(); ++i)
  {
    tableView_->model()->setData(tableView_->model()->index(0, i), variablesNames != variablesList ? variables_[i].c_str() : variablesNames[i]);
    tableView_->model()->setHeaderData(i, Qt::Horizontal, variablesNames[i]);
  }
}


Distribution CopulaInferenceResultWizard::getCopula() const
{
  CopulaInferenceResultWidget * widget = static_cast<CopulaInferenceResultWidget*>(inferenceResultStackWidget_->currentWidget());
  Distribution copula(widget->getCopula());

  // if Normal copula
  if (tableView_)
  {
    Indices indices(variables_.getSize());
    for (UnsignedInteger i = 0; i < variables_.getSize(); ++i)
    {
      for (UnsignedInteger j = 0; j < variables_.getSize(); ++j)
      {
        if (variables_[i] == tableView_->model()->data(tableView_->model()->index(0, j)).toString().toStdString())
        {
          indices[i] = j;
          break;
        }
      }
    }
    copula = copula.getMarginal(indices);
  }
  return copula;
}


bool CopulaInferenceResultWizard::validateCurrentPage()
{
  CopulaInferenceResultWidget * widget = dynamic_cast<CopulaInferenceResultWidget*>(inferenceResultStackWidget_->currentWidget());
  if (!widget)
  {
    errorMessageLabel_->setTemporaryErrorMessage(tr("No copula with a dimension of %1 in this result").arg(variables_.getSize()));
    return false;
  }
  if (!widget->isSelectedCopulaValid())
  {
    errorMessageLabel_->setTemporaryErrorMessage(tr("The selected copula is not valid."));
    return false;
  }
  if (tableView_)
  {
    Indices indices(variables_.getSize());
    Description varList;
    for (UnsignedInteger i = 0; i < variables_.getSize(); ++i)
    {
      for (UnsignedInteger j = 0; j < variables_.getSize(); ++j)
      {
        if (variables_[i] == tableView_->model()->data(tableView_->model()->index(0, j)).toString().toStdString())
        {
          indices[i] = j;
          varList.add(variables_[i]);
        }
      }
    }
    if (!indices.check(variables_.getSize()))
    {
      errorMessageLabel_->setTemporaryErrorMessage(tr("Invalid list %1. The elements must be single-use.").arg(varList.__str__().c_str()));
      return false;
    }
  }

  return QWizard::validateCurrentPage();
}
}
