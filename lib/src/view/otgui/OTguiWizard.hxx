// OTguiWizard.hxx

#ifndef OTGUI_WIZARD_H
#define OTGUI_WIZARD_H

#include <QWizard>

namespace OTGUI {
class OTguiWizard : public QWizard
{
  Q_OBJECT

public:
  OTguiWizard(QWidget * parent=0);
};
}
#endif