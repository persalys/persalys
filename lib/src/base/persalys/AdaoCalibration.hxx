//                                               -*- C++ -*-
/**
 *  @brief ADAO Calibration backend
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_ADAOCALIBRATION_HXX
#define PERSALYS_ADAOCALIBRATION_HXX

#include <openturns/CalibrationAlgorithmImplementation.hxx>

namespace PERSALYS
{

class AdaoCalibration : public OT::CalibrationAlgorithmImplementation
{
  CLASSNAME
public:
  /** Constructor with parameters */
  AdaoCalibration(const OT::String & methodName,
                  const OT::Function & model,
                  const OT::Sample & inputObservations,
                  const OT::Sample & outputObservations,
                  const OT::Point & candidate,
                  const OT::CovarianceMatrix & parameterCovariance,
                  const OT::CovarianceMatrix & errorCovariance);

  /** Run algorithm */
  virtual void run();

private:
  OT::String methodName_;
  OT::Function model_;

  OT::Sample inputObservations_;

  OT::CovarianceMatrix errorCovariance_;
};
}
#endif
