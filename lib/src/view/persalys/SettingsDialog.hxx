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
#ifndef PERSALYS_PERSALYSDIALOG_HXX
#define PERSALYS_PERSALYSDIALOG_HXX

#include "persalys/PersalysPrivate.hxx"

#include <openturns/OTType.hxx>

#include <QDialog>

namespace PERSALYS
{
class PERSALYS_VIEW_API SettingsDialog : public QDialog
{
  Q_OBJECT

public:
  SettingsDialog(QWidget* parent = 0);

  void setnProcesses(const OT::UnsignedInteger n);
  OT::UnsignedInteger getnProcesses() const;

private:
  OT::UnsignedInteger processNumber_ = 0;
};
}
#endif
