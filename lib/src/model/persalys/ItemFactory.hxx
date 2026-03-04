//                                               -*- C++ -*-
/**
 *  @brief QObject
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
#ifndef PERSALYS_ITEMFACTORY_HXX
#define PERSALYS_ITEMFACTORY_HXX

#include "persalys/Analysis.hxx"
#include "persalys/DesignOfExperiment.hxx"
#include "persalys/LimitState.hxx"
#include "persalys/TranslationManager.hxx"

#include <QAction>

namespace PERSALYS
{
class StudyItem;
class Item;
class PERSALYS_MODEL_API ItemFactory : public QObject
{
  Q_OBJECT

public:
  ItemFactory();
  ~ItemFactory() override;

  StudyItem * getParentStudyItem()
  {
    return parentStudyItem_;
  };
  OT::String availableAnalysisName(const QString &baseName) const;
  QString getParentTitleType(const QString &objectName) const;
  Item * getTitleItem(const QString &objectName);

  template <class T>
  void newAnalysis(const QString &analysisName, const T &input, const bool isGeneralWizard = false)
  {
    try
    {
      emit wizardRequested(getParentStudyItem(), createAnalysis(analysisName, input), isGeneralWizard);
    }
    catch (const std::exception & e)
    {
      // connect to PhysicalModelDiagramWindow to show error message
      emit showErrorMessageRequested(TranslationManager::GetTranslatedErrorMessage(e.what()));
    }
  }
  Analysis createAnalysis(const QString &analysisName, const DesignOfExperiment &doe) const;
  Analysis createAnalysis(const QString &analysisName, const LimitState &limitState)  const;

  QAction * createAction(const QString &analysisName, const PhysicalModel &model);
  QAction * createAction(const QString &analysisName, const DataFieldModel &model);
  QAction * createAction(const QString &analysisName, const DesignOfExperiment &doe);
  QAction * createAction(const QString &analysisName, const LimitState &limitState);

signals:
  void showErrorMessageRequested(const QString & message);
  void wizardRequested(StudyItem*, const Analysis&, const bool isGeneralWizard = false);
  void doeEvaluationWizardRequested(const Analysis&, const bool isGeneralWizard = false);
  void mmExportWizardRequested(StudyItem*, const Analysis&, const bool isGeneralWizard = false);
  void doeExportWizardRequested(const StudyItem*);
  void wizardRequested(const PhysicalModel&);
  void openPropertiesRequested();
  void wizardRequested(StudyItem*, const DesignOfExperiment& designOfExp);
  void analysisRequested(StudyItem*, const Analysis&);

protected:
  StudyItem * parentStudyItem_      = nullptr;

  QAction * newSymbolicModel_       = nullptr;
  QAction * newPythonModel_         = nullptr;
#ifdef PERSALYS_HAVE_OTFMI
  QAction * newFMIModel_            = nullptr;
#endif
  QAction * newCouplingModel_       = nullptr;
  QAction * newSymbolicFieldModel_  = nullptr;
  QAction * newPythonFieldModel_    = nullptr;
  QAction * newDataModel_           = nullptr;
  QAction * newDataFieldModel_      = nullptr;

  QAction * newModelEvaluation_     = nullptr;
  QAction * newScreening_           = nullptr;
  QAction * newOptimization_        = nullptr;
  QAction * newMoOptimization_      = nullptr;
  QAction * newDesignOfExperiment_  = nullptr;
  QAction * newProbabilisticModel_  = nullptr;
  QAction * newCentralTendency_     = nullptr;
  QAction * newSensitivityAnalysis_ = nullptr;

  QAction * newLimitState_          = nullptr;
  QAction * newObservations_        = nullptr;
};
}
#endif
