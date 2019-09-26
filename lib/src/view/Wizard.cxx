//                                               -*- C++ -*-
/**
 *  @brief Specific QWizard for persalys
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
#include "persalys/Wizard.hxx"

#include "persalys/FileTools.hxx"
#include "persalys/AnalysisWizard.hxx"

#include <QIcon>
#include <QAbstractButton>
#include <QDesktopServices>

namespace PERSALYS
{
QMap<QString, QString> Wizard::DocLinks_;

Wizard::Wizard(QWidget * parent)
  : QWizard(parent)
{
  setWindowIcon(QIcon(":/images/persalys.ico"));
  setWizardStyle(QWizard::MacStyle);
  setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/OT_image256x256.png"));
  setButtonText(QWizard::BackButton, tr("Back"));
  setButtonText(QWizard::FinishButton, tr("Finish"));
  setButtonText(QWizard::CancelButton, tr("Cancel"));
  setOption(QWizard::NoDefaultButton, true);
  setOption(QWizard::NoBackButtonOnStartPage, true);
  setOption(QWizard::HaveHelpButton, true);
  button(QWizard::HelpButton)->setIcon(QIcon(":/images/documentinfo.png"));
  connect(this, SIGNAL(helpRequested()), this, SLOT(showHelp()));
  resize(800, 600);
}


void Wizard::showHelp()
{
  FileTools::OpenUrl(FileTools::GetDocumentationUrl(getDocLink(), FileTools::docGUI));
}


void Wizard::InitializeDocLInks()
{
  DocLinks_["ModelEvaluationWizard"] = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#vectmodelevalwizard";
  DocLinks_["FieldModelEvaluationWizard"] = "user_manual/graphical_interface/field_analysis/user_manual_field_analysis.html#fieldmodelevalwizard";
  DocLinks_["ScreeningAnalysisWizard"] = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#screeningwizard";
  DocLinks_["OptimizationWizard"] = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#optimizationwizard";
  DocLinks_["MetaModelAnalysisWizard"] = "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#metamodelwizard";
  DocLinks_["InferenceWizard"] = "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#marginalsinferencewizard";
  DocLinks_["CopulaInferenceWizard"] = "user_manual/graphical_interface/data_analysis/user_manual_data_analysis.html#dependenceinferencewizard";
  DocLinks_["CentralTendencyWizard"] = "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#centraltendencywizard";
  DocLinks_["FieldMonteCarloWizard"] = "user_manual/graphical_interface/field_analysis/user_manual_field_analysis.html#fieldcentraltendencywizard";
  DocLinks_["SensitivityAnalysisWizard"] = "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#sensitivitywizard";
  DocLinks_["ReliabilityAnalysisWizard"] = "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#thresholdexceedancewizard";
  DocLinks_["DesignOfExperimentWizard"] = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#doeinputwizard";
  DocLinks_["DesignOfExperimentEvaluationWizard"] = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#doeevalwizard";
  DocLinks_["ScreeningResultWizard"] = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#screeningresultwizard";
  DocLinks_["InferenceResultWizard"] = "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#inferenceresultwizard";
  DocLinks_["CopulaInferenceResultWizard"] = "user_manual/graphical_interface/probabilistic_analysis/user_manual_probabilistic_analysis.html#dependenceinferenceresultwizard";
  DocLinks_["MeshDefinitionWizard"] = "user_manual/graphical_interface/field_model/user_manual_field_model.html#meshdefinitionwizard";
  DocLinks_["CalibrationAnalysisWizard"] = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#calibrationwizard";
  DocLinks_["ObservationsWizard"] = "user_manual/graphical_interface/deterministic_analysis/user_manual_deterministic_analysis.html#observationwizard";
}


QString Wizard::getDocLink()
{
  if (DocLinks_.empty())
    InitializeDocLInks();

  QString wizardClassName = QString(this->metaObject()->className()).split("::")[1];
  if (wizardClassName == "ModelEvaluationWizard")
  {
    const QString analysisType = dynamic_cast<AnalysisWizard*>(this)->getAnalysis().getImplementation()->getClassName().c_str();
    if (analysisType.contains("Field"))
      wizardClassName = "FieldModelEvaluationWizard";
  }
  return DocLinks_.contains(wizardClassName) ? DocLinks_[wizardClassName] : "";
}
}
