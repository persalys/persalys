//                                               -*- C++ -*-
/**
 *  @brief QSpinBox with logarithmic increment
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_LOGSPINBOX_HXX
#define PERSALYS_LOGSPINBOX_HXX

#include "persalys/UIntSpinBox.hxx"

#include <QSpinBox>

namespace PERSALYS
{
/// This SpinBox has a logarithmic increment : e.g. value=base*value
/// when clicking on the up arrow
/// by default base=10
/// also the default range is (1, max int)
class PERSALYS_UTILS_API LogSpinBox : public UIntSpinBox
{
  Q_OBJECT

public :
  LogSpinBox(int base = 10, QWidget* parent = 0);

  /// base accessor
  void setBase(int base);
  int base() const;

  virtual void stepBy(int steps);

private:
  int base_;
};
}
#endif