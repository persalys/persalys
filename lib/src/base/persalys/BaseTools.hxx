//                                               -*- C++ -*-
/**
 *  @brief base tools
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include <regex>

namespace PERSALYS
{
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
  static OT::String GetOTPointStr(const OT::Point& values, const OT::String& separator = ",");
  static OT::String GetOTDescriptionStr(const OT::Description& values, const bool quote = true);
  static OT::String GetOTIndicesStr(const OT::Indices& values);
  static OT::String GetOTBoolCollectionStr(const OT::Interval::BoolCollection& values);
  static OT::String GetOTCorrelationMatrixStr(const OT::CorrelationMatrix &correlationMatrix);
  static OT::String GetOTNormalCopulaStr(const OT::Distribution &distribution);
  static OT::Description GetOTIntervalDescription(const OT::Interval& interval);

private:
  OT::Collection<std::pair<OT::String, OT::String> > pairsCollection_;
};

class PERSALYS_BASE_API Tools
{
public:
  static OT::Sample ImportSample(const OT::String& fileName);
  static OT::String GetLocaleString(const OT::String& str);
  static int IsUTF8(const char *data, size_t size);
  static void ComputeBounds(const InputCollection& inputs, OT::Point& startingPoint, OT::Interval& bounds);
};


class PERSALYS_BASE_API TimeCriteria
{
  friend class WithStopCriteriaAnalysis;

public:
  TimeCriteria() : startTime_(Now()), elapsedTime_(0.), maximumElapsedTime_(0.), stopRequested_(false)
  {};
  virtual ~TimeCriteria() {};
  void setStartTime(const OT::Scalar startTime);
  OT::Scalar getStartTime() const;
  void setMaxElapsedTime(const OT::Scalar seconds);
  void stop();
  void incrementElapsedTime();
  OT::Scalar getElapsedTime() const;
  /** System time in seconds */
  static OT::Scalar Now();

private:
  OT::Scalar startTime_;
  mutable OT::Scalar elapsedTime_;
  OT::Scalar maximumElapsedTime_;
  bool stopRequested_;
};

inline OT::String EscapePath(const OT::FileName & filename)
{
  OT::FileName escapedPath = std::regex_replace(filename, std::regex("\\\\"), "\\\\\\\\");
  return escapedPath;
}

inline OT::String EscapeQuotes(OT::String code)
{
  return std::regex_replace(code, std::regex("\'"), "\\\'");
}

inline OT::Description EscapeSpecialCharacters(const OT::Description & desc)
{
  OT::Description escapedDesc;
  for(OT::UnsignedInteger i = 0; i < desc.getSize(); ++i)
    escapedDesc.add(std::regex_replace(desc[i], std::regex("[^0-9a-zA-Z]"), "\\$&"));
  return escapedDesc;
}

inline void ValidateVariables(const OT::Description & desc) {
  std::regex variable("([_a-zA-Z][_a-zA-Z0-9]*)");
  std::smatch what;
  for (OT::UnsignedInteger i=0; i < desc.getSize(); ++ i) {
    if (!std::regex_match(desc[i], what, variable))
      throw OT::InvalidArgumentException(HERE) << "Invalid variable name";
  }
}

}
#endif
