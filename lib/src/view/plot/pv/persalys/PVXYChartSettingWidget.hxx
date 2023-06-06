//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure paraview chart
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_PVXYCHARTSETTINGWIDGET_HXX
#define PERSALYS_PVXYCHARTSETTINGWIDGET_HXX

#include "persalys/PVPlotSettingWidget.hxx"
#include "persalys/PVXYChartViewWidget.hxx"
#include "persalys/ValueLineEdit.hxx"

#include <QComboBox>
#include <QCheckBox>
#include <QToolButton>

namespace PERSALYS
{
class PERSALYS_PLOTPV_API PVXYChartSettingWidget : public PVPlotSettingWidget
{
  Q_OBJECT

public:
  PVXYChartSettingWidget(PVXYChartViewWidget * pvViewWidget,
                         const QStringList &outputNames,
                         const bool isScatter,
                         QWidget *parent = nullptr);

protected:
  void addXYAxisTabs();
  void addMarkerTab(const bool canChangeColor);

public slots:
  void updateLineEdits();
  void setColor();

protected:
  PVXYChartViewWidget * pvXYViewWidget_;
  QTabWidget * propertiesTabWidget_;
  QComboBox * xAxisComboBox_;
  QComboBox * yAxisComboBox_;
private:
  QLineEdit * titleLineEdit_;
  QLineEdit * axisLabelLineEdit_[2];
  ValueLineEdit * axisMinValueLineEdit_[2];
  ValueLineEdit * axisMaxValueLineEdit_[2];
  QCheckBox * axisLogScaleCheckBox_[2];
  QToolButton * colorButton_;
};


class PERSALYS_PLOTPV_API XYChartSettingWidget : public PVXYChartSettingWidget
{
  Q_OBJECT

public:
  XYChartSettingWidget(PVXYChartViewWidget *pvViewWidget, QWidget *parent = nullptr);
};


class PERSALYS_PLOTPV_API BagChartSettingWidget : public PVXYChartSettingWidget
{
  Q_OBJECT

public:
  BagChartSettingWidget(PVXYChartViewWidget *pvViewWidget, QWidget *parent = nullptr);
};


class PERSALYS_PLOTPV_API TrajectoriesSettingWidget : public PVXYChartSettingWidget
{
  Q_OBJECT

public:
  TrajectoriesSettingWidget(PVXYChartViewWidget *pvViewWidget, const QStringList &xAxisNames, const QStringList &dataNames, QWidget *parent = nullptr);
  TrajectoriesSettingWidget(PVXYChartViewWidget *pvViewWidget, const QStringList &dataNames, QWidget *parent = nullptr);
};


class PERSALYS_PLOTPV_API ScatterSettingWidget : public PVXYChartSettingWidget
{
  Q_OBJECT

public:
  ScatterSettingWidget(PVXYChartViewWidget *pvViewWidget,
                       const OT::Collection<OT::Sample> &samples,
                       const OT::Collection<OT::Sample> &rankSamples,
                       const QStringList &inputNames,
                       const QStringList &outputNames,
                       QWidget *parent = nullptr);

  ScatterSettingWidget(PVXYChartViewWidget *pvViewWidget,
                       const OT::Sample &sample,
                       const OT::Sample &rankSample,
                       const QStringList &inputNames,
                       const QStringList &outputNames,
                       QWidget *parent = nullptr);

  ScatterSettingWidget(PVXYChartViewWidget *pvViewWidget,
                       const OT::Collection<OT::Sample> &samples,
                       const OT::Collection<OT::Sample> &rankSamples,
                       QWidget *parent = nullptr);

public slots:
  void updateYComboBox();
};


class PERSALYS_PLOTPV_API MultiPDFSettingWidget : public PVXYChartSettingWidget
{
  Q_OBJECT

public:
  MultiPDFSettingWidget(PVXYChartViewWidget *pvViewWidget,
                       const QStringList &inputNames,
                       const QStringList &outputNames,
                       QWidget *parent = nullptr);
};
}
#endif
