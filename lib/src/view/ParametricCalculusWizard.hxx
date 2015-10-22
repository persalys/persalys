// ParametricCalculusWizard.hxx

#ifndef PARAMETRICCALCULUSWIZARD_H
#define PARAMETRICCALCULUSWIZARD_H

#include "ParametricCalculusTableModel.hxx"
#include "OTStudy.hxx"

#include <QWizard>

namespace OTGUI {
class ParametricCalculusWizard : public QWizard
{
  Q_OBJECT

public:
  ParametricCalculusWizard(OTStudy * study, const PhysicalModel & physicalModel);
  ParametricCalculusWizard(const Calculus & calculus);

  void validate();

protected:
  void buildInterface();

public slots:
  void setCalculus(Calculus & calculus);

private:
  ParametricCalculusTableModel * model_;
  Calculus calculus_;
  OTStudy * OTStudy_;
};
}
#endif