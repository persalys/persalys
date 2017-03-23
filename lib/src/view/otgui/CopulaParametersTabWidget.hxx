//                                               -*- C++ -*-
/**
 *  @brief QTabWidget for the parameters and PDF/CDF copula
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
#ifndef OTGUI_COPULAPARAMETERSTABWIDGET_HXX
#define OTGUI_COPULAPARAMETERSTABWIDGET_HXX

#include "otgui/GraphConfigurationWidget.hxx"
#include "otgui/DataSample.hxx"

#include <QTabWidget>
#include <QLabel>

namespace OTGUI {
class OTGUI_API CopulaParametersTabWidget : public QTabWidget
{
  Q_OBJECT

public:
  CopulaParametersTabWidget(const OT::Distribution& distribution,
                            const OT::NumericalSample& sample,
                            const DataSample::NumericalSampleCollection& kendallPlotData,
                            QWidget* parent = 0);

protected:
  void buildInterface();

public slots:
  void stateChanged();  
  void showHideGraphConfigurationWidget(int index);

signals:
  void graphWindowActivated(QWidget*);
  void graphWindowDeactivated();

private:
  OT::Distribution distribution_;
  OT::NumericalSample sample_;
  OT::PersistentCollection<OT::NumericalSample> kendallPlotData_;
  GraphConfigurationWidget * pdf_cdfPlotGraphConfigWidget_;
  GraphConfigurationWidget * kendallPlotGraphConfigWidget_;
  QLabel * analysisErrorMessageLabel_;
};
}
#endif