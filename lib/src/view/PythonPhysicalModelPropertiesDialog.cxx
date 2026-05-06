//                                               -*- C++ -*-
/**
 *  @brief QDialog
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

#ifdef PERSALYS_HAVE_YDEFX
#include "persalys/YACSPhysicalModel.hxx"
#include "persalys/YACSCouplingPhysicalModel.hxx"
#include <ydefx/ResourceWidget.hxx>
#endif

#include "persalys/PythonPhysicalModelPropertiesDialog.hxx"
#include "persalys/ProcessNumberSpinBox.hxx"

#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QCheckBox>
#include <QButtonGroup>
#include <QRadioButton>

using namespace OT;

namespace PERSALYS
{
  PythonPhysicalModelPropertiesDialog::PythonPhysicalModelPropertiesDialog(PythonPhysicalModel* model, QWidget* parent)
    : QDialog(parent)
  {
    setWindowTitle(QString(model->getName().c_str()));
    QVBoxLayout * mainLayout = new QVBoxLayout;

#ifdef PERSALYS_HAVE_YDEFX
    YACSPhysicalModel *yacsModel = dynamic_cast<YACSPhysicalModel*>(model);
    YACSCouplingPhysicalModel *yacsCouplingModel = dynamic_cast<YACSCouplingPhysicalModel*>(model);
    if (!yacsModel && !yacsCouplingModel)
      throw std::runtime_error("PythonPhysicalModelPropertiesDialog: cannot cast to YACS model");

    QRadioButton *localButton = new QRadioButton(tr("Local"));
    QButtonGroup *buttonGroup = new QButtonGroup;
    buttonGroup->addButton(localButton, 0);
    mainLayout->addWidget(localButton);
#endif

    ProcessNumberSpinBox *processNumberSpinBox = new ProcessNumberSpinBox;
    processNumberSpinBox->setProcessNumber(model->getProcessNumber());
    mainLayout->addWidget(processNumberSpinBox);

#ifdef PERSALYS_HAVE_YDEFX
    QRadioButton *hpcButton = new QRadioButton(tr("HPC cluster"));
    buttonGroup->addButton(hpcButton, 1);
    hpcButton->setChecked(yacsModel ? yacsModel->getUseYACS() : true);
    localButton->setChecked(!hpcButton->isChecked());
    mainLayout->addWidget(hpcButton);

    ydefx::ResourceWidget* rw = new ydefx::ResourceWidget(yacsModel ? yacsModel->jobParameters() : yacsCouplingModel->jobParameters());
    if (yacsModel)
    {
      rw->setEnabled(yacsModel->getUseYACS());
      processNumberSpinBox->setEnabled(!yacsModel->getUseYACS());
    }
    if (yacsCouplingModel)
    {
      // YACSCouplingPhysicalModel cannot switch
      localButton->setEnabled(false);
      hpcButton->setEnabled(false);
      processNumberSpinBox->setEnabled(false);
    }
    mainLayout->addWidget(rw);

    connect(buttonGroup, &QButtonGroup::idClicked, [=](int id)
    {
      if (yacsModel)
      {
        yacsModel->setUseYACS(id == 1);
        processNumberSpinBox->setEnabled(id == 0);
        rw->setEnabled(id == 1);
      }
    });
#endif

    QDialogButtonBox * settingsButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(settingsButtons, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(settingsButtons, &QDialogButtonBox::accepted, [ = ]
    {
      model->setProcessNumber(processNumberSpinBox->getProcessNumber());
      model->setParallel(processNumberSpinBox->getProcessNumber() != 1);
      emit QDialog::accepted();
    });
    connect(settingsButtons, &QDialogButtonBox::rejected, this, &QDialog::reject);
    mainLayout->addWidget(settingsButtons);
    setLayout(mainLayout);
  }
}
