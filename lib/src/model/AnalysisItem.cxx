//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of analysis
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/AnalysisItem.hxx"

#include "otgui/PhysicalModelAnalysis.hxx"
#include "otgui/FunctionalChaosAnalysis.hxx"
#include "otgui/KrigingAnalysis.hxx"

#include "otgui/PhysicalModelItem.hxx"
#include "otgui/DesignOfExperimentItem.hxx"
#include "otgui/LimitStateItem.hxx"
#include "otgui/OTStudyItem.hxx"
#include "otgui/SobolAnalysis.hxx"
#include "otgui/SRCAnalysis.hxx"
#include "otgui/ApproximationAnalysis.hxx"
#include "otgui/MonteCarloAnalysis.hxx"
#include "otgui/SimulationReliabilityAnalysis.hxx"
#include "otgui/MonteCarloReliabilityAnalysis.hxx"
#include "otgui/FORMImportanceSamplingAnalysis.hxx"

#include <openturns/OTBase.hxx>

#include <QDebug>

using namespace OT;

namespace OTGUI {

AnalysisItem::AnalysisItem(const Analysis & analysis)
  : OTguiItem(QString::fromUtf8(analysis.getName().c_str()), analysis.getImplementation()->getClassName().c_str())
  , Observer(analysis.getImplementation()->getClassName())
  , analysis_(analysis)
{
  analysis_.addObserver(this);

  buildActions();
}


void AnalysisItem::buildActions()
{
  // modify analysis action
  modifyAnalysis_ = new QAction(QIcon(":/images/run-build.png"), tr("Modify"), this);
  modifyAnalysis_->setStatusTip(tr("Modify the analysis"));
  connect(modifyAnalysis_, SIGNAL(triggered()), this, SLOT(modifyAnalysis()));

  // remove analysis action
  removeAnalysis_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAnalysis_->setStatusTip(tr("Remove the analysis"));
  connect(removeAnalysis_, SIGNAL(triggered()), this, SLOT(removeAnalysis()));

  // add actions
  if (getType() != "DataAnalysis" &&
      getType() != "CopulaInferenceAnalysis" &&
      getType() != "ImportanceSamplingAnalysis") // there is no wizard associated with these analyses <==> impossible to modify them
  {
    appendAction(modifyAnalysis_);
  }

  appendAction(removeAnalysis_);
}


void AnalysisItem::setData(const QVariant & value, int role)
{
  if (role == Qt::EditRole)
    analysis_.getImplementation()->setName(value.toString().toLocal8Bit().data());

  QStandardItem::setData(value, role);
}


Analysis AnalysisItem::getAnalysis() const
{
  return analysis_;
}


void AnalysisItem::updateAnalysis(const Analysis & analysis)
{
  analysis_.getImplementation().get()->removeObserver(this);
  analysis_ = analysis;
  if (!analysis_.getImplementation().get()->getObservers().size())
    analysis_.addObserver(this);

  // update the implementation of the analysis stored in OTStudy
  getParentOTStudyItem()->getOTStudy().getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());
}


void AnalysisItem::setDesignOfExperiment(const DesignOfExperiment& designOfExperiment)
{
  if (dynamic_cast<DesignOfExperimentAnalysis*>(getAnalysis().getImplementation().get()))
    dynamic_cast<DesignOfExperimentAnalysis*>(getAnalysis().getImplementation().get())->setDesignOfExperiment(designOfExperiment);
}


void AnalysisItem::stopAnalysis()
{
  analysis_.stop();
}


