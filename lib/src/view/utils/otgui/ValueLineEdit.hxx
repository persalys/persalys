//                                               -*- C++ -*-
/**
 *  @brief QLineEdit with double value
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
#ifndef OTGUI_VALUELINEEDIT_HXX
#define OTGUI_VALUELINEEDIT_HXX

#include <QLineEdit>

namespace OTGUI {
class ValueLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  // text constructor
  ValueLineEdit(const QString text = "", QWidget *parent = 0);

  // value constructor
  ValueLineEdit(const double value, QWidget *parent = 0);

  // value accessors
  void setValue(const double value);
  double value();

  // check value
  void check();

  // clear and make not editable
  void deactivate();
};
}
#endif