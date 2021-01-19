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

    QLabel * nThreadsLabel = new QLabel(tr("Number of processes"));
    nThreadsLabel->setToolTip("0: "+tr("all cores"));
    tabLayout->addWidget(nThreadsLabel, 0, 0);

    QSpinBox * nThreadsSpinBox = new QSpinBox;
    nThreadsSpinBox->setToolTip("0: "+tr("all cores"));
    nThreadsSpinBox->setMinimum(0);
    nThreadsSpinBox->setMaximum(std::thread::hardware_concurrency());
    QSettings settings;
    nThreadsSpinBox->setValue(settings.value("nThreads").toUInt());
    tabLayout->addWidget(nThreadsSpinBox, 0, 1);

    tabWidget->addTab(tab, tr("&General"));
    mainLayout->addWidget(tabWidget);
    QDialogButtonBox * settingsButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(settingsButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(settingsButtons, &QDialogButtonBox::accepted, [=] {
        setnThreads(nThreadsSpinBox->value());
        emit QDialog::accepted();
      });
    connect(settingsButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    mainLayout->addWidget(settingsButtons);
    setLayout(mainLayout);
  }

  void SettingsDialog::setnThreads(const UnsignedInteger n) {
    nTrheads_ = n;
  }

  UnsignedInteger SettingsDialog::getnThreads() const {
    return nTrheads_;
  }



}
