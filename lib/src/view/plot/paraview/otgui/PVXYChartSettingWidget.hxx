//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure paraview chart
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
#ifndef OTGUI_PVXYCHARTSETTINGWIDGET_HXX
#define OTGUI_PVXYCHARTSETTINGWIDGET_HXX

#include "otgui/PVXYChartViewWidget.hxx"
#include "otgui/ValueLineEdit.hxx"
#include "otgui/ListWidgetWithCheckBox.hxx"

#include <QComboBox>
#include <QButtonGroup>
#include <QCheckBox>

namespace OTGUI {
class OTGUI_API PVXYChartSettingWidget : public QWidget
{
  Q_OBJECT

public:
  enum Type {NoType, Scatter};

  PVXYChartSettingWidget(PVXYChartViewWidget * pvViewWidget,
                         const OT::Sample& sample,
                         const OT::Sample& sampleRank,
                         QStringList inputNames=QStringList(),
                         QStringList outputNames=QStringList(),
                         Type plotType=NoType,
                         QWidget * parent=0);

  PVXYChartSettingWidget(PVXYChartViewWidget * pvViewWidget,
                         const OT::Sample& sample,
                         const OT::Sample& sampleRank,
                         const OT::Sample& failedSample,
                         const OT::Sample& failedSampleRank,
                         const OT::Sample& notEvalSample,
                         const OT::Sample& notEvalSampleRank,
                         Type plotType=NoType,
                         QWidget * parent=0);

  int getCurrentPlotIndex() const;

protected:
  void buildInterface();

public slots:
  void modifyData(bool);
  void updateLineEdits();
  void updateYComboBox();
  void plotChanged();
  void updateTitle();
  void updateXLabel();
  void updateYLabel();
  void updateXrange();
  void updateYrange();
  void setColor();
  void setMarkerStyle(const int);
  void setRepresentationToDisplay();
signals:
  void currentPlotChanged(QString varX, QString varY);

private:
  PVXYChartViewWidget * pvViewWidget_;
  OT::Sample sample_;
  OT::Sample sampleRank_;
  OT::Sample failedSample_;
  OT::Sample failedSampleRank_;
  OT::Sample notEvalSample_;
  OT::Sample notEvalSampleRank_;
  QStringList inputNames_;
  QStringList outputNames_;
  Type plotType_;
  QComboBox * xAxisComboBox_;
  QComboBox * yAxisComboBox_;
  QLineEdit * titleLineEdit_;
  QLineEdit * xlabelLineEdit_;
  ValueLineEdit * xmin_;
  ValueLineEdit * xmax_;
  QCheckBox * xlogScaleCheckBox_;
  QLineEdit * ylabelLineEdit_;
  ValueLineEdit * ymin_;
  ValueLineEdit * ymax_;
  QCheckBox * ylogScaleCheckBox_;
  QToolButton * colorButton_;
  QComboBox * markerStyles_;
  ListWidgetWithCheckBox * reprListWidget_;
};
}
#endif
