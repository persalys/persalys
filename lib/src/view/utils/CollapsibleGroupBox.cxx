/*
 * This file is part of the KDE project
 * Copyright (C) 2015 David Edmundson <davidedmundson@kde.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "persalys/CollapsibleGroupBox.hxx"

#include <QLayout>
#include <QStyle>
#include <QPainter>
#include <QStyleOption>
#include <QAction>
#include <QMouseEvent>

namespace PERSALYS
{

class CollapsibleGroupBoxPrivate
{
public:
  CollapsibleGroupBoxPrivate(CollapsibleGroupBox *q);
  void updateChildrenVisibility(bool visible);
  void recalculateHeaderSize();
  QSize contentSize() const;
  QSize contentMinimumSize() const;

  CollapsibleGroupBox *q;
  QString title;
  bool isExpanded;
  bool headerContainsMouse;
  QSize headerSize;
  int shortcutId;
};


CollapsibleGroupBoxPrivate::CollapsibleGroupBoxPrivate(CollapsibleGroupBox* q)
  : q(q)
  , isExpanded(false)
  , headerContainsMouse(false)
  , shortcutId(0)
{}


CollapsibleGroupBox::CollapsibleGroupBox(QWidget* parent)
  : QWidget(parent)
  , d(new CollapsibleGroupBoxPrivate(this))
{
  d->recalculateHeaderSize();
  setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
  setFocusPolicy(Qt::TabFocus);
  setMouseTracking(true);
}


CollapsibleGroupBox::~CollapsibleGroupBox()
{
  delete d;
}


void CollapsibleGroupBox::setTitle(const QString& title)
{
  d->title = title;
  d->recalculateHeaderSize();

  update();
  updateGeometry();

  if (d->shortcutId)
    releaseShortcut(d->shortcutId);

  d->shortcutId = grabShortcut(QKeySequence::mnemonic(title));

  setAccessibleName(title);

  emit titleChanged();
}


QString CollapsibleGroupBox::title() const
{
  return d->title;
}


void CollapsibleGroupBox::setExpanded(bool expanded)
{
  if (expanded == d->isExpanded)
  {
    return;
  }

  d->isExpanded = expanded;
  emit expandedChanged();

  d->updateChildrenVisibility(d->isExpanded);

  //when going from collapsed to expanded changing the child visibility calls an updateGeometry
  //which calls sizeHint with expanded true before the first frame of the animation kicks in
  //trigger an effective frame 0
  if (expanded)
    setFixedHeight(d->headerSize.height());
  else
    setFixedHeight(d->contentSize().height() + d->headerSize.height());
}


bool CollapsibleGroupBox::isExpanded() const
{
  return d->isExpanded;
}


void CollapsibleGroupBox::collapse()
{
  setExpanded(false);
}


void CollapsibleGroupBox::expand()
{
  setExpanded(true);
}


void CollapsibleGroupBox::toggle()
{
  setExpanded(!d->isExpanded);
}


void CollapsibleGroupBox::paintEvent(QPaintEvent *event)
{
  QPainter p(this);

  // bold the title
  QFont font(p.font());
  font.setBold(true);
  p.setFont(font);

  QStyleOptionButton baseOption;
  baseOption.initFrom(this);
  baseOption.rect = QRect(0, 0, width(), d->headerSize.height());
  baseOption.text = d->title;

  if (d->headerContainsMouse)
    baseOption.state |= QStyle::State_MouseOver;

  QStyle::PrimitiveElement element;
  if (d->isExpanded)
    element = QStyle::PE_IndicatorArrowDown;
  else
    element = isLeftToRight() ? QStyle::PE_IndicatorArrowRight : QStyle::PE_IndicatorArrowLeft;

  // -- label option
  QStyleOptionButton labelOption = baseOption;
  labelOption.rect = style()->subElementRect(QStyle::SE_CheckBoxContents, &labelOption, this);
  // move the label at x=0
  labelOption.rect.moveLeft(0);
  style()->drawControl(QStyle::CE_CheckBoxLabel, &labelOption, &p, this);

  // -- indicator option
  QStyleOptionButton indicatorOption = baseOption;
  indicatorOption.rect = style()->subElementRect(QStyle::SE_CheckBoxIndicator, &indicatorOption, this);

  // move the arrow after the text
  const int textWidth = p.boundingRect(labelOption.rect, 0, d->title).width();
  indicatorOption.rect.moveRight(textWidth + indicatorOption.rect.width()); // move arrow after text

  style()->drawPrimitive(element, &indicatorOption, &p, this);

  Q_UNUSED(event)
}


QSize CollapsibleGroupBox::sizeHint() const
{
  if (d->isExpanded)
    return d->contentSize() + QSize(0, d->headerSize.height());
  else
    return QSize(d->contentSize().width(), d->headerSize.height());
}


QSize CollapsibleGroupBox::minimumSizeHint() const
{
  int minimumWidth = qMax(d->contentSize().width(), d->headerSize.width());
  return QSize(minimumWidth, d->headerSize.height());
}


bool CollapsibleGroupBox::event(QEvent *event)
{
  switch (event->type())
  {
    case QEvent::StyleChange:
    /*fall through*/
    case QEvent::FontChange:
      d->recalculateHeaderSize();
      break;
    case QEvent::Shortcut:
    {
      QShortcutEvent *se = static_cast<QShortcutEvent*>(event);
      if(d->shortcutId == se->shortcutId())
      {
        toggle();
        return true;
      }
      break;
    }
    case QEvent::ChildAdded:
    {
      QChildEvent *ce = static_cast<QChildEvent*>(event);
      if (ce->child()->isWidgetType())
        static_cast<QWidget*>(ce->child())->setVisible(d->isExpanded);
      break;
    }
    case QEvent::LayoutRequest:
      setFixedHeight(sizeHint().height());
      break;
    default:
      break;
  }

  return QWidget::event(event);
}


