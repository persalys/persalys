//                                               -*- C++ -*-
/**
 *  @brief QAction
 *
 *  Copyright 2015-2022 EDF-Phimeca
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
#ifndef PERSALYS_ACTIONS_HXX
#define PERSALYS_ACTIONS_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QAction>

namespace PERSALYS
{
class PERSALYS_VIEW_API Actions : public QObject
{
  Q_OBJECT

public:
  Actions(QObject * parent);
  QAction * newAction() const;
  QAction * openAction() const;
  QAction * saveAction() const;
  QAction * saveAsAction() const;
  QAction * importPyAction() const;
  QAction * closeAction() const;
  QAction * exitAction() const;

private:
  QAction * new_;
  QAction * open_;
  QAction * save_;
  QAction * saveAs_;
  QAction * importPy_;
  QAction * close_;
  QAction * exit_;
};
}
#endif
