//                                               -*- C++ -*-
/**
 *  @brief QLineEdit for time
 *
 *  Copyright 2015-2024 EDF-Phimeca
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
#ifndef PERSALYS_TIMELINEEDIT_HXX
#define PERSALYS_TIMELINEEDIT_HXX

#include <QLineEdit>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{
class PERSALYS_UTILS_API TimeLineEdit : public QLineEdit
{
  Q_OBJECT

public:
  // constructor with text
  TimeLineEdit(const QString & text, QWidget *parent = nullptr);

  // constructor with seconds
  TimeLineEdit(const unsigned int seconds = 0, QWidget *parent = nullptr);

  int getSeconds() const;
  void setSeconds(const int seconds);
};
}
#endif
