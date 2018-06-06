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
#include "otgui/PieChartView.hxx"

#include "otgui/FileTools.hxx"

#include <QPainter>
#include <QDebug>
#include <QScrollBar>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QApplication>
#include <QFileDialog>
#include <QImageWriter>
#include <QMenu>
#include <QAction>

#include <cmath>

using namespace OT;

namespace OTGUI
{

// not editable model
class PieChartModel : public QStandardItemModel
{
public:
  PieChartModel(int rows, int columns, QObject * parent = 0)
    : QStandardItemModel(rows, columns, parent)
  {
  }

  Qt::ItemFlags flags(const QModelIndex & index) const
  {
    return QStandardItemModel::flags(index) & ~Qt::ItemIsEditable;
  }
};


PieChartView::PieChartView(const PointWithDescription& data, QWidget *parent)
  : QAbstractItemView(parent)
  , margin_(8)
  , totalSize_(200)
  , pieSize_(totalSize_ - 2 * margin_)
  , validItems_(0)
  , totalValue_(0.0)
  , rubberBand_(0)
  , plotName_("")
{
  // fixed size
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  // no scroll bar
  horizontalScrollBar()->setRange(0, 0);
  verticalScrollBar()->setRange(0, 0);

  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  // set model
  setData(data);

  // context menu
  setContextMenuPolicy(Qt::CustomContextMenu);
  connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenu(QPoint)));
}


void PieChartView::setData(const PointWithDescription& valuesAndDescription)
{
  PointWithDescription data(valuesAndDescription);

  // check
  UnsignedInteger size = data.getSize();
  if (size == 0)
  {
    qDebug() << "In PieChartView::setData: no data";
    throw std::exception();
  }

  Scalar l1Norm = data.norm1();
  if (l1Norm == .0)
  {
    qDebug() << "In PieChartView::setData: norm is null";
    throw std::exception();
  }

  // normalize
  for (UnsignedInteger i = 0; i < size; ++i)
  {
    data[i] /= l1Norm;
    data[i] *= 100;
  }

  // colors
  QVector<QColor> colors;
  colors = generateSegmentsColors((int)data.getSize());

  // model
  PieChartModel * standardItemModel = new PieChartModel(size, 2, this);

  QFontMetrics fm(font());
  int maxTextWidth = 0;

  for (UnsignedInteger i = 0; i < size; ++i)
  {
    const QString text = QString::fromUtf8(data.getDescription()[i].c_str());
    const int textWidth = fm.width(text);
    if (textWidth > maxTextWidth)
      maxTextWidth = textWidth;

    standardItemModel->setData(standardItemModel->index(i, 0, QModelIndex()), text);
    standardItemModel->setData(standardItemModel->index(i, 1, QModelIndex()), QString::number(data[i]));
    standardItemModel->setData(standardItemModel->index(i, 0, QModelIndex()), colors[i], Qt::DecorationRole);
    totalValue_ += data[i];
    ++validItems_;
  }
  setModel(standardItemModel);

  viewport()->update();

  // resize
  const int iconSize = itemRect(standardItemModel->index(size - 1, 0, QModelIndex())).height();
  const int width = totalSize_ + maxTextWidth + iconSize + 2 * margin_;
  const int height = qMax(totalSize_, itemRect(standardItemModel->index(size - 1, 0, QModelIndex())).bottomRight().y());
  setFixedSize(width, height);
}


void PieChartView::setPlotName(const QString& plotName)
{
  plotName_ = plotName;
}


