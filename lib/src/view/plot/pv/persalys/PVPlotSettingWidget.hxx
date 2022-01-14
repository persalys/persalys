//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure paraview plots
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_PVPLOTSETTINGWIDGET_HXX
#define PERSALYS_PVPLOTSETTINGWIDGET_HXX

#include "PVViewWidget.hxx"

#include <QGridLayout>

namespace PERSALYS
{

class PERSALYS_PLOTPV_API PVPlotSettingWidget : public QWidget
{
  Q_OBJECT

public:
  PVPlotSettingWidget(PVViewWidget* pvViewWidget, QWidget* parent = 0);

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

protected:
  void addSelectDataWidget(const QString &labelName, const QList<bool> &checked = QList<bool>());
  void addRankWidget(const bool checkState);
  void addExportLayout();

protected:
  PVViewWidget * pvViewWidget_;
  QStringList plotNames_;
  OT::Collection<OT::Sample> samples_;
  OT::Collection<OT::Sample> rankSamples_;
  QGridLayout * frameLayout_;
};

class PERSALYS_PLOTPV_API MultiPlotSettingWidget : public PVPlotSettingWidget
{
  Q_OBJECT

public:
  MultiPlotSettingWidget(PVViewWidget* pvViewWidget, const OT::Sample& sample, const OT::Sample& sampleRank, QWidget* parent = 0);
};
}
#endif
