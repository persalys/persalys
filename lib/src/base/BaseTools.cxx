//                                               -*- C++ -*-
/**
 *  @brief base tools
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#include "persalys/BaseTools.hxx"

#include <stdint.h>
#include <chrono>

using namespace OT;

namespace PERSALYS
{

Parameters::Parameters()
  : pairsCollection_()
{
}


Parameters::~Parameters()
{
}


void Parameters::add(const String& name, const String& value)
{
  std::pair<String, String> newPair;
  newPair.first = name;
  newPair.second = value;
  pairsCollection_.add(newPair);
}


void Parameters::add(const String& name, const double& value)
{
  add(name, OSS() << value);
}


void Parameters::add(const String& name, const UnsignedInteger& value)
{
  add(name, OSS() << value);
}


void Parameters::add(const String& name, const Point& values)
{
  add(name, GetOTPointStr(values, ";"));
}


void Parameters::add(const Parameters& parameters)
{
  for (UnsignedInteger i = 0; i < parameters.getSize(); ++i)
    add(parameters[i].first, parameters[i].second);
}

String Parameters::get(const String& name) const
{
  for (UnsignedInteger i = 0; i < getSize(); ++i)
  {
    if (pairsCollection_[i].first == name)
      return pairsCollection_[i].second;
  }
  throw InvalidArgumentException(HERE) << "Key " << name << " not found in parameters";
}

UnsignedInteger Parameters::getSize() const
{
  return pairsCollection_.getSize();
}


std::pair<String, String> Parameters::operator[](const UnsignedInteger index) const
{
  return pairsCollection_[index];
}


bool Parameters::operator==(const Parameters& other) const
{
  if (getSize() != other.getSize())
    return false;
  bool equality = true;
  for (UnsignedInteger i = 0; i < getSize(); ++i)
  {
    equality &= (pairsCollection_[i].first == other[i].first);
    equality &= (pairsCollection_[i].second == other[i].second);
    if (!equality)
      return false;
  }
  return true;
}


String Parameters::GetOTSampleStr(const Sample& values)
{
  OSS sampleOss;
  sampleOss << "[\n";
  for (UnsignedInteger i = 0; i < values.getSize(); ++i)
  {
    sampleOss << "[";
    Point pt(values[i]);
    std::copy(pt.begin(), pt.end(), OSS_iterator<Scalar>(sampleOss, ", "));
    sampleOss << "]";
    if (i < (values.getSize() - 1))
      sampleOss << ",\n";
  }
  sampleOss << "]\n";

  return sampleOss;
}

String Parameters::GetOTSampleCollectionStr(const ProcessSample& processSample)
{
  OSS strOut;
  if (!processSample.getSize())
    return strOut;

  // SampleCollection
  strOut << "[";
  for(UnsignedInteger iTraj=0; iTraj<processSample.getSize(); iTraj++)
  {
    strOut << "[";
    for(UnsignedInteger iTime=0; iTime<processSample[iTraj].getSize(); ++iTime)
    {
      strOut << GetOTPointStr(processSample[iTraj][iTime]);
      if (iTime < processSample[iTraj].getSize() - 1)
        strOut << ", ";
    }
    strOut << "]";
    if (iTraj < processSample.getSize() - 1)
      strOut << ",\n ";
  }
  strOut << "]\n";
  // Mesh
  //strOut << "
  return strOut;
}


String Parameters::GetOTPointStr(const Point& values, const String& separator, bool useBrackets)
{
  return GetStr(values, separator, useBrackets, false);
}

String Parameters::GetOTPointWithDescriptionStr(const PointWithDescription& values)
{
  return GetStr(values, ", ", false, false);
}

String Parameters::GetOTIndicesStr(const Indices& values)
{
  return GetStr(values, ", ", true, false);
}

String Parameters::GetOTDescriptionStr(const Description& values, const bool quote, bool useBrackets)
{
  return GetStr(values, ", ", useBrackets, quote);
}

String Parameters::GetOTBoolCollectionStr(const Interval::BoolCollection& values)
{
  OSS oss;
  oss << "[";

  for (UnsignedInteger i = 0 ; i < values.getSize() ; ++i)
  {
    if (i > 0)
      oss << ", ";
    oss << (values[i] ? "True" : "False");
  }
  oss << "]";

  return oss.str();
}


String Parameters::GetOTCorrelationMatrixStr(const CorrelationMatrix &correlationMatrix)
{
  OSS oss;
  oss.setPrecision(12);

  oss << "R = ot.CorrelationMatrix(" << correlationMatrix.getNbRows() << ")\n";

  for (UnsignedInteger row = 0; row < correlationMatrix.getNbRows(); ++ row)
  {
    for (UnsignedInteger col = row + 1; col < correlationMatrix.getNbRows(); ++ col)
    {
      if (correlationMatrix(row, col) != 0.0)
      {
        oss << "R[" << row << ", " << col << "] = " << correlationMatrix(row, col) << "\n";
      }
    }
  }
  return oss;
}


String Parameters::GetOTNormalCopulaStr(const Distribution &distribution)
{
  OSS oss;
  if (distribution.getImplementation()->getClassName() != "NormalCopula")
    return oss;
  oss <<  GetOTCorrelationMatrixStr(distribution.getCorrelation());
  oss << "copula = ot.NormalCopula(ot.NormalCopula.GetCorrelationFromSpearmanCorrelation(R))\n";
  return oss;
}


Description Parameters::GetOTIntervalDescription(const Interval& interval)
{
  Description resu(interval.getDimension());

  for (UnsignedInteger i = 0; i < interval.getDimension(); ++i)
  {
    String intervalStr_i;

    intervalStr_i += (interval.getFiniteLowerBound()[i] ? "[" : "]");
    if (interval.getFiniteLowerBound()[i])
      intervalStr_i += OSS() << interval.getLowerBound()[i];
    else
      intervalStr_i += "-∞";
    intervalStr_i += ", ";
    if (interval.getFiniteUpperBound()[i])
      intervalStr_i += OSS() << interval.getUpperBound()[i];
    else
      intervalStr_i += "+∞";
    intervalStr_i += (interval.getFiniteUpperBound()[i] ? "]" : "[");

    resu[i] = intervalStr_i;
  }
  return resu;
}

// ------------------------ Tools --------------------------------------

Sample Tools::ImportRawSample(const String &fileName)
{
  std::vector<String> separatorsList{" ", ",", ";"};
  std::vector<String> numSepList{".", ","};

  Sample sampleFromFile;
  Sample testSample;

  UnsignedInteger maxNumberOfElements = 0;

  for (const String &numSep : numSepList)
  {
    for (const String &separator : separatorsList)
    {
      if (separator == numSep)
        continue;
      try
      {
        testSample = Sample::ImportFromTextFile(fileName, separator, 0, numSep);
      }
      catch (const InvalidArgumentException &)
      {
        // wrong separator
        testSample.clear();
      }
      // Select num/col separator pair leading to the largest sample
      if (testSample.getSize() * testSample.getDimension() > maxNumberOfElements)
      {
        maxNumberOfElements = testSample.getSize() * testSample.getDimension();
        sampleFromFile = testSample;
      }
    }
  }

  if (!sampleFromFile.getSize())
    throw InvalidArgumentException(HERE) << "The file does not contain a sample and/or the file encoding is not valid (use utf-8)";

  // deduplicate identifiers
  std::map<String, int, std::less<>> occurrences;
  Description description(sampleFromFile.getDescription());
  for (UnsignedInteger i = 0; i < description.getSize(); ++ i)
  {
    std::map<String, int, std::less<>>::iterator it = occurrences.find(description[i]);
    if (it == occurrences.end())
      occurrences[description[i]] = 1;
    else
    {
      ++ occurrences[description[i]];
      description[i] = (OSS() << description[i] << "_" << occurrences[description[i]]);
    }
  }
  sampleFromFile.setDescription(description);

  return sampleFromFile;
}

Sample Tools::transposeSample(const Sample &sample)
{
  Sample transposedSample(sample.getDimension(), sample.getSize());

  for(UnsignedInteger j=0; j<sample.getDimension(); ++j)
    for(UnsignedInteger i=0; i<sample.getSize(); ++i)
      transposedSample(j,i) = sample(i,j);
  
  return transposedSample;
}

Sample Tools::ImportSample(const String& fileName, const DataOrder order)
{
  const Sample sampleFromFile{ImportRawSample(fileName)};

  switch (order)
  {
    case Columns:
      return sampleFromFile;
    case Rows:
      return transposeSample(sampleFromFile);
    default:
      throw InvalidArgumentException(HERE) << "Unknown data order";
  }
}

Sample Tools::ImportMesh(const String &filename)
{
  const Sample sampleFromFile{ImportRawSample(filename)};

  if (sampleFromFile.getSize() > 1 && sampleFromFile.getDimension() > 1)
    throw InvalidArgumentException(HERE) << "The file contains more than one row or column. It must only contain the mesh.";

  if (sampleFromFile.getDimension() > sampleFromFile.getSize())
    return transposeSample(sampleFromFile);
  
  return sampleFromFile;
}


void Tools::ComputeBounds(const InputCollection& inputs, Point& startingPoint, Interval& bounds)
{
  const UnsignedInteger nbInputs = inputs.getSize();
  startingPoint = Point(nbInputs);
  bounds = Interval(nbInputs);

  Point lowerBounds(nbInputs);
  Point upperBounds(nbInputs);

  for (UnsignedInteger i = 0; i < nbInputs; ++i)
  {
    startingPoint[i] = inputs[i].getValue();

    if (!inputs[i].isStochastic())
    {
      lowerBounds[i] = -0.1;
      upperBounds[i] = 0.1;
      if (startingPoint[i] != 0)
      {
        lowerBounds[i] = startingPoint[i] - 0.1 * std::abs(startingPoint[i]);
        upperBounds[i] = startingPoint[i] + 0.1 * std::abs(startingPoint[i]);
      }
    }
    else
    {
      const Distribution distribution = inputs[i].getDistribution();
      // lower bound
      if (distribution.getRange().getFiniteLowerBound()[0])
        lowerBounds[i] = distribution.getRange().getLowerBound()[0];
      else
        lowerBounds[i] = distribution.computeQuantile(0.05)[0];
      // upper bound
      if (distribution.getRange().getFiniteUpperBound()[0])
        upperBounds[i] = distribution.getRange().getUpperBound()[0];
      else
        upperBounds[i] = distribution.computeQuantile(0.95)[0];

      // check if the interval contains the starting point
      if (!Interval(lowerBounds[i], upperBounds[i]).contains(Point(1, startingPoint[i])))
        startingPoint[i] = (upperBounds[i] + lowerBounds[i]) * 0.5;
    }
  }
  bounds.setLowerBound(lowerBounds);
  bounds.setUpperBound(upperBounds);
}

String Tools::GetNormalizedVariable(const String& variable)
{
  return std::regex_replace(variable, std::regex("[^0-9a-zA-Z_]"), "_");
}

Description Tools::GetNormalizedVariables(const Description& variables)
{
  Description normVariables(variables.getSize());
  for (UnsignedInteger i = 0; i < variables.getSize(); ++ i)
  {
    normVariables[i] = GetNormalizedVariable(variables[i]);
  }
  return normVariables;
}

/* Method EscapeHTML() escapes HTML special characters in a string using regex */
String Tools::EscapeHTML(const String &text)
{
  String result = text;
  result = std::regex_replace(result, std::regex("&"), "&amp;");
  result = std::regex_replace(result, std::regex("<"), "&lt;");
  result = std::regex_replace(result, std::regex(">"), "&gt;");
  result = std::regex_replace(result, std::regex("\""), "&quot;");
  result = std::regex_replace(result, std::regex("'"), "&#39;");
  return result;
}

// TimeCriteria methods

void TimeCriteria::setStartTime(const Scalar startTime)
{
  startTime_ = startTime;
}
Scalar TimeCriteria::getStartTime() const
{
  return startTime_;
}
void TimeCriteria::setMaxElapsedTime(const Scalar seconds)
{
  maximumElapsedTime_ = seconds;
}

void TimeCriteria::incrementElapsedTime()
{
  elapsedTime_ = Now() - startTime_;
}
Scalar TimeCriteria::getElapsedTime() const
{
  return elapsedTime_;
}
Scalar TimeCriteria::Now()
{
  std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
  return 1e-3 * std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}
}
