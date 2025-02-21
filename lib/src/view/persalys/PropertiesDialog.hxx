//                                               -*- C++ -*-
/**
 *  @brief QDialog
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
#ifndef PERSALYS_PROPERTIESDIALOG_HXX
#define PERSALYS_PROPERTIESDIALOG_HXX

#include "persalys/PersalysPrivate.hxx"
#include "persalys/ProcessNumberSpinBox.hxx"

#include <openturns/OTType.hxx>

#include <QDialog>

namespace PERSALYS
{
  class PERSALYS_VIEW_API PropertiesDialog : public QDialog
  {
    Q_OBJECT

  public:
    PropertiesDialog(QWidget* parent = 0);

    void setProcessNumber(const OT::UnsignedInteger n) { processNumberSpinBox_->setProcessNumber(n);};
    OT::UnsignedInteger getProcessNumber() const {return processNumberSpinBox_->getProcessNumber();};

  private:
    ProcessNumberSpinBox * processNumberSpinBox_ = nullptr;
  };

}
#endif
