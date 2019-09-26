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
#include "persalys/AdaoCalibration.hxx"

#include <openturns/Normal.hxx>

using namespace OT;

namespace PERSALYS
{

/* Constructor with parameters */
AdaoCalibration::AdaoCalibration(const String & methodName,
                                 const Function & model,
                                 const Sample & inputObservations,
                                 const Sample & outputObservations,
                                 const Point & candidate,
                                 const CovarianceMatrix & parameterCovariance,
                                 const CovarianceMatrix & errorCovariance)
  : CalibrationAlgorithmImplementation(outputObservations, Normal(candidate, parameterCovariance))
  , methodName_(methodName)
  , model_(model)
  , inputObservations_(inputObservations)
  , errorCovariance_(errorCovariance)
{
}

void AdaoCalibration::run()
{
  throw NotYetImplementedException(HERE) << "TODO";
}

}
