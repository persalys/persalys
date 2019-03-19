//                                               -*- C++ -*-
/**
 *  @brief QTableView without header that allows resizing of columns
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
#ifndef OTGUI_RESIZABLEHEADERLESSTABLEVIEW_HXX
#define OTGUI_RESIZABLEHEADERLESSTABLEVIEW_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/Indices.hxx>

#include <QHeaderView>
#include <QTableView>
#include <QMouseEvent>

namespace OTGUI
{
/*
 * Subclass of QTableView that provides notification when the mouse cursor
 *  enters/leaves a column boundary.
 */
class OTGUI_API HeaderlessTableView: public QTableView
{
public:
  explicit HeaderlessTableView(QWidget *parent = 0);

  /*
   * @return The index of the column whose right hand boundary the cursor lies
   *         on or -1 if not on a boundary.
   */
  int column_index() const;

protected:
  virtual bool eventFilter(QObject *obj, QEvent *event);

  /*
   * Called whenever the cursor enters or leaves a column boundary.  if
   * `entered' is true then the index of the column can be obtained using
   * `column_index()'.
   */
  virtual void entered_column_boundary(bool entered);

private:
  int boundary_width_;
  int column_index_;
};


/*
 * Subclass of HeaderlessTableView that allows resizing of columns.
 */
class OTGUI_API ResizableHeaderlessTableView: public HeaderlessTableView
{
public:
  explicit ResizableHeaderlessTableView(QWidget *parent = 0);

  void resizeWithOptimalWidth();
  void resizeWithOptimalHeight();

protected:
  virtual bool eventFilter(QObject *obj, QEvent *event);
  /*
   * Override entered_column_boundary to update the cursor sprite when
   * entering/leaving a column boundary.
   */
  virtual void entered_column_boundary(bool entered);

private:
  bool dragging_;
  QPoint mouse_pos_;
  QCursor cursor_;
  OT::Indices minimumSectionsSizes_;
};
}
#endif
