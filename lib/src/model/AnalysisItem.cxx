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

#include "otgui/ModelEvaluation.hxx"
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
  , Observer("Analysis")
  , analysis_(analysis)
  , modifyAnalysis_(0)
  , removeAnalysis_(0)
{
  analysis_.addObserver(this);

  buildActions();
}


void AnalysisItem::buildActions()
{
  const QString analysisType = data(Qt::UserRole).toString();

  if (analysisType == "DesignOfExperimentEvaluation") // no action for this analysis
    return;

  // modify analysis action
  if (analysisType != "DataAnalysis" &&
      analysisType != "CopulaInferenceAnalysis" &&
      analysisType != "ImportanceSamplingAnalysis") // there is no wizard associated with these analyses <=> impossible to modify them
  {
    modifyAnalysis_ = new QAction(QIcon(":/images/run-build.png"), tr("Modify"), this);
    modifyAnalysis_->setStatusTip(tr("Modify the analysis"));
    connect(modifyAnalysis_, SIGNAL(triggered()), this, SLOT(modifyAnalysis()));

    appendAction(modifyAnalysis_);
  }

  // remove analysis action
  removeAnalysis_ = new QAction(QIcon(":/images/window-close.png"), tr("Remove"), this);
  removeAnalysis_->setStatusTip(tr("Remove the analysis"));
  connect(removeAnalysis_, SIGNAL(triggered()), this, SLOT(removeAnalysis()));

  appendAction(removeAnalysis_);
}


void AnalysisItem::setData(const QVariant & value, int role)
{
  // rename
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
  // update analysis_
  analysis_.getImplementation().get()->removeObserver(this);
  analysis_ = analysis;
  if (!analysis_.getImplementation().get()->getObservers().size())
    analysis_.addObserver(this);

  // update analysis type
  setData(analysis_.getImplementation()->getClassName().c_str(), Qt::UserRole);

  // update the implementation of the analysis stored in OTStudy
  getParentOTStudyItem()->getOTStudy().getAnalysisByName(analysis.getName()).setImplementationAsPersistentObject(analysis.getImplementation());
}


void AnalysisItem::setDesignOfExperiment(const DesignOfExperiment& designOfExperiment)
{
  // update the design of experiment
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

  const QString analysisType = data(Qt::UserRole).toString();

  // -- IF PhysicalModelAnalysis

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
    if (analysisType != "ModelEvaluation")
    {
      // must have stochastic variables
      if (!item->getPhysicalModel().hasStochasticInputs())
      {
        emit emitErrorMessageRequested(tr("The physical model must have stochastic inputs."));
        return;
      }
      // if sensitivity analysis
      if (analysisType == "SobolAnalysis" || analysisType == "SRCAnalysis")
      {
        // must have an independent copula
        if (!item->getPhysicalModel().getComposedDistribution().hasIndependentCopula())
        {
          emit emitErrorMessageRequested(tr("The model must have an independent copula to compute a sensitivity analysis but here the inputs are correlated."));
          return;
        }
      }
    }
  // TODO check limitstate?
  }

  // -- IF DesignOfExperimentAnalysis

  // check data model
  else if (dynamic_cast<DesignOfExperimentAnalysis*>(analysis_.getImplementation().get()))
  {
    DesignOfExperimentItem * item = static_cast<DesignOfExperimentItem*>(getParentItem("DesignOfExperiment"));
    if (!item)
      return;

    // must have at least a point
    if (!item->getDesignOfExperiment().getSample().getSize())
    {
      emit emitErrorMessageRequested(tr("The sample is empty."));
      return;
    }
    // if meta model analysis
    if (analysisType == "FunctionalChaosAnalysis" || analysisType == "KrigingAnalysis")
    {
      // must have output data
      if (!item->getDesignOfExperiment().getOutputSample().getSize())
      {
        emit emitErrorMessageRequested(tr("The model must have at least one output."));
        return;
      }
    }
  }

  // emit signal to StudyTreeView to open the wizard
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


void AnalysisItem::processLaunched()
{
  // change icon
  setData(QIcon(":/images/green-arrow-right.png"), Qt::DecorationRole);

  // emit signal to disable run analysis/close study/import script...
  // warn the other objects that an analysis is running
  emit analysisInProgressStatusChanged(true);
}


