//                                               -*- C++ -*-
/**
 *  @brief QWizard to import a data model
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
#include "otgui/DataModelWizard.hxx"

#include "otgui/ImportDataModelPage.hxx"

using namespace OT;

namespace OTGUI {

DataModelWizard::DataModelWizard(const DesignOfExperiment & designOfExperiment, QWidget* parent)
  : OTguiWizard(parent)
{
  if (!dynamic_cast<const DataModel*>(&*designOfExperiment.getImplementation()))
    throw InvalidArgumentException(HERE) << "Can not build the datamodel wizard\n";
  dataModel_ = *dynamic_cast<const DataModel*>(&*designOfExperiment.getImplementation());

  buildInterface();
}


void DataModelWizard::buildInterface()
{
  setWindowTitle(tr("Data model"));

  ImportDataModelPage * importPage = new ImportDataModelPage(dataModel_);
  connect(importPage, SIGNAL(dataModelChanged(const DataModel&)), this, SLOT(setDataModel(const DataModel&)));
  addPage(importPage);
}


DataModel DataModelWizard::getDataModel() const
{
  return dataModel_;
}


void DataModelWizard::setDataModel(const DataModel & dataModel)
{
  dataModel_ = dataModel;
}
}