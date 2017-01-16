/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Copyright (C) 2010 Stephen Kelly <steveire@gmail.com>
** Contact: http://www.qt-project.org/legal
**
** This file is part of the test suite of the Qt Toolkit.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file COPYING.LIB included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/


#ifndef MODELTEST_H
#define MODELTEST_H

#include <QtCore/QObject>
#include <QtCore/QAbstractItemModel>
#include <QtCore/QStack>

#include "proxymodeltestsuite_export.h"

class PROXYMODELTESTSUITE_EXPORT ModelTest : public QObject
{
  Q_OBJECT

public:
  enum Mode {
    Normal,
    Pedantic
  };

  ModelTest( QAbstractItemModel *model, QObject *parent = nullptr );
  ModelTest( QAbstractItemModel *model, Mode testType, QObject *parent = nullptr );

private Q_SLOTS:
  void nonDestructiveBasicTest();
  void rowCount();
  void columnCount();
  void hasIndex();
  void index();
  void parent();
  void data();

protected Q_SLOTS:
  void runAllTests();
  void layoutAboutToBeChanged();
  void layoutChanged();
  void modelAboutToBeReset();
  void modelReset();
  void rowsAboutToBeInserted( const QModelIndex &parent, int start, int end );
  void rowsInserted( const QModelIndex & parent, int start, int end );
  void rowsAboutToBeRemoved( const QModelIndex &parent, int start, int end );
  void rowsRemoved( const QModelIndex & parent, int start, int end );
  void rowsAboutToBeMoved(const QModelIndex &, int, int, const QModelIndex &, int);
  void rowsMoved(const QModelIndex &, int, int, const QModelIndex &, int);
  void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight);
  void headerDataChanged(Qt::Orientation orientation, int start, int end);

  void ensureConsistent();
  void ensureSteady();

private:
  void checkChildren( const QModelIndex &parent, int currentDepth = 0 );
  void refreshStatus();
  void persistStatus(const QModelIndex &index);
  void init();

  QAbstractItemModel * const model;

  struct Status {
    enum Type {
      Idle,
      InsertingRows,
      RemovingRows,
      MovingRows,
      ChangingLayout,
      Resetting
    };

    Type type;

    QList<QPersistentModelIndex> persistent;
    QList<QModelIndex> nonPersistent;
  } status;

  struct Changing {
    QModelIndex parent;
    int oldSize;
    QVariant last;
    QVariant next;
  };
  QStack<Changing> insert;
  QStack<Changing> remove;

  bool fetchingMore;
  const bool pedantic;

  QList<QPersistentModelIndex> changing;
};

#endif
