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

#include "otgui/ValueLineEdit.hxx"

#include "otgui/OTguiException.hxx"

namespace OTGUI {

ValueLineEdit::ValueLineEdit(const QString text, QWidget *parent)
  : QLineEdit(parent)
{
  setText(text);
}

ValueLineEdit::ValueLineEdit(const double value, QWidget *parent)
  : QLineEdit(parent)
{
  setValue(value);
}


void ValueLineEdit::setValue(const double value)
{
  setText(QString::number(value));
  //TODO setText(QString::number(value, 'g', precision_));
  setEnabled(true);
}


double ValueLineEdit::value()
{
  check();
  return text().toDouble();
}


void ValueLineEdit::check()
{
  QString value = text();
  if (value[0].toLower() == 'e' || value.isEmpty() || value.toLower() == "e" || value == "-")
    throw InvalidValueException(HERE) << "The value '" << value.toStdString() << "' is invalid";
  bool ok;
  value.toDouble(&ok);
  if (!ok)
    throw InvalidValueException(HERE) << "The value '" << value.toStdString() << "' is invalid";
}


void ValueLineEdit::deactivate()
{
  clear();
  setEnabled(false);
}
}