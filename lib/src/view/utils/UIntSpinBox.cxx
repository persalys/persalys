//                                               -*- C++ -*-
/**
 *  @brief QSpinBox for unsigned integer with scientific notation allowed
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/UIntSpinBox.hxx"

#include <QLineEdit>

#include <limits>

namespace OTGUI
{

UIntSpinBox::UIntSpinBox(QWidget* parent)
  : DoubleSpinBox(parent)
{
  setMinimum(1);
  setMaximum(std::numeric_limits<int>::max());
  setSingleStep(1);
  // can not write a "-" or a "." : allow numbers in [1, max]
  lineEdit()->setValidator(new QRegExpValidator(QRegExp("[0-9]+[eE][+]?[0-9]+"), this));
}
}
