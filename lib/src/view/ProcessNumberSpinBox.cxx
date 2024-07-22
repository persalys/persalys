//                                               -*- C++ -*-
/**
 *  @brief QWidget to set model process number
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

#include "persalys/ProcessNumberSpinBox.hxx"
#include "persalys/UIntSpinBox.hxx"
#include "persalys/BaseTools.hxx"

#include <QHBoxLayout>

using namespace OT;

namespace PERSALYS
{

  ProcessNumberSpinBox::ProcessNumberSpinBox(QWidget *parent)
    : QWidget(parent)
  {
    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    label_ = new QLabel(tr("Number of processes"));
    mainLayout->addWidget(label_);
    spinBox_ = new UIntSpinBox(this);
    spinBox_->setMinimum(1);
    spinBox_->setMaximum(GetNumberOfPhysicalCores()*2);
    mainLayout->addWidget(spinBox_);

    connect(spinBox_, QOverload<double>::of(&UIntSpinBox::valueChanged), [ = ](double n) {
      emit valueChanged((UnsignedInteger)n);
    });
  }

  void ProcessNumberSpinBox::setTitle(const QString& title)
  {
    label_->setText(title);
  }

  void ProcessNumberSpinBox::setProcessNumber(const UnsignedInteger& n)
  {
    spinBox_->setValue(n);
  }

  void ProcessNumberSpinBox::setToolTip(const QString& text)
  {
    label_->setToolTip(text);
    spinBox_->setToolTip(text);
  }

  UnsignedInteger ProcessNumberSpinBox::getProcessNumber() const
  {
    return spinBox_->value();
  }

}
