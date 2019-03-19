//                                               -*- C++ -*-
/**
 *  @brief QSpinBox with logarithmic increment
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "otgui/LogSpinBox.hxx"

namespace OTGUI
{

LogSpinBox::LogSpinBox(int base, QWidget * parent)
  : UIntSpinBox(parent)
  , base_(base)
{
}


// switch button accessor
void LogSpinBox::setBase(int base)
{
  base_ = base;
}


int LogSpinBox::base() const
{
  return base_;
}


void LogSpinBox::stepBy(int steps)
{
  if (steps > 0)
    setValue(base_ * value());
  else
    setValue(value() / base_);
}
}