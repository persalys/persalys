//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a quantile analysis
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/QuantileAnalysisWizard.hxx"
#include "persalys/QuantileAnalysis.hxx"

#include "persalys/EditButtonDelegate.hxx"
#include "persalys/LineEditWithQValidatorDelegate.hxx"

#include <QVBoxLayout>
#include <QRadioButton>

#include <qwt_legend.h>

using namespace OT;

namespace PERSALYS
{

  QuantileAnalysisIntroPage::QuantileAnalysisIntroPage(QWidget* parent)
    : QWizardPage(parent)
  {
    setWindowTitle(tr("Quantile estimation - Interest variables"));
    QVBoxLayout * pageLayout = new QVBoxLayout(this);

    // output selection
    outputsSelectionGroupBox_ = new OutputsSelectionGroupBox(this);
    outputsSelectionGroupBox_->setTitle(tr("Variable(s) of interest"));
    pageLayout->addWidget(outputsSelectionGroupBox_);

    // choose the method
    QGroupBox * methodBox = new QGroupBox(tr("Method"));
    QVBoxLayout * methodLayout = new QVBoxLayout(methodBox);

    methodGroup_ = new QButtonGroup(this);

    // MC
    QRadioButton * buttonMC = new QRadioButton(tr("Monte Carlo"));
    buttonMC->setChecked(true);
    methodGroup_->addButton(buttonMC, QuantileAnalysisResult::MonteCarlo);
    methodLayout->addWidget(buttonMC);

    // GPD
    QRadioButton * buttonGeneralizedPareto = new QRadioButton(tr("Excess with Generalized Pareto distribution"));
    methodGroup_->addButton(buttonGeneralizedPareto, QuantileAnalysisResult::GeneralizedPareto);
    methodLayout->addWidget(buttonGeneralizedPareto);

    pageLayout->addWidget(methodBox);

    // error message
    errorMessageLabel_ = new TemporaryLabel;
    connect(outputsSelectionGroupBox_, SIGNAL(outputsSelectionChanged(QStringList)), errorMessageLabel_, SLOT(reset()));

    pageLayout->addStretch();
    pageLayout->addWidget(errorMessageLabel_);
  }


  void QuantileAnalysisIntroPage::initialize(const Analysis& analysis)
  {

    const QuantileAnalysis * analysis_ptr = dynamic_cast<const QuantileAnalysis*>(analysis.getImplementation().get());
    if (!analysis_ptr)
      return;

    // method
    if (analysis_ptr->getType() == QuantileAnalysisResult::MonteCarlo)
      methodGroup_->button(QuantileAnalysisResult::MonteCarlo)->click();
    else if (analysis_ptr->getType() == QuantileAnalysisResult::GeneralizedPareto)
      methodGroup_->button(QuantileAnalysisResult::GeneralizedPareto)->click();
    else
      throw InvalidArgumentException(HERE) << "QuantileAnalysisIntroPage::initialize : Unknown method";

    // update outputs list
    outputsSelectionGroupBox_->updateComboBoxModel(analysis_ptr->getDesignOfExperiment().getSample().getDescription(), analysis_ptr->getInterestVariables());
  }


  int QuantileAnalysisIntroPage::nextId() const
  {
    return QuantileAnalysisWizard::Page_Probability;
  }

  bool QuantileAnalysisIntroPage::validatePage()
  {
    if (outputsSelectionGroupBox_->getSelectedOutputsNames().size() < 1)
    {
      errorMessageLabel_->setErrorMessage(tr("At least 1 outputs must be selected"));
      return false;
    }
    emit outputSelected();
    return QWizardPage::validatePage();
  }


  UnsignedInteger QuantileAnalysisIntroPage::getType() const
  {
    return methodGroup_->checkedId();
  }


  Description QuantileAnalysisIntroPage::getInterestVariables() const
  {
    const QStringList outputsList = outputsSelectionGroupBox_->getSelectedOutputsNames();

    Description outputNames(outputsList.size());
    for (int i = 0; i < outputsList.size(); ++i)
      outputNames[i] = outputsList[i].toUtf8().constData();

    return outputNames;
  }

