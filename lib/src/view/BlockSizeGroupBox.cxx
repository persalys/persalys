//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for block size parameter
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#include "otgui/BlockSizeGroupBox.hxx"

#include <QLabel>
#include <QGridLayout>

using namespace OT;

namespace OTGUI
{

BlockSizeGroupBox::BlockSizeGroupBox(const QString &title, const bool withReplicationSize, QWidget* parent)
  : QGroupBox(title, parent)
{
  QGridLayout * layout = new QGridLayout(this);

  if (withReplicationSize)
  {
    QLabel * replicationSizeLabel = new QLabel(tr("Replication size"));
    layout->addWidget(replicationSizeLabel, 0, 0);
    replicationSizeSpinbox_ = new LogSpinBox;
    replicationSizeSpinbox_->setMinimum(2.);
    layout->addWidget(replicationSizeSpinbox_, 0, 1);
    connect(replicationSizeSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(replicationSizeChanged(double)));
  }

  QLabel * blockSizeLabel = new QLabel(tr("Block size"));
  layout->addWidget(blockSizeLabel, 1, 0);
  blockSizeSpinbox_ = new UIntSpinBox;
  blockSizeLabel->setBuddy(blockSizeSpinbox_);
  layout->addWidget(blockSizeSpinbox_, 1, 1);

  connect(blockSizeSpinbox_, SIGNAL(valueChanged(double)), this, SIGNAL(blockSizeChanged(double)));
}


UnsignedInteger BlockSizeGroupBox::getBlockSizeValue() const
{
  return (UnsignedInteger) blockSizeSpinbox_->value();
}


void BlockSizeGroupBox::setBlockSizeValue(const UnsignedInteger value)
{
  blockSizeSpinbox_->setValue(value);
}


UnsignedInteger BlockSizeGroupBox::getReplicationSizeValue() const
{
  return (UnsignedInteger) replicationSizeSpinbox_->value();
}


void BlockSizeGroupBox::setReplicationSizeValue(const UnsignedInteger value)
{
  replicationSizeSpinbox_->setValue(value);
}
}
