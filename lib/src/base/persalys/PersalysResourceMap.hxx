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

#ifndef PERSALYS_RESOURCEMAP_HXX
#define PERSALYS_RESOURCEMAP_HXX

#include <openturns/Object.hxx>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{

  struct PERSALYS_BASE_API PersalysResourceMap_init
  {
    PersalysResourceMap_init();
  };

  static PersalysResourceMap_init __PersalysResourceMap_initializer;

}
#endif
