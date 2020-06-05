/*
    SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugin.h"

#include <QQmlContext>
#include <QDebug>

#include <KDescendantsProxyModel>
#include <KNumberModel>
#include <KColumnHeadersModel>
#include "ksortfilterproxymodel.h"
#include "kconcatenaterowsproxymodel_qml.h"
#include "kdescendantsproxymodel_qml.h"

void Plugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);
}

void Plugin::registerTypes(const char *uri)
{
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
    qmlRegisterAnonymousType<QAbstractItemModel>(uri, 1);
#else
    qmlRegisterType<QAbstractItemModel>();
#endif
    qmlRegisterExtendedType<KConcatenateRowsProxyModel,KConcatenateRowsProxyModelQml>(uri, 1, 0, "KConcatenateRowsProxyModel");
    qmlRegisterType<KDescendantsProxyModelQml>(uri, 1, 0, "KDescendantsProxyModel");
    qmlRegisterType<KNumberModel>(uri, 1, 0, "KNumberModel");
    qmlRegisterType<KColumnHeadersModel>(uri, 1, 0, "KColumnHeadersModel");
    qmlRegisterType<KSortFilterProxyModel>(uri, 1, 0, "KSortFilterProxyModel");
}
