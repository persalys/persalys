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
#ifndef PERSALYS_BASETOOLS_HXX
#define PERSALYS_BASETOOLS_HXX

#include "Input.hxx"

#include <openturns/OTType.hxx>
#include <openturns/ProcessSample.hxx>
#include <openturns/CovarianceModel.hxx>
#include <regex>
#include <thread>
#include <algorithm>

namespace PERSALYS
{

#ifndef SWIG

// class to build a collection of parameters
class PERSALYS_BASE_API Parameters
{
public:
  Parameters();
  virtual ~Parameters();

  void add(const OT::String& name, const OT::String& value);
  void add(const OT::String& name, const double& value);
  void add(const OT::String& name, const OT::UnsignedInteger& value);
  void add(const OT::String& name, const OT::Point& values);
  void add(const Parameters& parameters);
  OT::String get(const OT::String & name) const;

  OT::UnsignedInteger getSize() const;
  std::pair<OT::String, OT::String> operator[](const OT::UnsignedInteger index) const;
  bool operator==(const Parameters& other) const;
  static OT::String GetOTSampleStr(const OT::Sample& values);
  static OT::String GetOTSampleCollectionStr(const OT::ProcessSample& processSample);
  static OT::String GetOTPointStr(const OT::Point& values, const OT::String& separator = ", ", const bool useBrackets = true);
  static OT::String GetOTPointPythonStr(const OT::Point& values);
  static OT::String GetOTPointWithDescriptionStr(const OT::PointWithDescription& values);
  static OT::String GetOTDescriptionStr(const OT::Description& values, const bool quote = true, const bool useBrackets = true);
  static OT::String GetOTIndicesStr(const OT::Indices& values);
  static OT::String GetOTBoolCollectionStr(const OT::Interval::BoolCollection& values);
  static OT::String GetOTCorrelationMatrixStr(const OT::CorrelationMatrix &correlationMatrix);
  static OT::String GetOTNormalCopulaStr(const OT::Distribution &distribution);
  static OT::Description GetOTIntervalDescription(const OT::Interval& interval);
  static OT::String GetOTCovModelCollectionStr(const OT::Collection<OT::CovarianceModel>& covarianceModels);
  static OT::String GetOTBoolStr(const OT::Bool value);

private:
  template <typename T>
  static OT::String GetStr(const T& values, const OT::String &separator, const bool useBrackets, const bool quote)
  {
    if (values.isEmpty())
      return useBrackets ? "[]" : "";

    OT::OSS oss;
    if (useBrackets)
      oss << "[";
    
    const OT::String quoteStr(quote ? "'" : "");
    OT::UnsignedInteger lastIndex = values.getSize() - 1u;
    for (OT::UnsignedInteger i = 0 ; i < lastIndex ; ++i)
    {
      oss << quoteStr << values[i] << quoteStr << separator;
    }
    oss << quoteStr << values[lastIndex] << quoteStr;
  
    if (useBrackets)
      oss << "]";
    
    return oss.str();
  }

private:
  OT::Collection<std::pair<OT::String, OT::String> > pairsCollection_;
};

#endif // SWIG

class PERSALYS_BASE_API Tools
{
public:
  enum DataOrder {Columns, Rows, Unknown};

#ifndef SWIG
  static OT::Sample ImportSample(const OT::String& fileName, const DataOrder order=Columns);
  static OT::Sample ImportMesh(const OT::String &filename);
  static void ComputeBounds(const InputCollection& inputs, OT::Point& startingPoint, OT::Interval& bounds);
  static OT::String GetNormalizedVariable(const OT::String& variable);
  static OT::Description GetNormalizedVariables(const OT::Description& variables);
  static OT::Sample transposeSample(const OT::Sample &sample);

  /** Method EscapeHTML() escapes HTML special characters in a string */
  static OT::String EscapeHTML(const OT::String &text);

private:
  static OT::Sample ImportRawSample(const OT::String &filename);
#endif // SWIG
};

#ifndef SWIG
class PERSALYS_BASE_API TimeCriteria
{
  friend class WithStopCriteriaAnalysis;

public:
  TimeCriteria() : startTime_(Now()) {};
  virtual ~TimeCriteria() {};
  void setStartTime(const OT::Scalar startTime);
  OT::Scalar getStartTime() const;
  void setMaxElapsedTime(const OT::Scalar seconds);
  void incrementElapsedTime();
  OT::Scalar getElapsedTime() const;
  /** System time in seconds */
  static OT::Scalar Now();

private:
  OT::Scalar startTime_;
  mutable OT::Scalar elapsedTime_ = 0.0;
  OT::Scalar maximumElapsedTime_ = 0.0;
};

inline OT::Description EscapeNewLines(const OT::Description & desc)
{
  OT::Description escapedDesc;
  for(OT::UnsignedInteger i = 0; i < desc.getSize(); ++i)
    escapedDesc.add(std::regex_replace(desc[i], std::regex("(?:\\r\\n|\\n|\\r)"), "\\n"));
  return escapedDesc;
}

inline void ValidateVariables(const OT::Description & desc)
{
  std::regex variable("([_a-zA-Z][_a-zA-Z0-9]*)");
  std::smatch what;
  for (OT::UnsignedInteger i = 0; i < desc.getSize(); ++ i)
  {
    if (!std::regex_match(desc[i], what, variable))
      throw OT::InvalidArgumentException(HERE) << "Invalid variable name";
  }
}

inline OT::UnsignedInteger GetNumberOfPhysicalCores()
{
  return std::max(std::thread::hardware_concurrency()/2, 1u);
}

template <typename TIObject>
bool hasMoreThanOneObjectWithName(const OT::Collection<TIObject> &coll, const OT::String &name)
{
  return std::count_if(coll.begin(), coll.end(),
                       [&](const TIObject& obj){ return obj.getName() == name; }) >= 2u;
}

#endif // SWIG
} // namespace PERSALY

#endif // PERSALYS_BASETOOLS_HXX
