//                                               -*- C++ -*-
/**
 *  @brief QWidget for the inference results
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
#ifndef OTGUI_INFERENCERESULTWIDGET_HXX
#define OTGUI_INFERENCERESULTWIDGET_HXX

#include "otgui/InferenceResult.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/ResizableTableViewWithoutScrollBar.hxx"
#include "otgui/PlotWidget.hxx"

#include <QTabWidget>
#include <QLabel>

Q_DECLARE_METATYPE(OT::Distribution)

namespace OTGUI {
class OTGUI_API InferenceResultWidget : public QWidget
{
  Q_OBJECT

public:
  InferenceResultWidget(const bool displayPDF_QQPlot=true, QWidget* parent=0);

  OT::Distribution getDistribution() const;
  bool isSelectedDistributionValid() const;

protected:
  void buildInterface();

public slots:
  void updateDistributionTable(const InferenceResult& result, const QString& variableName);
  void updateRadioButtonsDistributionTable(QModelIndex);
  void updateParametersTable(QModelIndex=QModelIndex());
  void updateGraphs(QModelIndex=QModelIndex());
signals:
  void currentDistributionChanged();

private:
  bool displayPDF_QQPlot_;
  QTabWidget * tabWidget_;
  FittingTestResult currentFittingTestResult_;
  ResizableTableViewWithoutScrollBar * distTableView_;
  CustomStandardItemModel * distTableModel_;
  ResizableTableViewWithoutScrollBar * distParamTableView_;
  CustomStandardItemModel * distParamTableModel_;
  QLabel * analysisErrorMessageLabel_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  PlotWidget * qqPlot_;
};
}
#endif
