//                                               -*- C++ -*-
/**
 *  @brief File tools
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
#ifndef PERSALYS_FILETOOLS_HXX
#define PERSALYS_FILETOOLS_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/Sample.hxx>

#include <QObject>
#include <QImage>
#include <QUrl>

namespace PERSALYS
{
class PERSALYS_MODEL_API FileTools : public QObject
{
  Q_OBJECT

public:
  enum docType {docGUI, docOT};
  static void SetCurrentDir(const QString &fileName);
  static QString GetCurrentDir();
  static void ExportData(const OT::Sample& sample, QWidget * parent);
  static void ExportData(const QString& text, QWidget * parent);
  static void ExportImage(const QImage& imgae, QWidget * parent);
  static QString GetDocumentationDirectoryPath();
  static QUrl GetDocumentationUrl(const QString &urlLink, const docType type);
  static const QString OpenTURNSUrlLink;
  static void OpenUrl(const QUrl& url);
};
}
#endif
