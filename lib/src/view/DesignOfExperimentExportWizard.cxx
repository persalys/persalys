/**
 *  @brief QWizard to export a design of experimetns as a data model
 *
 *  Copyright 2015-2025 EDF-Phimeca
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

#include "persalys/DesignOfExperimentExportWizard.hxx"
#include "persalys/DesignOfExperimentEvaluation.hxx"
#include "persalys/ParametersWidget.hxx"

#include <QWizardPage>
#include <QGroupBox>
#include <QDebug>

using namespace OT;

namespace PERSALYS
{
DesignOfExperimentExportWizard::DesignOfExperimentExportWizard(const StudyItem * item, QWidget * parent)
  : Wizard(parent)
{
  buildInterface();
  const Study study = item->getStudy();
  for (UnsignedInteger i = 0; i < study.getAnalyses().getSize(); ++i)
  {
    const auto * doeEvaluation = dynamic_cast<const DesignOfExperimentEvaluation*>(study.getAnalyses()[i].getImplementation().get());
    if (doeEvaluation && doeEvaluation->hasValidResult())
    {
      auto* comboItem = new QStandardItem(QString::fromUtf8(doeEvaluation->getName().c_str()));
      comboItem->setData(QVariant::fromValue(study.getAnalyses()[i]));
      doeComboBoxModel_->appendRow(comboItem);
    }
  }
  connect(doeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DesignOfExperimentExportWizard::updateWidgets);
  updateWidgets();
}

void DesignOfExperimentExportWizard::buildInterface()
{
  setWindowTitle(tr("Export Design Of Experiment"));

  auto * page = new QWizardPage(this);
  page->setTitle(tr("Export Design Of Experiment as Data set"));
  auto * pageLayout = new QVBoxLayout(page);

  doeComboBox_ = new QComboBox;
  doeComboBoxModel_ = new QStandardItemModel(doeComboBox_);

  doeComboBox_->setModel(doeComboBoxModel_);
  pageLayout->addWidget(doeComboBox_);

  parametersLayout_ = new QVBoxLayout;
  pageLayout->addLayout(parametersLayout_);

  pageLayout->addStretch();
  addPage(page);
}

Analysis DesignOfExperimentExportWizard::getAnalysis() const
{
  Q_ASSERT(doeComboBox_);
  Q_ASSERT(doeComboBoxModel_);

  const int itemRow = doeComboBox_->currentIndex();
  if (itemRow < 0)
    return Analysis();
  
  QVariant variant = doeComboBoxModel_->item(itemRow)->data();
  if (variant.canConvert<Analysis>())
    return variant.value<Analysis>();
  else
    return Analysis();
}

void DesignOfExperimentExportWizard::updateWidgets()
{
  if (!doeComboBox_->count())
    return;
  
  if (const auto * analysis_ptr = dynamic_cast<const DesignOfExperimentEvaluation*>(getAnalysis().getImplementation().get()); !analysis_ptr)
  {
    qDebug() << "DesignOfExperimentExportWizard::updateWidgets: The analysis is not a DesignOfExperimentEvaluation";
    return;
  }

  const Parameters analysisParameters{getAnalysis().getImplementation().get()->getParameters()};
  if(!analysisParameters.getSize())
    return;
  
  QLayoutItem * child;
  while ((child = parametersLayout_->takeAt(0)) != nullptr)
  {
    delete child->widget();
    delete child;
  }

  parametersLayout_->addWidget(new ParametersWidget(tr("Design Of Experiment export parameters"), analysisParameters));
}

} // namespace PERSALYS