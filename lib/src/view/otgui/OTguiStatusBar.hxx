// OTguiStatusBar.hxx

#ifndef OTGUISTATUSBAR_H
#define OTGUISTATUSBAR_H

#include <QStatusBar>
#include <QLabel>

namespace OTGUI {
class OTguiStatusBar : public QStatusBar
{
    Q_OBJECT

public:
  OTguiStatusBar(QWidget *parent = 0);

public slots:
  void showErrorMessage(const QString & message);
  void showMessage(const QString & message);
  void clearMessage();

private:
  QLabel* messageLabel_;
    
};
}
#endif