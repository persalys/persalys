//                                               -*- C++ -*-
/**
 *  @brief QStandardItem, observer of design of experiments
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
#ifndef PERSALYS_DESIGNOFEXPERIMENTDEFINITIONITEM_HXX
#define PERSALYS_DESIGNOFEXPERIMENTDEFINITIONITEM_HXX

#include "persalys/AnalysisItem.hxx"

namespace PERSALYS
{
class PERSALYS_MODEL_API DesignOfExperimentDefinitionItem : public AnalysisItem
{
  Q_OBJECT

public:
  DesignOfExperimentDefinitionItem(const Analysis& analysis);

  QVariant data(int role) const override;
  OT::Sample getOriginalInputSample() const;

  void appendItem(const Analysis& analysis) override;

  void update(Observable * source, const OT::String & message) override;
  void fill();

protected:
  void buildActions() override;

public slots:
  void updateAnalysis(const Analysis & analysis) override;
  void removeAnalysis() override;
  void appendEvaluationItem();
  void createEvaluation();

private:
  QAction * evaluateAction_ = nullptr;
};
}
#endif
