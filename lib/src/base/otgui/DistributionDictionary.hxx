// DistributionDictionary.hxx

#ifndef OTGUI_DISTRIBUTIONDICTIONARY_H
#define OTGUI_DISTRIBUTIONDICTIONARY_H

#include "Distribution.hxx"

namespace OTGUI {
class DistributionDictionary
{
public:
  /* Get the list of the available distributions */
  static OT::Description GetAvailableDistributions();

  /* Build a distribution with native parameters from the mean value */
  static OT::Distribution BuildDistribution(const OT::String & distributionName, const double mu);

  /* Compute standard deviation from mean */
  static double ComputeSigmaFromMu(const double mu);
};
}
#endif