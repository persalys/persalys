// ParametricAnalysisWizard.hxx

#ifndef PARAMETRICANALYSISWIZARD_H
#define PARAMETRICANALYSISWIZARD_H

#include "otgui/ParametricAnalysisTableModel.hxx"
#include "otgui/OTStudy.hxx"

#include <QWizard>

namespace OTGUI {
class ParametricAnalysisWizard : public QWizard
{
  Q_OBJECT

public:
  ParametricAnalysisWizard(OTStudy * study, const PhysicalModel & physicalModel);
  ParametricAnalysisWizard(const Analysis & analysis);

  void validate();

protected:
  void buildInterface();

public slots:
  void setAnalysis(Analysis & analysis);

private:
  ParametricAnalysisTableModel * model_;
  Analysis analysis_;
  OTStudy * OTStudy_;
};
}
#endif