void AnalysisItem::processFinished()
{
  // change icon
  setData(QVariant(), Qt::DecorationRole);

  // emit signal to enable run analysis/close study/import script...
  // warn the other objects that an analysis is finished
  emit analysisInProgressStatusChanged(false);
}


void AnalysisItem::update(Observable* source, const String& message)
{
  if (message == "analysisFinished")
  {
    // emit signal to the StudyTreeView to create a window
    emit analysisFinished(this);
  }
  else if (message == "analysisBadlyFinished")
  {
    // emit signal to the StudyTreeView to create a window
    emit analysisBadlyFinished(this);
  }
  else if (message == "metaModelCreated")
  {
    if (dynamic_cast<FunctionalChaosAnalysis*>(analysis_.getImplementation().get()))
    {
      PhysicalModel metaModel(dynamic_cast<FunctionalChaosAnalysis*>(analysis_.getImplementation().get())->getResult().getMetaModel());
    // emit signal to OTStudyItem to add metaModel in study_
      emit metaModelCreated(metaModel);
    }
    else if (dynamic_cast<KrigingAnalysis*>(analysis_.getImplementation().get()))
    {
      PhysicalModel metaModel(dynamic_cast<KrigingAnalysis*>(analysis_.getImplementation().get())->getResult().getMetaModel());
    // emit signal to OTStudyItem to add metaModel in study_
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
    // emit signal to AnalysisWindow to update the information message
    emit messageChanged(analysis_.getInformationMessage().c_str());
  }
  else if (message == "progressValueChanged")
  {
    // emit signal to AnalysisWindow to upate the progress bar
    emit progressValueChanged(analysis_.getProgressValue());
  }
  else if (message == "analysisRemoved")
  {
    emit removeRequested(row());
  }
}


QStandardItem* AnalysisItem::getParentItem(const QString itemType)
{
  // get the parent item with the type itemType
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

  const QString analysisType = analysis.getImplementation()->getClassName().c_str();

  if (analysisType == "ModelEvaluation")
  {
    const ModelEvaluation evaluation(*dynamic_cast<const ModelEvaluation*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Point to be evaluated");

    // Parameters values
    const Point point(evaluation.getInputValues());
    QString pointText = "[";
    for (UnsignedInteger i=0; i<point.getDimension(); ++i)
    {
      pointText += QString::number(point[i]);
      if (i < point.getDimension()-1)
        pointText += "; ";
    }
    pointText += "]";
    valuesList << pointText;
  }
  else if (analysisType == "TaylorExpansionMomentsAnalysis")
  {
    // Parameters names
    namesList << tr("Algorithm");
    // Parameters values
    valuesList << tr("Taylor expansion moments");
  }
  else if (analysisType == "MonteCarloAnalysis")
  {
    const MonteCarloAnalysis MCanalysis(*dynamic_cast<const MonteCarloAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm");
    if (MCanalysis.isConfidenceIntervalRequired())
      namesList << tr("Confidence level");
    namesList << tr("Maximum coefficient of variation")
              << tr("Maximum elapsed time")
              << tr("Maximum calls")
              << tr("Block size")
              << tr("Seed");

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
    valuesList << QString::number(MCanalysis.getBlockSize())
               << QString::number(MCanalysis.getSeed());
  }
  else if (analysisType == "KrigingAnalysis")
  {
    const KrigingAnalysis kriging(*dynamic_cast<const KrigingAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm")
              << tr("Covariance model");
    if (kriging.getCovarianceModel().getImplementation()->getClassName() == "MaternModel")
      namesList << tr("nu");
    else if (kriging.getCovarianceModel().getImplementation()->getClassName() == "GeneralizedExponential")
      namesList << tr("p");
    namesList << tr("Parameters optimization")
              << tr("Scale")
              << tr("Amplitude")
              << tr("Trend basis")
              << tr("Leave-one-out validation");

    // Parameters values
    valuesList << tr("Kriging")
               << QString(kriging.getCovarianceModel().getImplementation()->getClassName().c_str());
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
    // amplitude
    QString amplitudeText;
    for (UnsignedInteger i=0; i<kriging.getCovarianceModel().getAmplitude().getSize(); ++i)
    {
      amplitudeText += QString::number(kriging.getCovarianceModel().getAmplitude()[i]);
      if (i < kriging.getCovarianceModel().getAmplitude().getSize()-1)
        amplitudeText += "; ";
    }
    // basis
    QString basisType(tr("Constant"));
    const UnsignedInteger dim = kriging.getBasis().getDimension();
    if (kriging.getBasis().getSize() == (dim+1))
      basisType = tr("Linear");
    else if (kriging.getBasis().getSize() == ((dim+1)*(dim+2)/2))
      basisType = tr("Quadratic");

    valuesList << scaleText
               << amplitudeText
               << basisType
               << (kriging.isLeaveOneOutValidation()? tr("yes") : tr("no"));
  }
  else if (analysisType == "FunctionalChaosAnalysis")
  {
    const FunctionalChaosAnalysis chaos(*dynamic_cast<const FunctionalChaosAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm")
              << tr("Chaos degree")
              << tr("Sparse")
              << tr("Leave-one-out validation");

    // Parameters values
    valuesList << tr("Functional chaos")
               << QString::number(chaos.getChaosDegree())
               << (chaos.getSparseChaos()? tr("yes") : tr("no"))
               << (chaos.isLeaveOneOutValidation()? tr("yes") : tr("no"));
  }
  else if (analysisType == "SobolAnalysis")
  {
    const SobolAnalysis sobolAnalysis(*dynamic_cast<const SobolAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm")
              << tr("Maximum coefficient of variation")
              << tr("Maximum elapsed time")
              << tr("Maximum calls")
              << tr("Block size")
              << tr("Seed");

    // Parameters values
    valuesList << tr("Sobol")
               << QString::number(sobolAnalysis.getMaximumCoefficientOfVariation());
    if (sobolAnalysis.getMaximumElapsedTime() < (UnsignedInteger)std::numeric_limits<int>::max())
      valuesList << QString::number(sobolAnalysis.getMaximumElapsedTime()) + "(s)";
    else
      valuesList << "- (s)";
    if (sobolAnalysis.getMaximumCalls() < (UnsignedInteger)std::numeric_limits<int>::max())
      valuesList << QString::number(sobolAnalysis.getMaximumCalls());
    else
      valuesList << "-";
    valuesList << QString::number(sobolAnalysis.getBlockSize())
               << QString::number(sobolAnalysis.getSeed());
  }
  else if (analysisType == "SRCAnalysis")
  {
    const SRCAnalysis srcAnalysis(*dynamic_cast<const SRCAnalysis*>(&*analysis.getImplementation()));

    // Parameters names
    namesList << tr("Algorithm")
              << tr("Sample size")
              << tr("Seed");

    // Parameters values
    valuesList << tr("Standardized Regression Coefficients")
               << QString::number(srcAnalysis.getSimulationsNumber())
               << QString::number(srcAnalysis.getSeed());
  }
  else if (analysisType == "FORMAnalysis")
  {
    const ApproximationAnalysis approxAnalysis(*dynamic_cast<const ApproximationAnalysis*>(&*analysis.getImplementation().get()));

    // Parameters names
    namesList << tr("Algorithm")
              << tr("Optimization algorithm")
              << tr("Physical starting point")
              << tr("Maximum iterations number")
              << tr("Maximum absolute error")
              << tr("Maximum relative error")
              << tr("Maximum residual error")
              << tr("Maximum constraint error");

    // Parameters values
    valuesList << tr("FORM")
               << QString(approxAnalysis.getOptimizationAlgorithm().getImplementation()->getClassName().c_str());

    // starting point
    const Point startingPoint(approxAnalysis.getPhysicalStartingPoint());
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
    valuesList << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumIterationNumber())
               << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumAbsoluteError())
               << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumRelativeError())
               << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumResidualError())
               << QString::number(approxAnalysis.getOptimizationAlgorithm().getMaximumConstraintError());
  }
  else if (dynamic_cast<const SimulationReliabilityAnalysis*>(&*analysis.getImplementation()))
  {
    const SimulationReliabilityAnalysis simuAnalysis(*dynamic_cast<const SimulationReliabilityAnalysis*>(&*analysis.getImplementation()));

    // - parameters names
    namesList << tr("Algorithm");
    if (dynamic_cast<const ImportanceSamplingAnalysis*>(&*analysis.getImplementation()))
      namesList << tr("Design point (standard space)");
    namesList << tr("Maximum coefficient of variation")
              << tr("Maximum elapsed time")
              << tr("Maximum calls")
              << tr("Block size")
              << tr("Seed");

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
      const Point startingPoint(dynamic_cast<const ImportanceSamplingAnalysis*>(&*analysis.getImplementation())->getStandardSpaceDesignPoint());
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