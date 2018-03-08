//                                               -*- C++ -*-
/**
 *  @brief Base class QMdiSubWindow for the results
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
 *  You should have received a copy of the GNU Lesser General Public
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef OTGUI_RESULTWINDOW_HXX
#define OTGUI_RESULTWINDOW_HXX

#include "otgui/OTguiSubWindow.hxx"
#include "otgui/AnalysisItem.hxx"
#include "otgui/ParametersWidget.hxx"

#include <QListWidget>

namespace OTGUI
{

// custom QListWidget for all result windows
class OTGUI_API OTguiListWidget : public QListWidget
{
public:
  OTguiListWidget(QWidget * parent = 0)
    : QListWidget(parent)
  {
    // style sheet
    const QString styleSheet = "QListWidget::item:selected { background-color: #a5d3a1;\
                                                             color: doubledarkgray; }";

    setStyleSheet(styleSheet);
  }
};

// base class for all result windows
class OTGUI_API ResultWindow : public OTguiSubWindow
{
  Q_OBJECT

public:
  ResultWindow(OTguiItem * item, QWidget * parent = 0);

protected:
  virtual void setParameters(const Analysis& analysis, const QString& title);
  virtual void setModelDescription(const Analysis& analysis);
//   QString getModelDescription(const Analysis& analysis);

protected:
  QWidget * parametersWidget_;
  QWidget * modelDescriptionWidget_;
};
}
#endif
