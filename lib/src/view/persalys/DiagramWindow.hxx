//                                               -*- C++ -*-
/**
 *  @brief SubWindow base class to define the diagram of a model
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
#ifndef PERSALYS_DIAGRAMWINDOW_HXX
#define PERSALYS_DIAGRAMWINDOW_HXX

#include "persalys/SubWindow.hxx"
#include "persalys/Item.hxx"
#include "persalys/DiagramPushButton.hxx"

#include <QGraphicsProxyWidget>
#include <QGraphicsScene>
#include <QTextEdit>

namespace PERSALYS
{
class PERSALYS_VIEW_API DiagramWindow : public SubWindow
{
  Q_OBJECT

public :
  DiagramWindow(Item * item, QWidget * parent = 0);

protected:
  void appendButton(DiagramPushButton *button, const unsigned int row, DiagramPushButton *parentButton = 0);
  void updateDiagram();

  int columnNumber_ = 0;
  QGraphicsScene * scene_;
  TitleLabel * titleLabel_;
  QMap<DiagramPushButton*, DiagramPushButton*> buttons_;
  QMap<DiagramPushButton*, QGraphicsProxyWidget*> proxies_;
  QTextEdit * textArea_;
};
}
#endif
