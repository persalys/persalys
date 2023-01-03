//                                               -*- C++ -*-
/**
 *  @brief Sample cleaning classes
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_DATACLEANING_HXX
#define PERSALYS_DATACLEANING_HXX

#include "persalys/PersalysPrivate.hxx"
#include <openturns/Sample.hxx>

namespace PERSALYS
{
  class PERSALYS_BASE_API DataCleaning : public OT::Object
  {
  CLASSNAME
  public:
    /* Default ctor */
    DataCleaning() {};
    /* Sample-based constructor */
    explicit DataCleaning(const OT::Sample& sample);

    /* Removes Nans/Infs in sample */
    void removeAllNans();

    /* Replaces Nans/Infs in sample point by point values*/
    void replaceAllNans(const OT::Point& point);

    /* Removes Nans/Infs in sample column */
    void removeNansByColumn(const OT::UnsignedInteger col);

    /* Replaces Nans/Infs in sample column by value */
    void replaceNansByColumn(const OT::UnsignedInteger col, const OT::Scalar& val);

    /* Computes sample median absolute deviation */
    void computeMAD();

    /* Computes sample geometric median absolute deviation */
    void computeGeometricMAD();

    /* Column by column sample analysis.
     Allows marginals mean/median computation by ignoring Nans/Infs
     Evaluates number of Nans/Infs for each marginal*/
    void analyseSample();

    /* Sample accessor */
    OT::Sample getSample() const;

    /* Median accessor */
    OT::Point getMedian() const;

    /* Mean accessor */
    OT::Point getMean() const;

    /* MAD accessor */
    OT::Point getMAD() const;

    /* Geom. MAD accessor */
    OT::Scalar getGeometricMAD() const;

    /* Returns number of Nans/Infs in each sample column */
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
