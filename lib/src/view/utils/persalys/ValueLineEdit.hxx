//                                               -*- C++ -*-
/**
 *  @brief QLineEdit with double value
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_VALUELINEEDIT_HXX
#define PERSALYS_VALUELINEEDIT_HXX

#include <QLineEdit>
#include <QKeyEvent>
#include "persalys/PersalysPrivate.hxx"
#include <openturns/OTType.hxx>

namespace PERSALYS
{
class PERSALYS_UTILS_API ValueLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  // value constructor
  ValueLineEdit(const double value = 0., QWidget *parent = nullptr);

  // value accessors
  void setValue(const double value, const bool enabled = true);
  double value();

  // check value
  void check();

  // clear and make not editable
  void deactivate();

protected:
  virtual bool event(QEvent * event);
  virtual void keyPressEvent(QKeyEvent *e);
};

class PERSALYS_UTILS_API ValuesLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  // value constructor
  ValuesLineEdit(const OT::Point values = OT::Point(), QWidget *parent = nullptr);

  // value accessors
  void setValues(const OT::Point values, const bool enabled = true);
  OT::Point values();

//protected:
//  virtual bool event(QEvent * event);
//  virtual void keyPressEvent(QKeyEvent *e);
};
}
#endif
