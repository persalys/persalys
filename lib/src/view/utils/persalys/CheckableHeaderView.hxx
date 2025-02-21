//                                               -*- C++ -*-
/**
 *  @brief QHeaderView with a checkable first column
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
#ifndef PERSALYS_CHECKABLEHEADERVIEW_HXX
#define PERSALYS_CHECKABLEHEADERVIEW_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QHeaderView>
#include <QPainter>
#include <QMouseEvent>

namespace PERSALYS
{
class PERSALYS_UTILS_API CheckableHeaderView : public QHeaderView
{
  Q_OBJECT

public:
  CheckableHeaderView(Qt::Orientation orientation = Qt::Horizontal, QWidget* parent = 0);

  virtual void setModel(QAbstractItemModel* model);
  bool isChecked() const;

protected:
  int getMinimumSectionSize() const;
  virtual void paintSection(QPainter* painter, const QRect& rect, int logicalIndex) const;
  virtual void mousePressEvent(QMouseEvent *event);

public slots:
  void updateCheckState(const Qt::Orientation = Qt::Horizontal);

private:
  bool isChecked_;
  bool modelSignalBlocked_;
};
}
#endif
