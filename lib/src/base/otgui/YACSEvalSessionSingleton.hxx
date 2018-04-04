//                                               -*- C++ -*-
/**
 *  @brief YACSEvalSessionSingleton allocates an YACSEvalSession
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
#ifndef OTGUI_YACSEVALSESSIONSINGLETON_HXX
#define OTGUI_YACSEVALSESSIONSINGLETON_HXX

#include "otgui/OTGuiprivate.hxx"
#include <YACSEvalSession.hxx>

namespace OTGUI
{
/**
 * @class YACSEvalSessionSingleton
 *
 * The class that allocates an YACSEvalSession.
 */
class OTGUI_API YACSEvalSessionSingleton
{

private:
  /** Default constructor */
  YACSEvalSessionSingleton();
  /** Destructor */
  virtual ~YACSEvalSessionSingleton();

public:
  static YACSEvalSession * Get();
  static void Reset();

private:
  YACSEvalSession * session_;
  static YACSEvalSessionSingleton *singleton_;
};
}
#endif
