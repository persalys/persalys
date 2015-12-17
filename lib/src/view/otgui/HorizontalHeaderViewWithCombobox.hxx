// HorizontalHeaderViewWithCombobox.hxx

#ifndef OTGUI_HORIZONTALHEADERVIEWWITHCOMBOBOX_H
#define OTGUI_HORIZONTALHEADERVIEWWITHCOMBOBOX_H

#include "Description.hxx"
#include "Indices.hxx"

#include <QHeaderView>
#include <QShowEvent>
#include <QComboBox>

namespace OTGUI {
class HorizontalHeaderViewWithCombobox : public QHeaderView
{
  Q_OBJECT

public:
  HorizontalHeaderViewWithCombobox(QStringList comboBoxItems, QVector<int> columns, QWidget *parent=0);

  void showEvent(QShowEvent * e);
  void scrollContentsBy(int dx, int dy);
  void fixComboPositions();

public slots:
  void handleSectionResized(int);
  void handleSectionMoved(int, int, int);
  void setHeaderData(int);

private:
  QMap<int, QComboBox *> boxes_;
  QStringList comboBoxItems_;
  QVector<int> columnsWithComboBox_;
};
}
#endif