/*
    Returns the item that covers the coordinate given in the view.
*/
QModelIndex PieChartView::indexAt(const QPoint &point) const
{
  if (validItems_ == 0)
    return QModelIndex();

  // Transform the view coordinates into contents widget coordinates.
  int wx = point.x() + horizontalScrollBar()->value();
  int wy = point.y() + verticalScrollBar()->value();

  if (wx < totalSize_)
  {
    double cx = wx - totalSize_ / 2;
    double cy = totalSize_ / 2 - wy; // positive cy for items above the center

    // Determine the distance from the center point of the pie chart.
    double d = std::sqrt(std::pow(cx, 2) + std::pow(cy, 2));

    if (d == 0 || d > pieSize_ / 2)
      return QModelIndex();

    // Determine the angle of the point.
    double angle = (180 / M_PI) * std::acos(cx / d);
    if (cy < 0)
      angle = 360 - angle;

    // Find the relevant slice of the pie.
    double startAngle = 0.0;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row)
    {
      QModelIndex index = model()->index(row, 1, rootIndex());
      double value = model()->data(index).toDouble();

      if (value > 0.0)
      {
        double sliceAngle = 360 * value / totalValue_;

        if (angle >= startAngle && angle < (startAngle + sliceAngle))
          return model()->index(row, 1, rootIndex());

        startAngle += sliceAngle;
      }
    }
  }
  else
  {
    double itemHeight = QFontMetrics(viewOptions().font).height();
    int listItem = int((wy - margin_) / itemHeight);
    int validRow = 0;

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row)
    {
      QModelIndex index = model()->index(row, 1, rootIndex());
      if (model()->data(index).toDouble() > 0.0)
      {
        if (listItem == validRow)
          return model()->index(row, 0, rootIndex());

        // Update the list index that corresponds to the next valid row.
        ++validRow;
      }
    }
  }

  return QModelIndex();
}


/*
    Returns the rectangle of the item at position \a index in the
    model. The rectangle is in contents coordinates.
*/
QRect PieChartView::itemRect(const QModelIndex &index) const
{
  if (!index.isValid())
    return QRect();

  // Check whether the index's row is in the list of rows represented
  // by slices.
  QModelIndex valueIndex;

  if (index.column() != 1)
    valueIndex = model()->index(index.row(), 1, rootIndex());
  else
    valueIndex = index;

  if (model()->data(valueIndex).toDouble() <= 0.0)
    return QRect();

  int listItem = 0;
  for (int row = index.row() - 1; row >= 0; --row)
  {
    if (model()->data(model()->index(row, 1, rootIndex())).toDouble() > 0.0)
      listItem++;
  }

  double itemHeight;

  switch (index.column())
  {
    case 0:
      itemHeight = QFontMetrics(viewOptions().font).height();

      return QRect(totalSize_,
                   int(margin_ + listItem * itemHeight),
                   totalSize_ - margin_,
                   int(itemHeight));
    case 1:
      return viewport()->rect();
  }
  return QRect();
}


QRegion PieChartView::itemRegion(const QModelIndex &index) const
{
  if (!index.isValid())
    return QRegion();

  if (index.column() != 1)
    return itemRect(index);

  if (model()->data(index).toDouble() <= 0.0)
    return QRegion();

  double startAngle = 0.0;
  for (int row = 0; row < model()->rowCount(rootIndex()); ++row)
  {
    QModelIndex sliceIndex = model()->index(row, 1, rootIndex());
    double value = model()->data(sliceIndex).toDouble();

    if (value > 0.0)
    {
      double angle = 360 * value / totalValue_;

      if (sliceIndex == index)
      {
        QPainterPath slicePath;
        slicePath.moveTo(totalSize_ / 2, totalSize_ / 2);
        slicePath.arcTo(margin_, margin_, margin_ + pieSize_, margin_ + pieSize_, startAngle, angle);
        slicePath.closeSubpath();

        return QRegion(slicePath.toFillPolygon().toPolygon());
      }

      startAngle += angle;
    }
  }

  return QRegion();
}


bool PieChartView::isIndexHidden(const QModelIndex & /*index*/) const
{
  return false;
}


int PieChartView::horizontalOffset() const
{
  return 0;
}


int PieChartView::verticalOffset() const
{
  return 0;
}


void PieChartView::mousePressEvent(QMouseEvent *event)
{
  QAbstractItemView::mousePressEvent(event);
  origin_ = event->pos();
  if (!rubberBand_)
    rubberBand_ = new QRubberBand(QRubberBand::Rectangle, viewport());
  rubberBand_->setGeometry(QRect(origin_, QSize()));
  rubberBand_->show();
}


void PieChartView::mouseMoveEvent(QMouseEvent *event)
{
  if (rubberBand_)
    rubberBand_->setGeometry(QRect(origin_, event->pos()).normalized());
  QAbstractItemView::mouseMoveEvent(event);
}


