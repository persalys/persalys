//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a calibration analysis
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
#ifndef PERSALYS_CALIBRATIONANALYSISWIZARD_HXX
#define PERSALYS_CALIBRATIONANALYSISWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/OptimizationWizard.hxx"
#include "persalys/ParametersDefinitionWizard.hxx"
#include "persalys/ExperimentTableModel.hxx"
#include "persalys/OptimizationWizard.hxx"
#include "persalys/CalibrationTableModel.hxx"
#include "persalys/ExportableTableView.hxx"

#include <openturns/OptimizationAlgorithm.hxx>

#include <QComboBox>
#include <QCheckBox>
#include <QWizardPage>
#include <QLabel>
#include <QButtonGroup>
#include <QStandardItemModel>

namespace PERSALYS
{
class PERSALYS_VIEW_API CalibrationIntroPage : public QWizardPage
{
  Q_OBJECT

public:

  enum Algorithm {LinearLeastSquares, NonlinearLeastSquares, LinearGaussian, NonlinearGaussian};

  CalibrationIntroPage(QWidget* parent = 0);

  void initialize(const Analysis& analysis, QList<DesignOfExperiment> observationsList);
  DesignOfExperiment getObservations() const;
  int getMethodId() const;

public slots:
  void changeObservationsLabel(int);
signals:
  void methodChanged(int);
  void observationsChanged(const DesignOfExperiment&);

private:
  QComboBox * observationsComboBox_;
  QStandardItemModel * observationsComboBoxModel_;
  QLabel * observationsLabel_;
  QButtonGroup * methodGroup_;
};


class PERSALYS_VIEW_API CalibrationReferencePointPage : public QWizardPage
{
  Q_OBJECT

  friend class TestCalibrationAnalysisWizard;

public:
  CalibrationReferencePointPage(QWidget* parent = 0);

  OT::PointWithDescription getFixedValues() const;
  OT::PointWithDescription getCalibratedInputs() const;
  virtual bool validatePage();

public slots:
  void updateData(const PhysicalModel& model);
signals:
  void modelChanged(const PhysicalModel&);

private:
  PhysicalModel model_;
  QLabel * calibratedLabel_;
  QLabel * fixedLabel_;
  ExportableTableView * tableView_;
  CalibrationTableModel * tableModel_;
  TemporaryLabel * errorMessageLabel_;
};


class PERSALYS_VIEW_API CalibrationDistributionPage : public QWizardPage
{
  Q_OBJECT

public:
  CalibrationDistributionPage(const QString& title, const QString& subTitle, QWidget* parent = 0);

  OT::Distribution getDistribution() const;

public slots:
  void updateData(const PhysicalModel& model);
signals:
  void updateWidgetRequested();

private:
  PhysicalModel model_;
};


class PERSALYS_VIEW_API CalibrationParametersPage : public QWizardPage
{
  Q_OBJECT

public:
  enum OptimAlgo {CMinpackAlgo, CeresAlgo};
  CalibrationParametersPage(QWidget* parent = 0);
  void initialize(const Analysis& analysis);
  void updateAnalysis(CalibrationAnalysis * analysis_ptr);
  virtual void initializePage();

protected:
  void buildInterface();
  void updateLabel();

private:
  DoubleSpinBox * ciLengthSpinbox_;
  QWidget * bootStrapWidget_;
  QCheckBox * bootstrapCheckBox_;
  UIntSpinBox * bootStrapSizeSpinBox_;
  OptimizationStoppingCriteria * stoppingCriteriaLayout_;
  QWidget * maxEvalWidget_;
  QLabel * maxNbEvalLabel_;
};


class PERSALYS_VIEW_API CalibrationAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestCalibrationAnalysisWizard;

public:
  enum {Page_Intro, Page_RefPoint, Page_PriorDist, Page_ObsErrorDist, Page_Param};

  CalibrationAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard = false, QWidget* parent = 0);

  virtual Analysis getAnalysis() const;
  virtual int nextId() const;

protected:
  void buildInterface();

public slots:
  void updatePages(const DesignOfExperiment&);
  void initializePages();

private:
  QList<DesignOfExperiment> observationsList_;
  CalibrationIntroPage * introPage_;
  CalibrationReferencePointPage * refPointPage_;
  CalibrationDistributionPage * priorDistPage_;
  CalibrationDistributionPage * obsErrorDistPage_;
  CalibrationParametersPage * paramPage_;
};
}
#endif
