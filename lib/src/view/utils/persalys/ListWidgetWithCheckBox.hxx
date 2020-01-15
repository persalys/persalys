//                                               -*- C++ -*-
/**
 *  @brief QListWidget with checkable items
 *
 *  Copyright 2015-2020 EDF-Phimeca
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
#ifndef PERSALYS_LISTWIDGETWITHCHECKBOX_HXX
#define PERSALYS_LISTWIDGETWITHCHECKBOX_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QListWidget>

namespace PERSALYS
{
class PERSALYS_API ListWidgetWithCheckBox : public QListWidget
{
  Q_OBJECT

public:
  ListWidgetWithCheckBox(const QString &title, const QStringList &itemNames, QWidget * parent = 0);
  ListWidgetWithCheckBox(const QString &title, const QStringList &itemNames, const QStringList &selectedItemNames, QWidget * parent = 0);

  QStringList getCheckedItemNames() const;
  QStringList getItemNames() const;

protected:
  void buildInterface();
  void updateTitleItem();

public slots:
  void setCheckedNames(const QStringList &selectedItemNames);
  void updateCheckState(QListWidgetItem *);
signals:
  void checkedItemsChanged(const QStringList &itemsNames);
  void checkedItemsChanged(const QList<int> &itemsIndices);

private:
  QString title_;
  QStringList itemNames_;
  QStringList checkedItemNames_;
};
}
#endif
