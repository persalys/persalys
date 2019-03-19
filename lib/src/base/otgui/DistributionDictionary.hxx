//                                               -*- C++ -*-
/**
 *  @brief Helper to build OT distribution
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
#ifndef OTGUI_DISTRIBUTIONDICTIONARY_HXX
#define OTGUI_DISTRIBUTIONDICTIONARY_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/DistributionFactory.hxx>

namespace OTGUI
{
class OTGUI_API DistributionDictionary
{
public:
  /** Build a distribution factory */
  static OT::DistributionFactory BuildDistributionFactory(const OT::String & distributionName);

  /** Build a copula factory */
  static OT::DistributionFactory BuildCopulaFactory(const OT::String & distributionName);

  /** Build a distribution with native parameters from the mean value */
  static OT::Distribution BuildDistribution(const OT::String & distributionName, const double mu);

  /** Get the parameters collection */
  static OT::Distribution::PointWithDescriptionCollection GetParametersCollection(const OT::Distribution & distribution);

  /** Update de distribution */
  static void UpdateDistribution(OT::Distribution & distribution, const OT::PointWithDescription & description, OT::UnsignedInteger parametersType);

  /** Compute standard deviation from mean */
  static double ComputeSigmaFromMu(const double mu);
};
}
#endif
