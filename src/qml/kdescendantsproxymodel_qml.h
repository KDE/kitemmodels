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
 * \brief Flattens a tree model into a list.
 *
 * Set \l model to the source tree model. Each item in the flattened list can
 * be expanded or collapsed using its row in this proxy model.
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
     * The source tree model to flatten.
     * \since 5.62
     */

    /*!
     * \qmlproperty var KDescendantsProxyModel::sourceModel
     * The source tree model to flatten. This inherited property refers to the
     * same model as \l model.
     */

    /*!
     * \qmlproperty bool KDescendantsProxyModel::displayAncestorData
     * \default false
     * Whether display data includes the item's ancestors.
     * \since 5.62
     */

    /*!
     * \qmlproperty string KDescendantsProxyModel::ancestorSeparator
     * \default " / "
     * The separator between ancestor values when displayAncestorData is true.
     * \since 5.62
     */

    /*!
     * \qmlproperty bool KDescendantsProxyModel::expandsByDefault
     * \default true
     * Whether all items are expanded when the source model is loaded.
     * \since 5.74
     */

    /*!
     * \qmlmethod QModelIndex KDescendantsProxyModel::mapToSource(QModelIndex proxyIndex)
     * Returns the source model index corresponding to \a proxyIndex.
     *
     * \sa mapFromSource()
     */

    /*!
     * \qmlmethod QModelIndex KDescendantsProxyModel::mapFromSource(QModelIndex sourceIndex)
     * Returns the proxy model index corresponding to \a sourceIndex.
     *
     * \sa mapToSource()
     */

    /*!
     * \qmlmethod QItemSelection KDescendantsProxyModel::mapSelectionToSource(QItemSelection proxySelection)
     * Returns the source model selection corresponding to \a proxySelection.
     */

    /*!
     * \qmlmethod QItemSelection KDescendantsProxyModel::mapSelectionFromSource(QItemSelection sourceSelection)
     * Returns the proxy model selection corresponding to \a sourceSelection.
     */

    /*!
     * \qmlmethod void KDescendantsProxyModel::expandChildren(int row)
     * Expands the children of the item at \a row in this proxy model.
     */
    Q_INVOKABLE void expandChildren(int row);
    /*!
     * \qmlmethod void KDescendantsProxyModel::collapseChildren(int row)
     * Collapses the children of the item at \a row in this proxy model.
     */
    Q_INVOKABLE void collapseChildren(int row);
    /*!
     * \qmlmethod void KDescendantsProxyModel::toggleChildren(int row)
     * Toggles the expansion of the item at \a row in this proxy model.
     */
    Q_INVOKABLE void toggleChildren(int row);
};
