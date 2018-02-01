//                                               -*- C++ -*-
/**
 *  @brief QGraphicsLineItem to draw arrows
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/Arrow.hxx"

#include <math.h>

namespace OTGUI
{

Arrow::Arrow(QPointF startP, QPointF endP, QGraphicsItem *parent)
  : QObject()
  , QGraphicsLineItem(parent)
  , startP_(startP)
  , endP_(endP)
  , color_(Qt::red)
  , arrowHead_()
{
  setFlag(QGraphicsItem::ItemIsSelectable, true);
  setPen(QPen(color_, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
}


int Arrow::type() const
{
  return Type;
}


QRectF Arrow::boundingRect() const
{
  const qreal extra = (pen().width() + 20) / 2.0;

  return QRectF(line().p1(), QSizeF(line().p2().x() - line().p1().x(),
                                    line().p2().y() - line().p1().y())).normalized().adjusted(-extra, -extra, extra, extra);
}


QPainterPath Arrow::shape() const
{
  QPainterPath path = QGraphicsLineItem::shape();
  path.addPolygon(arrowHead_);
  return path;
}


void Arrow::setColor(const QColor& color)
{
  color_ = color;
}


void Arrow::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
  // color
  QPen myPen = pen();
  myPen.setColor(color_);

  painter->setPen(myPen);
  painter->setBrush(color_);

  // line
  if (startP_.y() != endP_.y())
  {
    QPointF middlePoint(startP_.x(), endP_.y());
    painter->drawLine(QLineF(middlePoint, startP_));
    setLine(QLineF(endP_, middlePoint));
  }
  else
  {
    setLine(QLineF(endP_, startP_));
  }
  painter->drawLine(line());

  if (color_ == Qt::red)
  {
    QImage image(":/images/user-busy.png");
    QPointF imagePoint((endP_.x() + startP_.x()) / 2 - image.width() / 2, endP_.y() - image.height() / 2);
    painter->drawImage(imagePoint, image);
  }

  // arrow head
  const double Pi = 3.14;

  double angle = ::acos(line().dx() / line().length());
  if (line().dy() >= 0)
    angle = (Pi * 2) - angle;

  qreal arrowSize = 10;
  QPointF arrowP1 = line().p1() + QPointF(sin(angle + Pi / 3) * arrowSize,
                                          cos(angle + Pi / 3) * arrowSize);
  QPointF arrowP2 = line().p1() + QPointF(sin(angle + Pi - Pi / 3) * arrowSize,
                                          cos(angle + Pi - Pi / 3) * arrowSize);

  arrowHead_.clear();
  arrowHead_ << line().p1() << arrowP1 << arrowP2;

  painter->drawPolygon(arrowHead_);
}


void Arrow::setValidity(bool validity)
{
  setColor(validity ? Qt::black : Qt::red);
}
}