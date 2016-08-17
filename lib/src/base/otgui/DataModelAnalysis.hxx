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
#ifndef OTGUI_DATAMODELANALYSIS_HXX
#define OTGUI_DATAMODELANALYSIS_HXX

#include "AnalysisImplementation.hxx"
#include "DataModel.hxx"

namespace OTGUI {
class OTGUI_API DataModelAnalysis : public AnalysisImplementation
{
public:
  /** Default constructor */
  DataModelAnalysis();

  /** Constructor with parameters */
  DataModelAnalysis(const OT::String & name, const DataModel & dataModel);

  /** Virtual constructor */
  virtual DataModelAnalysis * clone() const;

  DataModel getDataModel() const;
  void setDataModel(const DataModel & physicalModel);

  OT::String getModelName() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

private:
  DataModel dataModel_;
};
}
#endif