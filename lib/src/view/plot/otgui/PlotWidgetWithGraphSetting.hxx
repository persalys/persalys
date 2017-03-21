//                                               -*- C++ -*-
/**
 *  @brief QWidget to display Parameters in a table
 *
 *  Copyright 2015-2017 EDF-Phimeca
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
#ifndef OTGUI_PLOTWIDGETWITHGRAPHSETTING_HXX
#define OTGUI_PLOTWIDGETWITHGRAPHSETTING_HXX

#include "otgui/PlotWidget.hxx"
#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <QVBoxLayout>

namespace OTGUI {
class OTGUI_API PlotWidgetWithGraphSetting : public QWidget
{
  Q_OBJECT

public:
  PlotWidgetWithGraphSetting(const int indexWidget,
                             const QVector<PlotWidget*> plotWidgets,
                             const GraphConfigurationWidget::Type plotType=GraphConfigurationWidget::NoType,
                             QWidget * parent=0
                            )
    : QWidget(parent)
    , indexWidget_(indexWidget)
    , graphConfigWidget_(new GraphConfigurationWidget(plotWidgets, QStringList(), QStringList(), plotType))
  {
    QVBoxLayout * widgetLayout = new QVBoxLayout(this);
    if (plotWidgets.size() == 1)
      widgetLayout->addWidget(plotWidgets[0]);
    else if (plotWidgets.size() == 2)
    {
      ResizableStackedWidget * stackedWidget = new ResizableStackedWidget;
      stackedWidget->addWidget(plotWidgets[0]);
      stackedWidget->addWidget(plotWidgets[1]);
      widgetLayout->addWidget(stackedWidget);
      connect(graphConfigWidget_, SIGNAL(currentPlotChanged(int)), stackedWidget, SLOT(setCurrentIndex(int)));
    }
  }


  virtual ~PlotWidgetWithGraphSetting()
  {
    delete graphConfigWidget_;
    graphConfigWidget_ = 0;
  }


public slots:
  void showHideGraphConfigurationWidget(int index)
  {
    if (index == indexWidget_ && isVisible())
      emit graphWindowActivated(graphConfigWidget_);
  }


signals:
  void graphWindowActivated(QWidget*);
  void currentPlotChanged(int);

private:
  int indexWidget_;
  GraphConfigurationWidget * graphConfigWidget_;
};
}
#endif