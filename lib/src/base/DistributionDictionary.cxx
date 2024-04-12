//                                               -*- C++ -*-
/**
 *  @brief Helper to build OT distribution
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#include "persalys/DistributionDictionary.hxx"

#include <openturns/SpecFunc.hxx>
#include <openturns/OTDistribution.hxx>

using namespace OT;

namespace PERSALYS
{

std::vector<String> DistributionDictionary::ContinuousDistributions_ =
{
  "Arcsine", "Beta", "ChiSquare", "Exponential", "Gamma", "Gumbel", "InverseNormal",
  "Laplace", "Logistic", "LogNormal", "LogUniform", "Normal", "Rayleigh", /*"SmoothedUniform", */ "Student",
  "Trapezoidal", "Triangular", "Uniform", "WeibullMax", "WeibullMin"
};
std::vector<String> DistributionDictionary::DiscreteDistributions_ =
{
  "Bernoulli", "Geometric", "Binomial", "Geometric", "Hypergeometric", "NegativeBinomial",
  "Poisson", "Skellam", "UserDefined", "ZipfMandelbrot"
};


/* Build a distribution factory */
DistributionFactory DistributionDictionary::BuildDistributionFactory(const String & distributionName)
{
  if (distributionName == "Arcsine") return ArcsineFactory();
  else if (distributionName == "Beta") return BetaFactory();
  else if (distributionName == "ChiSquare") return ChiSquareFactory();
  else if (distributionName == "Exponential") return ExponentialFactory();
  else if (distributionName == "Gamma") return GammaFactory();
  else if (distributionName == "Gumbel") return GumbelFactory();
  else if (distributionName == "InverseNormal") return InverseNormalFactory();
  else if (distributionName == "Laplace") return LaplaceFactory();
  else if (distributionName == "Logistic") return LogisticFactory();
  else if (distributionName == "LogNormal") return LogNormalFactory();
  else if (distributionName == "LogUniform") return LogUniformFactory();
  else if (distributionName == "Normal") return NormalFactory();
  else if (distributionName == "Pareto") return ParetoFactory();
  else if (distributionName == "Rayleigh") return RayleighFactory();
  else if (distributionName == "Student") return StudentFactory();
  else if (distributionName == "Trapezoidal") return TrapezoidalFactory();
  else if (distributionName == "Triangular") return TriangularFactory();
  else if (distributionName == "Uniform") return UniformFactory();
  else if (distributionName == "WeibullMax") return WeibullMaxFactory();
  else if (distributionName == "WeibullMin") return WeibullMinFactory();
  else if (distributionName == "Weibull") return WeibullMinFactory();
  else
  {
    throw InvalidArgumentException(HERE) << "DistributionDictionary::BuildDistributionFactory unknown distribution: " << distributionName;
  }
}


