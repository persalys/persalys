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
#ifndef PERSALYS_QUANTILEANALYSISWIZARD_HXX
#define PERSALYS_QUANTILEANALYSISWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/QuantileTableModel.hxx"
#include "persalys/QuantileThresholdTableModel.hxx"
#include "persalys/OutputsSelectionGroupBox.hxx"
#include "persalys/TemporaryLabel.hxx"
#include "persalys/ResizableHeaderlessTableView.hxx"
#include "persalys/ValueLineEdit.hxx"
#include "persalys/PlotWidget.hxx"

#include <QButtonGroup>
#include <QLineEdit>
#include <QCheckBox>

namespace PERSALYS
{
  class PERSALYS_VIEW_API QuantileAnalysisIntroPage : public QWizardPage
  {
    Q_OBJECT

  public:
    QuantileAnalysisIntroPage(QWidget* parent = 0);

    void initialize(const Analysis& analysis);
    OT::UnsignedInteger getType() const;
    OT::Description getInterestVariables() const;

    virtual int nextId() const;
    virtual bool validatePage();

  signals:
    void outputSelected();

  private:
    OutputsSelectionGroupBox * outputsSelectionGroupBox_ = nullptr;
    QButtonGroup * methodGroup_ = nullptr;
    TemporaryLabel * errorMessageLabel_ = nullptr;
    OT::Description interestVariables_;
  };

  class PERSALYS_VIEW_API QuantileAnalysisProbabilityPage : public QWizardPage
  {
    Q_OBJECT

  public:
    QuantileAnalysisProbabilityPage(QWidget* parent = 0);

    void initialize(const Analysis& analysis);
    Analysis getAnalysis(const OT::String& name, const PhysicalModel& physicalModel) const;
    OT::Point getDefaultTargetProbability() const {return targetLineEdit_->values();};

    OT::Collection<OT::Point> getTargetProbabilities() const {
      return tableModel_->getTargetProbabilities();
    };

    OT::Indices getTailTypes() const {
      return tableModel_->getTailTypes();
    }
    virtual int nextId() const;
    virtual bool validatePage();

  signals:
    void probabilitiesSelected();
  protected:
    void buildInterface();
    void keyPressEvent (QKeyEvent * event) override;
  private:
    ResizableHeaderlessTableView * tableView_ = nullptr;
    QuantileTableModel * tableModel_ = nullptr;
    TemporaryLabel * errorMessageLabel_ = nullptr;
    ValuesLineEdit * targetLineEdit_ = nullptr;

  };

  class PERSALYS_VIEW_API QuantileAnalysisThresholdPage : public QWizardPage
  {
    Q_OBJECT

  public:
    QuantileAnalysisThresholdPage(QWidget* parent = 0);

    void initialize(const Analysis& analysis);
    Analysis getAnalysis(const OT::String& name, const PhysicalModel& physicalModel) const;

    virtual int nextId() const;
    virtual bool validatePage();

    OT::Sample getThreshold() const {
      return tableModel_->getThreshold();
    }
  signals:
    void thresholdSelected();
  protected:
    void buildInterface();
    void plotMeanExcess(OT::Graph graph, const QString& varName);
  private:
    ResizableHeaderlessTableView * tableView_ = nullptr;
    QuantileThresholdTableModel * tableModel_ = nullptr;
    TemporaryLabel * errorMessageLabel_ = nullptr;
    QComboBox * marginalComboBox_ = nullptr;
    QComboBox * tailComboBox_ = nullptr;
    PlotWidget * plotWidget_ = nullptr;
  };

  class PERSALYS_VIEW_API QuantileAnalysisWizard : public AnalysisWizard
  {
    Q_OBJECT

  public:
    enum {Page_Intro, Page_Probability, Page_Threshold};

    QuantileAnalysisWizard(const Analysis& analysis, QWidget* parent = 0);
    void initialize(const Analysis& analysis);

    virtual Analysis getAnalysis() const;
    virtual int nextId() const;

  protected:
    void buildInterface();

  private:
    QuantileAnalysisIntroPage * introPage_ = nullptr;
    QuantileAnalysisProbabilityPage * probabilityPage_ = nullptr;
    QuantileAnalysisThresholdPage * thresholdPage_ = nullptr;
  };
}
#endif
