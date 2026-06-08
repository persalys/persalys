//                                               -*- C++ -*-
/**
 *  @brief SubWindow base class to define the diagram of a model
 *
 *  Copyright 2015-2026 EDF-Phimeca
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
#include <QTextBrowser>

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
  auto *splitter = new QSplitter(Qt::Vertical);

  auto *graphView = new QGraphicsView(this);
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
  QGraphicsProxyWidget *proxy = scene_->addWidget(button);
  if (!parentButton)
    proxy->setPos(0, row);
  else
  {
    Q_ASSERT(proxies_.contains(parentButton));
    proxy->setPos(proxies_[parentButton]->pos().x() + 1, row);
    columnNumber_ = std::max(columnNumber_, (int)proxy->x());
  }
  proxies_[button] = proxy;
  
  QGraphicsProxyWidget *overlayProxy = scene_->addWidget(button->createOverlay());
  overlayProxy->setPos(proxy->pos());
  overlayProxy->setZValue(proxy->zValue() + 1);
  overlayProxy->setAcceptHoverEvents(true);
  overlayProxy->setAcceptedMouseButtons(Qt::NoButton);
  overlayProxy->setFlag(QGraphicsItem::ItemIsSelectable, false);
  overlayProxy->setFlag(QGraphicsItem::ItemIsFocusable, false);
  overlayProxies_[button] = overlayProxy;
}


void DiagramWindow::updateDiagram()
{
  QList<DiagramPushButton *> buttons = buttons_.keys();

  int boxWidth = 0;
  int boxHeight = 0;

  for (auto const *button : buttons)
  {
    // connection with textArea_ to display messages
    connect(button->getOverlay(), &HoverOverlay::messageChanged, textArea_, &QTextBrowser::setHtml);
    // get button max size
    boxWidth = std::max(boxWidth, button->width());
    boxHeight = std::max(boxHeight, button->height());
  }

  const int horizontalSpace = 60;
  const int verticalSpace = 20;
  const int rowPos = boxHeight + verticalSpace;
  const int columnPos = boxWidth + horizontalSpace;

  for (auto *button : buttons)
  {
    // resize button : we want that all the buttons have the same dimensions
    button->resize(boxWidth, boxHeight);
    button->getOverlay()->resize(boxWidth, boxHeight);

    // set button position
    const QPointF newPos(proxies_[button]->pos().x() * columnPos, proxies_[button]->pos().y() * rowPos);
    proxies_[button]->setPos(newPos);
    overlayProxies_[button]->setPos(newPos);
  }

  // arrows : do it after resizing/moving buttons
  const int buttonMargin = 3;
  const QPointF hOffset(boxWidth + buttonMargin, boxHeight * 0.5);
  const QPointF vOffset(-buttonMargin, boxHeight * 0.5);

  for (auto *button : buttons)
  {
    if (proxies_[button]->pos().x() > 0)
    {
      Q_ASSERT(proxies_.contains(buttons_[button]));
      const QPointF parentPos(proxies_[buttons_[button]]->pos());
      const QPointF newPos(proxies_[button]->pos().x(), proxies_[button]->pos().y());

      auto * arrow = new Arrow(parentPos + hOffset, newPos + vOffset);

      connect(button, SIGNAL(enabledChanged(bool)), arrow, SLOT(setValidity(bool)));
      scene_->addItem(arrow);
    }
  }
}
}