void CollapsibleGroupBox::mousePressEvent(QMouseEvent *event)
{
  const QRect headerRect(0, 0, width(), d->headerSize.height());
  if (headerRect.contains(event->pos()))
    toggle();
  event->setAccepted(true);
}


//if mouse has changed whether it is in the top bar or not refresh to change arrow icon
void CollapsibleGroupBox::mouseMoveEvent(QMouseEvent *event)
{
  const QRect headerRect(0, 0, width(), d->headerSize.height());
  bool headerContainsMouse = headerRect.contains(event->pos());

  if (headerContainsMouse != d->headerContainsMouse)
  {
    d->headerContainsMouse = headerContainsMouse;
    update();
  }

  QWidget::mouseMoveEvent(event);
}


void CollapsibleGroupBox::leaveEvent(QEvent *event)
{
  d->headerContainsMouse = false;
  update();
  QWidget::leaveEvent(event);
}


void CollapsibleGroupBox::keyPressEvent(QKeyEvent *event)
{
  //event might have just propogated up from a child, if so we don't want to react to it
  if (!hasFocus())
    return;

  const int key = event->key();
  if (key == Qt::Key_Space || key == Qt::Key_Enter || key == Qt::Key_Return)
  {
    toggle();
    event->setAccepted(true);
  }
}


void CollapsibleGroupBox::resizeEvent(QResizeEvent *event)
{
  if (layout())
  {
    //we don't want the layout trying to fit the current frame of the animation so always set it to the target height
    const QMargins margins(contentsMargins());
    layout()->setGeometry(QRect(margins.left(), margins.top(), width() - margins.left() - margins.right(), layout()->sizeHint().height()));
  }

  QWidget::resizeEvent(event);
}


void CollapsibleGroupBoxPrivate::recalculateHeaderSize()
{
  QStyleOption option;
  option.initFrom(q);

  QSize textSize = q->style()->itemTextRect(option.fontMetrics, QRect(), Qt::TextShowMnemonic, false,
                   title).size();

  headerSize = q->style()->sizeFromContents(QStyle::CT_CheckBox, &option, textSize, q);
  q->setContentsMargins(q->style()->pixelMetric(QStyle::PM_LayoutLeftMargin), headerSize.height(), 0, 0);
}


void CollapsibleGroupBoxPrivate::updateChildrenVisibility(bool visible)
{
  foreach (QObject *child, q->children())
  {
    QWidget *widget = qobject_cast<QWidget*>(child);
    if (widget)
      widget->setVisible(visible);
  }
}


QSize CollapsibleGroupBoxPrivate::contentSize() const
{
  if (q->layout())
    return q->layout()->sizeHint();

  return QSize(0, 0);
}


QSize CollapsibleGroupBoxPrivate::contentMinimumSize() const
{
  if (q->layout())
    return q->layout()->minimumSize();

  return QSize(0, 0);
}
}
