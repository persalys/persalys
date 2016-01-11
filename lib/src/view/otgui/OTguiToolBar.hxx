// OTguiToolBar.hxx

#ifndef OTGUITOOLBAR_H
#define OTGUITOOLBAR_H

#include <QToolBar>

namespace OTGUI {
class OTguiToolBar : public QToolBar
{
  Q_OBJECT

public:
  OTguiToolBar();

protected:
  void buildActions();

public slots:
signals:
  void createNewOTStudy();
  void loadOTStudy();
};
}
#endif