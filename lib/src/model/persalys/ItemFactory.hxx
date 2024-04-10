//                                               -*- C++ -*-
/**
 *  @brief QObject
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
  virtual ~ItemFactory();

  StudyItem * getParentStudyItem() { return parentStudyItem_; };
  OT::String availableAnalysisName(const QString &baseName);
  QString getParentTitleType(const QString &objectName) const;
  Item * getTitleItem(const QString &objectName);

  template <class T>
  void newAnalysis(const QString &analysisName, const T &input, const bool isGeneralWizard = false)
  {
    emit wizardRequested(getParentStudyItem(), createAnalysis(analysisName, input), isGeneralWizard);
  }
  Analysis createAnalysis(const QString &analysisName, const DesignOfExperiment &doe);
  Analysis createAnalysis(const QString &analysisName, const LimitState &limitState);
  Analysis createAnalysis(const QString &analysisName, const Analysis &analysis);

  QAction * createAction(const QString &analysisName, const PhysicalModel &model);
  QAction * createAction(const QString &analysisName, const DesignOfExperiment &doe);
  QAction * createAction(const QString &analysisName, const LimitState &limitState);
  QAction * createAction(const QString &analysisName, const Analysis &analysis);

signals:
  void wizardRequested(StudyItem*, const Analysis&, const bool isGeneralWizard = false);
  void doeEvaluationWizardRequested(const Analysis&, const bool isGeneralWizard = false);
  void mmExportWizardRequested(StudyItem*, const Analysis&, const bool isGeneralWizard = false);
  void wizardRequested(const PhysicalModel&);
  void wizardRequested(StudyItem*, const DesignOfExperiment& designOfExp);

protected:
  StudyItem * parentStudyItem_ = nullptr;

  QAction * newSymbolicModel_ = nullptr;
  QAction * newPythonModel_ = nullptr;
#ifdef PERSALYS_HAVE_YACS
  QAction * newYACSModel_ = nullptr;
#endif
#ifdef PERSALYS_HAVE_OTFMI
  QAction * newFMIModel_ = nullptr;
#endif
  QAction * newCouplingModel_ = nullptr;
  QAction * newSymbolicFieldModel_ = nullptr;
  QAction * newPythonFieldModel_ = nullptr;
  QAction * newDataModel_ = nullptr;

  QAction * newModelEvaluation_ = nullptr;
  QAction * newScreening_ = nullptr;
  QAction * newOptimization_ = nullptr;
  QAction * newMoOptimization_ = nullptr;
  QAction * newDesignOfExperiment_ = nullptr;
  QAction * newProbabilisticModel_ = nullptr;
  QAction * newCentralTendency_ = nullptr;
  QAction * newSensitivityAnalysis_ = nullptr;

  QAction * newLimitState_ = nullptr;
  QAction * newObservations_ = nullptr;
};
}
#endif
