#include "otgui/OTguiWizard.hxx"

#include <QIcon>

namespace OTGUI {

OTguiWizard::OTguiWizard(QWidget * parent)
  : QWizard(parent)
{
  setWindowIcon(QIcon(":/images/OT_icon16x16.png"));
  setWizardStyle(QWizard::MacStyle);
  setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/OT_image256x256.png"));
  setButtonText(QWizard::BackButton, tr("Back"));
  setButtonText(QWizard::CancelButton, tr("Cancel"));
  setOption(QWizard::NoDefaultButton, true);
  resize(800, 600);
}
}