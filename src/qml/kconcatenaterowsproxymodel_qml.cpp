/*
    SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "kconcatenaterowsproxymodel_qml.h"

#include "qmldeprecated.h"

#if KITEMMODELS_BUILD_DEPRECATED_SINCE(5, 80)

KConcatenateRowsProxyModelQml::KConcatenateRowsProxyModelQml(QObject *wrappedObject)
    : QObject(wrappedObject)
    , q(qobject_cast<KConcatenateRowsProxyModel *>(wrappedObject)){QML_DEPRECATED("KConcatenateRowsProxyModelQml", "5.80", "No known users.")}

    KConcatenateRowsProxyModelQml::~KConcatenateRowsProxyModelQml()
{
}

QQmlListProperty<QAbstractItemModel> KConcatenateRowsProxyModelQml::sources()
{
    return QQmlListProperty<QAbstractItemModel>(this,
                                                q,
                                                &KConcatenateRowsProxyModelQml::appendSource,
                                                &KConcatenateRowsProxyModelQml::sourceCount,
                                                &KConcatenateRowsProxyModelQml::source,
                                                &KConcatenateRowsProxyModelQml::clear);
}

void KConcatenateRowsProxyModelQml::appendSource(QQmlListProperty<QAbstractItemModel> *list, QAbstractItemModel *newItem)
{
    auto q = static_cast<KConcatenateRowsProxyModel *>(list->data);
    q->addSourceModel(newItem);
}

int KConcatenateRowsProxyModelQml::sourceCount(QQmlListProperty<QAbstractItemModel> *list)
{
    auto q = static_cast<KConcatenateRowsProxyModel *>(list->data);
    return q->sources().count();
}

QAbstractItemModel *KConcatenateRowsProxyModelQml::source(QQmlListProperty<QAbstractItemModel> *list, int index)
{
    auto q = static_cast<KConcatenateRowsProxyModel *>(list->data);
    return q->sources().at(index);
}

void KConcatenateRowsProxyModelQml::clear(QQmlListProperty<QAbstractItemModel> *list)
{
    auto q = static_cast<KConcatenateRowsProxyModel *>(list->data);
    const auto sources = q->sources();
    for (auto s : sources) {
        q->removeSourceModel(s);
    }
}

#endif
