//                                               -*- C++ -*-
/**
 *  @brief QLineEdit with double value
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

#include "persalys/ValueLineEdit.hxx"

#include "persalys/AppliException.hxx"
#include "persalys/StudyTreeViewModel.hxx"

#include <QDoubleValidator>

namespace PERSALYS
{

ValueLineEdit::ValueLineEdit(const double value, QWidget *parent)
  : QLineEdit(parent)
{
  QDoubleValidator * validator = new QDoubleValidator(this);
  validator->setLocale(QLocale::C);
  setValidator(validator);
  setValue(value);
}


void ValueLineEdit::setValue(const double value, const bool enabled)
{
  setText(QString::number(value, 'g', StudyTreeViewModel::DefaultSignificantDigits));
  //TODO setText(QString::number(value, 'g', precision_));
  setEnabled(enabled);
}


double ValueLineEdit::value()
{
  check();
  return text().toDouble();
}


void ValueLineEdit::check()
{
  QString value = text();
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


bool ValueLineEdit::event(QEvent * event)
{
  QString value = text();
  int pos = 0;
  // if 1e400 -> correct syntax -> but not a valid double -> not accepted -> no editingFinished signal
  if (event->type() == QEvent::FocusOut && validator()->validate(value, pos) != QValidator::Acceptable)
    emit editingFinished();

  return QLineEdit::event(event);
}


void ValueLineEdit::keyPressEvent(QKeyEvent *e)
{
  QLineEdit::keyPressEvent(e);
  QString value = text();
  int pos = 0;
  // if 1e400 -> not a valid double -> but not accepted -> no editingFinished signal
  if (e->key() == Qt::Key_Return && validator()->validate(value, pos) != QValidator::Acceptable)
    emit editingFinished();
}
}
