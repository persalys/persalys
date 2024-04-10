//                                               -*- C++ -*-
/**
 *  @brief QGroupBox for block size parameter
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
#ifndef PERSALYS_BLOCKSIZEGROUPBOX_HXX
#define PERSALYS_BLOCKSIZEGROUPBOX_HXX

#include "persalys/LogSpinBox.hxx"

#include <openturns/OTtypes.hxx>

#include <QGroupBox>

namespace PERSALYS
{
class PERSALYS_VIEW_API BlockSizeGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  BlockSizeGroupBox(const QString &title, const bool withReplicationSize = false, QWidget* parent = 0);

  OT::UnsignedInteger getBlockSizeValue() const;
  void setBlockSizeValue(const OT::UnsignedInteger value);

  OT::UnsignedInteger getReplicationSizeValue() const;
  void setReplicationSizeValue(const OT::UnsignedInteger value);

signals:
  void blockSizeChanged(double);
  void replicationSizeChanged(double);

private:
  UIntSpinBox * blockSizeSpinbox_;
  LogSpinBox * replicationSizeSpinbox_;
};
}
#endif
