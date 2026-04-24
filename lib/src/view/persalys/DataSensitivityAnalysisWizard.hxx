//                                               -*- C++ -*-
/**
 *  @brief QWizard to define a data sensitivity analysis
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

#ifndef PERSALYS_DATASENSITIVITYANALYSISWIZARD_HXX
#define PERSALYS_DATASENSITIVITYANALYSISWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/ErrorWidget.hxx"
#include "persalys/DataSensitivityAnalysis.hxx"

#include <openturns/CovarianceModel.hxx>
#include <openturns/Point.hxx>

#include <QButtonGroup>
#include <QCheckBox>
#include <QComboBox>
#include <QTableView>

namespace PERSALYS
{

class PERSALYS_VIEW_API DataSensitivityAnalysisIntroPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit DataSensitivityAnalysisIntroPage(DesignOfExperiment::Type doeType, QWidget* parent = nullptr);

  void initialize(DataSensitivityAnalysis * analysis_ptr);
  unsigned char getType() const;
  OT::Description getInterestVariables() const;

  int nextId() const override;
  bool validatePage() override;

signals:
  void pageValidated();

private:
  OutputsSelectionGroupBox  * outputsSelectionGroupBox_  = nullptr;
  QButtonGroup              * methodGroup_               = nullptr;
  ErrorWidget               * warningWidget_             = nullptr;
  ErrorWidget               * errorWidget_               = nullptr;
};

class HSICCovarianceModelsTableModel;
class HSICAlphaTableModel;
class CriticalDomainTableModel;

class PERSALYS_VIEW_API DataSensitivityAnalysisHSICParametersPage : public QWizardPage
{
  Q_OBJECT

public:
  DataSensitivityAnalysisHSICParametersPage(
    OT::UnsignedInteger sampleSize, 
    const OT::Description & variableNames, 
    DataSensitivityAnalysis::HSICType type, 
    QWidget* parent = nullptr
  );

  void initialize(DataSensitivityAnalysis * analysis_ptr);

  bool computeAsymptoticPValues() const;
  bool computePermutationPValues() const;
  bool useUStatistic() const;
  OT::Collection<OT::CovarianceModel> getCovarianceModels() const;

  int nextId() const override;
  bool validatePage() override;

signals:
  void pageValidated();

private:
    DataSensitivityAnalysis::HSICType type_;
    QCheckBox                         * computeAsymptoticPValuesCheckBox_   = nullptr;
    QCheckBox                         * computePermutationPValuesCheckBox_  = nullptr;
    QComboBox                         * useUStatisticComboBox_              = nullptr;
    HSICCovarianceModelsTableModel    * covarianceTableModel_               = nullptr;
    QTableView                        * covarianceTableView_                = nullptr;
    ErrorWidget                       * errorWidget_                        = nullptr;
    DataSensitivityAnalysis           * analysis_ptr_                       = nullptr;
};

class PERSALYS_VIEW_API DataSensitivityAnalysisCriticalDomainPage : public QWizardPage
{
  Q_OBJECT

public:
  explicit DataSensitivityAnalysisCriticalDomainPage(QWidget* parent = nullptr);

  void initialize(DataSensitivityAnalysis * analysis_ptr);

  OT::Interval getCriticalDomain() const;
  OT::Point getAlphas() const;

  int nextId() const override;
  bool validatePage() override;

signals:
  void pageValidated();

private:
    CriticalDomainTableModel          * criticalDomainTableModel_           = nullptr;
    QTableView                        * criticalDomainTableView_            = nullptr;
    HSICAlphaTableModel               * alphaTableModel_                    = nullptr;
    QTableView                        * alphaTableView_                     = nullptr;
    ErrorWidget                       * errorWidget_                        = nullptr;
    DataSensitivityAnalysis           * analysis_ptr_                       = nullptr;
};

class PERSALYS_VIEW_API DataSensitivityAnalysisWizard : public AnalysisWizard
{
  Q_OBJECT

  friend class TestDataSensitivityAnalysisWizard;

public:
  enum Page {Intro, GlobalHSICParameters, TargetHSICParameters, ConditionalHSICParameters, CriticalDomain};

  explicit DataSensitivityAnalysisWizard(const Analysis& analysis, QWidget* parent = nullptr);

  Analysis getAnalysis() const override;
  int nextId() const override;

private:
  DataSensitivityAnalysisIntroPage              * introPage_                      = nullptr;
  DataSensitivityAnalysisHSICParametersPage     * globalHSICParametersPage_       = nullptr;
  DataSensitivityAnalysisCriticalDomainPage     * criticalDomainPage_             = nullptr;
  DataSensitivityAnalysisHSICParametersPage     * targetHSICParametersPage_       = nullptr;
  DataSensitivityAnalysisHSICParametersPage     * conditionalHSICParametersPage_  = nullptr;
  const DataSensitivityAnalysis                 * analysis_ptr_                   = nullptr;

};

} // namespace PERSALYS

#endif // PERSALYS_DATASENSITIVITYANALYSISWIZARD_HXX