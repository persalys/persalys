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
#include "persalys/DataCleaning.hxx"

#include <openturns/SpecFunc.hxx>

using namespace OT;

namespace PERSALYS
{
  CLASSNAMEINIT(DataCleaning)

  DataCleaning::DataCleaning(const Sample& sample)
    : Object()
    , sample_(sample)
    , median_(sample.getDimension())
    , mean_(sample.getDimension())
    , mad_(sample.getDimension())
    , geomMad_(0)
    , nanFounds_(sample.getDimension())
  {
    analyseSample();
  }

  void DataCleaning::removeAllNans() {
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

  void DataCleaning::replaceAllNans(const Point& point) {
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

  void DataCleaning::removeNansByColumn(const UnsignedInteger col) {
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

  void DataCleaning::replaceNansByColumn(const UnsignedInteger col, const OT::Scalar& val) {
    if(!sample_.getSize())
      return;
    for(UnsignedInteger i=0; i<sample_.getSize(); ++i) {
      if(!SpecFunc::IsNormal(sample_(i,col))) {
        sample_(i,col) = val;
      }
    }
  }

  void DataCleaning::computeMAD() {
    Sample tmp(sample_.getSize(), sample_.getDimension());
    for(UnsignedInteger i=0; i<sample_.getSize(); ++i)
      for(UnsignedInteger j=0; j<sample_.getDimension(); ++j)
        tmp(i,j) = std::abs(sample_(i,j) - median_[j]);
    mad_ = tmp.computeMedian();
  }

  void DataCleaning::computeGeometricMAD() {
    computeMAD();
    geomMad_ = mad_.norm();
  }

  void DataCleaning::analyseSample() {
    if(!sample_.getSize())
      return;
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

  Sample DataCleaning::getSample() const {
    return sample_;
  }

  Point DataCleaning::getMedian() const {
    return median_;
  }

  Point DataCleaning::getMean() const {
    return mean_;
  }

  Point DataCleaning::getMAD() const {
    return mad_;
  }

  Scalar DataCleaning::getGeometricMAD() const {
    return geomMad_;
  }

  Point DataCleaning::getNanNumbers() const {
    return nanFounds_;
  }
}
