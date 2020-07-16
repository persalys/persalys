//                                               -*- C++ -*-
/**
 *  @brief Class to define observations
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_OBSERVATIONS_HXX
#define PERSALYS_OBSERVATIONS_HXX

#include "DataModel.hxx"
#include "BaseTools.hxx"

namespace PERSALYS
{

class PERSALYS_BASE_API Observations : public DataModel
{
  CLASSNAME

public:
  /** Default constructor */
  Observations();
  /** Constructor with parameters */
  Observations(const OT::String &name,
               const PhysicalModel &physicalModel);
  /** Constructor with parameters */
  Observations(const OT::String &name,
               const PhysicalModel &physicalModel,
               const OT::String &fileName,
               const OT::Indices &inputColumns,
               const OT::Indices &outputColumns,
               const OT::Description &inputNames,
               const OT::Description &outputNames);
  /** Constructor with parameters */
  Observations(const OT::String & name,
               const PhysicalModel &physicalModel,
               const OT::Sample &inSample,
               const OT::Sample &outSample);

  /** Virtual constructor */
  virtual Observations * clone() const;

  virtual void removeAllObservers();

  virtual void setColumns(const OT::Indices &inputColumns,
                          const OT::Description &inputNames,
                          const OT::Indices &outputColumns,
                          const OT::Description &outputNames);

  /** String converter */
  virtual OT::String __repr__() const;

protected:
  virtual OT::Sample importSample(const OT::String& fileName);
  virtual void setDefaultColumns();
  virtual void update();
  virtual void orderSamples();
};
}
#endif
