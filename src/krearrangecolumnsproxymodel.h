/*
    Copyright (c) 2015 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
    Authors: David Faure <david.faure@kdab.com>

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

#ifndef REARRANGECOLUMNSPROXYMODEL_H
#define REARRANGECOLUMNSPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QScopedPointer>
#include "kitemmodels_export.h"

class KRearrangeColumnsProxyModelPrivate;

/**
 * This proxy shows specific columns from the source model, in any order.
 * This allows to reorder columns, as well as not showing all of them.
 *
 * The proxy supports source models that have a tree structure.
 * It also supports editing, and propagating changes from the source model.
 *
 * Showing the same source column more than once is not supported.
 *
 * Author: David Faure, KDAB
 * @since 5.12
 */
class KITEMMODELS_EXPORT KRearrangeColumnsProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    /**
     * Creates a KRearrangeColumnsProxyModel proxy.
     * Remember to call setSourceModel afterwards.
     */
    explicit KRearrangeColumnsProxyModel(QObject *parent = 0);
    /**
     * Destructor.
     */
    ~KRearrangeColumnsProxyModel();

    // API

    /**
     * Set the chosen source columns, in the desired order for the proxy columns
     * columns[proxyColumn=0] is the source column to show in the first proxy column, etc.
     *
     * Example: QVector<int>() << 2 << 1;
     * This examples configures the proxy to hide column 0, show column 2 from the source model,
     * then show column 1 from the source model.
     */
    void setSourceColumns(const QVector<int> &columns);

    // Implementation

    /// @reimp
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    /// @reimp
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    /// @reimp
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    /// @reimp
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;

    /// @reimp
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const Q_DECL_OVERRIDE;
    /// @reimp
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;

    /// @reimp
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    int proxyColumnForSourceColumn(int sourceColumn) const;
    int sourceColumnForProxyColumn(int proxyColumn) const;

private:
    const QScopedPointer<KRearrangeColumnsProxyModelPrivate> d_ptr;
};

#endif
