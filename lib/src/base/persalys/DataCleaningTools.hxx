//                                               -*- C++ -*-
/**
 *  @brief Sample cleaning classes
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
#ifndef PERSALYS_DATACLEANINGTOOLS_HXX
#define PERSALYS_DATACLEANINGTOOLS_HXX

#include "persalys/PersalysPrivate.hxx"
#include <openturns/Sample.hxx>

namespace PERSALYS
{
  class PERSALYS_BASE_API DataCleaningTools : public OT::Object
  {
  CLASSNAME
  public:
    DataCleaningTools() {};
    DataCleaningTools(const OT::Sample& sample);

    void removeAllNans();
    void replaceAllNans(const OT::Point& point);
    void removeNansByColumn(const OT::UnsignedInteger col);
    void replaceNansByColumn(const OT::UnsignedInteger col, const OT::Scalar& val);
    void computeMAD();
    void computeGeometricMAD();
    void analyseSample();
    OT::Sample getSample() const;
    OT::Point getMedian() const;
    OT::Point getMean() const;
    OT::Point getMAD() const;
    OT::Scalar getGeometricMAD() const;
    OT::Point getNanNumbers() const;

  private:
    OT::Sample sample_;
    OT::Point median_;
    OT::Point mean_;
    OT::Point mad_;
    OT::Scalar geomMad_;
    OT::Point nanFounds_;
  };
}
#endif
