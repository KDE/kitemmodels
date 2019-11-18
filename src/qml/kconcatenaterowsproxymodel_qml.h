/*
    Copyright (c) 2019 David Edmundson <davidedmundson@kde.org>

    This library is free software; you can redistribute it and/or modify it
    under the terms of the GNU Library General Public License as published by
    the Free Software Foundation; either version 2 of the License, or (at your
    option) any later version.

    This library is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
    License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to the
    Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
    02110-1301, USA.
*/

//This class exposes KConcatenateRowsProxyModel in a more QML friendly way

#pragma once

#include <QObject>
#include <QQmlListProperty>
#include <KConcatenateRowsProxyModel>

class KConcatenateRowsProxyModelQml : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<QAbstractItemModel> sources READ sources)
    Q_CLASSINFO("DefaultProperty", "sources")
public:
    explicit KConcatenateRowsProxyModelQml(QObject *wrappedObject = nullptr);
    ~KConcatenateRowsProxyModelQml();

    QQmlListProperty<QAbstractItemModel> sources();

    static void appendSource(QQmlListProperty<QAbstractItemModel>*, QAbstractItemModel*);
    static int sourceCount(QQmlListProperty<QAbstractItemModel>*);
    static QAbstractItemModel* source(QQmlListProperty<QAbstractItemModel>*, int);
    static void clear(QQmlListProperty<QAbstractItemModel>*);

private:
    KConcatenateRowsProxyModel *q;
};
