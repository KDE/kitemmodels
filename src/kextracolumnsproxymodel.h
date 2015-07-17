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

#ifndef KEXTRACOLUMNSPROXYMODEL_H
#define KEXTRACOLUMNSPROXYMODEL_H

#include <QIdentityProxyModel>
#include <QScopedPointer>
#include "kitemmodels_export.h"

class KExtraColumnsProxyModelPrivate;

/**
 * This proxy appends extra columns (after all existing columns).
 *
 * The proxy supports source models that have a tree structure.
 * It also supports editing, and propagating changes from the source model.
 * Row insertion/removal, column insertion/removal in the source model are supported.
 *
 * Not supported: adding/removing extra columns at runtime; having a different number of columns in subtrees;
 * drag-n-drop support in the extra columns; moving columns.
 *
 * Derive from KExtraColumnsProxyModel, call appendColumn (typically in the constructor) for each extra column,
 * and reimplement extraColumnData() to allow KExtraColumnsProxyModel to retrieve the data to show in the extra columns.
 *
 * If you want your new column(s) to be somewhere else than at the right of the existing columns, you can
 * use a KRearrangeColumnsProxyModel on top.
 *
 * Author: David Faure, KDAB
 * @since 5.13
 */
class KITEMMODELS_EXPORT KExtraColumnsProxyModel : public QIdentityProxyModel
{
    Q_OBJECT
public:
    /**
     * Base class constructor.
     * Remember to call setSourceModel afterwards, and appendColumn.
     */
    explicit KExtraColumnsProxyModel(QObject *parent = 0);
    /**
     * Destructor.
     */
    ~KExtraColumnsProxyModel();

    // API

    /**
     * Appends an extra column.
     * @param header an optional text for the horizontal header
     */
    void appendColumn(const QString &header = QString());

    /**
     * This method is called by data() for extra columns.
     * Reimplement this method to return the data for the extra columns.
     *
     * @param parent the parent model index (only useful in tree models)
     * @param row the row number for which the model is querying for data (child of @p parent, if set)
     * @param extraColumn the number of the extra column, starting at 0 (this doesn't require knowing how many columns the source model has)
     * @param role the role being queried
     * @return the data at @p row and @p extraColumn
     */
    virtual QVariant extraColumnData(const QModelIndex &parent, int row, int extraColumn, int role = Qt::DisplayRole) const = 0;

    /**
     * This method is called by setData() for extra columns.
     * Reimplement this method to set the data for the extra columns, if editing is supported.
     * Remember to call extraColumnDataChanged() after changing the data storage.
     * The default implementation returns false.
     */
    virtual bool setExtraColumnData(const QModelIndex &parent, int row, int extraColumn, const QVariant &data, int role = Qt::EditRole);

    /**
     * This method can be called by your derived class when the data in an extra column has changed.
     * The use case is data that changes "by itself", unrelated to setData.
     */
    void extraColumnDataChanged(const QModelIndex &parent, int row, int extraColumn, const QVector<int> &roles);

    /**
     * Returns the extra column number (0, 1, ...) for a given column number of the proxymodel.
     * This basically means subtracting the amount of columns in the source model.
     */
    int extraColumnForProxyColumn(int proxyColumn) const;
    /**
     * Returns the proxy column number for a given extra column number (starting at 0).
     * This basically means adding the amount of columns in the source model.
     */
    int proxyColumnForExtraColumn(int extraColumn) const;


    // Implementation
    /// @reimp
    void setSourceModel(QAbstractItemModel *model) Q_DECL_OVERRIDE;
    /// @reimp
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const Q_DECL_OVERRIDE;
    /// @reimp
    QItemSelection mapSelectionToSource(const QItemSelection &selection) const Q_DECL_OVERRIDE;
    /// @reimp
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    /// @reimp
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    /// @reimp
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;
    /// @reimp
    QModelIndex sibling(int row, int column, const QModelIndex &idx) const Q_DECL_OVERRIDE;
    /// @reimp
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    /// @reimp
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    /// @reimp
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    /// @reimp
    QModelIndex parent(const QModelIndex &child) const Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(KExtraColumnsProxyModel)
    Q_PRIVATE_SLOT(d_func(), void _ec_sourceLayoutAboutToBeChanged(const QList<QPersistentModelIndex> &, QAbstractItemModel::LayoutChangeHint))
    Q_PRIVATE_SLOT(d_func(), void _ec_sourceLayoutChanged(const QList<QPersistentModelIndex> &, QAbstractItemModel::LayoutChangeHint))
    const QScopedPointer<KExtraColumnsProxyModelPrivate> d_ptr;

};

#endif