void PieChartView::mouseReleaseEvent(QMouseEvent *event)
{
  QAbstractItemView::mouseReleaseEvent(event);
  if (rubberBand_)
    rubberBand_->hide();
  viewport()->update();
}


QModelIndex PieChartView::moveCursor(QAbstractItemView::CursorAction cursorAction,
                                     Qt::KeyboardModifiers /*modifiers*/)
{
  QModelIndex current = currentIndex();

  switch (cursorAction)
  {
    case MoveLeft:
    case MoveUp:
      if (current.row() > 0)
        current = model()->index(current.row() - 1, current.column(), rootIndex());
      else
        current = model()->index(0, current.column(), rootIndex());
      break;
    case MoveRight:
    case MoveDown:
      if (current.row() < rows(current) - 1)
        current = model()->index(current.row() + 1, current.column(), rootIndex());
      else
        current = model()->index(rows(current) - 1, current.column(), rootIndex());
      break;
    default:
      break;
  }

  viewport()->update();
  return current;
}


void PieChartView::draw(QPainter& painter)
{
  QItemSelectionModel *selections = selectionModel();
  QStyleOptionViewItem option = viewOptions();

  QBrush background = option.palette.base();
  QPen foreground(option.palette.color(QPalette::WindowText));

  painter.setRenderHint(QPainter::Antialiasing);
  painter.setPen(foreground);


  // Viewport rectangles
  QRect pieRect = QRect(margin_, margin_, pieSize_, pieSize_);

  if (validItems_ <= 0)
    return;

  painter.save();
  painter.translate(pieRect.x(), pieRect.y());

  painter.drawEllipse(0, 0, pieSize_, pieSize_);

  double startAngle = 0.0;
  int row;

  for (row = 0; row < model()->rowCount(rootIndex()); ++row)
  {
    QModelIndex index = model()->index(row, 1, rootIndex());
    double value = model()->data(index).toDouble();

    if (value > 0.0)
    {
      double angle = 360 * value / totalValue_;

      QModelIndex colorIndex = model()->index(row, 0, rootIndex());
      QColor color = QColor(model()->data(colorIndex, Qt::DecorationRole).toString());

      if (currentIndex() == index)
        painter.setBrush(QBrush(color, Qt::Dense4Pattern));
      else if (selections->isSelected(index))
        painter.setBrush(QBrush(color, Qt::Dense3Pattern));
      else
        painter.setBrush(QBrush(color));

      painter.drawPie(0, 0, pieSize_, pieSize_, int(startAngle * 16), int(angle * 16));

      startAngle += angle;
    }
  }
  painter.restore();

  int keyNumber = 0;

  for (row = 0; row < model()->rowCount(rootIndex()); ++row)
  {
    QModelIndex index = model()->index(row, 1, rootIndex());
    double value = model()->data(index).toDouble();

    if (value > 0.0)
    {
      QModelIndex labelIndex = model()->index(row, 0, rootIndex());

      option.rect = visualRect(labelIndex);
      if (selections->isSelected(labelIndex))
        option.state |= QStyle::State_Selected;
      if (currentIndex() == labelIndex)
        option.state |= QStyle::State_HasFocus;
      itemDelegate()->paint(&painter, option, labelIndex);

      ++keyNumber;
    }
  }
}


void PieChartView::paintEvent(QPaintEvent *event)
{
  QPainter painter(viewport());
  draw(painter);
}


void PieChartView::resizeEvent(QResizeEvent * /* event */)
{
  updateGeometries();
}


int PieChartView::rows(const QModelIndex &index) const
{
  return model()->rowCount(model()->parent(index));
}


void PieChartView::scrollTo(const QModelIndex &index, ScrollHint)
{
  // no scroll bar
}


