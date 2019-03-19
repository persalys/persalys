//                                               -*- C++ -*-
/**
 *  @brief QWidget for the inference results
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef OTGUI_INFERENCERESULTWIDGET_HXX
#define OTGUI_INFERENCERESULTWIDGET_HXX

#include "otgui/InferenceResult.hxx"
#include "otgui/CustomStandardItemModel.hxx"
#include "otgui/ExportableTableView.hxx"
#include "otgui/PlotWidget.hxx"
#include "otgui/DocumentationToolButton.hxx"
#include "otgui/TemporaryLabel.hxx"

#include <QTabWidget>
#include <QScrollArea>

Q_DECLARE_METATYPE(OT::Distribution)

namespace OTGUI
{
class OTGUI_API InferenceResultWidget : public QScrollArea
{
  Q_OBJECT

public:
  InferenceResultWidget(const bool displayPDF_QQPlot = true, QWidget* parent = 0);

  OT::Distribution getDistribution() const;
  bool isSelectedDistributionValid() const;

protected:
  void buildInterface();

public slots:
  void updateDistributionTable(const double level, const InferenceResult& result, const QString& variableName);
  void updateRadioButtonsDistributionTable(QModelIndex);
  void updateParametersTable(QModelIndex = QModelIndex());
  void updateGraphs(QModelIndex = QModelIndex());
  void openUrl();
signals:
  void currentDistributionChanged();

private:
  bool displayPDF_QQPlot_;
  QTabWidget * tabWidget_;
  FittingTestResult currentFittingTestResult_;
  ExportableTableView * distTableView_;
  CustomStandardItemModel * distTableModel_;
  CopyableTableView * distParamTableView_;
  CustomStandardItemModel * distParamTableModel_;
  DocumentationToolButton * infoButton_;
  TemporaryLabel * analysisErrorMessageLabel_;
  PlotWidget * pdfPlot_;
  PlotWidget * cdfPlot_;
  PlotWidget * qqPlot_;
};
}
#endif
