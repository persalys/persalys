//                                               -*- C++ -*-
/**
 *  @brief QWidget to set model process number
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
#ifndef PERSALYS_PROCESSNUMBERSPINBOX_HXX
#define PERSALYS_PROCESSNUMBERSPINBOX_HXX

#include "persalys/UIntSpinBox.hxx"

#include <openturns/OTType.hxx>
#include <QWidget>
#include <QLabel>


namespace PERSALYS
{
  class PERSALYS_VIEW_API ProcessNumberSpinBox : public QWidget
  {
    Q_OBJECT

  public:
    ProcessNumberSpinBox(QWidget *parent=0);
    void setTitle(const QString& title);
    void setToolTip(const QString& text);

  public slots:
    void setProcessNumber(const OT::UnsignedInteger& n);
    OT::UnsignedInteger getProcessNumber() const;

  signals:
    void valueChanged(const OT::UnsignedInteger& n);
  private:
    UIntSpinBox * spinBox_ = nullptr;
    QLabel * label_ = nullptr;
  };
}
#endif
