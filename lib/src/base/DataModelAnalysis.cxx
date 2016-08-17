//                                               -*- C++ -*-
/**
 *  @brief Top-level class for all analysis with data model
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/DataModelAnalysis.hxx"

using namespace OT;

namespace OTGUI {

/* Default constructor */
DataModelAnalysis::DataModelAnalysis()
  : AnalysisImplementation()
{
}


/* Constructor with parameters */
DataModelAnalysis::DataModelAnalysis(const String & name, const DataModel & dataModel)
  : AnalysisImplementation(name)
  , dataModel_(dataModel)
{
}


/* Virtual constructor */
DataModelAnalysis* DataModelAnalysis::clone() const
{
  return new DataModelAnalysis(*this);
}


DataModel DataModelAnalysis::getDataModel() const
{
  return dataModel_;
}


void DataModelAnalysis::setDataModel(const DataModel & dataModel)
{
  dataModel_ = dataModel;
}


String DataModelAnalysis::getModelName() const
{
  return dataModel_.getName();
}


/* Method save() stores the object through the StorageManager */
void DataModelAnalysis::save(Advocate & adv) const
{
  AnalysisImplementation::save(adv);
  adv.saveAttribute("dataModel_", dataModel_);
}


/* Method load() reloads the object from the StorageManager */
void DataModelAnalysis::load(Advocate & adv)
{
  AnalysisImplementation::load(adv);
  adv.loadAttribute("dataModel_", dataModel_);
}
}