//                                               -*- C++ -*-
/**
 *  @brief QWizard to export a metamodel as physical model
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

#include "persalys/MetaModelExportWizard.hxx"
#include "persalys/StudyImplementation.hxx"
#include "persalys/ParametersWidget.hxx"
#include "persalys/QtTools.hxx"
#include "persalys/PolynomialRegressionAnalysis.hxx"
#include "persalys/KrigingAnalysis.hxx"
#include "persalys/FunctionalChaosAnalysis.hxx"

#include <QVBoxLayout>
#include <QGroupBox>
#include <QDebug>

Q_DECLARE_METATYPE(PERSALYS::Analysis)

using namespace OT;

namespace PERSALYS
{
  MetaModelExportWizard::MetaModelExportWizard(const Analysis& analysis, const bool isGeneralWizard, QWidget* parent)
    : Wizard(parent)
    , mmsComboBox_(0)
    , mmsComboBoxModel_(0)
    , parametersLayout_(0)
  {

    buildInterface();

    // set list of Metamodel analysis items
    if (isGeneralWizard) {
      if (Observer * obs = analysis.getImplementation().get()->getObserver("Study")) {
        StudyImplementation * study = dynamic_cast<StudyImplementation*>(obs);
        Q_ASSERT(study);
        for (UnsignedInteger i = 0; i < study->getAnalyses().getSize(); ++i) {
          const MetaModelAnalysis * mmanalysis = dynamic_cast<const MetaModelAnalysis*>(study->getAnalyses()[i].getImplementation().get());
          if (mmanalysis && mmanalysis->hasValidResult()) {
            QStandardItem * comboItem = new QStandardItem(QString::fromUtf8(mmanalysis->getName().c_str()));
            comboItem->setData(QVariant::fromValue(study->getAnalyses()[i]));
            mmsComboBoxModel_->appendRow(comboItem);
          }
        }
      }
      connect(mmsComboBox_, SIGNAL(currentIndexChanged(int)), this, SLOT(updateWidgets()));
    } else {
      QStandardItem * comboItem = new QStandardItem(QString::fromUtf8(analysis.getName().c_str()));
      comboItem->setData(QVariant::fromValue(analysis));
      mmsComboBoxModel_->appendRow(comboItem);
    }
    updateWidgets();
  }

  void MetaModelExportWizard::buildInterface() {
    // set window title
    setWindowTitle(tr("Export metamodel as a physical model"));

    // create a page
    QWizardPage * page = new QWizardPage(this);
    page->setTitle(tr("Export metamodel as a physical model"));
    QVBoxLayout * pageLayout = new QVBoxLayout(page);

    // label
    QGroupBox * groupBox = new QGroupBox(tr("Metamodel"));
    QVBoxLayout * groupBoxLayout = new QVBoxLayout(groupBox);
    pageLayout->addWidget(groupBox);

    // combo box with the list of MetaModelAnalysisItem
    mmsComboBox_ = new QComboBox;
    mmsComboBoxModel_ = new QStandardItemModel(mmsComboBox_);

    mmsComboBox_->setModel(mmsComboBoxModel_);
    groupBoxLayout->addWidget(mmsComboBox_);

    parametersLayout_ = new QVBoxLayout;
    pageLayout->addLayout(parametersLayout_);

    pageLayout->addStretch();
    addPage(page);
  }

  Analysis MetaModelExportWizard::getAnalysis() const {
    Q_ASSERT(mmsComboBox_);
    Q_ASSERT(mmsComboBoxModel_);

    const int itemRow = mmsComboBox_->currentIndex();
    if (itemRow < 0)
      return Analysis();

    QVariant variant = mmsComboBoxModel_->item(itemRow)->data();
    if (variant.canConvert<Analysis>())
      return variant.value<Analysis>();
    return Analysis();
  }

  void MetaModelExportWizard::updateWidgets() {
    if (!mmsComboBox_->count())
      return;

    const MetaModelAnalysis * analysis_ptr = dynamic_cast<const MetaModelAnalysis*>(getAnalysis().getImplementation().get());
    if (!analysis_ptr) {
      qDebug() << "MetamodelExportWizard::updateWidgets: The analysis is not a MetaModelAnalysis";
      return;
    }

    const Parameters analysisParameters(getAnalysis().getImplementation().get()->getParameters());
    if (!analysisParameters.getSize())
      return;

    QLayoutItem *child;
    while ((child = parametersLayout_->takeAt(0)) != 0) {
      delete child->widget();
      delete child;
    }

    parametersLayout_->addWidget(new ParametersWidget(tr("Metamodel creation parameters"), analysisParameters));

  }

}
