//                                               -*- C++ -*-
/**
 *  @brief File tools
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/FileTools.hxx"

#include <QSettings>
#include <QDir>
#include <QFileDialog>
#include <QGridLayout>
#include <QComboBox>
#include <QStandardItem>
#include <QLabel>
#include <QMessageBox>
#include <QApplication>
#include <QTextStream>
#include <QImageWriter>
#include <QCoreApplication>
#include <QDesktopServices>
#include <QProcess>

namespace PERSALYS
{
const QString FileTools::OpenTURNSUrlLink = "http://openturns.github.io/openturns/latest/";

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
  QFileDialog * dlg = new QFileDialog(parent);
  dlg->setOption(QFileDialog::DontUseNativeDialog, true);
  dlg->setFileMode(QFileDialog::AnyFile);
  dlg->setNameFilters(QStringList() << tr("CSV source files (*.csv)"));

  const QStringList colSepLabels = QStringList() << tr("Comma") << tr("Semi-colon") << tr("Space");

  QLabel * lblColSep = new QLabel(tr("Column separator:"));
  QComboBox * boxColSep = new QComboBox;
  boxColSep->addItems(colSepLabels);
  boxColSep->setItemData(0,",");
  boxColSep->setItemData(1,";");
  boxColSep->setItemData(2," ");
  boxColSep->setCurrentIndex(0);

  QGridLayout * layout = static_cast<QGridLayout*>(dlg->layout());
  int row = layout->rowCount();
  layout->addWidget(lblColSep, row, 0);
  layout->addWidget(boxColSep, row, 1);

  const QStringList numSepLabels = QStringList() << tr("Point") << tr("Comma");

  QLabel * lblNumSep = new QLabel(tr("Numerical separator:"));
  QComboBox * boxNumSep = new QComboBox;
  boxNumSep->addItems(numSepLabels);
  boxNumSep->setItemData(0,".");
  boxNumSep->setItemData(1,",");
  boxNumSep->setCurrentIndex(0);

  layout->addWidget(lblNumSep, ++row, 0);
  layout->addWidget(boxNumSep, row, 1);

  // Disable comma as numSep if already set as colSep
  QStandardItemModel *model = qobject_cast<QStandardItemModel *>(boxNumSep->model());
  Q_ASSERT(model != nullptr);

  QStandardItem *commaItem = model->item(1);
  commaItem->setFlags(boxColSep->currentIndex() == 0 ? commaItem->flags() & ~Qt::ItemIsEnabled
                 : commaItem->flags() | Qt::ItemIsEnabled);

  connect(boxColSep, QOverload<int>::of(&QComboBox::currentIndexChanged), [=](int index) {
      commaItem->setFlags(index == 0 ? commaItem->flags() & ~Qt::ItemIsEnabled
                     : commaItem->flags() | Qt::ItemIsEnabled);
    });

  QString fileName;
  if(dlg->exec())
    fileName = dlg->selectedFiles()[0];

  if (!fileName.isEmpty())
  {
    if (!fileName.endsWith(".csv"))
      fileName += ".csv";

    SetCurrentDir(fileName);

    try
    {
      sample.exportToCSVFile(fileName.toLocal8Bit().data(),
                             boxColSep->itemData(boxColSep->currentIndex()).toString().toStdString(),
                             boxNumSep->itemData(boxNumSep->currentIndex()).toString().toStdString());
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


QString FileTools::GetDocumentationDirectoryPath()
{
  // case 1: try to use an environment variable
  QString userManualDir = std::getenv("PERSALYS_HTML_DOCUMENTATION_PATH");
  if (!userManualDir.isEmpty() && QDir(userManualDir).exists())
    return userManualDir;

  // case 2: search the path of the documentation
  const QString appliDirPath(QCoreApplication::applicationDirPath());
  QDir dirPath(appliDirPath);
  dirPath.cdUp();

  // case 2-1: on Linux when the documentation is built and put in the install directory
  userManualDir = QDir::toNativeSeparators(QString("%1/%2/html/").arg(INSTALL_PATH).arg(DOCUMENTATION_INSTALL_PATH));
  if (!userManualDir.isEmpty() && QDir(userManualDir).exists())
    return userManualDir;

  // case 2-2: on Linux when using the AppImage
  userManualDir = QDir::toNativeSeparators(QString("%1/%2/html/").arg(dirPath.path()).arg(DOCUMENTATION_INSTALL_PATH));
  if (!userManualDir.isEmpty() && QDir(userManualDir).exists())
    return userManualDir;

  return "";
}


QUrl FileTools::GetDocumentationUrl(const QString &urlLink, const docType type)
{
  QUrl url;

  QStringList pathAndFragment(urlLink.split("#"));

  if (QFileInfo(urlLink).isAbsolute())
    url = QUrl::fromLocalFile(QDir::toNativeSeparators(pathAndFragment[0]));
  else if (type == docGUI)
    url = QUrl::fromLocalFile(QDir::toNativeSeparators(GetDocumentationDirectoryPath() + pathAndFragment[0]));
  else
    url = QUrl(OpenTURNSUrlLink + pathAndFragment[0]);

  // set url fragment (text after #)
  if (pathAndFragment.size() == 2)
    url.setFragment(pathAndFragment[1]);

  return url;
}


void FileTools::OpenUrl(const QUrl& url)
{
#ifdef _WIN32
  // workaround for Windows if the url has a fragment
  // https://bugreports.qt.io/browse/QTBUG-14460
  if (url.isLocalFile() && url.hasFragment())
  {
    // create a temporary html file which contains the right file path with fragment
    const QString fileName(QDir::toNativeSeparators(QDir::tempPath() + "/persalys_html_temp_file.html"));
    QFile file(fileName);

    if (file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
      QTextStream stream(&file);
      QString htmlText("<html><meta http-equiv=Refresh content=\"0; url=" + url.toString() + "\"><body></body></html>;");
      stream << htmlText;
      file.close();
      // open the file
      QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
      return;
    }
  }
#elif defined(PERSALYS_APPIMAGE) && QT_VERSION >= QT_VERSION_CHECK(5,10,0)
  // the browser process fails in the inherited AppImage env
  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  env.remove("LD_LIBRARY_PATH");
  QProcess process;
  process.setProcessEnvironment(env);
  process.setProgram("xdg-open");
  process.setArguments(QStringList() << url.toString());
  if (process.startDetached())
    return;
#endif
  QDesktopServices::openUrl(url);
}
}
