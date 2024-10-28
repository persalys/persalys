//                                               -*- C++ -*-
/**
 *  @brief Custom ResourceMap keys
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

#include "persalys/PersalysResourceMap.hxx"
#include "persalys/BaseTools.hxx"

#include <openturns/ResourceMap.hxx>
#include <mutex>

using namespace OT;

namespace PERSALYS
{

  PersalysResourceMap_init::PersalysResourceMap_init()
  {
    static std::once_flag flag;
    std::call_once(flag, [&]()
    {
#ifdef _WIN32
      // ProcessPoolExecutor startup penalty is much higher
      OT::ResourceMap::AddAsUnsignedInteger("PythonPhysicalModel-DefaultProcessNumber", 1);
#else
      OT::ResourceMap::AddAsUnsignedInteger("PythonPhysicalModel-DefaultProcessNumber", GetNumberOfPhysicalCores());
#endif
    });
  }

}
