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
#include "persalys/DiagramWindow.hxx"

#include "persalys/Arrow.hxx"

#include <QVBoxLayout>
#include <QGraphicsView>
#include <QSplitter>

using namespace OT;

namespace PERSALYS
{

DiagramWindow::DiagramWindow(Item * item, QWidget * parent)
  : SubWindow(item, parent)
{
  QVBoxLayout * mainLayout = new QVBoxLayout(this);

  titleLabel_ = new TitleLabel(tr("Study workflow"));
  mainLayout->addWidget(titleLabel_);

  // Widgets
  QSplitter * splitter = new QSplitter(Qt::Vertical);

  QGraphicsView * graphView = new QGraphicsView(this);
  scene_ = new QGraphicsScene(this);

  // -- set scene --
  graphView->setScene(scene_);
  splitter->addWidget(graphView);
  splitter->setStretchFactor(0, 3);

  // -- text area --
  textArea_ = new QTextEdit;
  textArea_->setReadOnly(true);
  splitter->addWidget(textArea_);
  splitter->setStretchFactor(1, 1);

  mainLayout->addWidget(splitter);
}


void DiagramWindow::appendButton(DiagramPushButton *button, const unsigned int row, DiagramPushButton *parentButton)
{
  buttons_[button] = parentButton;
  QGraphicsProxyWidget * proxy = new QGraphicsProxyWidget;
  proxy->setWidget(button);
  scene_->addItem(proxy);
  if (!parentButton)
    proxy->setPos(0, row);
  else
  {
    Q_ASSERT(proxies_.contains(parentButton));
    proxy->setPos(proxies_[parentButton]->pos().x() + 1, row);
    columnNumber_ = std::max(columnNumber_, (int)proxy->x());
  }
  proxies_[button] = proxy;
}


void DiagramWindow::updateDiagram()
{
  QList<DiagramPushButton *> buttons = buttons_.keys();

  int boxWidth = 0;
  int boxHeight = 0;

  for (int i = 0; i < buttons.size(); ++i)
  {
    // connection with textArea_ to display messages
    connect(buttons[i], SIGNAL(messageChanged(QString)), textArea_, SLOT(setHtml(QString)));
    // get button max size
    boxWidth = std::max(boxWidth, buttons[i]->width());
    boxHeight = std::max(boxHeight, buttons[i]->height());
  }

  const int horizontalSpace = 60;
  const int verticalSpace = 20;
  const int rowPos = boxHeight + verticalSpace;
  const int columnPos = boxWidth + horizontalSpace;

  for (int i = 0; i < buttons.size(); ++i)
  {
    // resize button : we want that all the buttons have the same dimensions
    buttons[i]->resize(boxWidth, boxHeight);

    // set button position
    const QPointF newPos(proxies_[buttons[i]]->pos().x() * columnPos, proxies_[buttons[i]]->pos().y() * rowPos);
    proxies_[buttons[i]]->setPos(newPos);
  }

  // arrows : do it after resizing/moving buttons
  const int buttonMargin = 3;
  const QPointF hOffset(boxWidth + buttonMargin, boxHeight * 0.5);
  const QPointF vOffset(-buttonMargin, boxHeight * 0.5);

  for (int i = 0; i < buttons.size(); ++i)
  {
    if (proxies_[buttons[i]]->pos().x() > 0)
    {
      Q_ASSERT(proxies_.contains(buttons_[buttons[i]]));
      const QPointF parentPos(proxies_[buttons_[buttons[i]]]->pos());
      const QPointF newPos(proxies_[buttons[i]]->pos().x(), proxies_[buttons[i]]->pos().y());

      Arrow * arrow = new Arrow(parentPos + hOffset, newPos + vOffset);

      connect(buttons[i], SIGNAL(enabledChanged(bool)), arrow, SLOT(setValidity(bool)));
      scene_->addItem(arrow);
    }
  }
}
}
