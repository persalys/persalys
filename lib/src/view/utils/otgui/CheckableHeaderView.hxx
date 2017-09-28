//                                               -*- C++ -*-
/**
 *  @brief QHeaderView with a checkable first column
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
#ifndef OTGUI_CHECKABLEHEADERVIEW_HXX
#define OTGUI_CHECKABLEHEADERVIEW_HXX

#include "otgui/OTGuiprivate.hxx"

#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>

namespace OTGUI
{
class OTGUI_API CheckableHeaderView : public QHeaderView
{
  Q_OBJECT

public:
  CheckableHeaderView(QWidget* parent = 0);

  virtual void setModel(QAbstractItemModel* model);

protected:
  int getMinimumSectionSize() const;
  virtual void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;
  virtual void mousePressEvent(QMouseEvent *event);

public slots:
  void setChecked(bool checked);

private:
  bool isChecked_;
};
}
#endif