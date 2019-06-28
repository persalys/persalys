//                                               -*- C++ -*-
/**
 *  @brief Generic table
 *
 *  Copyright 2015-2019 EDF-Phimeca
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
#ifndef PERSALYS_COPYABLETABLEVIEW_HXX
#define PERSALYS_COPYABLETABLEVIEW_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QTableView>

namespace PERSALYS
{

class PERSALYS_API CopyableTableView : public QTableView
{
  Q_OBJECT

public:
  /** Default constructor */
  CopyableTableView(QWidget * parent = 0);

  QSize sizeHint() const;
  QSize minimumSizeHint() const;
  void resizeToContents();
  void resizeWithOptimalHeight();

public slots:
  void copy();

protected:
  /** format the text */
  QString getFormattedText() const;

  QAction * copyAction_;

private:
  void buildActions();
};
}
#endif
