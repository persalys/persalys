//                                               -*- C++ -*-
/**
 *  @brief QWidget for plot matrices
 *
 *  Copyright 2015-2016 EDF
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
#include "otgui/PlotMatrixWidget.hxx"

#include "otgui/PlotWidget.hxx"

#include <QVBoxLayout>
#include <QHeaderView>
#include <QFileDialog>
#include <QImageWriter>
#include <QPainter>

#include <qwt_plot_renderer.h>
#include <qwt_plot_canvas.h>

using namespace OT;

namespace OTGUI {

PlotMatrixWidget::PlotMatrixWidget(const OT::NumericalSample & inputSample, const OT::NumericalSample & outputSample, QWidget* parent)
  : QWidget(parent)
  , inputNames_(QStringList())
  , outputNames_(QStringList())
{
  int nbInputs = inputSample.getDimension();
  int nbOutputs = outputSample.getDimension();

  for (int i=0; i<nbInputs; ++i)
    inputNames_ << inputSample.getDescription()[i].c_str();
  for (int i=0; i<nbOutputs; ++i)
    outputNames_ << outputSample.getDescription()[i].c_str();

  QVBoxLayout * plotMatrixLayout = new QVBoxLayout(this);

  // matrix title
  QHBoxLayout * tableWidgetTitleLayout = new QHBoxLayout;
  matrixTitleLabel_ = new QLabel("Scatter plots matrix");
  matrixTitleLabel_->setStyleSheet("font: bold;");
  tableWidgetTitleLayout->addWidget(matrixTitleLabel_);
  plotMatrixLayout->addLayout(tableWidgetTitleLayout);

  // matrix
  nbInputsToDisplay_ = nbInputs;
  nbOutputsToDisplay_ = nbOutputs;

  tableWidget_ = new QTableWidget(nbOutputs, nbInputs);
  tableWidget_->setSelectionMode(QAbstractItemView::NoSelection);
  tableWidget_->setHorizontalHeaderLabels(inputNames_);
  tableWidget_->setVerticalHeaderLabels(outputNames_);
  tableWidget_->horizontalHeader()->setResizeMode(QHeaderView::Fixed);
  tableWidget_->verticalHeader()->setResizeMode(QHeaderView::Fixed);
  tableWidget_->horizontalHeader()->setDefaultSectionSize(100);
  tableWidget_->verticalHeader()->setDefaultSectionSize(100);

  // fill matrix and create images of the plotWidgets
  QPen scatterPen = QPen(Qt::blue, 1);
  for (int i=0; i<nbOutputs; ++i)
  {
    for (int j=0; j<nbInputs; ++j)
    {
      QWidget * cellMatrixWidget = new QWidget;
      QVBoxLayout * cellMatrixLayout = new QVBoxLayout(cellMatrixWidget);
      PlotWidget * plot = new PlotWidget;
      plot->plotScatter(inputSample.getMarginal(j), outputSample.getMarginal(i), scatterPen);
      plot->enableAxis(QwtPlot::xBottom, false);
      plot->enableAxis(QwtPlot::yLeft, false);
      // hide borders: #f6f7fa is white
      plot->canvas()->setStyleSheet("QwtPlotCanvas {background-color: #f6f7fa;}");
      cellMatrixLayout->addWidget(plot);
      tableWidget_->setCellWidget(i, j, cellMatrixWidget);

      QPixmap pixmap(100, 100);
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


QImage PlotMatrixWidget::getMatrixImage()
{
  int width = matrixImages_[0].width();
  int height = matrixImages_[0].height();

  int nbInputs = tableWidget_->columnCount();
  int nbOutputs = tableWidget_->rowCount();

  // total width of the table (width of an image * number of plotwidgets + width of the vertical header)
  // the width of the vertical header is set to the half of an image width
  int totalWidth = width*(nbInputsToDisplay_+0.5);
  // total height of the table (height of an image * number of plotwidgets + height of the horizontal header)
  // the height of the horizontal header is set to the half of an image height
  int totalHeight = height*(nbOutputsToDisplay_+0.5);

  // determine the height of the optimal rectangle being able to contain the title
  int aFakeHeight = 1;
  QImage aFakeImage(QSize(totalWidth+5, aFakeHeight), QImage::Format_ARGB32_Premultiplied);
  QPainter aFakePainter(&aFakeImage);
  QRectF titleRect = aFakePainter.boundingRect(QRectF(QPointF(0, 0), QSizeF(totalWidth+5, aFakeHeight)), Qt::TextWordWrap|Qt::AlignHCenter, getTitle());
  int alpha = titleRect.height();

  // create the image of the matrix
  QImage resultImage(QSize(totalWidth+5, totalHeight+5+alpha), QImage::Format_ARGB32_Premultiplied);
  resultImage.fill(Qt::white);

  QPainter painter(&resultImage);
  painter.setPen(QPen(Qt::black, 0.5));

  // add title
  painter.drawText(QRectF(QPointF(0, 0), QSizeF(totalWidth, alpha)), getTitle(), QTextOption(Qt::AlignCenter));

  int it_i = 0;
  int it_j = 0;
  for (int i=0; i<nbOutputs; ++i)
  {
    if (!tableWidget_->isRowHidden(i))
    {
      // add output names
      painter.drawText(QRectF(QPointF(0, height*(it_i+0.5)+alpha), QSizeF(width*0.5, height)), outputNames_[i], QTextOption(Qt::AlignCenter));
      it_j = 0;
      for(int j=0; j<nbInputs; ++j)
      {
        if (!tableWidget_->isColumnHidden(j))
        {
          // add plotWidget image
          painter.drawImage(width*(it_j+0.5), height*(it_i+0.5)+alpha, matrixImages_[j+nbInputs*i]);
          ++it_j;
        }
      }
      // add horizontal lines
      painter.drawLine(0, height*(it_i+0.5)+alpha, totalWidth, height*(it_i+0.5)+alpha);
      ++it_i;
    }
  }
  it_j = 0;
  for(int j=0; j<nbInputs; ++j)
  {
    if (!tableWidget_->isColumnHidden(j))
    {
      // add input names
      painter.drawText(QRectF(QPointF(width*(it_j+0.5), alpha), QSizeF(width, height*0.5)), inputNames_[j], QTextOption(Qt::AlignCenter));
      // add vertical lines
      painter.drawLine(width*(it_j+0.5), alpha, width*(it_j+0.5), totalHeight+alpha);
      ++it_j;
    }
  }

  // add outside lines of the table
  painter.drawRect(QRectF(QPointF(0, alpha), QSizeF(totalWidth, totalHeight)));

  return resultImage;
}


void PlotMatrixWidget::setInputsToDisplay(QStringList inputs)
{
  nbInputsToDisplay_ = 0;
  for (int i=0; i<inputNames_.size(); ++i)
  {
    if (inputs.contains(inputNames_[i]))
    {
      ++nbInputsToDisplay_;
      tableWidget_->showColumn(i);
    }
    else
      tableWidget_->hideColumn(i);
  }
}


void PlotMatrixWidget::setOutputsToDisplay(QStringList outputs)
{
  nbOutputsToDisplay_ = 0;
  for (int i=0; i<outputNames_.size(); ++i)
  {
    if (outputs.contains(outputNames_[i]))
    {
      ++nbOutputsToDisplay_;
      tableWidget_->showRow(i);
    }
    else
      tableWidget_->hideRow(i);
  }
}


void PlotMatrixWidget::exportPlot()
{

  QString fileName = QFileDialog::getSaveFileName(this, tr("Export plot"),
                     QDir::homePath(),
                     tr("Images (*.bmp *.jpg *.jpeg *.png *.ppm *.xbm *.xpm *.tiff)"));

  if (!fileName.isEmpty())
  {
    QString format = QFileInfo(fileName).suffix().toLower();

    if (format.isEmpty())
    {
      fileName += ".png";
      format = "png";
    }
    if (QImageWriter::supportedImageFormats().indexOf(format.toLatin1()) >= 0)
    {
      bool saveOperationSucceed = getMatrixImage().save(fileName, format.toLatin1());
      if (!saveOperationSucceed)
        std::cerr<<"Export doesn't work\n";
    }
    else
    {
      std::cerr<<"Format not supported\n";
    }
  }
}
}