/* Build a copula factory */
DistributionFactory DistributionDictionary::BuildCopulaFactory(const String& distributionName)
{
  if (distributionName == "AliMikhailHaq") return AliMikhailHaqCopulaFactory();
  else if (distributionName == "Bernstein") return BernsteinCopulaFactory();
  else if (distributionName == "Clayton") return ClaytonCopulaFactory();
  else if (distributionName == "FarlieGumbelMorgenstern") return FarlieGumbelMorgensternCopulaFactory();
  else if (distributionName == "Frank") return FrankCopulaFactory();
  else if (distributionName == "Gumbel") return GumbelCopulaFactory();
  else if (distributionName == "Independent") return IndependentCopulaFactory();
  else if (distributionName == "Normal") return NormalCopulaFactory();
  else
    throw InvalidArgumentException(HERE) << "DistributionDictionary::BuildCopulaFactory unknown copula: " << distributionName;
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
    else if (distributionName == "Bernoulli")
    {
      return Bernoulli();
    }
    else if (distributionName == "Beta")
    {
      return BetaMuSigma(mu, sigma, mu - 3 * sigma, mu + 3 * sigma).getDistribution();
    }
    else if (distributionName == "Binomial")
    {
      return Binomial();
    }
    else if (distributionName == "ChiSquare")
    {
      return ChiSquare(1.); // arbitrary value for nu
    }
    else if (distributionName == "Exponential")
    {
      return Exponential(1.0 / sigma, mu - sigma);
    }
    else if (distributionName == "Gamma")
    {
      return GammaMuSigma(mu, sigma, mu - 10.0 * sigma).getDistribution(); // arbitrary gamma
    }
    else if (distributionName == "Geometric")
    {
      return Geometric();
    }
    else if (distributionName == "Gumbel")
    {
      return GumbelMuSigma(mu, sigma).getDistribution();
    }
    else if (distributionName == "Hypergeometric")
    {
      return Hypergeometric(4, 1, 1);
    }
    else if (distributionName == "InverseNormal")
    {
      if (mu <= 0)
        return InverseNormal(0.1, 0.01);
      return InverseNormal(mu, mu * mu * mu / (sigma * sigma));
    }
    else if (distributionName == "Laplace")
    {
      return Laplace(mu, sqrt(2.0) / sigma);
    }
    else if (distributionName == "Logistic")
    {
      return Logistic(mu, sigma / SpecFunc::PI_SQRT3);
    }
    else if (distributionName == "LogNormal")
    {
      return LogNormalMuSigma(mu, sigma, mu - 10.0 * sigma).getDistribution(); // arbitrary gamma
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
      double deltaSquareRoot = sqrt(mu2 * mu2 + sigma2 * sigma2);
      double muLog = log(mu2 * mu2 / deltaSquareRoot);
      double sigmaLog = sqrt(2 * log(deltaSquareRoot / mu2));

      return LogUniform(muLog - sqrt(3.0) * sigmaLog, muLog + sqrt(3.0) * sigmaLog);
    }
    else if (distributionName == "NegativeBinomial")
    {
      return NegativeBinomial();
    }
    else if (distributionName == "Normal")
    {
      return Normal(mu, sigma);
    }
    else if (distributionName == "Pareto")
    {
      return Pareto(sigma, 3.0, mu);
    }
    else if (distributionName == "Poisson")
    {
      return Poisson();
    }
    else if (distributionName == "Rayleigh")
    {
      return Rayleigh(sigma / 0.6551363775620335530939357, mu - sigma * 1.253314137315500251207882 / 0.6551363775620335530939357);
    }
    else if (distributionName == "Skellam")
    {
      return Skellam();
    }
    else if (distributionName == "SmoothedUniform")
    {
      return SmoothedUniform(mu - sigma * sqrt(3.0), mu + sigma * sqrt(3.0), sigma);
    }
    else if (distributionName == "Student")
    {
      return Student(3., mu, sigma); // arbitrary value for nu
    }
    else if (distributionName == "Trapezoidal")
    {
      return Trapezoidal(mu - 2.0 * sigma, mu - sigma, mu + sigma, mu + 2.0 * sigma);
    }
    else if (distributionName == "Triangular")
    {
      return Triangular(mu - sqrt(6.0) * sigma, mu, mu + sqrt(6.0) * sigma);
    }
    else if (distributionName == "Uniform")
    {
      return Uniform(mu - sigma * sqrt(3.0), mu + sigma * sqrt(3.0));
    }
    else if (distributionName == "UserDefined")
    {
      Sample values(2, 1);
      values(1, 0) = 1.;
      return UserDefined(values, Point(2, 0.5));
    }
    else if (distributionName == "WeibullMax")
    {
      return WeibullMaxMuSigma(mu, sigma, mu + 10.0 * sigma).getDistribution(); // arbitrary gamma
    }
    else if (distributionName == "WeibullMin")
    {
      return WeibullMinMuSigma(mu, sigma, mu - 10.0 * sigma).getDistribution(); // arbitrary gamma
    }
    else if (distributionName == "ZipfMandelbrot")
    {
      return ZipfMandelbrot();
    }
    else
    {
      throw InvalidArgumentException(HERE) << "The given distribution " << distributionName << " is not available.";
    }
  }
  catch (InvalidArgumentException & ex)
  {
    Log::Error(OSS() << "DistributionDictionary::BuildDistribution: Error when creating the distribution " << distributionName << "\n");
    throw InvalidArgumentException(HERE) << ex.what();
  }
}


