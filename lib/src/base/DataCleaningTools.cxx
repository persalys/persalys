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
#include "persalys/DataCleaningTools.hxx"

#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace PERSALYS
{
  CLASSNAMEINIT(DataCleaningTools)

  DataCleaningTools::DataCleaningTools(const Sample& sample)
    : Object()
    , sample_(sample)
    , median_(Point(sample.getDimension()))
    , mean_(Point(sample.getDimension()))
    , mad_(Point(sample.getDimension()))
    , geomMad_(0)
    , nanFounds_(Point(sample.getDimension()))
  {
    analyseSample();
  }

  void DataCleaningTools::removeAllNans() {
    if(!sample_.getSize())
      return;
    Sample sample(0, sample_.getDimension());
    for(UnsignedInteger i=0; i<sample_.getSize(); ++i) {
      Bool nanFound = false;
      for(UnsignedInteger j=0; j<sample_.getDimension(); ++j) {
        if(!SpecFunc::IsNormal(sample_(i,j))) {
          nanFound = true;
          break;
        }
      }
      if(!nanFound)
        sample.add(sample_[i]);
    }
    sample_ = sample;
  }

  void DataCleaningTools::replaceAllNans(const Point& point) {
    if(!sample_.getSize())
      return;
    for(UnsignedInteger i=0; i<sample_.getSize(); ++i) {
      for(UnsignedInteger j=0; j<sample_.getDimension(); ++j) {
        if(!SpecFunc::IsNormal(sample_(i,j))) {
          sample_(i,j) = point[j];
        }
      }
    }
  }

  void DataCleaningTools::removeNansByColumn(const UnsignedInteger col) {
    if(!sample_.getSize())
      return;
    Sample sample(0, sample_.getDimension());
    for(UnsignedInteger i=0; i<sample_.getSize(); ++i) {
      if(SpecFunc::IsNormal(sample_(i,col))) {
        sample.add(sample_[i]);
      }
    }
    sample_ = sample;
  }

  void DataCleaningTools::replaceNansByColumn(const UnsignedInteger col, const OT::Scalar& val) {
    if(!sample_.getSize())
      return;
    for(UnsignedInteger i=0; i<sample_.getSize(); ++i) {
      if(!SpecFunc::IsNormal(sample_(i,col))) {
        sample_(i,col) = val;
      }
    }
  }

  void DataCleaningTools::computeMAD() {
    Sample tmp(sample_.getSize(), sample_.getDimension());
    for(UnsignedInteger i=0; i<sample_.getSize(); ++i)
      for(UnsignedInteger j=0; j<sample_.getDimension(); ++j)
        tmp(i,j) = abs(sample_(i,j) - median_[j]);
    mad_ = tmp.computeMedian();
  }

  void DataCleaningTools::computeGeometricMAD() {
    computeMAD();
    geomMad_ = mad_.norm();
  }

  void DataCleaningTools::analyseSample() {
    if(!sample_.getSize())
      return;
    nanFounds_ = Point(sample_.getDimension());
    for(UnsignedInteger j=0; j<sample_.getDimension(); ++j) {
      Sample sample = sample_.getMarginal(j);
      Sample cleanedSample(0,1);
      for(UnsignedInteger i=0; i<sample.getSize(); ++i) {
        if(!SpecFunc::IsNormal(sample(i,0))) {
          nanFounds_[j]++;
        } else {
          cleanedSample.add(Point(1,sample(i,0)));
        }
      }
      mean_[j] = cleanedSample.computeMean()[0];
      median_[j] = cleanedSample.computeMedian()[0];
    }
  }

  Sample DataCleaningTools::getSample() const {
    return sample_;
  }

  Point DataCleaningTools::getMedian() const {
    return median_;
  }

  Point DataCleaningTools::getMean() const {
    return mean_;
  }

  Point DataCleaningTools::getMAD() const {
    return mad_;
  }

  Scalar DataCleaningTools::getGeometricMAD() const {
    return geomMad_;
  }

  Point DataCleaningTools::getNanNumbers() const {
    return nanFounds_;
  }
}
