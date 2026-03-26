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
#include "persalys/InstallPackageDialog.hxx"

#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLabel>
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>

namespace PERSALYS
{

InstallPackageDialog::InstallPackageDialog(QWidget *parent)
  : QDialog(parent)
  , packageNameEdit_(new QLineEdit(this))
  , installButton_(nullptr)
{
  setWindowTitle(tr("Install a package"));
  setWindowIcon(QIcon(":/images/persalys.ico"));
  setMinimumWidth(420);

  auto * mainLayout = new QVBoxLayout(this);
  mainLayout->setSpacing(12);

  // --- description ---
  auto * descLabel = new QLabel(
    tr("Install a Python package into the Persalys environment using pip.\n"
       "The installation progress will be shown in the Python Console."),
    this);
  descLabel->setWordWrap(true);
  mainLayout->addWidget(descLabel);

  // --- package name ---
  auto * formLayout = new QFormLayout;
  formLayout->setFieldGrowthPolicy(QFormLayout::ExpandingFieldsGrow);
  formLayout->addRow(tr("Package name:"), packageNameEdit_);
  packageNameEdit_->setPlaceholderText(tr("e.g. requests, numpy==1.26.0"));
  mainLayout->addLayout(formLayout);

  // --- buttons ---
  auto * buttonBox = new QDialogButtonBox(
    QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
  installButton_ = buttonBox->button(QDialogButtonBox::Ok);
  installButton_->setText(tr("Install"));
  installButton_->setEnabled(false);
  mainLayout->addWidget(buttonBox);

  connect(packageNameEdit_, SIGNAL(textChanged(QString)), this, SLOT(updateInstallButton()));
  connect(buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttonBox, SIGNAL(rejected()), this, SLOT(reject()));
}


void InstallPackageDialog::updateInstallButton()
{
  installButton_->setEnabled(!packageNameEdit_->text().trimmed().isEmpty());
}


QString InstallPackageDialog::getInstallCommand() const
{
  // Sanitise the package spec: strip surrounding whitespace
  const QString pkg = packageNameEdit_->text().trimmed();

  // Build a safe Python one-liner executed via execAndWait.
  // * subprocess.run is used so that stdout/stderr are captured and forwarded
  //   to the Python Console for the user to follow the progress.
  // * The package name is embedded as a Python string literal; single-quotes
  //   inside the name are escaped to avoid injection.
  QString safePkg = pkg;
  safePkg.replace("'", "\\'");

  QStringList pipArgs;
  // On Windows sys.executable is persalys.exe, not python.exe.
  // sys.prefix points to the root of the embedded Python distribution
  // (i.e. $INSTDIR) where python.exe lives, so we use that on Windows.
  pipArgs << "_py" << "'-m'" << "'pip'" << "'install'" << "'--user'";
  pipArgs << QString("'%1'").arg(safePkg);

  // Full command printed to the console, then result summary
  const QString cmd =
    QString("import subprocess, sys, os; "
            "_py = 'pythonw.exe' if sys.platform == 'win32' else sys.executable; "
            "_proc = subprocess.run([%1], capture_output=True, text=True); "
            "print(_proc.stdout, end=''); "
            "print(_proc.stderr, end=''); "
            "print('\\nPackage installed successfully.' if _proc.returncode == 0 "
            "else '\\nInstallation failed (exit code %d).' % _proc.returncode)")
    .arg(pipArgs.join(", "));

  return cmd;
}

}
