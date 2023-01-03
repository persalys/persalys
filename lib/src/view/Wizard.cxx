//                                               -*- C++ -*-
/**
 *  @brief Specific QWizard for persalys
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#include "persalys/Wizard.hxx"

#include "persalys/FileTools.hxx"
#include "persalys/AnalysisWizard.hxx"

#include <QIcon>
#include <QAbstractButton>
#include <QDesktopServices>

namespace PERSALYS
{

Wizard::Wizard(QWidget * parent)
  : QWizard(parent)
{
  setWindowIcon(QIcon(":/images/persalys.ico"));
  setWizardStyle(QWizard::MacStyle);
  setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/Ps-icon-144.png"));
  setButtonText(QWizard::BackButton, tr("Back"));
  setButtonText(QWizard::FinishButton, tr("Finish"));
  setButtonText(QWizard::CancelButton, tr("Cancel"));
  setOption(QWizard::NoDefaultButton, true);
  setOption(QWizard::NoBackButtonOnStartPage, true);
  setOption(QWizard::HaveHelpButton, true);
  button(QWizard::HelpButton)->setIcon(QIcon(":/images/documentinfo.png"));
  connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
  resize(800, 500);
}


void Wizard::showHelp()
{
  FileTools::OpenUrl(FileTools::GetDocumentationUrl(docLink_, FileTools::docGUI));
}
}
