/*
    SPDX-FileCopyrightText: 2020 Marco Martin <mart@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// This class exposes KDescendantsProxyModel in a more QML friendly way

#pragma once

#include <KDescendantsProxyModel>
#include <QObject>
#include <QPointer>

#include <qqmlregistration.h>

/*!
 * \qmltype KDescendantsProxyModel
 * \inqmlmodule org.kde.kitemmodels
 * \nativetype KDescendantsProxyModel
 * \brief Proxy Model for restructuring a Tree into a list.
 */
class KDescendantsProxyModelQml : public KDescendantsProxyModel
{
    Q_OBJECT
    QML_NAMED_ELEMENT(KDescendantsProxyModel)

public:
    explicit KDescendantsProxyModelQml(QObject *parent = nullptr);
    ~KDescendantsProxyModelQml() override;

    /*!
     * \qmlproperty var KDescendantsProxyModel::model
     * \since 5.62
     */

    /*!
     * \qmlproperty bool KDescendantsProxyModel::displayAncestorData
     * \since 5.62
     */

    /*!
     * \qmlmethod KDescendantsProxyModel::expandChildren(int row)
     */
    Q_INVOKABLE void expandChildren(int row);
    /*!
     * \qmlmethod KDescendantsProxyModel::collapseChildren(int row)
     */
    Q_INVOKABLE void collapseChildren(int row);
    /*!
     * \qmlmethod KDescendantsProxyModel::toggleChildren(int row)
     */
    Q_INVOKABLE void toggleChildren(int row);
};
