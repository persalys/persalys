//                                               -*- C++ -*-
/**
 *  @brief QComboBox which acts as a selection button
 *
 *  Copyright 2015-2023 EDF-Phimeca
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
#ifndef PERSALYS_TITLEDCOMBOBOX_HXX
#define PERSALYS_TITLEDCOMBOBOX_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QComboBox>
#include <QPaintEvent>

namespace PERSALYS
{
class PERSALYS_UTILS_API TitledComboBox : public QComboBox
{
  Q_OBJECT

public:
/// Combo box which acts as a selection button
/// use signals activated(int) or textActivated(QString)
  TitledComboBox(const QString & title = QString(""), QWidget *parent = nullptr);
  TitledComboBox(const QIcon & icon, const QString & title = QString(""), QWidget *parent = nullptr);

signals:
  void textActivated(QString);

protected:
  void paintEvent(QPaintEvent *);
  void wheelEvent(QWheelEvent *);

private:
  QString title_;
  QIcon icon_;

private slots:
  void activateText(int);
};
}
#endif
