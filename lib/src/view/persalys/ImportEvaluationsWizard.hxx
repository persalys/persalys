/**
 *  @brief QWizard to import design of experiment evaluations
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

#ifndef PERSALYS_IMPORTEVALUATIONSWIZARD_HXX
#define PERSALYS_IMPORTEVALUATIONSWIZARD_HXX

#include "persalys/AnalysisWizard.hxx"
#include "persalys/ImportedDesignPage.hxx"

namespace PERSALYS
{
class PERSALYS_VIEW_API ImportEvaluationsWizard: public AnalysisWizard
{
  Q_OBJECT

public:
  explicit ImportEvaluationsWizard(const Analysis &designOfExperiment, QWidget *parent = nullptr);

  Analysis getAnalysis() const override;

protected:
  void buildInterface();

private:
  ImportedDesignPage *importPage_ = nullptr;
};
} // namespace PERSALYS


#endif // PERSALYS_IMPORTEVALUATIONWIZARD_HXX