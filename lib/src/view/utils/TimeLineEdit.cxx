//                                               -*- C++ -*-
/**
 *  @brief QLineEdit for time
 *
 *  Copyright 2015-2021 EDF-Phimeca
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

#include "persalys/TimeLineEdit.hxx"
#include <QtGlobal>
#include <QRegularExpression>

namespace PERSALYS
{

TimeLineEdit::TimeLineEdit(const QString text, QWidget *parent)
  : QLineEdit(parent)
{
  setInputMask("999\\d\\ 99\\h\\ 99\\m\\ 99\\s");
  setText(text);
}


TimeLineEdit::TimeLineEdit(const unsigned int seconds, QWidget *parent)
  : QLineEdit(parent)
{
  setInputMask("999\\d\\ 99\\h\\ 99\\m\\ 99\\s");
  setSeconds(seconds);
}


int TimeLineEdit::getSeconds() const
{
  QString s = text();
  QStringList list = s.split(QRegularExpression("d|h|m|s"));

  int seconds = 0;
#if QT_VERSION >= QT_VERSION_CHECK(5,15,0)
  seconds += list[0].split("", Qt::SkipEmptyParts).join("").trimmed().toInt() * 24 * 3600;
#else
  seconds += list[0].split("", QString::SkipEmptyParts).join("").trimmed().toInt() * 24 * 3600;
#endif
  seconds += list[1].trimmed().toInt() * 3600;
  seconds += list[2].trimmed().toInt() * 60;
  seconds += list[3].trimmed().toInt();

  return seconds;
}


void TimeLineEdit::setSeconds(const int seconds)
{
  // convert seconds
  int sec = seconds % 60;
  int minutes = seconds / 60;
  int hours = minutes / 60;
  minutes %= 60;
  int days = std::min(hours / 24, 999); // the max days is 999
  hours %= 24;

  // set text: XXX d XX h XX m XX s
  QString duration;
  if (days) duration += QString::number(days);
  duration += "d";
  if (hours) duration += QString::number(hours);
  duration += "h";
  if (minutes) duration += QString::number(minutes);
  duration += "m";
  if (sec) duration += QString::number(sec);
  duration += "s";

  setText(duration);
}
}
