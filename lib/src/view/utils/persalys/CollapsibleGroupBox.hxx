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

#ifndef PERSALYS_COLLAPSIBLEGROUPBOX_HXX
#define PERSALYS_COLLAPSIBLEGROUPBOX_HXX

#include <QWidget>
#include <QTimeLine>
#include "persalys/PersalysPrivate.hxx"

namespace PERSALYS
{

class CollapsibleGroupBoxPrivate;

/**
 * @class CollapsibleGroupBox
 *
 * A groupbox featuring a clickable header and arrow indicator that can be
 * expanded and collapsed to reveal the contents.
 *
 * When expanded, the widget will resize to fit the sizeHint of child items.
 *
 * @since 5.16
 */

class PERSALYS_UTILS_API CollapsibleGroupBox : public QWidget
{
  Q_OBJECT

public:
  explicit CollapsibleGroupBox(QWidget *parent = nullptr);
  CollapsibleGroupBox(const QString &title, QWidget *parent = nullptr);
  virtual ~CollapsibleGroupBox();

  /**
   * Set the title that will be permanently shown at the top of the collapsing box
   * Mnemonics are supported
   */
  void setTitle(const QString &title);

  /**
   * The title
   */
  QString title() const;

  /**
   * Set whether contents are shown
   *
   * The default is false until the user clicks
   */
  void setExpanded(bool expanded);

  /**
   * Whether contents are shown
   * During animations, this will reflect the target state at the end of the animation
   */
  bool isExpanded() const;

  QSize sizeHint() const;
  QSize minimumSizeHint() const;

public Q_SLOTS:
  /**
   * Expands if collapsed and vice versa
   */
  void toggle();

  /**
   * Equivalent to setExpanded(true)
   */
  void expand();

  /**
   * Equivalent to setExpanded(false)
   */
  void collapse();

Q_SIGNALS:
  /**
   * Emitted when the title is changed
   */
  void titleChanged();

  /**
   * Emitted when the widget expands or collapsed
   */
  void expandedChanged();

protected:
  void paintEvent(QPaintEvent*);

  bool event(QEvent*);
  void mousePressEvent(QMouseEvent*);
  void mouseMoveEvent(QMouseEvent*);
  void leaveEvent(QEvent*);
  void keyPressEvent(QKeyEvent*);
  void resizeEvent(QResizeEvent*);

private:
  CollapsibleGroupBoxPrivate *const d;
};
}
#endif
