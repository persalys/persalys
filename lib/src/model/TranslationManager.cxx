//                                               -*- C++ -*-
/**
 *  @brief QObject to translate words
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#include "otgui/TranslationManager.hxx"

namespace OTGUI
{

boost::bimap< QString, QString > TranslationManager::DistributionsNames_;
boost::bimap< QString, QString > TranslationManager::DistributionsParametersNames_;


void TranslationManager::InitializeDistributionsNames()
{
  // list of all the available distributions
  DistributionsNames_.insert(type("Arcsine", tr("Arcsine")));
  DistributionsNames_.insert(type("Beta", tr("Beta")));
  DistributionsNames_.insert(type("ChiSquare", tr("ChiSquare")));
  DistributionsNames_.insert(type("Exponential", tr("Exponential")));
  DistributionsNames_.insert(type("Gamma", tr("Gamma")));
  DistributionsNames_.insert(type("Gumbel", tr("Gumbel")));
  DistributionsNames_.insert(type("InverseNormal", tr("InverseNormal")));
  DistributionsNames_.insert(type("Laplace", tr("Laplace")));
  DistributionsNames_.insert(type("Logistic", tr("Logistic")));
  DistributionsNames_.insert(type("LogNormal", tr("LogNormal")));
  DistributionsNames_.insert(type("LogUniform", tr("LogUniform")));
  DistributionsNames_.insert(type("Normal", tr("Normal")));
  DistributionsNames_.insert(type("Rayleigh", tr("Rayleigh")));
  DistributionsNames_.insert(type("Student", tr("Student")));
  DistributionsNames_.insert(type("Trapezoidal", tr("Trapezoidal")));
  DistributionsNames_.insert(type("Triangular", tr("Triangular")));
  DistributionsNames_.insert(type("Uniform", tr("Uniform")));
  DistributionsNames_.insert(type("Weibull", tr("Weibull")));
}

void TranslationManager::InitializeDistributionsParametersNames()
{
  DistributionsParametersNames_.insert(type("mean", tr("mean")));
  DistributionsParametersNames_.insert(type("standard deviation", tr("standard deviation")));
}


QString TranslationManager::GetTranslatedDistributionName(const std::string& name)
{
  if (DistributionsNames_.empty())
    InitializeDistributionsNames();

  bimap_type::left_const_iterator left_iter = DistributionsNames_.left.find(name.c_str());

  if (left_iter != DistributionsNames_.left.end())
    return left_iter->second;

  return name.c_str();
}


QString TranslationManager::GetTranslatedDistributionParameterName(const std::string& name)
{
  if (DistributionsParametersNames_.empty())
    InitializeDistributionsParametersNames();

  bimap_type::left_const_iterator left_iter = DistributionsParametersNames_.left.find(name.c_str());

  if (left_iter != DistributionsParametersNames_.left.end())
    return left_iter->second;

  return name.c_str();
}


std::string TranslationManager::GetDistributionName(const QString& name)
{
  if (DistributionsNames_.empty())
    InitializeDistributionsNames();

  bimap_type::right_const_iterator right_iter = DistributionsNames_.right.find(name);

  if (right_iter != DistributionsNames_.right.end())
    return right_iter->second.toStdString();

  return name.toStdString();
}


/* Get the list of the available distributions */
QStringList TranslationManager::GetAvailableDistributions()
{
  if (DistributionsNames_.empty())
    InitializeDistributionsNames();

  QStringList distributions;
  for (bimap_type::left_const_iterator left_iter = DistributionsNames_.left.begin(), iend = DistributionsNames_.left.end(); left_iter != iend; ++left_iter)
    distributions << left_iter->second;
  return distributions;
}
}
