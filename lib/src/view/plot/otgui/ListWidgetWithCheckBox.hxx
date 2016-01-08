// ListWidgetWithCheckBox.hxx

#ifndef LISTWIDGETWITHCHECKBOX_H
#define LISTWIDGETWITHCHECKBOX_H

#include <QListWidget>

namespace OTGUI {
class ListWidgetWithCheckBox : public QListWidget 
{ 
  Q_OBJECT

public: 
  ListWidgetWithCheckBox(QString title, QStringList itemNames, QWidget * parent=0);

public slots: 
  void checkStateChanged(int state);
signals:
  void checkedItemsChanged(QStringList itemsNames);

private: 
 QStringList itemNames_;
 QStringList checkedItemsNames_;
};
}
#endif