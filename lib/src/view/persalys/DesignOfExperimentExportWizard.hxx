/**
 *  @brief QWizard to export a design of experimetns as a data model
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

#ifndef PERSALYS_DESIGNOFEXPERIMENTEXPORTWIZARD_HXX
#define PERSALYS_DESIGNOFEXPERIMENTEXPORTWIZARD_HXX

#include "persalys/Wizard.hxx"
#include "persalys/Analysis.hxx"
#include "persalys/StudyItem.hxx"

#include <QComboBox>
#include <QStandardItemModel>
#include <QVBoxLayout>

namespace PERSALYS
{
class PERSALYS_VIEW_API DesignOfExperimentExportWizard : public Wizard
{
  Q_OBJECT

  friend class TestDesignOfExperimentExportWizard;

public:
 explicit DesignOfExperimentExportWizard(const StudyItem * item, QWidget * parent = nullptr);

  Analysis getAnalysis() const;

protected:
  void buildInterface();
protected slots:
  void updateWidgets();

private:
  QComboBox           * doeComboBox_      = nullptr;
  QStandardItemModel  * doeComboBoxModel_ = nullptr;
  QVBoxLayout         * parametersLayout_ = nullptr;

};

} // namespace PERSALYS

#endif // PERSALYS_DESIGNOFEXPERIMENTEXPORTWIZARD_HXX