//                                               -*- C++ -*-
/**
 *  @brief QSpinBox QSpinBox for unsigned integer with scientific notation allowed
 *
 *  Copyright 2015-2017 EDF-Phimeca
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

namespace OTGUI {

UIntSpinBox::UIntSpinBox(QWidget* parent)
  : DoubleSpinBox(parent)
{
  setDecimals(0);
  lineEdit()->setValidator(new QRegExpValidator(QRegExp("[0-9]+[eE][+]?[0-9]+")));
}
}