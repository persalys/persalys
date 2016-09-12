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
// File   : PyInterp.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#if !defined ( PYINTERP_H )
#define PYINTERP_H

// ========================================================
// set dllexport type for Win platform 
#ifdef WIN32
# if defined PYINTERP_EXPORTS || defined PyInterp_EXPORTS
#  define PYINTERP_EXPORT __declspec(dllexport)
# else
#  define PYINTERP_EXPORT __declspec(dllimport)
# endif
#else   // WIN32
# define PYINTERP_EXPORT
#endif  // WIN32

// ========================================================

#include <Python.h>

// avoid warning messages
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4251)
#endif

#endif // PYINTERP_H
