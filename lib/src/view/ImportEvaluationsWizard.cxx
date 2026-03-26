/**
 *  @brief QWizard to import design of experiment evaluations
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

#include "persalys/ImportEvaluationsWizard.hxx"

namespace PERSALYS
{

ImportEvaluationsWizard::ImportEvaluationsWizard(const Analysis &designOfExperiment, QWidget *parent)
: AnalysisWizard(designOfExperiment, parent)
{
  buildInterface();
}

void ImportEvaluationsWizard::buildInterface()
{
  setWindowTitle(tr("Import design of experiments evaluations"));

  importPage_ = new ImportedDesignPage(this);
  importPage_->initialize(analysis_);
  addPage(importPage_);
}

Analysis ImportEvaluationsWizard::getAnalysis() const
{
  return importPage_->getAnalysis();
}

} // namespace PERSALYS
