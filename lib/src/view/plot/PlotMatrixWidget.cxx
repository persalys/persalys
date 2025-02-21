//                                               -*- C++ -*-
/**
 *  @brief QWidget for plot matrices
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
#include "persalys/PlotMatrixWidget.hxx"

#include "persalys/PlotWidget.hxx"
#include "persalys/FileTools.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QPainter>

#include <qwt_plot_renderer.h>
#include <qwt_plot_canvas.h>

using namespace OT;

namespace PERSALYS
{

PlotMatrixWidget::PlotMatrixWidget(const OT::Sample & sample1, const OT::Sample & sample2, QWidget* parent)
  : QWidget(parent)
  , inputNames_()
  , outputNames_()
  , columnsNames_()
  , rowsNames_()
{
  const int nbColumns = sample1.getDimension();
  const int nbRows = sample2.getDimension();

  QString typeNameSuffix = "XX";
  bool isPlotMatrixXX = true;
  if (sample1 != sample2)
  {
    isPlotMatrixXX = false;
    typeNameSuffix = "YX";
  }

  for (int  i = 0; i < nbColumns; ++i)
    columnsNames_ << QString::fromUtf8(sample1.getDescription()[i].c_str());
  for (int i = 0; i < nbRows; ++i)
    rowsNames_ << QString::fromUtf8(sample2.getDescription()[i].c_str());

  QVBoxLayout * plotMatrixLayout = new QVBoxLayout(this);

  // matrix title
  QHBoxLayout * tableWidgetTitleLayout = new QHBoxLayout;
  matrixTitleLabel_ = new QLabel(tr("Scatter plots matrix"));
  matrixTitleLabel_->setStyleSheet("QLabel {font: bold;}");
  tableWidgetTitleLayout->addWidget(matrixTitleLabel_);
  plotMatrixLayout->addLayout(tableWidgetTitleLayout);

  // matrix
  nbColumnsToDisplay_ = nbColumns;
  nbRowsToDisplay_ = nbRows;

  const int sizePixmap = 200;

  tableWidget_ = new QTableWidget(nbRows, nbColumns);
  tableWidget_->setSelectionMode(QAbstractItemView::NoSelection);
  tableWidget_->setHorizontalHeaderLabels(columnsNames_);
  tableWidget_->setVerticalHeaderLabels(rowsNames_);
  tableWidget_->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  tableWidget_->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
  tableWidget_->horizontalHeader()->setDefaultSectionSize(sizePixmap);
  tableWidget_->verticalHeader()->setDefaultSectionSize(sizePixmap);

  // fill matrix and create images of the plotWidgets
  QPen scatterPen = QPen(Qt::blue, 1);
  for (int i = 0; i < nbRows; ++i)
  {
    for (int j = 0; j < nbColumns; ++j)
    {
      QWidget * cellMatrixWidget = new QWidget;
      QVBoxLayout * cellMatrixLayout = new QVBoxLayout(cellMatrixWidget);
      PlotWidget * plot = new PlotWidget(tr("plotmatrix") + typeNameSuffix, true);
      if (isPlotMatrixXX && i == j)
        plot->plotHistogram(sample1.getMarginal(j));
      else
        plot->plotScatter(sample1.getMarginal(j), sample2.getMarginal(i), scatterPen);
      plot->enableAxis(QwtPlot::xBottom, false);
      plot->enableAxis(QwtPlot::yLeft, false);
      // hide borders
      plot->canvas()->setStyleSheet("QwtPlotCanvas {background-color: #ffffff;}");
      cellMatrixLayout->addWidget(plot);
      tableWidget_->setCellWidget(i, j, cellMatrixWidget);

      QPixmap pixmap(sizePixmap, sizePixmap);
      pixmap.fill();
      QwtPlotRenderer renderer;
      renderer.renderTo(plot, pixmap);
      matrixImages_.append(pixmap.toImage());
    }
  }
  plotMatrixLayout->addWidget(tableWidget_);
}


QString PlotMatrixWidget::getTitle() const
{
  return matrixTitleLabel_->text();
}


void PlotMatrixWidget::setTitle(const QString & title)
{
  matrixTitleLabel_->setText(title);
}


QStringList PlotMatrixWidget::getInputNames() const
{
  return inputNames_;
}


QStringList PlotMatrixWidget::getOutputNames() const
{
  return outputNames_;
}

void PlotMatrixWidget::setInputNames(const QStringList inputNames)
{
  inputNames_ = inputNames;
}


void PlotMatrixWidget::setOutputNames(const QStringList outputNames)
{
  outputNames_ = outputNames;
}


QStringList PlotMatrixWidget::getColumnsNames() const
{
  return columnsNames_;
}


QStringList PlotMatrixWidget::getRowsNames() const
{
  return rowsNames_;
}


QImage PlotMatrixWidget::getMatrixImage()
{
  const int width = matrixImages_[0].width();
  const int height = matrixImages_[0].height();

  const int nbColumns = tableWidget_->columnCount();
  const int nbRows = tableWidget_->rowCount();

  // total width of the table (width of an image * number of plotwidgets + width of the vertical header)
  // the width of the vertical header is set to the half of an image width
  const int totalWidth = width * (nbColumnsToDisplay_ + 0.5);
  // total height of the table (height of an image * number of plotwidgets + height of the horizontal header)
  // the height of the horizontal header is set to the half of an image height
  const int totalHeight = height * (nbRowsToDisplay_ + 0.5);

  // determine the height of the optimal rectangle being able to contain the title
  const int aFakeHeight = 1;
  QImage aFakeImage(QSize(totalWidth + 5, aFakeHeight), QImage::Format_ARGB32_Premultiplied);
  QPainter aFakePainter(&aFakeImage);
  const QRectF titleRect = aFakePainter.boundingRect(QRectF(QPointF(0, 0), QSizeF(totalWidth + 5, aFakeHeight)),
                           Qt::TextWordWrap | Qt::AlignHCenter,
                           getTitle());
  const int alpha = titleRect.height();

  // create the image of the matrix
  QImage resultImage(QSize(totalWidth + 5, totalHeight + 5 + alpha), QImage::Format_ARGB32_Premultiplied);
  resultImage.fill(Qt::white);

  QPainter painter(&resultImage);
  painter.setPen(QPen(Qt::black, 0.5));

  // add title
  painter.drawText(QRectF(QPointF(0, 0), QSizeF(totalWidth, alpha)),
                   getTitle(),
                   QTextOption(Qt::AlignCenter));

  int it_i = 0;
  int it_j = 0;
  for (int i = 0; i < nbRows; ++i)
  {
    if (!tableWidget_->isRowHidden(i))
    {
      // add output names
      painter.drawText(QRectF(QPointF(0, height * (it_i + 0.5) + alpha), QSizeF(width * 0.5, height)),
                       rowsNames_[i],
                       QTextOption(Qt::AlignCenter));
      it_j = 0;
      for(int j = 0; j < nbColumns; ++j)
      {
        if (!tableWidget_->isColumnHidden(j))
        {
          // add plotWidget image
          painter.drawImage(width * (it_j + 0.5),
                            height * (it_i + 0.5) + alpha,
                            matrixImages_[j + nbColumns * i]);
          ++it_j;
        }
      }
      // add horizontal lines
      painter.drawLine(0,
                       height * (it_i + 0.5) + alpha,
                       totalWidth,
                       height * (it_i + 0.5) + alpha);
      ++it_i;
    }
  }
  it_j = 0;
  for(int j = 0; j < nbColumns; ++j)
  {
    if (!tableWidget_->isColumnHidden(j))
    {
      // add input names
      painter.drawText(QRectF(QPointF(width * (it_j + 0.5), alpha), QSizeF(width, height * 0.5)),
                       columnsNames_[j],
                       QTextOption(Qt::AlignCenter));
      // add vertical lines
      painter.drawLine(width * (it_j + 0.5),
                       alpha,
                       width * (it_j + 0.5),
                       totalHeight + alpha);
      ++it_j;
    }
  }

  // add outside lines of the table
  painter.drawRect(QRectF(QPointF(0, alpha), QSizeF(totalWidth, totalHeight)));

  return resultImage;
}


void PlotMatrixWidget::setColumnsToDisplay(QStringList columns)
{
  nbColumnsToDisplay_ = 0;
  for (int i = 0; i < columnsNames_.size(); ++i)
  {
    if (columns.contains(columnsNames_[i]))
    {
      ++nbColumnsToDisplay_;
      tableWidget_->showColumn(i);
    }
    else
      tableWidget_->hideColumn(i);
  }
}


void PlotMatrixWidget::setRowsToDisplay(QStringList rows)
{
  nbRowsToDisplay_ = 0;
  for (int i = 0; i < rowsNames_.size(); ++i)
  {
    if (rows.contains(rowsNames_[i]))
    {
      ++nbRowsToDisplay_;
      tableWidget_->showRow(i);
    }
    else
      tableWidget_->hideRow(i);
  }
}


void PlotMatrixWidget::exportPlot()
{
  FileTools::ExportImage(getMatrixImage(), this);
}
}
