/*
    SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
    SPDX-FileContributor: David Faure <david.faure@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef REARRANGECOLUMNSPROXYMODEL_H
#define REARRANGECOLUMNSPROXYMODEL_H

#include "kitemmodels_export.h"

#include <QtGlobal>

#if QT_VERSION < QT_VERSION_CHECK(7, 0, 0)
#include <QIdentityProxyModel>
#else
#include <QAbstractProxyModel>
#endif

#include <memory>

class KRearrangeColumnsProxyModelPrivate;

/*!
 * \class KRearrangeColumnsProxyModel
 * \inmodule KItemModels
 * \brief This proxy shows specific columns from the source model, in any order.
 *
 * This allows to reorder columns, as well as not showing all of them.
 *
 * The proxy supports source models that have a tree structure.
 * It also supports editing, and propagating changes from the source model.
 *
 * Showing the same source column more than once is not supported.
 *
 * \since 5.12
 */
#if QT_VERSION < QT_VERSION_CHECK(7, 0, 0)
// QIdentityProxyModel is only kept for binary compatibility, its signal forwarding isn't used
class KITEMMODELS_EXPORT KRearrangeColumnsProxyModel : public QIdentityProxyModel
#else
class KITEMMODELS_EXPORT KRearrangeColumnsProxyModel : public QAbstractProxyModel
#endif
{
    Q_OBJECT
public:
    /*!
     * Creates a KRearrangeColumnsProxyModel proxy.
     * Remember to call setSourceModel() afterwards.
     */
    explicit KRearrangeColumnsProxyModel(QObject *parent = nullptr);
    ~KRearrangeColumnsProxyModel() override;

    // API

    /*!
     * Set the chosen source columns, in the desired order for the proxy columns
     * columns[proxyColumn=0] is the source column to show in the first proxy column, etc.
     *
     * Example: QList<int>() << 2 << 1;
     * This examples configures the proxy to hide column 0, show column 2 from the source model,
     * then show column 1 from the source model.
     */
    void setSourceColumns(const QList<int> &columns);

    // Implementation

    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool hasChildren(const QModelIndex &parent = QModelIndex()) const override;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;
    bool moveRows(const QModelIndex &sourceParent, int sourceRow, int count, const QModelIndex &destinationParent, int destinationChild) override;
    bool canDropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const override;

    /*!
     * Returns the proxy column for the given source column
     * or -1 if the source column isn't shown in the proxy.
     * \since 5.56
     */
    int proxyColumnForSourceColumn(int sourceColumn) const;

    /*!
     * Returns the source column for the given proxy column.
     * \since 5.56
     */
    int sourceColumnForProxyColumn(int proxyColumn) const;

    void setSourceModel(QAbstractItemModel *sourceModel) override;

private:
    friend class KRearrangeColumnsProxyModelPrivate;
    std::unique_ptr<KRearrangeColumnsProxyModelPrivate> const d_ptr;
};

#endif
