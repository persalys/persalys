//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure graphics
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
#ifndef PERSALYS_GRAPHCONFIGURATIONWIDGET_HXX
#define PERSALYS_GRAPHCONFIGURATIONWIDGET_HXX

#include "persalys/BoxPlot.hxx"
#include "persalys/ValueLineEdit.hxx"
#include "persalys/ListWidgetWithCheckBox.hxx"

#include <QGridLayout>
#include <QComboBox>
#include <QCheckBox>

namespace PERSALYS
{

class PERSALYS_PLOT_API GraphConfigurationWidget : public QWidget
{
  Q_OBJECT

public:
  GraphConfigurationWidget(const QVector<PlotWidget *> &plotWidgets, QWidget *parent = 0);

  int getCurrentPlotIndex() const;

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

protected:
  void addXYAxisTabs(const bool xAxisWithLabels = false);
  void addExportLayout();

public slots:
  virtual void updateLineEdits();
  virtual void currentPlotIndexChanged(int i = 0);
  void updateRange(QwtPlot::Axis);
signals:
  void currentPlotChanged(int i = 0);

protected:
  QVector<PlotWidget *> plotWidgets_;
  int plotIndex_;
  QGridLayout * frameLayout_;
  QTabWidget * propertiesTabWidget_;
  QLineEdit * titleLineEdit_;
  QLineEdit * axisLabelLineEdit_[2];
  ValueLineEdit * axisMinValueLineEdit_[2];
  ValueLineEdit * axisMaxValueLineEdit_[2];
};


class PERSALYS_PLOT_API SimpleGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  SimpleGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, QWidget *parent = 0);
  SimpleGraphSetting(PlotWidget *plotWidget, QWidget *parent = 0);
};


class PERSALYS_PLOT_API ScatterGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  ScatterGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, const QStringList &outputNames, QWidget *parent = 0);
public slots:
  void updateYComboBox();
  virtual void currentPlotIndexChanged(int i = 0);
private:
  QComboBox * xAxisComboBox_;
  QComboBox * yAxisComboBox_;
  QCheckBox * rankCheckBox_;
};


class PERSALYS_PLOT_API PDFGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  enum PDFType {Distribution, Result, Copula, Ksi};
  PDFGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, const PDFType type, QWidget *parent = 0);
  PDFGraphSetting(const QVector<PlotWidget *> &plotWidgets, const PDFType type, QWidget *parent = 0);
public slots:
  void updateYComboBox();
  virtual void currentPlotIndexChanged(int i = 0);
private:
  QComboBox * xAxisComboBox_;
  QComboBox * yAxisComboBox_;
  QComboBox * reprComboBox_;
};


class PERSALYS_PLOT_API BoxPlotGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  BoxPlotGraphSetting(BoxPlot *plotWidget, const QStringList &inputNames, const QList<bool> & checked, QWidget *parent = 0);
};


class PERSALYS_PLOT_API SensitivityIndicesGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  SensitivityIndicesGraphSetting(PlotWidget *plotWidget, QWidget *parent = 0);
};

class PERSALYS_PLOT_API FrontsGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  FrontsGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &outputNames, QWidget *parent = 0);
public slots:
  void updateYComboBox();
  virtual void currentPlotIndexChanged(int i = 0);
private:
  QComboBox * xAxisComboBox_;
  QComboBox * yAxisComboBox_;

};
}
#endif
