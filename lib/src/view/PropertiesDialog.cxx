//                                               -*- C++ -*-
/**
 *  @brief QDialog
 *
 *  Copyright 2015-2024 EDF-Phimeca
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

#include "persalys/PropertiesDialog.hxx"

#include <QVBoxLayout>
#include <QDialogButtonBox>

using namespace OT;

namespace PERSALYS
{

  PropertiesDialog::PropertiesDialog(QWidget* parent)
    : QDialog(parent)
  {
    QVBoxLayout * mainLayout = new QVBoxLayout;
    processNumberSpinBox_ = new ProcessNumberSpinBox;
    mainLayout->addWidget(processNumberSpinBox_);

    QDialogButtonBox * settingsButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(settingsButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(settingsButtons, &QDialogButtonBox::accepted, [ = ]
    {
      setProcessNumber(processNumberSpinBox_->getProcessNumber());
      emit QDialog::accepted();
    });
    connect(settingsButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(settingsButtons);
    setLayout(mainLayout);
  }
}
