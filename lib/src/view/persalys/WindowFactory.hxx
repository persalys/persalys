//                                               -*- C++ -*-
/**
 *  @brief Window factory
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
#ifndef PERSALYS_WINDOWFACTORY_HXX
#define PERSALYS_WINDOWFACTORY_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/AnalysisWizard.hxx"
#include "persalys/PhysicalModelItem.hxx"
#include "persalys/AnalysisItem.hxx"
#include "persalys/DesignOfExperimentItem.hxx"
#include "persalys/DataFieldModelItem.hxx"

#include <QWidget>

namespace PERSALYS
{
class PERSALYS_VIEW_API WindowFactory
{
public:
  static SubWindow * GetWindow(Item* item, QWidget *parent = nullptr);
  static SubWindow * GetPhysicalModelWindow(PhysicalModelItem* item, QWidget *parent = nullptr);
  static SubWindow * GetDesignOfExperimentWindow(DesignOfExperimentItem* item, QWidget *parent = nullptr);
  static SubWindow * GetDataFieldModelWindow(DataFieldModelItem *item, QWidget *parent = nullptr);
  static AnalysisWizard * GetAnalysisWizard(const Analysis& analysis, const bool isGeneralWizard, QWidget *parent = nullptr);
  static SubWindow * GetAnalysisWindow(AnalysisItem* item, QWidget *parent = nullptr);
};
}
#endif
