//                                               -*- C++ -*-
/**
 *  @brief DoubleSpinBox with logarithmic increment
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_LOGDOUBLESPINBOX_HXX
#define OTGUI_LOGDOUBLESPINBOX_HXX

#include "otgui/DoubleSpinBox.hxx"

#include <QContextMenuEvent>

namespace OTGUI
{
class OTGUI_API LogDoubleSpinBox : public DoubleSpinBox
{
  Q_OBJECT

public:
  LogDoubleSpinBox(QWidget *parent = 0);

  virtual void stepBy(int steps);

  void setApplyToAllInContextMenu(const bool apply);

signals:
  void applyToAllRequested(double);

protected:
  virtual void contextMenuEvent(QContextMenuEvent * event);

private:
  bool applyToAllInContextMenu_;
};
}
#endif