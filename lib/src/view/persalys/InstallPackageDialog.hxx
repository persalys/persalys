//                                               -*- C++ -*-
/**
 *  @brief Dialog to install a Python package into the Persalys environment
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#ifndef PERSALYS_INSTALLPACKAGEDIALOG_HXX
#define PERSALYS_INSTALLPACKAGEDIALOG_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QDialog>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

namespace PERSALYS
{
/**
 * @brief Dialog for installing a Python package into the Persalys environment.
 *
 * Prompts the user for a package name and optionally installs in user mode
 * (recommended when Persalys was installed without administrator rights on
 * Windows, or to keep the package in a persistent per-user directory on Linux).
 */
class PERSALYS_VIEW_API InstallPackageDialog : public QDialog
{
  Q_OBJECT

public:
  explicit InstallPackageDialog(QWidget *parent = nullptr);

  /**
   * @brief Returns the Python one-liner that installs the requested package.
   *
   * The command uses subprocess so that output is forwarded to the Python
   * console and the user can follow the installation progress.
   */
  QString getInstallCommand() const;

private slots:
  void updateInstallButton();

private:
  QLineEdit * packageNameEdit_;
  QPushButton * installButton_;
};
}
#endif
