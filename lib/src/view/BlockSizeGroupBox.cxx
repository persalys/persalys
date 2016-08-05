//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for block size parameter
 *
 *  Copyright 2015-2016 EDF-Phimeca
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
#include "otgui/BlockSizeGroupBox.hxx"

#include "otgui/UIntSpinBox.hxx"

#include <QLabel>
#include <QHBoxLayout>

#include <limits>

namespace OTGUI {

BlockSizeGroupBox::BlockSizeGroupBox(const QString &title, QWidget* parent)
  : QGroupBox(title, parent)
{
  QHBoxLayout * layout = new QHBoxLayout(this);
  QLabel * blockSizeLabel = new QLabel(tr("Block size"));
  layout->addWidget(blockSizeLabel);
  blockSizeSpinbox_ = new UIntSpinBox;
  blockSizeSpinbox_->setMinimum(1);
  blockSizeSpinbox_->setMaximum(std::numeric_limits<int>::max());
  blockSizeSpinbox_->setSingleStep(100);
  blockSizeLabel->setBuddy(blockSizeSpinbox_);
  layout->addWidget(blockSizeSpinbox_);

  connect(blockSizeSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(blockSizeChanged(double)));
}


OT::UnsignedInteger BlockSizeGroupBox::getBlockSizeValue() const
{
  return (OT::UnsignedInteger) blockSizeSpinbox_->value();
}


void BlockSizeGroupBox::setBlockSizeValue(const OT::UnsignedInteger value)
{
  blockSizeSpinbox_->setValue(value);
}
}