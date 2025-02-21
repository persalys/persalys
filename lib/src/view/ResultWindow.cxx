//                                               -*- C++ -*-
/**
 *  @brief Base class QMdiSubWindow for the results
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
#include "persalys/ResultWindow.hxx"

#include "persalys/TranslationManager.hxx"

#include <QVBoxLayout>
#include <QTextEdit>

namespace PERSALYS
{

ResultWindow::ResultWindow(Item * item, QWidget * parent)
  : SubWindow(item, parent)
  , parametersWidget_(0)
  , modelDescriptionWidget_(0)
{
  // model description widget
  if (item)
  {
    AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(item);
    if (analysisItem)
      setModelDescription(analysisItem->getAnalysis());
  }
}


void ResultWindow::setParameters(const Analysis& analysis, const QString& title)
{
  const Parameters analysisParameters(analysis.getImplementation()->getParameters());
  if (!analysisParameters.getSize())
    return;

  parametersWidget_ = new QScrollArea;
  QWidget * paramWidget = new QWidget;
  QVBoxLayout * parametersWidgetLayout = new QVBoxLayout(paramWidget);
  parametersWidgetLayout->addWidget(new ParametersWidget(title, analysisParameters), 0, Qt::AlignTop);
  parametersWidget_->setWidget(paramWidget);
}


void ResultWindow::setModelDescription(const Analysis& analysis)
{
  if (analysis.getHtmlDescription().empty())
    return;

  QString modelString = "<!DOCTYPE html>\n<html>\n";
  // get model html description
  modelString += analysis.getHtmlDescription().c_str();

  // translate the description
  const QStringList beginMarker = QStringList() << "<h3>" << "<center>" << "<th>";
  const QStringList endMarker = QStringList() << "</h3>" << "</center>" << "</th>";

  for (int i = 0; i < beginMarker.size(); ++i)
  {
    int j = 0;
    while ((j = modelString.indexOf(beginMarker[i], j)) != -1)
    {
      const int endPos = modelString.indexOf(endMarker[i], j);
      const int markerSize = beginMarker[i].size();
      // get the word between the markers
      const QString subString = modelString.mid(j + markerSize, endPos - j - markerSize);
      // replace the word by its translation
      modelString.replace(j + markerSize, endPos - j - markerSize, TranslationManager::GetTranslatedParameterName(subString.toStdString()));
      ++j;
    }
  }
  modelString += "</body>\n</html>\n";

  modelDescriptionWidget_ = new QWidget;
  QVBoxLayout * widgetLayout = new QVBoxLayout(modelDescriptionWidget_);
  QTextEdit * edit = new QTextEdit;
  edit->setReadOnly(true);
  edit->setHtml(modelString);
  widgetLayout->addWidget(edit);
}
}
