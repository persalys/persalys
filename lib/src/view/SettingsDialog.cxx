//                                               -*- C++ -*-
/**
 *  @brief QDialog
 *
 *  Copyright 2015-2021 EDF-Phimeca
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

#include "persalys/SettingsDialog.hxx"
#include "persalys/SubWindow.hxx"

#include <QVBoxLayout>
#include <QSettings>
#include <QGridLayout>
#include <QSpinBox>
#include <QTabWidget>
#include <QDialogButtonBox>

#include <thread>

using namespace OT;

namespace PERSALYS
{

  SettingsDialog::SettingsDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowIcon(QIcon(":/images/run-build.png"));
    setWindowTitle(tr("Persalys settings"));

    QVBoxLayout * mainLayout = new QVBoxLayout;

    // header with logo, name and version
    QFrame * headerFrame = new QFrame;
    headerFrame->setFrameShape(QFrame::StyledPanel);

    QHBoxLayout * headerLayout = new QHBoxLayout;
    QLabel * logoLabel = new QLabel;
    logoLabel->setPixmap(QPixmap(":/images/Ps-icon-64.png"));
    headerLayout->addWidget(logoLabel);

    QLabel * phibootLabel = new QLabel("<font size=\"6\">Persalys</font><br>" + tr("Version %1").arg(PERSALYS_VERSION));
    headerLayout->addWidget(phibootLabel);
    headerLayout->addStretch();

    headerFrame->setLayout(headerLayout);
    mainLayout->addWidget(headerFrame);

    // tab widget
    QTabWidget * tabWidget = new QTabWidget;
    QWidget * tab = new QWidget;

    QGridLayout * tabLayout = new QGridLayout(tab);

    QLabel * nProcessesLabel = new QLabel(tr("Number of processes"));
    nProcessesLabel->setToolTip("0: "+tr("all cores"));
    tabLayout->addWidget(nProcessesLabel, 0, 0);

    QSpinBox * nProcessesSpinBox = new QSpinBox;
    nProcessesSpinBox->setToolTip("0: "+tr("all cores"));
    nProcessesSpinBox->setMinimum(0);
    nProcessesSpinBox->setMaximum(std::thread::hardware_concurrency());
    QSettings settings;
    nProcessesSpinBox->setValue(settings.value("nProcesses").toUInt());
    tabLayout->addWidget(nProcessesSpinBox, 0, 1);

    tabWidget->addTab(tab, tr("&General"));
    mainLayout->addWidget(tabWidget);
    QDialogButtonBox * settingsButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(settingsButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(settingsButtons, &QDialogButtonBox::accepted, [=] {
        setnProcesses(nProcessesSpinBox->value());
        emit QDialog::accepted();
      });
    connect(settingsButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(settingsButtons);
    setLayout(mainLayout);
  }

  void SettingsDialog::setnProcesses(const UnsignedInteger n) {
    processNumber_ = n;
  }

  UnsignedInteger SettingsDialog::getnProcesses() const {
    return processNumber_;
  }



}