/* Get the parameters collection */
Distribution::PointWithDescriptionCollection DistributionDictionary::GetParametersCollection(const Distribution & distribution)
{
  String distributionName = distribution.getImplementation()->getClassName();
  Distribution::PointWithDescriptionCollection nPWithDescColl(distribution.getParametersCollection());
  PointWithDescription nPWithDesc;

  if (distributionName == "Arcsine")
  {
    ArcsineMuSigma d1;
    nPWithDesc = d1.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d1.getDescription());
    nPWithDescColl.add(nPWithDesc);
  }
  else if (distributionName == "Beta")
  {
    BetaMuSigma d1;
    nPWithDesc = d1.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d1.getDescription());
    nPWithDescColl.add(nPWithDesc);
  }
  else if (distributionName == "Gamma")
  {
    GammaMuSigma d1;
    nPWithDesc = d1.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d1.getDescription());
    nPWithDescColl.add(nPWithDesc);
  }
  else if (distributionName == "Gumbel")
  {
    GumbelLambdaGamma d1;
    nPWithDesc = d1.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d1.getDescription());
    nPWithDescColl.add(nPWithDesc);

    GumbelMuSigma d2;
    nPWithDesc = d2.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d2.getDescription());
    nPWithDescColl.add(nPWithDesc);
  }
  else if (distributionName == "LogNormal")
  {
    LogNormalMuSigma d1;
    nPWithDesc = d1.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d1.getDescription());
    nPWithDescColl.add(nPWithDesc);

    LogNormalMuSigmaOverMu d2;
    nPWithDesc = d2.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d2.getDescription());
    nPWithDescColl.add(nPWithDesc);
  }
  else if (distributionName == "Normal")
  {
    nPWithDesc = nPWithDescColl[0];

    Description description(2);
    description[0] = "mu";
    description[1] = "sigma";
    nPWithDesc.setDescription(description);

    nPWithDescColl[0] = nPWithDesc;
  }
  else if (distributionName == "Student")
  {
    nPWithDesc = nPWithDescColl[0];

    Description description(3);
    description[0] = "nu";
    description[1] = "mu";
    description[2] = "sigma";
    nPWithDesc.setDescription(description);

    nPWithDescColl[0] = nPWithDesc;
  }
  else if (distributionName == "UserDefined")
  {
    nPWithDesc = PointWithDescription(2);

    Description description(2);
    description[0] = "Values";
    description[1] = "Probabilities";
    nPWithDesc.setDescription(description);

    nPWithDescColl.clear();
    nPWithDescColl.add(nPWithDesc);
  }
  else if (distributionName == "WeibullMax")
  {
    WeibullMaxMuSigma d1;
    nPWithDesc = d1.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d1.getDescription());
    nPWithDescColl.add(nPWithDesc);
  }
  else if (distributionName == "WeibullMin")
  {
    WeibullMinMuSigma d1;
    nPWithDesc = d1.inverse(distribution.getParameter());
    nPWithDesc.setDescription(d1.getDescription());
    nPWithDescColl.add(nPWithDesc);
  }

  return nPWithDescColl;
}


/* Update de distribution */
void DistributionDictionary::UpdateDistribution(Distribution & distribution,
    const PointWithDescription & description,
    UnsignedInteger parametersType)
{
  String distributionName = distribution.getImplementation()->getClassName();

  if (parametersType == 0)
  {
    Distribution::PointWithDescriptionCollection nPWithDescColl;
    nPWithDescColl.add(description);
    distribution.setParametersCollection(nPWithDescColl);
  }
  else
  {
    if (distributionName == "Arcsine")
    {
      if (parametersType == 1)
        distribution.setParameter(ArcsineMuSigma(description[0], description[1]).evaluate());
    }
    else if (distributionName == "Beta")
    {
      if (parametersType == 1)
        distribution.setParameter(BetaMuSigma(description[0], description[1], description[2], description[3]).evaluate());
    }
    else if (distributionName == "Gamma")
    {
      if (parametersType == 1)
        distribution.setParameter(GammaMuSigma(description[0], description[1], description[2]).evaluate());
    }
    else if (distributionName == "Gumbel")
    {
      if (parametersType == 1)
        distribution.setParameter(GumbelLambdaGamma(description[0], description[1]).evaluate());
      else if (parametersType == 2)
        distribution.setParameter(GumbelMuSigma(description[0], description[1]).evaluate());
    }
    else if (distributionName == "LogNormal")
    {
      if (parametersType == 1)
        distribution.setParameter(LogNormalMuSigma(description[0], description[1], description[2]).evaluate());
      else if (parametersType == 2)
        distribution.setParameter(LogNormalMuSigmaOverMu(description[0], description[1], description[2]).evaluate());
    }
    else if (distributionName == "WeibullMax")
    {
      if (parametersType == 1)
        distribution.setParameter(WeibullMaxMuSigma(description[0], description[1], description[2]).evaluate());
    }
    else if (distributionName == "WeibullMin")
    {
      if (parametersType == 1)
        distribution.setParameter(WeibullMinMuSigma(description[0], description[1], description[2]).evaluate());
    }
  }
}


/* Compute standard deviation from mean */
double DistributionDictionary::ComputeSigmaFromMu(const double mu)
{
  double sigma = 0.1 * fabs(mu);
  if (sigma < 1e-3)
    sigma = 1.0;
  return sigma;
}
}
