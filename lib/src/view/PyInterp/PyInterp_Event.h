// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  File   : PyInterp_Event.h
//  Author : Sergey Anikin (OPEN CASCADE S.A.S.), Adrien Bruneton (CEA/DEN)

#ifndef PYINTERP_EVENT_H
#define PYINTERP_EVENT_H

#include "PyInterp.h"

#include <QEvent>

class PyInterp_Request;

class PYINTERP_EXPORT PyInterp_Event : public QEvent
{
  PyInterp_Event();
  PyInterp_Event( const PyInterp_Event& );

public:
  // Execution state
  enum {
    ES_NOTIFY = QEvent::User + 5000,
    ES_OK,
    ES_ERROR,
    ES_INCOMPLETE,
    ES_LAST };

  PyInterp_Event( int type, PyInterp_Request* request );
  virtual ~PyInterp_Event();

  PyInterp_Request* GetRequest() const;
  operator PyInterp_Request*() const;

private:
  PyInterp_Request* myRequest;
};

#endif // PYINTERP_EVENT_H
