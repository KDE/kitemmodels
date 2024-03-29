/*
    SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plugin.h"

#include <QDebug>
#include <QQmlContext>

#include "kdescendantsproxymodel_qml.h"
#include "krolenames.h"
#include "ksortfilterproxymodel.h"
#include <KColumnHeadersModel>
#include <KDescendantsProxyModel>
#include <KNumberModel>

using namespace Qt::StringLiterals;

void Plugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
    Q_UNUSED(engine);
    Q_UNUSED(uri);
}

void Plugin::registerTypes(const char *uri)
{
    qmlRegisterAnonymousType<QAbstractItemModel>(uri, 1);

    // 1.0
    qmlRegisterType<KDescendantsProxyModelQml>(uri, 1, 0, "KDescendantsProxyModel");
    qmlRegisterType<KNumberModel>(uri, 1, 0, "KNumberModel");
    qmlRegisterType<KColumnHeadersModel>(uri, 1, 0, "KColumnHeadersModel");
    qmlRegisterType<KSortFilterProxyModel>(uri, 1, 0, "KSortFilterProxyModel");

    // 1.1
    qmlRegisterUncreatableType<KRoleNames>(uri, 1, 1, "KRoleNames", u"KRoleNames can only be used as an attached property"_s);
}

#include "moc_plugin.cpp"
