//                                               -*- C++ -*-
/**
 *  @brief QWizard to extract data of a Field
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#include "persalys/ExtractDataFieldWizard.hxx"

#include "persalys/StudyItem.hxx"

#include "persalys/ListWidgetWithCheckBox.hxx"
#include "persalys/TitledComboBox.hxx"

#include <QVBoxLayout>
#include <QPushButton>
#include <QList>

using namespace OT;

namespace PERSALYS
{

ExtractDataFieldWizard::ExtractDataFieldWizard(const Analysis& analysis, QWidget *parent)
  : QDialog(parent)
  , analysis_(0)
  , nodes_()
  , errorMessageLabel_(0)
{
  setWindowTitle(tr("Extract data at nodes of the mesh"));

  FieldMonteCarloAnalysis * mc = dynamic_cast<FieldMonteCarloAnalysis*>(analysis.getImplementation().get());
  analysis_ = mc;
  const UnsignedInteger nbNodes = mc->getResult().getProcessSample().getMesh().getVerticesNumber();
  const Description meshNodesNames(Description::BuildDefault(nbNodes, mc->getResult().getProcessSample().getMesh().getDescription()[0]));

  // build dialog
  QVBoxLayout * layout = new QVBoxLayout(this);

  // label
  layout->addWidget(new QLabel(tr("Choose nodes of the mesh")));

  // combobox
  TitledComboBox * combo = new TitledComboBox("-- " + tr("Select") + " --");

  QStringList items;
  for (UnsignedInteger i = 0; i < meshNodesNames.getSize(); ++i)
    items.append(QString::fromUtf8(meshNodesNames[i].c_str()));

  ListWidgetWithCheckBox * listWidget = new ListWidgetWithCheckBox("-- " + tr("Select") + " --", items, this);
  connect(listWidget, SIGNAL(checkedItemsChanged(QList<int>)), this, SLOT(updateSelectedNodes(QList<int>)));
  combo->setModel(listWidget->model());
  combo->setView(listWidget);
  layout->addWidget(combo);

  // error message label
  errorMessageLabel_ = new TemporaryLabel;
  layout->addWidget(errorMessageLabel_);

  // cancel and ok buttons
  QHBoxLayout * buttonsLayout = new QHBoxLayout;
  buttonsLayout->addStretch();

  QPushButton * cancelButton = new QPushButton(tr("&Cancel"));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
  buttonsLayout->addWidget(cancelButton);

  QPushButton * okButton = new QPushButton(tr("&OK"));
  connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
  buttonsLayout->addWidget(okButton);

  layout->addLayout(buttonsLayout);
}


DataModel ExtractDataFieldWizard::getDataModel()
{
  // get analysis information
  const UnsignedInteger nbModelOutput = analysis_->getResult().getProcessSample().getDimension();
  const UnsignedInteger nbOutput = nbModelOutput * nodes_.size();
  const UnsignedInteger nbInputPt = analysis_->getResult().getDesignOfExperiment().getInputSample().getSize();
  const UnsignedInteger nbNodes = analysis_->getResult().getProcessSample().getMesh().getVerticesNumber();
  const Description meshNodesNames(Description::BuildDefault(nbNodes, analysis_->getResult().getProcessSample().getMesh().getDescription()[0]));
  const Description outDesc(analysis_->getResult().getProcessSample().getField(0).getValues().getDescription());

  // get data to extract
  Sample sample(nbInputPt, nbOutput);
  for (UnsignedInteger in = 0; in < nbInputPt; ++in)
  {
    Point dataIn;
    for (int n = 0; n < nodes_.size(); ++n)
      dataIn.add(analysis_->getResult().getProcessSample().getField(in).getValueAtIndex(nodes_[n]));
    sample[in] = dataIn;
  }

  // define sample description
  Description desc;
  for (int n = 0; n < nodes_.size(); ++n)
    for (UnsignedInteger out = 0; out < nbModelOutput; ++out)
      desc.add(outDesc[out] + "_" + meshNodesNames[nodes_[n]]);
  sample.setDescription(desc);

  // define new data model
  const String modelName(tr("Extraction_%1_").arg(analysis_->getName().c_str()).toStdString());
  AnalysisItem * analysisItem = dynamic_cast<AnalysisItem*>(analysis_->getObserver("AnalysisItem"));
  const String availableName(analysisItem->getParentStudyItem()->getStudy().getAvailableDataModelName(modelName));
  DataModel model(availableName, analysis_->getResult().getDesignOfExperiment().getInputSample(), sample);

  return model;
}


void ExtractDataFieldWizard::updateSelectedNodes(const QList<int>& nodes)
{
  nodes_ = nodes;
  errorMessageLabel_->setErrorMessage(nodes_.size() < 1 ? tr("Select at least one node") : "");
}


void ExtractDataFieldWizard::done(int r)
{
  if (QDialog::Accepted == r && nodes_.size() < 1)  // ok was pressed and no nodes were selected
    return;

  QDialog::done(r);
}
}
