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

#include "plugin.h"

#include <QQmlContext>
#include <QDebug>

#include <KDescendantsProxyModel>
#include "kconcatenaterowsproxymodel_qml.h"

void Plugin::initializeEngine(QQmlEngine *engine, const char *uri)
{
}

void Plugin::registerTypes(const char *uri)
{
    qmlRegisterType<QAbstractItemModel>();
    qmlRegisterExtendedType<KConcatenateRowsProxyModel,KConcatenateRowsProxyModelQml>(uri, 1, 0, "KConcatenateRowsProxyModel");
    qmlRegisterType<KDescendantsProxyModel>(uri, 1, 0, "KDescendantsProxyModel");
}


