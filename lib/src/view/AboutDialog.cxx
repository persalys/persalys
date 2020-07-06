//                                               -*- C++ -*-
/**
 *  @brief QDialog
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#include "persalys/AboutDialog.hxx"

#include "persalys/ParametersTableView.hxx"
#include "persalys/SubWindow.hxx"

#include <openturns/PlatformInfo.hxx>

#include <QFrame>
#include <QLabel>
#include <QIcon>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>

#include <patchlevel.h>
#include <qwt.h>
#ifdef PERSALYS_HAVE_PARAVIEW
#include <vtkPVConfig.h>
#endif
#ifdef PERSALYS_HAVE_YACS
#include <YACS_version.h>
#endif

using namespace OT;

namespace PERSALYS
{

AboutDialog::AboutDialog(QWidget* parent)
  : QDialog(parent)
{
  setWindowIcon(QIcon(":/images/persalys.ico"));
  setWindowTitle(tr("About Persalys"));

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

  // About tab
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QLabel * descriptionLabel = new QLabel(tr("Persalys, an user interface for uncertainty analysis"));
  tabLayout->addWidget(descriptionLabel);

   QLabel * linkLabel = new QLabel;
   linkLabel->setText("<a href=\"https://www.persalys.fr\">www.persalys.fr</a>");
   linkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
   linkLabel->setOpenExternalLinks(true);
   tabLayout->addWidget(linkLabel);

  QLabel * copyrightLabel = new QLabel(tr("Copyright 2015-2020 EDF-Phimeca"));
  tabLayout->addWidget(copyrightLabel);

  //QLabel * licenseLabel = new QLabel("<a>"+tr("License:")+"&nbsp;<a href=\"https://www.gnu.org/licenses/lgpl-3.0.en.html\">GNU Lesser General Public License Version 3</a>");
  //licenseLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
  //licenseLabel->setOpenExternalLinks(true);
  //tabLayout->addWidget(licenseLabel);

  tabWidget->addTab(tab, tr("&About"));

  // Librairies tab
  tab = new QWidget;
  tabLayout = new QVBoxLayout(tab);

  QStringList names;
  QStringList values;
  names << "OpenTURNS"
        << "Qt"
        << "Python"
        << "Qwt";
  values << PlatformInfo::GetVersion().c_str()
         << qVersion()
         << PY_VERSION
         << QWT_VERSION_STR;
#ifdef PERSALYS_HAVE_PARAVIEW
  names  << QString("ParaView") + (SubWindow::SupportsOpenGL_3_2() ? "" : " (" + tr("disabled") + ")");
  values << PARAVIEW_VERSION_FULL;
#endif
#ifdef PERSALYS_HAVE_OTMORRIS
  names << "OTMorris";
  values << OTMORRIS_VERSION_STRING;
#endif
#ifdef PERSALYS_HAVE_OTFMI
  names << "OTFMI";
  values << OTFMI_VERSION_STRING;
#endif
#ifdef PERSALYS_HAVE_YACS
  names << "YACS";
  values << YACS_VERSION_STR;
#endif
  ParametersTableView * table = new ParametersTableView(names, values, false, false);
  table->setSelectionMode(QAbstractItemView::NoSelection);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);

  tabLayout->addWidget(table);

  tabWidget->addTab(tab, tr("&Librairies"));

  mainLayout->addWidget(tabWidget);

  // close button
  QHBoxLayout * closeButtonLayout = new QHBoxLayout;
  closeButtonLayout->addStretch();
  QPushButton * closeButton = new QPushButton(tr("&Close"));
  connect(closeButton, SIGNAL(clicked()), this, SLOT(accept()));
  closeButtonLayout->addWidget(closeButton);

  mainLayout->addLayout(closeButtonLayout);

  setLayout(mainLayout);
}
}
