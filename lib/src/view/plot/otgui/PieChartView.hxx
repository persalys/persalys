/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef OTGUI_PIECHARTVIEW_HXX
#define OTGUI_PIECHARTVIEW_HXX

#include "otgui/OTGuiprivate.hxx"

#include <openturns/NumericalPointWithDescription.hxx>

#include <QAbstractItemView>
#include <QPen>
#include <QMouseEvent>
#include <QRubberBand>
#include <QPainter>

namespace OTGUI {
class OTGUI_API PieChartView: public QAbstractItemView
{
  Q_OBJECT

public:
  PieChartView(const OT::NumericalPointWithDescription& data, QWidget* parent = 0);

  void setData(const OT::NumericalPointWithDescription& data);

  virtual QModelIndex indexAt(const QPoint &point) const;
  virtual QRect visualRect(const QModelIndex &index) const;
  virtual void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);

protected:
  virtual bool isIndexHidden(const QModelIndex &index) const;
  virtual int horizontalOffset() const;
  virtual int verticalOffset() const;

  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

  virtual QModelIndex moveCursor(QAbstractItemView::CursorAction cursorAction,
                          Qt::KeyboardModifiers modifiers);

  virtual void draw(QPainter& devicePainter);
  virtual void paintEvent(QPaintEvent *event);
  virtual void resizeEvent(QResizeEvent *event);

  virtual void setSelection(const QRect&, QItemSelectionModel::SelectionFlags command);

  virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;

  QVector<QColor> generateSegmentsColors(const int nbColors) const;

protected slots:
  void contextMenu(const QPoint & pos);
  void exportPlot();

private:
  QRect itemRect(const QModelIndex &item) const;
  QRegion itemRegion(const QModelIndex &index) const;
  int rows(const QModelIndex &index = QModelIndex()) const;
  virtual void updateGeometries();

private:
  int margin_;
  int totalSize_;
  int pieSize_;
  int validItems_;
  double totalValue_;
  QPoint origin_;
  QRubberBand *rubberBand_;
};
}
#endif