/*
    Find the indices corresponding to the extent of the selection.
*/
void PieChartView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags command)
{
  // Use content widget coordinates because we will use the itemRegion()
  // function to check for intersections.

  QRect contentsRect = rect.translated(horizontalScrollBar()->value(),
                                       verticalScrollBar()->value()).normalized();

  int rows = model()->rowCount(rootIndex());
  int columns = model()->columnCount(rootIndex());
  QModelIndexList indexes;

  for (int row = 0; row < rows; ++row)
  {
    for (int column = 0; column < columns; ++column)
    {
      QModelIndex index = model()->index(row, column, rootIndex());
      QRegion region = itemRegion(index);
      if (region.intersects(contentsRect))
        indexes.append(index);
    }
  }

  if (indexes.size() > 0)
  {
    int firstRow = indexes[0].row();
    int lastRow = indexes[0].row();
    int firstColumn = indexes[0].column();
    int lastColumn = indexes[0].column();

    for (int i = 1; i < indexes.size(); ++i)
    {
      firstRow = qMin(firstRow, indexes[i].row());
      lastRow = qMax(lastRow, indexes[i].row());
      firstColumn = qMin(firstColumn, indexes[i].column());
      lastColumn = qMax(lastColumn, indexes[i].column());
    }

    QItemSelection selection(model()->index(firstRow, firstColumn, rootIndex()),
                             model()->index(lastRow, lastColumn, rootIndex()));
    selectionModel()->select(selection, command);
  }
  else
  {
    QModelIndex noIndex;
    QItemSelection selection(noIndex, noIndex);
    selectionModel()->select(selection, command);
  }

  update();
}


void PieChartView::updateGeometries()
{
  horizontalScrollBar()->setPageStep(viewport()->width());
  verticalScrollBar()->setPageStep(viewport()->height());
}


/*
    Returns the position of the item in viewport coordinates.
*/
QRect PieChartView::visualRect(const QModelIndex &index) const
{
  QRect rect = itemRect(index);
  if (!rect.isValid())
    return rect;

  return QRect(rect.left() - horizontalScrollBar()->value(),
               rect.top() - verticalScrollBar()->value(),
               rect.width(),
               rect.height());
}


/*
    Returns a region corresponding to the selection in viewport coordinates.
*/
QRegion PieChartView::visualRegionForSelection(const QItemSelection &selection) const
{
  int ranges = selection.count();

  if (ranges == 0)
    return QRect();

  QRegion region;
  for (int i = 0; i < ranges; ++i)
  {
    QItemSelectionRange range = selection.at(i);
    for (int row = range.top(); row <= range.bottom(); ++row)
    {
      for (int col = range.left(); col <= range.right(); ++col)
      {
        QModelIndex index = model()->index(row, col, rootIndex());
        region += visualRect(index);
      }
    }
  }
  return region;
}


QVector<QColor> PieChartView::generateSegmentsColors(const int nbColors) const
{
  QVector<QColor> colors;

  int colorPos = 13;

  // get colors
  QStringList colorNames = QColor::colorNames();
  for (int i = 0; i < nbColors; i++)
  {
    QColor col(colorNames.at(colorPos % colorNames.count()));
    colorPos++;
    colors.append(col);
  }

  return colors;
}


void PieChartView::contextMenu(const QPoint & pos)
{
  QAction * exportPlotAction = new QAction(tr("Export plot"), this);
  connect(exportPlotAction, SIGNAL(triggered(bool)), this, SLOT(exportPlot()));

  QMenu * contextMenu(new QMenu(this));
  contextMenu->addAction(exportPlotAction);
  contextMenu->popup(mapToGlobal(pos));
}


void PieChartView::exportPlot()
{
  QString fileName = QFileDialog::getSaveFileName(this, tr("Export plot"),
                     FileTools::GetCurrentDir() + QDir::separator() + plotName_,
                     tr("Images (*.bmp *.jpg *.jpeg *.png *.ppm *.xbm *.xpm *.tiff)"));

  if (!fileName.isEmpty())
  {
    QString format = QFileInfo(fileName).suffix().toLower();
    FileTools::SetCurrentDir(fileName);

    if (format.isEmpty())
    {
      fileName += ".png";
      format = "png";
    }
    if (QImageWriter::supportedImageFormats().indexOf(format.toLatin1()) >= 0)
    {
      // create image
      QImage image(QSize(width(), height()), QImage::Format_RGB32);
      image.fill(QColor(Qt::white).rgb());
      QPainter painter(&image);
      draw(painter);

      // save image
      bool saveOperationSucceed = image.save(fileName, format.toLatin1());

      if (!saveOperationSucceed)
        QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Impossible to export the plot."));
    }
    else
    {
      QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Format not supported."));
    }
  }
}
}
