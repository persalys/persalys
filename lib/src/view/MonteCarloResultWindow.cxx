//                                               -*- C++ -*-
/**
 *  @brief QMdiSubWindow for the results of Monte Carlo
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
#include "otgui/MonteCarloResultWindow.hxx"

#include "otgui/ExportableTableView.hxx"
#include "otgui/SampleTableModel.hxx"
#include "otgui/MonteCarloAnalysis.hxx"

#include <QVBoxLayout>

using namespace OT;

namespace OTGUI {

MonteCarloResultWindow::MonteCarloResultWindow(AnalysisItem * item)
  : DataAnalysisWindow(item)
{
  if (dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation()))
    initialize(item);
  else
    throw InvalidArgumentException (HERE) << "Can NOT build the MonteCarloResultWindow: The analysis of the item is not valid";

  buildInterface();

  setWindowTitle(tr("Monte Carlo analysis results"));
}


void MonteCarloResultWindow::initialize(AnalysisItem* item)
{
  result_ = dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->getResult();

  isConfidenceIntervalRequired_ = dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->isConfidenceIntervalRequired();
  levelConfidenceInterval_ = dynamic_cast<MonteCarloAnalysis*>(&*item->getAnalysis().getImplementation())->getLevelConfidenceInterval();

  PhysicalModel model(dynamic_cast<const PhysicalModelAnalysis*>(&*item->getAnalysis().getImplementation())->getPhysicalModel());

  // inputs
  for (UnsignedInteger i=0; i<result_.getInputSample().getDimension(); ++i)
  {
    const String inputName = result_.getInputSample().getDescription()[i];
    stochInputNames_ << QString::fromUtf8(inputName.c_str());
    QString inputDescription;
    try
    {
      inputDescription = QString::fromUtf8(model.getInputByName(inputName).getDescription().c_str());
    }
    catch (std::exception) // maybe the variable does not exist anymore
    {
      //do nothing
    }
    if (!inputDescription.isEmpty())
      inAxisTitles_ << inputDescription;
    else
      inAxisTitles_ << stochInputNames_.last();
  }

  // outputs
  for (UnsignedInteger i=0; i<result_.getOutputSample().getDimension(); ++i)
  {
    const String outputName = result_.getOutputSample().getDescription()[i];
    outputNames_ << QString::fromUtf8(outputName.c_str());
    QString outputDescription;
    try
    {
      outputDescription = QString::fromUtf8(model.getOutputByName(outputName).getDescription().c_str());
    }
    catch (std::exception) // maybe the variable does not exist anymore
    {
      //do nothing
    }
    if (!outputDescription.isEmpty())
      outAxisTitles_ << outputDescription;
    else
      outAxisTitles_ << outputNames_.last();
  }

  sampleSizeTitle_ = tr("Number of calls:") + " ";

  setParameters(item->getAnalysis());

  tabOffset_ = 1;

  // first tab: Table --------------------------------
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);
  ExportableTableView * tabResultView = new ExportableTableView;
  SampleTableModel * tabResultModel = new SampleTableModel(result_.getSample(), tabResultView);
  tabResultView->setModel(tabResultModel);
  tabLayout->addWidget(tabResultView);

  tabWidget_->addTab(tab, tr("Result table"));

  resultsSampleIsValid_ = tabResultModel->sampleIsValid();
}


void MonteCarloResultWindow::setParameters(const Analysis & analysis)
{
  const MonteCarloAnalysis * MCanalysis = dynamic_cast<const MonteCarloAnalysis*>(&*analysis.getImplementation());

  // ParametersWidget
  QStringList namesList;
  namesList << tr("Algorithm");
  if (MCanalysis->isConfidenceIntervalRequired())
    namesList << tr("Confidence level");
  namesList << tr("Maximum coefficient of variation");
  namesList << tr("Maximum elapsed time");
  namesList << tr("Maximum calls");
  namesList << tr("Block size");
  namesList << tr("Seed");

  QStringList valuesList;
  valuesList << tr("Monte Carlo");
  if (MCanalysis->isConfidenceIntervalRequired())
    valuesList << QString::number(MCanalysis->getLevelConfidenceInterval()*100) + "\%";
  valuesList << QString::number(MCanalysis->getMaximumCoefficientOfVariation());
  if (MCanalysis->getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
    valuesList << QString::number(MCanalysis->getMaximumElapsedTime()) + "(s)";
  else
    valuesList << "- (s)";
  if (MCanalysis->getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
    valuesList << QString::number(MCanalysis->getMaximumCalls());
  else
    valuesList << "-";
  valuesList << QString::number(MCanalysis->getBlockSize());
  valuesList << QString::number(MCanalysis->getSeed()); 

  parametersWidget_ = new ParametersWidget(tr("Central tendency parameters:"), namesList, valuesList);
}
}
