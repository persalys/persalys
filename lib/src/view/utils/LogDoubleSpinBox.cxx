//                                               -*- C++ -*-
/**
 *  @brief DoubleSpinBox with logarithmic increment
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
#include "otgui/LogDoubleSpinBox.hxx"

#include <QMenu>
#include <QLineEdit>

#include <limits>
#include <cmath>

namespace OTGUI
{

LogDoubleSpinBox::LogDoubleSpinBox(QWidget * parent)
  : DoubleSpinBox(parent)
{
  setSingleStep(0.1);
}


void LogDoubleSpinBox::stepBy(int steps)
{
  if (fabs(value()) > std::numeric_limits<double>::min())
  {
    setValue(value() * pow(singleStep(), static_cast<double>(-steps)));
  }
  else
  {
    DoubleSpinBox::stepBy(steps);
  }
}


/* overwrite QAbstractSpinBox::contextMenuEvent to add "Apply to all" in the actions list

   in QAbstractSpinBox::contextMenuEvent the "Select all" action of
   QLineEdit is overridden by a "Select all" action of QSpinBox.
   The selectAll method of QSpinBox Selects all the text
   in the spinbox except the prefix and suffix.
   In OTGui there is no spinbox with prefix or suffix
   So we do not this operation here
*/
void LogDoubleSpinBox::contextMenuEvent(QContextMenuEvent* event)
{
  if (!applyToAllInContextMenu_)
  {
    DoubleSpinBox::contextMenuEvent(event);
    return;
  }

  QMenu * menu = lineEdit()->createStandardContextMenu();
  if (!menu)
    return;

  menu->addSeparator();

  const uint se = stepEnabled();
  QAction * up = menu->addAction(tr("&Step up"));
  up->setEnabled(se & StepUpEnabled);
  QAction * down = menu->addAction(tr("Step &down"));
  down->setEnabled(se & StepDownEnabled);

  menu->addSeparator();

  QAction * applyAction = menu->addAction(tr("Apply to all"));

  menu->addSeparator();

  const QPoint pos = (event->reason() == QContextMenuEvent::Mouse) ? event->globalPos() : mapToGlobal(QPoint(event->pos().x(), 0)) + QPoint(width() / 2, height() / 2);

  const QAction * action = menu->exec(pos);

  delete static_cast<QMenu *>(menu);

  if (action)
  {
    if (action == up)
    {
      stepBy(1);
    }
    else if (action == down)
    {
      stepBy(-1);
    }
    else if (action == applyAction)
    {
      emit applyToAllRequested(value());
    }
  }
  event->accept();
}


void LogDoubleSpinBox::setApplyToAllInContextMenu(const bool apply)
{
  applyToAllInContextMenu_ = apply;
}
}