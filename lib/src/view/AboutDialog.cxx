//                                               -*- C++ -*-
/**
 *  @brief QDialog
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#include "otgui/AboutDialog.hxx"

#include "otgui/ParametersTableView.hxx"
#include "otgui/SubWindow.hxx"

#include <openturns/PlatformInfo.hxx>

#include <QFrame>
#include <QLabel>
#include <QIcon>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTabWidget>

#include <patchlevel.h>
#include <qwt.h>
#ifdef OTGUI_HAVE_PARAVIEW
#include <vtkPVConfig.h>
#endif

using namespace OT;

namespace OTGUI
{

AboutDialog::AboutDialog(QWidget* parent)
  : QDialog(parent)
{
  setWindowIcon(QIcon(":/images/otgui.ico"));
  setWindowTitle(tr("About OTGui"));

  QVBoxLayout * mainLayout = new QVBoxLayout;

  // header with logo, name and version
  QFrame * headerFrame = new QFrame;
  headerFrame->setFrameShape(QFrame::StyledPanel);

  QHBoxLayout * headerLayout = new QHBoxLayout;
  QLabel * logoLabel = new QLabel;
  logoLabel->setPixmap(QPixmap(":/images/OT_icon32x32.png"));
  headerLayout->addWidget(logoLabel);

  QLabel * phibootLabel = new QLabel("<font size=\"6\">OTGui</font><br>" + tr("Version %1").arg(OTGUI_VERSION));
  headerLayout->addWidget(phibootLabel);
  headerLayout->addStretch();

  headerFrame->setLayout(headerLayout);

  mainLayout->addWidget(headerFrame);

  // tab widget
  QTabWidget * tabWidget = new QTabWidget;

  // About tab
  QWidget * tab = new QWidget;
  QVBoxLayout * tabLayout = new QVBoxLayout(tab);

  QLabel * descriptionLabel = new QLabel(tr("OTGui, an interface for OpenTURNS"));
  tabLayout->addWidget(descriptionLabel);

  QLabel * otLinkLabel = new QLabel();
  otLinkLabel->setText("<a href=\"http://www.openturns.org\">www.openturns.org</a>");
  otLinkLabel->setTextInteractionFlags(Qt::TextBrowserInteraction);
  otLinkLabel->setOpenExternalLinks(true);
  tabLayout->addWidget(otLinkLabel);

  QLabel * copyrightLabel = new QLabel(tr("Copyright 2015-2019 EDF-Phimeca"));
  tabLayout->addWidget(copyrightLabel);

  QLabel * licenseLabel = new QLabel(tr("License: GNU Lesser General Public License Version 3"));
  tabLayout->addWidget(licenseLabel);

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
#ifdef OTGUI_HAVE_PARAVIEW
  names  << QString("Paraview") + (SubWindow::HaveOpenGL32() ? "" : " (" + tr("disabled") + ")");
  values << PARAVIEW_VERSION_FULL;
#endif
#ifdef OTGUI_HAVE_OTMORRIS
  names << "OTMorris";
  values << OTMORRIS_VERSION_STRING;
#endif
#ifdef OTGUI_HAVE_OTFMI
  names << "OTFMI";
  values << OTFMI_VERSION_STRING;
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
