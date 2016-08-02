//                                               -*- C++ -*-
/**
 *  @brief Helper to build OT distribution
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
#include "otgui/DistributionDictionary.hxx"

#include "openturns/SpecFunc.hxx"
#include "openturns/ArcsineMuSigma.hxx"
#include "openturns/BetaMuSigma.hxx"
#include "openturns/ChiSquare.hxx"
#include "openturns/Exponential.hxx"
#include "openturns/InverseNormal.hxx"
#include "openturns/GammaMuSigma.hxx"
#include "openturns/Gumbel.hxx"
#include "openturns/GumbelAB.hxx"
#include "openturns/GumbelMuSigma.hxx"
#include "openturns/Laplace.hxx"
#include "openturns/LogNormal.hxx"
#include "openturns/LogNormalMuSigma.hxx"
#include "openturns/LogNormalMuSigmaOverMu.hxx"
#include "openturns/Logistic.hxx"
#include "openturns/LogUniform.hxx"
#include "openturns/Normal.hxx"
#include "openturns/Rayleigh.hxx"
#include "openturns/Student.hxx"
#include "openturns/Trapezoidal.hxx"
#include "openturns/Triangular.hxx"
#include "openturns/Uniform.hxx"
#include "openturns/Weibull.hxx"
#include "openturns/WeibullMuSigma.hxx"

using namespace OT;

namespace OTGUI {

/* Get the list of the available distributions */
Description DistributionDictionary::GetAvailableDistributions()
{
  Description listDistributions(18);
  listDistributions[0] = "Arcsine";
  listDistributions[1] = "Beta";
  listDistributions[2] = "ChiSquare";
  listDistributions[3] = "Gamma";
  listDistributions[4] = "Gumbel";
  listDistributions[5] = "Exponential";
  listDistributions[6] = "InverseNormal";
  listDistributions[7] = "Laplace";
  listDistributions[8] = "Logistic";
  listDistributions[9] = "LogNormal";
  listDistributions[10] = "LogUniform";
  listDistributions[11] = "Normal";
  listDistributions[12] = "Rayleigh";
  listDistributions[13] = "Student";
  listDistributions[14] = "Trapezoidal";
  listDistributions[15] = "Triangular";
  listDistributions[16] = "Uniform";
  listDistributions[17] = "Weibull";
  return listDistributions;
}


