//                                               -*- C++ -*-
/**
 *  @brief Class to import file data
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
#ifndef PERSALYS_DATAIMPORT_HXX
#define PERSALYS_DATAIMPORT_HXX

#include "persalys/PersalysPrivate.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/Sample.hxx>

namespace PERSALYS
{
class PERSALYS_BASE_API DataImport
{
public:

  /** Default constructor */
  DataImport();
  /** Constructor with parameters */
  DataImport(const OT::String & fileName,
             const OT::Indices & inputColumns,
             const OT::Indices & outputColumns = OT::Indices());

  virtual ~DataImport();

  OT::String getFileName() const;
  void setFileName(const OT::String& fileName,
                   const Tools::DataOrder order = Tools::DataOrder::Columns);

  OT::Indices getInputColumns() const;
  OT::Indices getOutputColumns() const;

  OT::Sample getSampleFromFile() const;

  /** String converter */
  virtual OT::String __repr__() const;

  /** Method save() stores the object through the StorageManager */
  void save(OT::Advocate & adv) const;

  /** Method load() reloads the object from the StorageManager */
  void load(OT::Advocate & adv);

protected:
  virtual void setColumns(const OT::Indices & inputColumns, const OT::Indices & outputColumns);
  virtual OT::Sample importSample(const OT::String& fileName);
  virtual OT::Sample importSample(const OT::String& fileName,
                                  const Tools::DataOrder order);
  virtual void check();
  virtual void setDefaultColumns();

protected:
  OT::String fileName_;
  OT::Indices outputColumns_;
  OT::Indices inputColumns_;
  OT::Sample sampleFromFile_;
};
}
#endif
