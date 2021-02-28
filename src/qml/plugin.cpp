/*
    SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugin.h"

#include <QDebug>
#include <QQmlContext>

#include "ksortfilterproxymodel.h"
#include <KColumnHeadersModel>
#include <KDescendantsProxyModel>
#include <KNumberModel>
#if KITEMMODELS_BUILD_DEPRECATED_SINCE(5, 80)
#include "kconcatenaterowsproxymodel_qml.h"
#endif
#include "kdescendantsproxymodel_qml.h"

void Plugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);
}

void Plugin::registerTypes(const char *uri)
{
    qmlRegisterAnonymousType<QAbstractItemModel>(uri, 1);
#if KITEMMODELS_BUILD_DEPRECATED_SINCE(5, 80)
    qmlRegisterExtendedType<KConcatenateRowsProxyModel, KConcatenateRowsProxyModelQml>(uri, 1, 0, "KConcatenateRowsProxyModel");
#endif
    qmlRegisterType<KDescendantsProxyModelQml>(uri, 1, 0, "KDescendantsProxyModel");
    qmlRegisterType<KNumberModel>(uri, 1, 0, "KNumberModel");
    qmlRegisterType<KColumnHeadersModel>(uri, 1, 0, "KColumnHeadersModel");
    qmlRegisterType<KSortFilterProxyModel>(uri, 1, 0, "KSortFilterProxyModel");
}
