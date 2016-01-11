// OTguiMenuBar.hxx

#ifndef OTGUIMENUBAR_H
#define OTGUIMENUBAR_H

#include <QMenuBar>

namespace OTGUI {
class OTguiMenuBar : public QMenuBar
{
  Q_OBJECT

public:
  OTguiMenuBar();

protected:
  void buildActions();

public slots:
signals:
  void createNewOTStudy();
  void loadOTStudy();
  void closeWindow();
};
}
#endif