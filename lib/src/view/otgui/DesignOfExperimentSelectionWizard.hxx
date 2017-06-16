//                                               -*- C++ -*-
/**
 *  @brief QWizard to select a design of experiment
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_DESIGNOFEXPERIMENTSELECTIONWIZARD_HXX
#define OTGUI_DESIGNOFEXPERIMENTSELECTIONWIZARD_HXX

#include "otgui/DesignOfExperimentDefinitionItem.hxx"
#include "otgui/OTguiWizard.hxx"

#include <QStandardItem>
#include <QComboBox>

namespace OTGUI {

class OTGUI_API DesignOfExperimentSelectionWizard : public OTguiWizard
{
  Q_OBJECT

public:
  DesignOfExperimentSelectionWizard(QList<QStandardItem*> listDOE, QWidget *parent=0);

  DesignOfExperimentDefinitionItem* getSelectedDesignOfExperimentItem();

private:
  QComboBox * comboBox_;
  QStandardItemModel * comboModel_;
};
}
#endif