//                                               -*- C++ -*-
/**
 *  @brief YACSEvalSessionSingleton allocates an YACSEvalSession
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
#include "otgui/YACSEvalSessionSingleton.hxx"
#include <Exception.hxx>

using namespace OT;

namespace OTGUI {

YACSEvalSession * YACSEvalSessionSingleton::session_ = 0;

/* Default constructor */
YACSEvalSessionSingleton::YACSEvalSessionSingleton()
{
  if (session_)
    throw InternalException(HERE) << "A YACS session already exists";
  session_ = new YACSEvalSession;
}


/* Destructor */
YACSEvalSessionSingleton::~YACSEvalSessionSingleton()
{
  delete session_;
  session_ = 0;
}


YACSEvalSession * YACSEvalSessionSingleton::Get()
{
  if (!session_)
    throw InternalException(HERE) << "No YACS session";
  if (!session_->isLaunched())
    session_->launch();
  return session_;
}
}
