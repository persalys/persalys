//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure graphics
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
  explicit GraphConfigurationWidget(const QVector<PlotWidget *> &plotWidgets, QWidget *parent = nullptr);

  int getCurrentPlotIndex() const;

  QSize sizeHint() const override;
  QSize minimumSizeHint() const override;

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
  QVector<PlotWidget *>         plotWidgets_;
  int                           plotIndex_             = 0;
  QGridLayout                   * frameLayout_         = nullptr;
  QTabWidget                    * propertiesTabWidget_ = nullptr;
  QLineEdit                     * titleLineEdit_       = nullptr;
  std::array<QLineEdit*, 2>     axisLabelLineEdit_     = {nullptr, nullptr};
  std::array<ValueLineEdit*, 2> axisMinValueLineEdit_  = {nullptr, nullptr};
  std::array<ValueLineEdit*, 2> axisMaxValueLineEdit_  = {nullptr, nullptr};
};


class PERSALYS_PLOT_API SimpleGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  SimpleGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, QWidget *parent = nullptr);
  explicit SimpleGraphSetting(PlotWidget *plotWidget, QWidget *parent = nullptr);
};


class PERSALYS_PLOT_API ScatterGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  ScatterGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, const QStringList &outputNames, QWidget *parent = nullptr);
public slots:
  void updateYComboBox();
  void currentPlotIndexChanged(int i = 0) override;
private:
  QComboBox * xAxisComboBox_  = nullptr;
  QComboBox * yAxisComboBox_  = nullptr;
  QCheckBox * rankCheckBox_   = nullptr;
};


class PERSALYS_PLOT_API PDFGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  enum PDFType {Distribution, Copula, Ksi};
  PDFGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &inputNames, const PDFType type, QWidget *parent = nullptr);
  PDFGraphSetting(const QVector<PlotWidget *> &plotWidgets, const PDFType type, QWidget *parent = nullptr);
public slots:
  void updateYComboBox();
  void currentPlotIndexChanged(int i = 0) override;
private:
  QComboBox * xAxisComboBox_  = nullptr;
  QComboBox * yAxisComboBox_  = nullptr;
  QComboBox * reprComboBox_   = nullptr;
};


class PERSALYS_PLOT_API BoxPlotGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  BoxPlotGraphSetting(BoxPlot *plotWidget, const QStringList &inputNames, const QList<bool> & checked, QWidget *parent = nullptr);
};


class PERSALYS_PLOT_API SensitivityIndicesGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  explicit SensitivityIndicesGraphSetting(PlotWidget *plotWidget, QWidget *parent = nullptr);
};

class PERSALYS_PLOT_API FrontsGraphSetting : public GraphConfigurationWidget
{
  Q_OBJECT

public:
  FrontsGraphSetting(const QVector<PlotWidget *> &plotWidgets, const QStringList &outputNames, QWidget *parent = nullptr);
public slots:
  void updateYComboBox();
  void currentPlotIndexChanged(int i = 0) override;
private:
  QComboBox * xAxisComboBox_ = nullptr;
  QComboBox * yAxisComboBox_ = nullptr;

};
}
#endif
