//                                               -*- C++ -*-
/**
 *  @brief Helper
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
#include "persalys/WindowFactory.hxx"

#include "persalys/SymbolicPhysicalModelWindow.hxx"
#include "persalys/PythonPhysicalModelWindow.hxx"
#include "persalys/MetaModelWindow.hxx"
#ifdef PERSALYS_HAVE_YACS
#include "persalys/YACSPhysicalModelWindow.hxx"
#endif
#ifdef PERSALYS_HAVE_OTFMI
#include "persalys/FMIPhysicalModelWindow.hxx"
#endif

#include "persalys/DesignOfExperimentWindow.hxx"
#include "persalys/DataAnalysisResultWindow.hxx"
#include "persalys/ModelEvaluationResultWindow.hxx"
#include "persalys/OptimizationResultWindow.hxx"
#include "persalys/MonteCarloResultWindow.hxx"
#include "persalys/TaylorExpansionMomentsResultWindow.hxx"
#include "persalys/SobolResultWindow.hxx"
#include "persalys/SRCResultWindow.hxx"
#include "persalys/SimulationReliabilityResultWindow.hxx"
#include "persalys/ApproximationResultWindow.hxx"
#include "persalys/FunctionalChaosResultWindow.hxx"
#include "persalys/KrigingResultWindow.hxx"
#include "persalys/InferenceResultWindow.hxx"
#include "persalys/CopulaInferenceResultWindow.hxx"
#include "persalys/FieldModelEvaluationResultWindow.hxx"
#include "persalys/FieldMonteCarloWizard.hxx"
#include "persalys/CalibrationResultWindow.hxx"
#ifdef PERSALYS_HAVE_OTMORRIS
#include "persalys/ScreeningAnalysisWizard.hxx"
#include "persalys/MorrisResultWindow.hxx"
#endif

#include "persalys/InferenceWizard.hxx"
#include "persalys/DesignOfExperimentWizard.hxx"
#include "persalys/ModelEvaluationWizard.hxx"
#include "persalys/OptimizationWizard.hxx"
#include "persalys/CentralTendencyWizard.hxx"
#include "persalys/SensitivityAnalysisWizard.hxx"
#include "persalys/ReliabilityAnalysisWizard.hxx"
#include "persalys/MetaModelAnalysisWizard.hxx"
#include "persalys/CopulaInferenceWizard.hxx"
#include "persalys/CalibrationAnalysisWizard.hxx"

namespace PERSALYS
{

SubWindow* WindowFactory::GetPhysicalModelWindow(PhysicalModelDefinitionItem* item, QWidget * parent)
{
  SubWindow * window = 0;

  const QString physicalModelType = item->getPhysicalModel().getImplementation()->getClassName().c_str();

  if (physicalModelType == "SymbolicPhysicalModel" || physicalModelType == "SymbolicFieldModel")
  {
    window = new SymbolicPhysicalModelWindow(item, parent);
  }
  else if (physicalModelType == "PythonPhysicalModel" || physicalModelType == "PythonFieldModel")
  {
    window = new PythonPhysicalModelWindow(item, parent);
  }
  else if (physicalModelType == "MetaModel")
  {
    window = new MetaModelWindow(item, parent);
  }
#ifdef PERSALYS_HAVE_YACS
  else if (physicalModelType == "YACSPhysicalModel")
  {
    window = new YACSPhysicalModelWindow(item, parent);
  }
#endif
#ifdef PERSALYS_HAVE_OTFMI
  else if (physicalModelType == "FMIPhysicalModel")
  {
    window = new FMIPhysicalModelWindow(item, parent);
  }
#endif
  else
  {
    qDebug() << "Error: In createNewPhysicalModelWindow: impossible to create PhysicalModelWindow\n";
  }

  return window;
}


AnalysisWizard* WindowFactory::GetAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard, QWidget * parent)
{
  AnalysisWizard * wizard = 0;

  const QString analysisType = analysis.getImplementation()->getClassName().c_str();

  if (analysisType.contains("ModelEvaluation"))
  {
    wizard = new ModelEvaluationWizard(analysis, parent);
  }
#ifdef PERSALYS_HAVE_OTMORRIS
  else if (analysisType == "MorrisAnalysis")
  {
    wizard = new ScreeningAnalysisWizard(analysis, parent);
  }
#endif
  else if (analysisType == "OptimizationAnalysis")
  {
    wizard = new OptimizationWizard(analysis, parent);
  }
  else if (analysisType == "FunctionalChaosAnalysis" ||
           analysisType == "KrigingAnalysis")
  {
    wizard = new MetaModelAnalysisWizard(analysis, isGeneralWizard, parent);
  }
  else if (analysisType == "InferenceAnalysis")
  {
    wizard = new InferenceWizard(analysis, parent);
  }
  else if (analysisType == "CopulaInferenceAnalysis")
  {
    wizard = new CopulaInferenceWizard(analysis, parent);
  }
  else if (analysisType == "MonteCarloAnalysis" ||
           analysisType == "TaylorExpansionMomentsAnalysis")
  {
    wizard = new CentralTendencyWizard(analysis, parent);
  }
  else if (analysisType == "FieldMonteCarloAnalysis")
  {
    wizard = new FieldMonteCarloWizard(analysis, parent);
  }
  else if (analysisType == "SobolAnalysis" ||
           analysisType == "SRCAnalysis")
  {
    wizard = new SensitivityAnalysisWizard(analysis, parent);
  }
  else if (analysisType == "MonteCarloReliabilityAnalysis" ||
           analysisType == "FORMImportanceSamplingAnalysis" ||
           analysisType == "FORMAnalysis" ||
           analysisType == "SORMAnalysis")
  {
    wizard = new ReliabilityAnalysisWizard(analysis, isGeneralWizard, parent);
  }
  else if (analysisType == "ImportedDesignOfExperiment" ||
           analysisType == "FixedDesignOfExperiment" ||
           analysisType == "GridDesignOfExperiment" ||
           analysisType == "ProbabilisticDesignOfExperiment")
  {
    wizard = new DesignOfExperimentWizard(analysis, parent);
  }
  else if (analysisType == "CalibrationAnalysis")
  {
    wizard = new CalibrationAnalysisWizard(analysis, isGeneralWizard, parent);
  }
  else
  {
    qDebug() << "Error: In GetAnalysisWizard: analysisType " << analysisType << " not recognized.\n";
  }

  return wizard;
}


SubWindow* WindowFactory::GetAnalysisWindow(AnalysisItem* item, QWidget * parent)
{
  SubWindow * resultWindow = 0;

  const QString analysisType = item->data(Qt::UserRole).toString();

  if (analysisType == "ModelEvaluation")
  {
    resultWindow = new ModelEvaluationResultWindow(item, parent);
  }
  else if (analysisType == "FieldModelEvaluation")
  {
    resultWindow = new FieldModelEvaluationResultWindow(item, parent);
  }
#ifdef PERSALYS_HAVE_OTMORRIS
  else if (analysisType == "MorrisAnalysis")
  {
    resultWindow = new MorrisResultWindow(item, parent);
  }
#endif
  else if (analysisType == "OptimizationAnalysis")
  {
    resultWindow = new OptimizationResultWindow(item, parent);
  }
  else if (analysisType == "MonteCarloAnalysis")
  {
    resultWindow = new MonteCarloResultWindow(item, parent);
  }
  else if (analysisType == "FieldMonteCarloAnalysis")
  {
    resultWindow = new FieldCentralTendencyResultWindow(item, parent);
  }
  else if (analysisType == "TaylorExpansionMomentsAnalysis")
  {
    resultWindow = new TaylorExpansionMomentsResultWindow(item, parent);
  }
  else if (analysisType == "SobolAnalysis")
  {
    resultWindow = new SobolResultWindow(item, parent);
  }
  else if (analysisType == "SRCAnalysis")
  {
    resultWindow = new SRCResultWindow(item, parent);
  }
  else if (analysisType == "MonteCarloReliabilityAnalysis" ||
           analysisType == "FORMImportanceSamplingAnalysis" ||
           analysisType == "ImportanceSamplingAnalysis")
  {
    resultWindow = new SimulationReliabilityResultWindow(item, parent);
  }
  else if (analysisType == "FORMAnalysis" ||
           analysisType == "SORMAnalysis")
  {
    resultWindow = new ApproximationResultWindow(item, parent);
  }
  else if (analysisType == "DataAnalysis")
  {
    resultWindow = new DataAnalysisResultWindow(item, parent);
  }
  else if (analysisType == "FunctionalChaosAnalysis")
  {
    resultWindow = new FunctionalChaosResultWindow(item, parent);
  }
  else if (analysisType == "KrigingAnalysis")
  {
    resultWindow = new KrigingResultWindow(item, parent);
  }
  else if (analysisType == "InferenceAnalysis")
  {
    resultWindow = new InferenceResultWindow(item, parent);
  }
  else if (analysisType == "CopulaInferenceAnalysis")
  {
    resultWindow = new CopulaInferenceResultWindow(item, parent);
  }
  else if (analysisType == "CalibrationAnalysis")
  {
    resultWindow = new CalibrationResultWindow(item, parent);
  }
  else if (analysisType.contains("DesignOfExperiment"))
  {
    // if DesignOfExperimentEvaluation FixedDesignOfExperiment GridDesignOfExperiment
    // ImportedDesignOfExperiment ProbabilisticDesignOfExperiment
    resultWindow = new DesignOfExperimentWindow(item, parent);
  }
  else
  {
    qDebug() << "Error: In WindowFactory::GetAnalysisWindow: analysisType " << analysisType << " not recognized.\n";
    throw OT::InvalidArgumentException(HERE) << "Analysis type " << analysisType.toStdString() << " not recognized.\n";
  }

  return resultWindow;
}
}
