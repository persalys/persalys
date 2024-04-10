//                                               -*- C++ -*-
/**
 *  @brief QGraphicsLineItem to draw arrows
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
#ifndef PERSALYS_ARROW_HXX
#define PERSALYS_ARROW_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QGraphicsLineItem>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

namespace PERSALYS
{
class PERSALYS_UTILS_API Arrow : public QObject, public QGraphicsLineItem
{
  Q_OBJECT

public:
  enum { Type = UserType + 4 };

  Arrow(QPointF startP, QPointF endP, QGraphicsItem* parent = 0);

  virtual int type() const;
  virtual QRectF boundingRect() const;
  virtual QPainterPath shape() const;
  void setColor(const QColor &color);

protected:
  virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem* option, QWidget* widget = 0);

public slots:
  void setValidity(bool);

private:
  QPointF startP_;
  QPointF endP_;
  QColor color_;
  QPolygonF arrowHead_;
};
}
#endif