void AnalysisItem::modifyAnalysis()
{
  // check if the analysis is running
  if (analysis_.isRunning())
  {
    emit emitErrorMessageRequested(tr("Can not modify a running analysis."));
    return;
  }

  const String analysisName = analysis_.getImplementation()->getClassName();

  // check physical model
  if (dynamic_cast<PhysicalModelAnalysis*>(analysis_.getImplementation().get()))
  {
    PhysicalModelItem * item = static_cast<PhysicalModelItem*>(getParentItem("PhysicalModelDiagram"));
    if (!item)
      return;

    // must have in/outputs
    if (!item->getPhysicalModel().isValid())
    {
      emit emitErrorMessageRequested(tr("The physical model must have inputs AND at least one selected output."));
      return;
    }
    // if proba analysis
    if (analysisName != "ModelEvaluation")
    {
      // must have stochastic variables
      if (!item->getPhysicalModel().hasStochasticInputs())
      {
        emit emitErrorMessageRequested(tr("The physical model must have stochastic inputs."));
        return;
      }
    }
    // if sensitivity
    if (analysisName == "SobolAnalysis" || analysisName == "SRCAnalysis")
    {
      // must have an independent copula
      if (!item->getPhysicalModel().getComposedDistribution().hasIndependentCopula())
      {
        emit emitErrorMessageRequested(tr("The model must have an independent copula to compute a sensitivity analysis but here the inputs are correlated."));
        return;
      }
    }
  // TODO check limitstate?
  }
  // check data model
  else if (dynamic_cast<DesignOfExperimentAnalysis*>(analysis_.getImplementation().get()))
  {
    DesignOfExperimentItem * item = static_cast<DesignOfExperimentItem*>(getParentItem("DesignOfExperiment"));
    if (!item)
      return;

    // must have data
    if (!item->getDesignOfExperiment().getSample().getSize())
    {
      emit emitErrorMessageRequested(tr("The sample is empty."));
      return;
    }
    // if meta model
    if (analysisName == "FunctionalChaosAnalysis" || analysisName == "KrigingAnalysis")
    {
      // must have output data
      if (!item->getDesignOfExperiment().getOutputSample().getSize())
      {
        emit emitErrorMessageRequested(tr("The model must have at least one output."));
        return;
      }
    }
  }

  // emit signal to open the wizard
  emit modifyAnalysisRequested(this);
}


void AnalysisItem::removeAnalysis()
{
  // check if the analysis is running
  if (analysis_.isRunning())
  {
    emit emitErrorMessageRequested(tr("Can not remove a running analysis."));
    return;
  }

  // remove
  if (getParentOTStudyItem())
    getParentOTStudyItem()->getOTStudy().remove(Analysis(analysis_));
}


void AnalysisItem::update(Observable* source, const String& message)
{
  if (message == "analysisLaunched")
  {
    emit analysisStatusChanged(true);
  }
  else if (message == "analysisFinished")
  {
    emit analysisFinished(this);
    emit analysisStatusChanged(false);
  }
  else if (message == "analysisBadlyFinished")
  {
    emit analysisBadlyFinished(this, analysis_.getErrorMessage().c_str());
    emit analysisStatusChanged(false);
  }
  else if (message == "metaModelCreated")
  {
    if (dynamic_cast<FunctionalChaosAnalysis*>(analysis_.getImplementation().get()))
    {
      PhysicalModel metaModel(dynamic_cast<FunctionalChaosAnalysis*>(analysis_.getImplementation().get())->getResult().getMetaModel());
      emit metaModelCreated(metaModel);
    }
    else if (dynamic_cast<KrigingAnalysis*>(analysis_.getImplementation().get()))
    {
      PhysicalModel metaModel(dynamic_cast<KrigingAnalysis*>(analysis_.getImplementation().get())->getResult().getMetaModel());
      emit metaModelCreated(metaModel);
    }
    // can NOT write here : getParentOTStudyItem()->getOTStudy().add(metaModel);
    // "add" operation implies the creation of a new QStandardItem
    // but notify("metaModelCreated") is called in a sub-thread...
    else
    {
      qDebug() << "Can not add the metamodel in the tree view. The metamodel analysis is not recognized.";
    }
  }
  else if (message == "informationMessageUpdated")
  {
    emit messageChanged(analysis_.getInformationMessage().c_str());
  }
  else if (message == "progressValueChanged")
  {
    emit progressValueChanged(analysis_.getProgressValue());
  }
  else if (message == "analysisRemoved")
  {
    emit removeRequested(row());
  }
}


QStandardItem* AnalysisItem::getParentItem(const QString itemType)
{
  QModelIndex seekRoot = index();
  while(seekRoot.parent() != QModelIndex())
  {
    seekRoot = seekRoot.parent();
    if (model()->itemFromIndex(seekRoot)->data(Qt::UserRole).toString() == itemType)
      return model()->itemFromIndex(seekRoot);
  }
  return 0;
}


