//                                               -*- C++ -*-
/**
 *  @brief QwtScaleDraw with locale insensitive labels
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
#ifndef OTGUI_CUSTOMSCALEDRAW_HXX
#define OTGUI_CUSTOMSCALEDRAW_HXX

#include "otgui/OTGuiprivate.hxx"

#include <qwt_scale_draw.h>

namespace OTGUI {
class OTGUI_API CustomScaleDraw : public QwtScaleDraw
{
public:
  CustomScaleDraw();

  virtual QwtText label(double v) const;
};
}
#endif