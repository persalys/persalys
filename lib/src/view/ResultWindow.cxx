//                                               -*- C++ -*-
/**
 *  @brief Base class QMdiSubWindow for the results
 *
 *  Copyright 2015-2026 EDF-Phimeca
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

/** Format a single limit-state event as "outputName op threshold". */
QString ResultWindow::FormatLimitStateEntry(const OT::String & outputName,
                                     const OT::ComparisonOperator & op,
                                     double threshold)
{
  QString opStr;
  const OT::String opName = op.getImplementation()->getClassName();
  if      (opName == "LessOrEqual")    opStr = "<=";
  else if (opName == "Greater")        opStr = ">";
  else if (opName == "GreaterOrEqual") opStr = ">=";
  else                                 opStr = "<";
  return QString("%1 %2 %3")
    .arg(QString::fromUtf8(outputName.c_str()), opStr, QString::number(threshold));
}

ResultWindow::ResultWindow(Item * item, QWidget * parent)
  : SubWindow(item, parent)
{
  // model description widget
  if (item)
  {
    const auto * analysisItem = dynamic_cast<AnalysisItem*>(item);
    if (analysisItem)
      ResultWindow::setModelDescription(analysisItem->getAnalysis());
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
  if (analysis.getHTMLDescription().empty())
    return;

  QString modelString = "<!DOCTYPE html>\n<html>\n";
  // get model html description
  modelString += analysis.getHTMLDescription().c_str();

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
