//                                               -*- C++ -*-
/**
 *  @brief QLabel with temporary message
 *
 *  Copyright 2015-2018 EDF-Phimeca
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
#ifndef OTGUI_TEMPORARYLABEL_HXX
#define OTGUI_TEMPORARYLABEL_HXX

#include "otgui/OTGuiprivate.hxx"

#include <QLabel>
#include <QTimeLine>

namespace OTGUI
{
class OTGUI_API TemporaryLabel : public QLabel
{
  Q_OBJECT

public:
  TemporaryLabel(QWidget *parent = 0);

public slots:
  void reset();
  void setMessage(const QString& message);
  void setErrorMessage(const QString& message);
  void setTemporaryErrorMessage(const QString& message);
  void reInitErrorMessage(QTimeLine::State);

private:
  QList<QTimeLine*> qtimelineList_;
};
}
#endif
