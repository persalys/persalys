//                                               -*- C++ -*-
/**
 *  @brief QWidget to configure paraview plots
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
#ifndef PERSALYS_PVPLOTSETTINGWIDGET_HXX
#define PERSALYS_PVPLOTSETTINGWIDGET_HXX

#include "persalys/PersalysPrivate.hxx"
#include "PVViewWidget.hxx"

#include <QComboBox>

namespace PERSALYS
{

class PERSALYS_API PVPlotSettingWidget : public QWidget
{
  Q_OBJECT

public:
  PVPlotSettingWidget(PVViewWidget* pvViewWidget, QWidget* parent = 0);
  PVPlotSettingWidget(PVViewWidget* pvViewWidget,
                      const OT::Sample& sample,
                      const OT::Sample& sampleRank,
                      QWidget* parent = 0);

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

protected:
  void buildInterface();

public slots:
  void modifyData(bool);
  void exportPlot();

private:
  PVViewWidget * pvViewWidget_;
  OT::Sample sample_;
  OT::Sample sampleRank_;
};
}
#endif
