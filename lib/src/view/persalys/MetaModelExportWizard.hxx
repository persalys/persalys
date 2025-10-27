//                                               -*- C++ -*-
/**
 *  @brief QWizard to export a metamodel as physical model
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

#ifndef PERSALYS_METAMODELEXPORTWIZARD_HXX
#define PERSALYS_METAMODELEXPORTWIZARD_HXX

#include "persalys/Wizard.hxx"

#include <QStandardItemModel>
#include <QComboBox>
#include <QVBoxLayout>
#include <QButtonGroup>

namespace PERSALYS
{
class PERSALYS_VIEW_API MetaModelExportWizard : public Wizard
{
  Q_OBJECT

public:
  enum ExportType {Symbolic, Python};

  MetaModelExportWizard(const Analysis& model, bool isGeneralWizard = false, QWidget* parent = 0);

  Analysis getAnalysis() const;
  ExportType getExportType() const;

protected:
  void buildInterface();
protected slots:
  void updateWidgets();

private:
  QComboBox           * mmsComboBox_      = nullptr;
  QStandardItemModel  * mmsComboBoxModel_ = nullptr;
  QVBoxLayout         * parametersLayout_ = nullptr;
  QButtonGroup        * exportTypeGroup_  = nullptr;
};
}

#endif
