//                                               -*- C++ -*-
/**
 *  @brief Class for imported sample
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
#ifndef OTGUI_IMPORTEDSAMPLE_HXX
#define OTGUI_IMPORTEDSAMPLE_HXX

#include "otgui/OTGuiprivate.hxx"
#include "openturns/OTType.hxx"

namespace OTGUI {
class OTGUI_API ImportedSample
{
public:
  /** Default constructor */
  ImportedSample();
  /** Constructor with parameters */
  ImportedSample(const OT::String & fileName, const OT::Indices & inputColumns);

  virtual ~ImportedSample();

  virtual void setFileName(const OT::String & fileName);
  OT::String getFileName() const;
  OT::Indices getInputColumns() const;
  void setInputColumns(const OT::Indices & inputColumns);

  static OT::NumericalSample ImportSample(const OT::String & fileName);

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  OT::NumericalSample sampleFromFile_;

private:
  OT::String fileName_;
  OT::Indices inputColumns_;
};
}
#endif