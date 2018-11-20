//                                               -*- C++ -*-
/**
 *  @brief OT tools
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_BASETOOLS_HXX
#define OTGUI_BASETOOLS_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/OTType.hxx>

namespace OTGUI
{
// class to build a collection of parameters
class OTGUI_API Parameters
{
public:
  Parameters();
  virtual ~Parameters();

  void add(const OT::String& name, const OT::String& value);
  void add(const OT::String& name, const double& value);
  void add(const OT::String& name, const OT::UnsignedInteger& value);
  void add(const OT::String& name, const OT::Point& values);
  OT::UnsignedInteger getSize() const;
  std::pair<OT::String, OT::String> operator[](const OT::UnsignedInteger index) const;
  static OT::String GetOTSampleStr(const OT::Sample& values);
  static OT::String GetOTPointStr(const OT::Point& values, const OT::String& separator = ",");
  static OT::String GetOTDescriptionStr(const OT::Description& values, const bool quote = true);
  static OT::String GetOTBoolCollectionStr(const OT::Interval::BoolCollection& values);
  static OT::Description GetOTIntervalDescription(const OT::Interval& interval);

private:
  OT::Collection<std::pair<OT::String, OT::String> > pairsCollection_;
};

class OTGUI_API Tools
{
public:
  static OT::String GetLocaleString(const OT::String& str);
};
}
#endif