void AnalysisItem::GetAnalysisParameters(const Analysis& analysis, QStringList& namesList, QStringList& valuesList)
{
  namesList.clear();
  valuesList.clear();

  if (analysis.getImplementation()->getClassName() == "MonteCarloAnalysis")
  {
    const MonteCarloAnalysis MCanalysis(*dynamic_cast<const MonteCarloAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm");
    if (MCanalysis.isConfidenceIntervalRequired())
      namesList << tr("Confidence level");
    namesList << tr("Maximum coefficient of variation");
    namesList << tr("Maximum elapsed time");
    namesList << tr("Maximum calls");
    namesList << tr("Block size");
    namesList << tr("Seed");

    // Parameters values
    valuesList << tr("Monte Carlo");
    if (MCanalysis.isConfidenceIntervalRequired())
      valuesList << QString::number(MCanalysis.getLevelConfidenceInterval()*100) + "\%";
    valuesList << QString::number(MCanalysis.getMaximumCoefficientOfVariation());
    if (MCanalysis.getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
      valuesList << QString::number(MCanalysis.getMaximumElapsedTime()) + "(s)";
    else
      valuesList << "- (s)";
    if (MCanalysis.getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
      valuesList << QString::number(MCanalysis.getMaximumCalls());
    else
      valuesList << "-";
    valuesList << QString::number(MCanalysis.getBlockSize());
    valuesList << QString::number(MCanalysis.getSeed());
  }
  else if (analysis.getImplementation()->getClassName() == "KrigingAnalysis")
  {
    const KrigingAnalysis kriging(*dynamic_cast<const KrigingAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm");
    namesList << tr("Covariance model");
    if (kriging.getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
      namesList << tr("nu");
    else if (kriging.getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
      namesList << tr("p");
    namesList << tr("Parameters optimization") << tr("Scale") << tr("Amplitude") << tr("Trend basis");
    namesList << tr("Leave-one-out validation");

    // Parameters values
    valuesList << tr("Kriging");
    valuesList << QString(kriging.getCovarianceModel().getImplementation()->getClassName().c_str());
    // covariance model parameters
    if (kriging.getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
    {
      double nu = dynamic_cast<MaternModel*>(&*kriging.getCovarianceModel().getImplementation())->getNu();
      valuesList << QString::number(nu);
    }
    else if (kriging.getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
    {
      double p = dynamic_cast<GeneralizedExponential*>(&*kriging.getCovarianceModel().getImplementation())->getP();
      valuesList << QString::number(p);
    }

    // Optimize parameters
    valuesList << (kriging.getOptimizeParameters()? tr("yes") : tr("no"));

    // scale
    QString scaleText;
    for (UnsignedInteger i=0; i<kriging.getCovarianceModel().getScale().getSize(); ++i)
    {
      scaleText += QString::number(kriging.getCovarianceModel().getScale()[i]);
      if (i < kriging.getCovarianceModel().getScale().getSize()-1)
        scaleText += "; ";
    }
    valuesList << scaleText;
    // amplitude
    QString amplitudeText;
    for (UnsignedInteger i=0; i<kriging.getCovarianceModel().getAmplitude().getSize(); ++i)
    {
      amplitudeText += QString::number(kriging.getCovarianceModel().getAmplitude()[i]);
      if (i < kriging.getCovarianceModel().getAmplitude().getSize()-1)
        amplitudeText += "; ";
    }
    valuesList << amplitudeText;
    // basis
    QString basisType(tr("Constant"));
    const UnsignedInteger dim = kriging.getBasis().getDimension();
    if (kriging.getBasis().getSize() == (dim+1))
      basisType = tr("Linear");
    else if (kriging.getBasis().getSize() == ((dim+1)*(dim+2)/2))
      basisType = tr("Quadratic");
    valuesList << basisType;

    valuesList << (kriging.isLeaveOneOutValidation()? tr("yes") : tr("no"));
  }
  else if (analysis.getImplementation()->getClassName() == "FunctionalChaosAnalysis")
  {
    const FunctionalChaosAnalysis chaos(*dynamic_cast<const FunctionalChaosAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm") << tr("Chaos degree") << tr("Sparse") << tr("Leave-one-out validation");

    // Parameters values
    valuesList << tr("Functional chaos");
    valuesList << QString::number(chaos.getChaosDegree());
    valuesList << (chaos.getSparseChaos()? tr("yes") : tr("no"));
    valuesList << (chaos.isLeaveOneOutValidation()? tr("yes") : tr("no"));
  }
  else if (analysis.getImplementation()->getClassName() == "SobolAnalysis")
  {
    const SobolAnalysis sobolAnalysis(*dynamic_cast<const SobolAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm") << tr("Maximum coefficient of variation") << tr("Maximum elapsed time");
    namesList << tr("Maximum calls") << tr("Block size") << tr("Seed");

    // Parameters values
    valuesList << tr("Sobol");
    valuesList << QString::number(sobolAnalysis.getMaximumCoefficientOfVariation());
    if (sobolAnalysis.getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
      valuesList << QString::number(sobolAnalysis.getMaximumElapsedTime()) + "(s)";
    else
      valuesList << "- (s)";
    if (sobolAnalysis.getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
      valuesList << QString::number(sobolAnalysis.getMaximumCalls());
    else
      valuesList << "-";
    valuesList << QString::number(sobolAnalysis.getBlockSize());
    valuesList << QString::number(sobolAnalysis.getSeed());
  }
  else if (analysis.getImplementation()->getClassName() == "SRCAnalysis")
  {
    const SRCAnalysis srcAnalysis(*dynamic_cast<const SRCAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm") << tr("Sample size") << tr("Seed");

    // Parameters values
    valuesList << tr("Standardized Regression Coefficients");
    valuesList << QString::number(srcAnalysis.getSimulationsNumber());
    valuesList << QString::number(srcAnalysis.getSeed());
  }
  else if (analysis.getImplementation()->getClassName() == "FORMAnalysis")
  {
    const ApproximationAnalysis approxAnalysis(*dynamic_cast<const ApproximationAnalysis*>(&*analysis.getImplementation().get()));

    // Parameters names
    namesList << tr("Algorithm");
    namesList << tr("Optimization algorithm");
    namesList << tr("Physical starting point");
    namesList << tr("Maximum iterations number");
    namesList << tr("Maximum absolute error");
    namesList << tr("Maximum relative error");
    namesList << tr("Maximum residual error");
    namesList << tr("Maximum constraint error");

    // Parameters values
    valuesList << tr("FORM");
    valuesList << QString(approxAnalysis.getOptimizationAlgorithm().getImplementation()->getClassName().c_str());

    // starting point
    const NumericalPoint startingPoint(approxAnalysis.getPhysicalStartingPoint());
    QString startingPointText = "[";
    for (UnsignedInteger i=0; i<startingPoint.getDimension(); ++i)
    {
      startingPointText += QString::number(startingPoint[i]);
      if (i < startingPoint.getDimension()-1)
        startingPointText += "; ";
    }
    startingPointText += "]";
    valuesList << startingPointText;

    // optimization algo parameters
    valuesList << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumIterationNumber());
    valuesList << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumAbsoluteError());
    valuesList << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumRelativeError());
    valuesList << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumResidualError());
    valuesList << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumConstraintError());
  }
  else if (dynamic_cast<const SimulationReliabilityAnalysis*>(&*analysis.getImplementation()))
  {
    const SimulationReliabilityAnalysis simuAnalysis(*dynamic_cast<const SimulationReliabilityAnalysis*>(&*analysis.getImplementation()));

    // - parameters names
    namesList << tr("Algorithm");
    if (dynamic_cast<const ImportanceSamplingAnalysis*>(&*analysis.getImplementation()))
      namesList << tr("Design point (standard space)");
    namesList << tr("Maximum coefficient of variation");
    namesList << tr("Maximum elapsed time");
    namesList << tr("Maximum calls");
    namesList << tr("Block size");
    namesList << tr("Seed");

    // Parameters values
    // algo
    if (dynamic_cast<const MonteCarloReliabilityAnalysis*>(&*analysis.getImplementation()))
      valuesList << tr("Monte Carlo");
    else if (dynamic_cast<const ImportanceSamplingAnalysis*>(&*analysis.getImplementation()))
    {
      if (dynamic_cast<const FORMImportanceSamplingAnalysis*>(&*analysis.getImplementation()))
        valuesList << tr("FORM - Importance sampling");
      else
        valuesList << tr("Importance sampling");

      // starting point
      const NumericalPoint startingPoint(dynamic_cast<const ImportanceSamplingAnalysis*>(&*analysis.getImplementation())->getStandardSpaceDesignPoint());
      QString startingPointText = "[";
      for (UnsignedInteger i=0; i<startingPoint.getDimension(); ++i)
      {
        startingPointText += QString::number(startingPoint[i]);
        if (i < startingPoint.getDimension()-1)
          startingPointText += "; ";
      }
      startingPointText += "]";
      valuesList << startingPointText;
    }

    // stop criteria
    valuesList << QString::number(simuAnalysis.getMaximumCoefficientOfVariation());
    if (simuAnalysis.getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
      valuesList << QString::number(simuAnalysis.getMaximumElapsedTime()) + "(s)";
    else
      valuesList << "- (s)";
    if (simuAnalysis.getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
      valuesList << QString::number(simuAnalysis.getMaximumCalls());
    else
      valuesList << "-";
    valuesList << QString::number(simuAnalysis.getBlockSize());

    // seed
    valuesList << QString::number(simuAnalysis.getSeed());
  }
}
}