//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for min/max table
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
#ifndef OTGUI_MINMAXTABLEGROUPBOX_HXX
#define OTGUI_MINMAXTABLEGROUPBOX_HXX

#include "otgui/DataSample.hxx"
#include "otgui/ResizableStackedWidget.hxx"

#include <QGroupBox>

namespace OTGUI {
class OTGUI_API MinMaxTableGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  MinMaxTableGroupBox(const DataSample& result, const bool onlyOutput=true, QWidget* parent=0);

protected:
  QWidget * getForOutputMinMaxTableView(const DataSample & result, const OT::UnsignedInteger outputIndex);
  QWidget * getForInputMinMaxTableView(const DataSample & result, const OT::UnsignedInteger inputIndex);

public slots:
  void setCurrentIndexStackedWidget(int index);

private:
  ResizableStackedWidget * stackedWidget_;
  bool onlyOutput_;
};
}
#endif