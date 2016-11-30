//                                               -*- C++ -*-
/**
 *  @brief QWizard to choose an inference result
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/InferenceResultWizard.hxx"

#include "otgui/InferenceAnalysis.hxx"

#include <QGridLayout>
#include <QScrollArea>
#include <QHeaderView>
#include <QGroupBox>
#include <QStyledItemDelegate>
#include <QPainter>
#include <QApplication>

using namespace OT;

namespace OTGUI {

class RadioButtonDelegate : public QStyledItemDelegate
{
public:
  RadioButtonDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
  {
  }

  void paint(QPainter *painter, 
             const QStyleOptionViewItem &option, 
             const QModelIndex &index) const
  {
    QStyleOptionButton optionButton;
    optionButton.rect = option.rect;
    optionButton.text  = index.data(Qt::DisplayRole).toString();

    if (index.data(Qt::CheckStateRole).toBool())
      optionButton.state |= QStyle::State_On;
    else
      optionButton.state |= QStyle::State_Off;

    optionButton.state |= QStyle::State_Enabled;

    QApplication::style()->drawControl(QStyle::CE_RadioButton, &optionButton, painter);
  }
};




InferenceResultWizard::InferenceResultWizard(const OTStudy& otStudy, QWidget* parent)
  : OTguiWizard(parent)
  , otStudy_(otStudy)
  , errorMessageLabel_(new QLabel)
{
  buildInterface();
}


void InferenceResultWizard::buildInterface()
{
  setWindowTitle(tr("Inference analyses results"));

  QWizardPage * page = new QWizardPage(this);
  QGridLayout * mainLayout = new QGridLayout(page);

  // choose InferenceAnalysis
  inferenceResultsComboBox_ = new QComboBox;
  mainLayout->addWidget(new QLabel(tr("Inference analysis")), 0, 0);
  mainLayout->addWidget(inferenceResultsComboBox_ , 0, 1);

  QStringList inferenceAnalyses;
  for (UnsignedInteger i=0; i<otStudy_.getAnalyses().getSize(); ++i)
  {
    if (otStudy_.getAnalyses()[i].getImplementation()->getClassName() == "InferenceAnalysis")
    {
      inferenceAnalyses << otStudy_.getAnalyses()[i].getName().c_str();
      inferenceResultsComboBox_->addItem(otStudy_.getAnalyses()[i].getName().c_str(), (int)i);
    }
  }
  // choose variable
  variablesComboBox_ = new QComboBox;
  connect(variablesComboBox_, SIGNAL(currentIndexChanged(QString)), this, SLOT(updateDistributionTable(QString)));
  mainLayout->addWidget(new QLabel(tr("Variable")), 1, 0);
  mainLayout->addWidget(variablesComboBox_, 1, 1);

  // display distributions
  QScrollArea * scrollArea = new QScrollArea;
  scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  scrollArea->setWidgetResizable(true);

  QWidget * distTabListWidget = new QWidget;
  QHBoxLayout * distTabListWidgetLayout = new QHBoxLayout(distTabListWidget);
  distTabListWidgetLayout->setSizeConstraint(QLayout::SetFixedSize);

  // -- table result
  QGroupBox * distGroupBox = new QGroupBox(tr("Accepted distributions"));
  QVBoxLayout * distGroupBoxLayout = new QVBoxLayout(distGroupBox);

  distTableView_ = new ResizableTableViewWithoutScrollBar;
  RadioButtonDelegate * delegate = new RadioButtonDelegate(distTableView_);
  distTableView_->setItemDelegateForColumn(0, delegate);
  distTableView_->setSelectionMode(QAbstractItemView::SingleSelection);
  distTableView_->setSelectionBehavior(QAbstractItemView::SelectRows);
  distTableView_->verticalHeader()->hide();

  distTableModel_ = new CustomStandardItemModel(0, 2, distTableView_);
  distTableView_->setModel(distTableModel_);
  connect(distTableView_->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(updateParametersTable(QModelIndex,QModelIndex)));
  connect(distTableView_, SIGNAL(clicked(QModelIndex)), this, SLOT(updateRadioButtonsDistributionTable(QModelIndex)));

  distGroupBoxLayout->addWidget(distTableView_);
  distGroupBoxLayout->addStretch();
  distTabListWidgetLayout->addWidget(distGroupBox);
  scrollArea->setWidget(distTabListWidget);

  // -- distribution parameters table
  QGroupBox * paramGroupBox = new QGroupBox(tr("Selected distribution parameters"));
  QVBoxLayout * paramGroupBoxLayout = new QVBoxLayout(paramGroupBox);

  distParamTableView_ = new ResizableTableViewWithoutScrollBar;
  distParamTableView_->horizontalHeader()->hide();
  distParamTableView_->verticalHeader()->hide();

  distParamTableModel_ = new CustomStandardItemModel(4, 2, distParamTableView_);
  distParamTableView_->setModel(distParamTableModel_);

  paramGroupBoxLayout->addWidget(distParamTableView_);
  paramGroupBoxLayout->addStretch();

  distTabListWidgetLayout->addWidget(paramGroupBox);

  mainLayout->addWidget(scrollArea, 2, 0, 1, 2);

  // update tables
  connect(inferenceResultsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateVariablesComboBox(int)));
  updateVariablesComboBox(0);

  // error message
  errorMessageLabel_->setWordWrap(true);
  mainLayout->addWidget(errorMessageLabel_, 3, 0, 1, 2);
  
  addPage(page);
}


void InferenceResultWizard::updateVariablesComboBox(int currentAnalysis)
{
  // reset
  errorMessageLabel_->setText("");
  variablesComboBox_->clear();

  // get variables names
  QStringList variablesNames;
  if (inferenceResultsComboBox_->count())
  {
    const int analysisIndex = inferenceResultsComboBox_->itemData(inferenceResultsComboBox_->currentIndex()).toInt();

    InferenceResult result(dynamic_cast<InferenceAnalysis*>(&*otStudy_.getAnalyses()[analysisIndex].getImplementation())->getResult());
    Collection< FittingTestResult > fittingTestResultCollection(result.getFittingTestResultCollection());

    for (UnsignedInteger i=0; i<fittingTestResultCollection.getSize(); ++i)
    {
      for (UnsignedInteger j=0; j<fittingTestResultCollection[i].getKolmogorovTestResults().getSize(); ++j)
      {
        if (fittingTestResultCollection[i].getKolmogorovTestResults()[j].getBinaryQualityMeasure())
        {
          variablesNames << fittingTestResultCollection[i].getVariableName().c_str();
          break;
        }
      }
    }
  }
  variablesComboBox_->addItems(variablesNames);

  // update tables
  if (variablesNames.size())
    variablesComboBox_->setCurrentIndex(0);
  else
  {
    updateDistributionTable("");
    QString errorMessage;
    if (inferenceResultsComboBox_->count())
      errorMessage = tr("The selected analysis does not contain accepted distributions");
    else
      errorMessage = tr("No available inference result in the current study");
    errorMessageLabel_->setText(QString("%1%2%3").arg("<font color=red>").arg(errorMessage).arg("</font>"));
  }
}


void InferenceResultWizard::updateDistributionTable(QString variableName)
{
  // reset
  distTableModel_->clear();

  // horizontal header
  distTableModel_->setHorizontalHeaderLabels(QStringList() << tr("Distributions") << tr("p-value"));

  // table
  const int analysisIndex = inferenceResultsComboBox_->itemData(inferenceResultsComboBox_->currentIndex()).toInt();

  if (!variableName.isEmpty())
  {
    // -- get results of the variable
    InferenceResult result(dynamic_cast<InferenceAnalysis*>(&*otStudy_.getAnalyses()[analysisIndex].getImplementation())->getResult());
    FittingTestResult fittingTestResult(result.getFittingTestResultForVariable(variableName.toStdString()));

    NumericalPoint pValues;
    QStringList distributionsNames;
    Collection < Distribution > distCollection;
    for (UnsignedInteger i=0; i<fittingTestResult.getKolmogorovTestResults().getSize(); ++i)
    {
      if (fittingTestResult.getKolmogorovTestResults()[i].getBinaryQualityMeasure())
      {
        pValues.add(fittingTestResult.getKolmogorovTestResults()[i].getPValue());
        distributionsNames << fittingTestResult.getTestedDistributions()[i].getImplementation()->getClassName().c_str();
        distCollection.add(fittingTestResult.getTestedDistributions()[i]);
      }
    }
    // -- sort indices list
    Indices indices(pValues.getSize());
    if (pValues.getSize() > 1)
    {
      indices.fill();
      for (int i=(pValues.getSize()-1); i>=0; --i)
      {
        for (int l=1; l<=i; ++l)
        {
          if (pValues[l-1] < pValues[l])
          {
            NumericalScalar temp = pValues[l-1];
            pValues[l-1] = pValues[l];
            pValues[l] = temp;
            UnsignedInteger ind_temp = indices[l-1];
            indices[l-1] = indices[l];
            indices[l] = ind_temp;
          }
        }
      }
    }

    // -- fill table
    for (int i=0; i<distributionsNames.size(); ++i)
    {
      distTableModel_->setNotEditableItem(i, 0, distributionsNames[indices[i]]);
      QVariant aVariant = QVariant::fromValue(distCollection[indices[i]]);
      distTableModel_->setData(distTableModel_->index(i, 0), aVariant, Qt::UserRole);
      distTableModel_->setNotEditableItem(i, 1, pValues[i], 3);
    }
    // -- update parameters table
    distTableView_->selectRow(0);
    distTableModel_->setData(distTableModel_->index(0, 0), true, Qt::CheckStateRole);
  }
  else
  {
    // update parameters table
    updateParametersTable();
  }

  // resize
  distTableView_->resizeToContents();
}


void InferenceResultWizard::updateRadioButtonsDistributionTable(QModelIndex current)
{
  for (int i=0; i<distTableModel_->rowCount(); ++i)
  {
    if (distTableModel_->index(i, 0).row() == current.row())
      distTableModel_->setData(distTableModel_->index(i, 0), true, Qt::CheckStateRole);
    else
      distTableModel_->setData(distTableModel_->index(i, 0), false, Qt::CheckStateRole);
  }
}


void InferenceResultWizard::updateParametersTable(QModelIndex current, QModelIndex previous)
{
  // reset
  distParamTableModel_->clear();
  distParamTableModel_->setColumnCount(2);

  QFont font;
  font.setBold(true);

  // headers
  distParamTableModel_->setNotEditableItem(0, 0, tr("Moments"));
  distParamTableView_->setSpan(0, 0, 1, 2);
  distParamTableModel_->setData(distParamTableModel_->index(0, 0), font, Qt::FontRole);

  distParamTableModel_->setNotEditableHeaderItem(1, 0, tr("Mean"));
  distParamTableModel_->setNotEditableHeaderItem(2, 0, tr("Standard deviation"));
  distParamTableModel_->setNotEditableHeaderItem(3, 0, tr("Skewness"));
  distParamTableModel_->setNotEditableHeaderItem(4, 0, tr("Kurtosis"));

  // fill
  if (current.isValid())
  {
    // -- get distribution
    QVariant variant = distTableModel_->data(distTableModel_->index(current.row(), 0), Qt::UserRole);
    Distribution distribution = variant.value<Distribution>();

    // -- set titles
    if (distribution.getImplementation()->getClassName() != "Normal") // mean and std already displayed
    {
      UnsignedInteger nbParamDesc = distribution.getParameterDescription().getSize();
      if (distribution.getImplementation()->getClassName() == "Student")
        nbParamDesc = 1; // display only nu (mean and std already displayed)

      distParamTableModel_->setNotEditableItem(5, 0, tr("Native parameters"));
      distParamTableView_->setSpan(5, 0, 1, 2);
      distParamTableModel_->setData(distParamTableModel_->index(5, 0), font, Qt::FontRole);

      for (UnsignedInteger i=0; i<nbParamDesc; ++i)
        distParamTableModel_->setNotEditableHeaderItem(6+i, 0, distribution.getParameterDescription()[i].c_str());
    }

    // -- set parameters values
    distParamTableModel_->setNotEditableItem(1, 1, distribution.getMean()[0]);
    distParamTableModel_->setNotEditableItem(2, 1, distribution.getStandardDeviation()[0]);
    try
    {
      distParamTableModel_->setNotEditableItem(3, 1, distribution.getSkewness()[0]);
    }
    catch (std::exception & ex)
    {
      distParamTableModel_->setNotEditableItem(3, 1, tr("-"));
    }
    try
    {
      distParamTableModel_->setNotEditableItem(4, 1, distribution.getKurtosis()[0]);
    }
    catch (std::exception & ex)
    {
      distParamTableModel_->setNotEditableItem(4, 1, tr("-"));
    }

    if (distribution.getImplementation()->getClassName() != "Normal") // mean and std already displayed
    {
      UnsignedInteger nbParamVal = distribution.getParameterDescription().getSize();
      if (distribution.getImplementation()->getClassName() == "Student")
        nbParamVal = 1; // display only nu (mean and std already displayed)

      for (UnsignedInteger i=0; i<nbParamVal; ++i)
        distParamTableModel_->setNotEditableItem(6+i, 1, distribution.getParameter()[i]);
    }
  }
  else
  {
    for (UnsignedInteger i=0; i<4; ++i)
     distParamTableModel_->setNotEditableItem(1+i, 1, tr("-"));
  }
  // resize
  distParamTableView_->resizeToContents();
}


Distribution InferenceResultWizard::getDistribution() const
{
  QModelIndex currentDistributionIndex = distTableModel_->index(distTableView_->currentIndex().row(), 0);

  QVariant variant = distTableModel_->data(currentDistributionIndex, Qt::UserRole);
  Distribution distribution = variant.value<Distribution>();

  return distribution;
}


bool InferenceResultWizard::validateCurrentPage()
{
  if (!inferenceResultsComboBox_->count() || !distTableModel_->rowCount())
    return false;

  return QWizard::validateCurrentPage();
}
}