/* Build a distribution with native parameters from the mean value */
Distribution DistributionDictionary::BuildDistribution(const String & distributionName, const double mu)
{
  const double sigma = ComputeSigmaFromMu(mu);
  try
  {
    if (distributionName == "Arcsine")
    {
      return ArcsineMuSigma(mu, sigma).getDistribution();
    }
    else if (distributionName == "Beta")
    {
      return BetaMuSigma(mu, sigma, mu-3*sigma, mu+3*sigma).getDistribution();
    }
    else if (distributionName == "ChiSquare")
    {
      return ChiSquare(1.); // arbitrary value for nu
    }
    else if (distributionName == "Gamma")
    {
      return GammaMuSigma(mu, sigma, mu-10.0*sigma).getDistribution(); // arbitrary gamma
    }
    else if (distributionName == "Gumbel")
    {
      return GumbelMuSigma(mu, sigma).getDistribution();
    }
    else if (distributionName == "Exponential")
    {
      return Exponential(1.0/sigma, mu-sigma);
    }
    else if (distributionName == "InverseNormal")
    {
      if (mu <= 0)
        return InverseNormal(0.01, 0.1);
      return InverseNormal(mu*mu*mu/(sigma*sigma), mu);
    }
    else if (distributionName == "Laplace")
    {
      return Laplace(sqrt(2.0)/sigma, mu);
    }
    else if (distributionName == "Logistic")
    {
      return Logistic(mu, sigma/SpecFunc::PI_SQRT3);
    }
    else if (distributionName == "LogNormal")
    {
      return LogNormalMuSigma(mu, sigma, mu-10.0*sigma).getDistribution(); // arbitrary gamma
    }
    else if (distributionName == "LogUniform")
    {
      double mu2 = mu;
      double sigma2 = sigma;
      if (mu2 <= 0)
      {
        mu2 = sqrt(3.);
        sigma2 = 1.;
      }
      double deltaSquareRoot = sqrt(mu2*mu2+sigma2*sigma2);
      double muLog = log(mu2*mu2/deltaSquareRoot);
      double sigmaLog = sqrt(2*log(deltaSquareRoot/mu2));

      return LogUniform(muLog-sqrt(3.0)*sigmaLog, muLog+sqrt(3.0)*sigmaLog);
    }
    else if (distributionName == "Normal")
    {
      return Normal(mu, sigma);
    }
    else if (distributionName == "Rayleigh")
    {
      return Rayleigh(sigma/0.6551363775620335530939357, mu-sigma*1.253314137315500251207882/0.6551363775620335530939357);
    }
    else if (distributionName == "Student")
    {
      return Student(3., mu, sigma); // arbitrary value for nu
    }
    else if (distributionName == "Trapezoidal")
    {
      return Trapezoidal(mu-2.0*sigma, mu-sigma, mu+sigma, mu+2.0*sigma);
    }
    else if (distributionName == "Triangular")
    {
      return Triangular(mu-sqrt(6.0)*sigma, mu, mu+sqrt(6.0)*sigma);
    }
    else if (distributionName == "Uniform")
    {
      return Uniform(mu-sigma*sqrt(3.0), mu+sigma*sqrt(3.0));
    }
    else if (distributionName == "Weibull")
    {
      return WeibullMuSigma(mu, sigma, mu-10.0*sigma).getDistribution(); // arbitrary gamma
    }
    else
    {
      throw InvalidArgumentException(HERE) << "The given distribution " << distributionName << "is not available.";
    }
  }
  catch (InvalidArgumentException & ex)
  {
    std::cerr << "Error when creating the distribution " << distributionName << std::endl;
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


/* Get the parameters collection */
Distribution::NumericalPointWithDescriptionCollection DistributionDictionary::GetParametersCollection(const Distribution & distribution)
{
  String distributionName = distribution.getImplementation()->getClassName();
  Distribution::NumericalPointWithDescriptionCollection NPWithDescColl(distribution.getParametersCollection());

  if (distributionName == "Gumbel")
  {
    Gumbel d = *dynamic_cast<Gumbel*>(&*distribution.getImplementation());
    NumericalPointWithDescription NPWithDesc;

    GumbelAB d1(d.getA(), d.getB());
    NPWithDesc = d1.getValues();
    NPWithDesc.setDescription(d1.getDescription());
    NPWithDescColl.add(NPWithDesc);

    GumbelMuSigma d2(d.getMu(), d.getSigma());
    NPWithDesc = d2.getValues();
    NPWithDesc.setDescription(d2.getDescription());
    NPWithDescColl.add(NPWithDesc);
  }
  else if (distributionName == "LogNormal")
  {
    LogNormal d = *dynamic_cast<LogNormal*>(&*distribution.getImplementation());
    NumericalPointWithDescription NPWithDesc;

    LogNormalMuSigma d1(d.getMu(), d.getSigma(), d.getGamma());
    NPWithDesc = d1.getValues();
    NPWithDesc.setDescription(d1.getDescription());
    NPWithDescColl.add(NPWithDesc);

    LogNormalMuSigmaOverMu d2(d.getMu(), d.getSigmaOverMu(), d.getGamma());
    NPWithDesc = d2.getValues();
    NPWithDesc.setDescription(d2.getDescription());
    NPWithDescColl.add(NPWithDesc);
  }
  else if (distributionName == "Normal")
  {
    NumericalPointWithDescription NPWithDesc(NPWithDescColl[0]);

    Description description(2);
    description[0] = "mean";
    description[1] = "standard deviation";
    NPWithDesc.setDescription(description);

    NPWithDescColl[0] = NPWithDesc;
  }
  else if (distributionName == "Student")
  {
    NumericalPointWithDescription NPWithDesc(NPWithDescColl[0]);

    Description description(3);
    description[0] = "nu";
    description[1] = "mean";
    description[2] = "standard deviation";
    NPWithDesc.setDescription(description);

    NPWithDescColl[0] = NPWithDesc;
  }
  else if (distributionName == "Weibull")
  {
    Weibull d = *dynamic_cast<Weibull*>(&*distribution.getImplementation());
    NumericalPointWithDescription NPWithDesc;

    WeibullMuSigma d1(d.getMu(), d.getSigma(), d.getGamma());
    NPWithDesc = d1.getValues();
    NPWithDesc.setDescription(d1.getDescription());
    NPWithDescColl.add(NPWithDesc);
  }

  return NPWithDescColl;
}


/* Update de distribution */
void DistributionDictionary::UpdateDistribution(Distribution & distribution,
                                                const NumericalPointWithDescription & description,
                                                UnsignedInteger parametersType)
{
  String distributionName = distribution.getImplementation()->getClassName();

  if (parametersType == 0)
  {
    Distribution::NumericalPointWithDescriptionCollection NPWithDescColl;
    NPWithDescColl.add(description);
    distribution.setParametersCollection(NPWithDescColl);
  }
  else
  {
    if (distributionName == "Gumbel")
    {
      Gumbel * d = static_cast<Gumbel*>(&*distribution.getImplementation());
      if (parametersType == 1)
        d->setAB(description[0], description[1]);
      else if (parametersType == 2)
        d->setMuSigma(description[0], description[1]);
    }
    else if (distributionName == "LogNormal")
    {
      LogNormal * d = static_cast<LogNormal*>(&*distribution.getImplementation());
      d->setGamma(description[2]);
      if (parametersType == 1)
        d->setMuSigma(description[0], description[1]);
      else if (parametersType == 2)
        d->setMuSigma(description[0], description[1]*description[0]);
    }
    else if (distributionName == "Weibull")
    {
      Weibull * d = static_cast<Weibull*>(&*distribution.getImplementation());
      d->setGamma(description[2]);
      d->setMuSigma(description[0], description[1]);
    }
  }
}


/* Compute standard deviation from mean */
double DistributionDictionary::ComputeSigmaFromMu(const double mu)
{
  double sigma = 0.1*fabs(mu);
  if (sigma < 1e-3)
    sigma = 1.0;
  return sigma;
}
}