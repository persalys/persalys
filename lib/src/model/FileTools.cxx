//                                               -*- C++ -*-
/**
 *  @brief File tools
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
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include "otgui/FileTools.hxx"

#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QApplication>
#include <QTextStream>
#include <QImageWriter>

namespace OTGUI
{

QString FileTools::GetCurrentDir()
{
  QSettings settings;
  QString result = settings.value("currentDir", QDir::homePath()).toString();
  QDir dir(result);
  if (!dir.exists())
  {
    result = QDir::homePath();
  }
  return result;
}


void FileTools::SetCurrentDir(const QString &fileName)
{
  QFileInfo info(fileName);
  if (info.exists())
  {
    QSettings settings;
    settings.setValue("currentDir", info.absolutePath());
  }
}


void FileTools::ExportData(const OT::Sample& sample, QWidget * parent)
{
  QString fileName = QFileDialog::getSaveFileName(parent,
                     tr("Export data..."),
                     GetCurrentDir() + QDir::separator() + tr("data"),
                     tr("CSV source files (*.csv)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".csv"))
      fileName += ".csv";

    SetCurrentDir(fileName);

    try
    {
      sample.exportToCSVFile(fileName.toLocal8Bit().data(), ",");
    }
    catch (std::exception & ex)
    {
      QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Impossible to export the data. ") + ex.what());
    }
  }
}


void FileTools::ExportData(const QString& text, QWidget * parent)
{
  QString fileName = QFileDialog::getSaveFileName(parent,
                     tr("Export data..."),
                     GetCurrentDir() + QDir::separator() + tr("data"),
                     tr("CSV source files (*.csv)"));

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".csv"))
      fileName += ".csv";

    SetCurrentDir(fileName);

    QFile file(fileName.toLocal8Bit().data());
    try
    {
      file.open(QIODevice::WriteOnly);
      QTextStream stream(&file);
      stream << text;
    }
    catch (std::exception & ex)
    {
      QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Impossible to export the data. ") + ex.what());
    }
    file.close();
  }
}


void FileTools::ExportImage(const QImage& image, QWidget * parent)
{
  QString fileName = QFileDialog::getSaveFileName(parent,
                     tr("Export image"),
                     GetCurrentDir() + QDir::separator() + tr("image"),
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
    if (QImageWriter::supportedImageFormats().indexOf(format.toLocal8Bit()) >= 0)
    {
      bool saveOperationSucceed = image.save(fileName.toLocal8Bit().data(), format.toLocal8Bit());
      if (!saveOperationSucceed)
        QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Impossible to export the image."));
    }
    else
    {
      QMessageBox::warning(QApplication::activeWindow(), tr("Warning"), tr("Format not supported."));
    }
  }
}
}
