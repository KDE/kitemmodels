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

#include "kconcatenaterowsproxymodel_qml.h"

KConcatenateRowsProxyModelQml::KConcatenateRowsProxyModelQml(QObject *wrappedObject):
    QObject(wrappedObject),
    q(qobject_cast<KConcatenateRowsProxyModel*>(wrappedObject))
{
}

KConcatenateRowsProxyModelQml::~KConcatenateRowsProxyModelQml()
{}

QQmlListProperty<QAbstractItemModel> KConcatenateRowsProxyModelQml::sources()
{
    return QQmlListProperty<QAbstractItemModel>(this, q,
             &KConcatenateRowsProxyModelQml::appendSource,
             &KConcatenateRowsProxyModelQml::sourceCount,
             &KConcatenateRowsProxyModelQml::source,
             &KConcatenateRowsProxyModelQml::clear);
}

void KConcatenateRowsProxyModelQml::appendSource(QQmlListProperty<QAbstractItemModel>* list, QAbstractItemModel* newItem)
{
    auto q = static_cast<KConcatenateRowsProxyModel*>(list->data);
    q->addSourceModel(newItem);
}

int KConcatenateRowsProxyModelQml::sourceCount(QQmlListProperty<QAbstractItemModel>* list)
{
    auto q = static_cast<KConcatenateRowsProxyModel*>(list->data);
    return q->sources().count();
}

QAbstractItemModel* KConcatenateRowsProxyModelQml::source(QQmlListProperty<QAbstractItemModel>* list, int index)
{
    auto q = static_cast<KConcatenateRowsProxyModel*>(list->data);
    return q->sources().at(index);
}

void KConcatenateRowsProxyModelQml::clear(QQmlListProperty<QAbstractItemModel>* list)
{
    auto q = static_cast<KConcatenateRowsProxyModel*>(list->data);
    const auto sources = q->sources();
    for (auto s: sources) {
        q->removeSourceModel(s);
    }
}



