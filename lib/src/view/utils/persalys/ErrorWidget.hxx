//                                               -*- C++ -*-
/**
 *  @brief QPlainTextEdit for Error and Warnings
 *
 *  Copyright 2015-2025 EDF-Phimeca
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
#ifndef PERSALYS_ERRORWIDGET_HXX
#define PERSALYS_ERRORWIDGET_HXX

#include "persalys/PersalysPrivate.hxx"

#include <QPlainTextEdit>
#include <QTextCursor>
#include <QTimeLine>

namespace PERSALYS
{
class PERSALYS_UTILS_API ErrorWidget : public QPlainTextEdit
{
  Q_OBJECT

public:
  enum MessageType {Error, Warning, Information};

  explicit ErrorWidget(QWidget *parent = nullptr);
  ~ErrorWidget() override;
  
  void setHeight(int height);
  void unsetHeight();
  void setWordWrap(bool on);
  void forceVisible(bool on);
  void usePadding(bool on);

public slots:
  void reset();
  void setMessage(const QString & message, MessageType type = Error, bool temporary = false, bool useFrame = true);
  void setFramelessErrorMessage(const QString & message);
  void setTemporaryFramelessErrorMessage(const QString & message);

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  int computeHeight() const;
  void applyStyle(MessageType type, bool useFrame);

private slots:
  void reInitErrorMessage(QTimeLine::State);


private:
  int   timersCounter_    = 0;
  bool  userFixedHeight_  = false;
  bool  forcedVisible_    = false;
  bool  usePadding_       = true;

};
} // namespace PERSALYS


#endif // PERSALYS_ERRORWIDGET_HXX