  QuantileAnalysisProbabilityPage::QuantileAnalysisProbabilityPage(QWidget* parent)
    : QWizardPage(parent)
  {
    QVBoxLayout * pageLayout = new QVBoxLayout(this);

    // Default target
    QGroupBox * targetBox = new QGroupBox(tr("Target probability"));
    QVBoxLayout * targetLayout = new QVBoxLayout(targetBox);

    QLabel * label = new QLabel(tr("Default target probabilities, separated with ';'"));
    targetLayout->addWidget(label);
    targetLineEdit_ = new ValuesLineEdit;
    targetLayout->addWidget(targetLineEdit_);

    // target for variables
    tableView_ = new ResizableHeaderlessTableView;
    tableView_->setEditTriggers(QTableView::AllEditTriggers);
    tableView_->resizeColumnsToContents();
    tableView_->setSpan(0,4,1,2);
    targetLayout->addWidget(tableView_);
    targetLayout->addStretch();
    pageLayout->addWidget(targetBox);

    errorMessageLabel_ = new TemporaryLabel;
    pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);
  }


  void QuantileAnalysisProbabilityPage::initialize(const Analysis& analysis)
  {
    const QuantileAnalysis * analysis_ptr = dynamic_cast<const QuantileAnalysis*>(analysis.getImplementation().get());

    if (!analysis_ptr)
      return;

    // update target probas
    targetLineEdit_->blockSignals(true);
    targetLineEdit_->setText(QtOT::PointToString(analysis_ptr->getDefaultTargetProbability()));
    targetLineEdit_->setFocus();
    targetLineEdit_->blockSignals(false);

    // fill table
    tableModel_ = new QuantileTableModel(*analysis_ptr, this);
    connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setErrorMessage(QString)));
    tableView_->setModel(tableModel_);
    tableView_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    tableView_->horizontalHeader()->setSectionResizeMode(5, QHeaderView::ResizeToContents);

    const QString numberPattern = QString("(\\d+\\.*\\d*e{0,1}[+-]*\\d*)");
    const QString numbersPattern = numberPattern + "(;{1}\\s*" + numberPattern + ")*$";
    LineEditWithQValidatorDelegate * delegate1 = new LineEditWithQValidatorDelegate(numbersPattern, tableView_);
    tableView_->setItemDelegateForColumn(4, delegate1);

    EditButtonDelegate * delegate2 = new EditButtonDelegate(tableView_, 1, 0.1);
    tableView_->setItemDelegateForColumn(5, delegate2);

    for (int i = 1; i < tableModel_->rowCount(); ++i)
    {
      tableView_->openPersistentEditor(tableModel_->index(i, 4));
      tableView_->openPersistentEditor(tableModel_->index(i, 5));
    }

    // resize table
    // tableView_->resizeWithOptimalWidth();

    connect(targetLineEdit_, &QLineEdit::editingFinished, [ = ] () {
      Collection<OT::Point> values;
      for (int i = 1; i < tableModel_->rowCount(); ++i)
        values.add(targetLineEdit_->values());
      tableModel_->setValues(values);
    });
  }

  int QuantileAnalysisProbabilityPage::nextId() const
  {
    return QuantileAnalysisWizard::Page_Threshold;
  }

  bool QuantileAnalysisProbabilityPage::validatePage()
  {
    if (!errorMessageLabel_->text().isEmpty())
      return false;
    emit probabilitiesSelected();
    return QWizardPage::validatePage();
  }

  void QuantileAnalysisProbabilityPage::keyPressEvent (QKeyEvent * event)
  {
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
      event->accept();
  }


  QuantileAnalysisThresholdPage::QuantileAnalysisThresholdPage(QWidget* parent)
    : QWizardPage(parent)
  {
    QVBoxLayout * pageLayout = new QVBoxLayout(this);

    QTabWidget * tabWidget = new QTabWidget;
    pageLayout->addWidget(tabWidget);

    // threshold for variables
    QVBoxLayout * thresholdLayout = new QVBoxLayout(tabWidget);

    tableView_ = new ResizableHeaderlessTableView;
    tableView_->setEditTriggers(QTableView::AllEditTriggers);

    thresholdLayout->addWidget(tableView_);
    thresholdLayout->addStretch();
    tabWidget->addTab(tableView_, tr("Threshold"));

    QWidget * tab = new QWidget;
    QGridLayout * tabLayout = new QGridLayout(tab);

    marginalComboBox_ = new QComboBox;
    tailComboBox_ = new QComboBox;
    tabLayout->addWidget(marginalComboBox_, 0, 0);
    tabLayout->addWidget(tailComboBox_, 0, 1);
    tabLayout->setColumnStretch(2, 1);

    plotWidget_ = new PlotWidget;
    tabLayout->addWidget(plotWidget_, 1, 0, 1, 3);
    tabWidget->addTab(tab, tr("Mean excess"));

    errorMessageLabel_ = new TemporaryLabel;
    pageLayout->addWidget(errorMessageLabel_, 0, Qt::AlignBottom);
  }



  void QuantileAnalysisThresholdPage::initialize(const Analysis& analysis)
  {
    QuantileAnalysis * analysis_ptr = dynamic_cast<QuantileAnalysis*>(analysis.getImplementation().get());

    if (!analysis_ptr)
      return;

    // fill table
    tableModel_ = new QuantileThresholdTableModel(*analysis_ptr, this);
    tableView_->setModel(tableModel_);
    tableView_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(4, QHeaderView::ResizeToContents);
    tableView_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    tableView_->setSpan(0,0,2,1);
    tableView_->setSpan(0,1,1,2);
    tableView_->setSpan(0,3,1,2);
    tableView_->setSpan(0,5,1,2);

    connect(tableModel_, SIGNAL(errorMessageChanged(QString)), errorMessageLabel_, SLOT(setErrorMessage(QString)));

    // plot widget
    if (marginalComboBox_->count())
      marginalComboBox_->clear();
    marginalComboBox_->addItems(QtOT::DescriptionToStringList(analysis_ptr->getInterestVariables()));
    if (tailComboBox_->count())
      tailComboBox_->clear();
    tailComboBox_->addItems(QStringList() << tr("Lower") << tr("Upper"));

    plotMeanExcess(analysis_ptr->plotMeanExcess(0,0), QString(analysis_ptr->getInterestVariables()[0].c_str()));

    connect(marginalComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), [ = ] (int iMarg)
    {
      if (iMarg >= 0)
      {
        const QString varName = QString(analysis_ptr->getInterestVariables()[marginalComboBox_->currentIndex()].c_str());
        plotMeanExcess(analysis_ptr->plotMeanExcess(iMarg, tailComboBox_->currentIndex()), varName);
      }
    });
    connect(tailComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), [ = ] (int iTail)
    {
      if (iTail >= 0)
      {
        const QString varName = QString(analysis_ptr->getInterestVariables()[marginalComboBox_->currentIndex()].c_str());
        plotMeanExcess(analysis_ptr->plotMeanExcess(marginalComboBox_->currentIndex(), iTail), varName);
      }
    });

  }

  void QuantileAnalysisThresholdPage::plotMeanExcess(Graph graph, const QString& varName)
  {
    plotWidget_->clear();
    QwtSymbol* symbol1 = new QwtSymbol(QwtSymbol::Ellipse,
                                       QBrush(Qt::red),
                                       QPen(Qt::red),
                                       QSize(0, 0));
    QwtSymbol* symbol2 = new QwtSymbol(QwtSymbol::Ellipse,
                                       QBrush(Qt::blue),
                                       QPen(Qt::blue),
                                       QSize(0, 0));
    QwtSymbol* symbol3 = new QwtSymbol(QwtSymbol::Ellipse,
                                       QBrush(Qt::blue),
                                       QPen(Qt::blue),
                                       QSize(0, 0));
    QwtPlotCurve::CurveStyle style = QwtPlotCurve::Lines;
    plotWidget_->plotCurve(graph.getDrawable(0).getData(), QPen(Qt::red, 1),
                           style, symbol1, QString("MRL"));
    plotWidget_->plotCurve(graph.getDrawable(1).getData(), QPen(Qt::blue, 1),
                           style, symbol2, QString("CI-"));
    plotWidget_->plotCurve(graph.getDrawable(2).getData(), QPen(Qt::blue, 1),
                           style, symbol3, QString("CI+"));

    plotWidget_->setAxisTitle(QwtPlot::yLeft, tr("Mean Excess"));
    plotWidget_->setAxisTitle(QwtPlot::xBottom, varName);
    QwtLegend * legend = new QwtLegend();
    plotWidget_->insertLegend(legend, QwtPlot::RightLegend);
  }

  int QuantileAnalysisThresholdPage::nextId() const
  {
    return -1;
  }

  bool QuantileAnalysisThresholdPage::validatePage()
  {
    if (!errorMessageLabel_->text().isEmpty())
      return false;

    return QWizardPage::validatePage();
  }

  QuantileAnalysisWizard::QuantileAnalysisWizard(const Analysis& analysis, QWidget* parent)
    : AnalysisWizard(analysis, parent)
  {
    buildInterface();
  }

  void QuantileAnalysisWizard::buildInterface()
  {
    setWindowTitle(tr("Quantile Analysis"));
    docLink_ = "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#quantileanalysis";

    // 1st page - Interest variables
    introPage_ = new QuantileAnalysisIntroPage(this);
    setPage(0, introPage_);

    // 2nd page - Target probabilities
    probabilityPage_ = new QuantileAnalysisProbabilityPage(this);
    setPage(1, probabilityPage_);

    // 3rd page - Threshold/parameters for tail fitting
    thresholdPage_ = new QuantileAnalysisThresholdPage(this);
    setPage(2, thresholdPage_);

    initialize(analysis_);
    setStartId(0);
  }

  void QuantileAnalysisWizard::initialize(const Analysis & analysis)
  {
    QuantileAnalysis * analysis_ptr = dynamic_cast<QuantileAnalysis*>(analysis.getImplementation().get());

    if (!analysis_ptr)
      return;

    introPage_->initialize(analysis_);
    connect(introPage_, &QuantileAnalysisIntroPage::outputSelected, [ = ]()
    {
      analysis_ptr->setInterestVariables(introPage_->getInterestVariables());
      analysis_ptr->setType(introPage_->getType());
      probabilityPage_->initialize(analysis_);
    });

    connect(probabilityPage_, &QuantileAnalysisProbabilityPage::probabilitiesSelected, [ = ] ()
    {
      if (analysis_ptr->getType() == QuantileAnalysisResult::GeneralizedPareto)
      {
        analysis_ptr->setTailTypes(probabilityPage_->getTailTypes());
        analysis_ptr->setTargetProbabilities(probabilityPage_->getTargetProbabilities());
        thresholdPage_->initialize(analysis_);
      }
    });

  }

  int QuantileAnalysisWizard::nextId() const
  {
    switch (currentId())
    {
    case Page_Intro:
      return introPage_->nextId();
    case Page_Probability:
      if (introPage_->getType() == QuantileAnalysisResult::MonteCarlo)
        return -1;
      else
        return probabilityPage_->nextId();
    case Page_Threshold:
      return thresholdPage_->nextId();
    default:
      return -1;
    }
  }

  Analysis QuantileAnalysisWizard::getAnalysis() const
  {
    QuantileAnalysis * analysis_ptr = dynamic_cast<QuantileAnalysis*>(analysis_.getImplementation().get());

    if (!analysis_ptr)
      throw InvalidArgumentException(HERE) << "QuantileAnalysisWizard::getAnalysis : Invalid analysis";

    QuantileAnalysis analysis(analysis_ptr->getName(), analysis_ptr->getDesignOfExperiment());
    analysis.setType(introPage_->getType());
    analysis.setInterestVariables(introPage_->getInterestVariables());
    analysis.setDefaultTargetProbability(probabilityPage_->getDefaultTargetProbability());
    analysis.setTailTypes(probabilityPage_->getTailTypes());
    analysis.setTargetProbabilities(probabilityPage_->getTargetProbabilities());
    if (introPage_->getType() == QuantileAnalysisResult::GeneralizedPareto)
      analysis.setThreshold(thresholdPage_->getThreshold());
    return analysis;
  }
}
