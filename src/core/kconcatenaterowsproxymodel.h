/*
    SPDX-FileCopyrightText: 2015 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.com>
    SPDX-FileContributor: David Faure <david.faure@kdab.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KCONCATENATEROWSPROXYMODEL_H
#define KCONCATENATEROWSPROXYMODEL_H

#include "kitemmodels_export.h"
#include <QAbstractItemModel>

#include <memory>

#if KITEMMODELS_ENABLE_DEPRECATED_SINCE(5, 80)

class KConcatenateRowsProxyModelPrivate;

/**
 * @class KConcatenateRowsProxyModel kconcatenaterowsproxymodel.h KConcatenateRowsProxyModel
 *
 * This proxy takes multiple source models and concatenates their rows.
 *
 * In other words, the proxy will have all rows of the first source model,
 * followed by all rows of the second source model, etc.
 *
 * Only flat models (lists and tables) are supported, no trees.
 *
 * All models are assumed to have the same number of columns.
 * More precisely, the number of columns of the first source model is used,
 * so all source models should have at least as many columns as the first source model,
 * and additional columns in other source models will simply be ignored.
 *
 * Source models can be added and removed at runtime, including the first source
 * model (but it should keep the same number of columns).
 *
 * Dynamic insertion and removal of rows and columns in any source model is supported.
 * dataChanged, layoutChanged and reset coming from the source models are supported.
 *
 * At the moment this model doesn't support editing, drag-n-drop.
 * It could be added though, nothing prevents it.
 *
 * This proxy does not inherit from QAbstractProxyModel because it uses multiple source
 * models, rather than a single one.
 *
 * Author: David Faure, KDAB
 * @since 5.14
 *
 * @deprecated Since 5.80 use QConcatenateTablesProxyModel instead, which is
 * part of Qt >= 5.13.
 */
class KITEMMODELS_EXPORT KConcatenateRowsProxyModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    /**
     * Creates a KConcatenateRowsProxyModel.
     * @param parent optional parent
     */
    KITEMMODELS_DEPRECATED_VERSION(5, 80, "Use QConcatenateTablesProxyModel")
    explicit KConcatenateRowsProxyModel(QObject *parent = nullptr);
    /**
     * Destructor.
     */
    ~KConcatenateRowsProxyModel() override;

    /**
     * Adds a source model @p sourceModel, after all existing source models.
     * @param sourceModel the source model
     *
     * The ownership of @p sourceModel is not affected by this.
     * The same source model cannot be added more than once.
     */
    Q_SCRIPTABLE void addSourceModel(QAbstractItemModel *sourceModel);

    /**
     * Removes the source model @p sourceModel.
     * @param sourceModel a source model previously added to this proxy
     *
     * The ownership of @sourceModel is not affected by this.
     */
    Q_SCRIPTABLE void removeSourceModel(QAbstractItemModel *sourceModel);

    /**
     * The currently set source models
     */
    QList<QAbstractItemModel *> sources() const;

    /**
     * Returns the proxy index for a given source index
     * @param sourceIndex an index coming from any of the source models
     * @return a proxy index
     * Calling this method with an index not from a source model is undefined behavior.
     */
    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;

    /**
     * Returns the source index for a given proxy index.
     * @param proxyIndex an index for this proxy model
     * @return a source index
     */
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

    /// @reimp
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /// @reimp
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole) override;
    /// @reimp
    QMap<int, QVariant> itemData(const QModelIndex &proxyIndex) const override;
    /// @reimp
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    /// @reimp
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    /// @reimp
    QModelIndex parent(const QModelIndex &index) const override;
    /// @reimp
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * The horizontal header data for the first source model is returned here.
     * @reimp
     */
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    /**
     * The column count for the first source model is returned here.
     * @reimp
     */
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    /**
     * The roles names for the first source model is returned here
     * @reimp
     */
    QHash<int, QByteArray> roleNames() const override;

private:
    Q_PRIVATE_SLOT(d, void slotRowsAboutToBeInserted(const QModelIndex &, int start, int end))
    Q_PRIVATE_SLOT(d, void slotRowsInserted(const QModelIndex &, int start, int end))
    Q_PRIVATE_SLOT(d, void slotRowsAboutToBeRemoved(const QModelIndex &, int start, int end))
    Q_PRIVATE_SLOT(d, void slotRowsRemoved(const QModelIndex &, int start, int end))
    Q_PRIVATE_SLOT(d, void slotColumnsAboutToBeInserted(const QModelIndex &parent, int start, int end))
    Q_PRIVATE_SLOT(d, void slotColumnsInserted(const QModelIndex &parent, int, int))
    Q_PRIVATE_SLOT(d, void slotColumnsAboutToBeRemoved(const QModelIndex &parent, int start, int end))
    Q_PRIVATE_SLOT(d, void slotColumnsRemoved(const QModelIndex &parent, int, int))
    Q_PRIVATE_SLOT(d, void slotDataChanged(const QModelIndex &from, const QModelIndex &to, const QVector<int> &roles))
    Q_PRIVATE_SLOT(d, void slotSourceLayoutAboutToBeChanged(QList<QPersistentModelIndex>, QAbstractItemModel::LayoutChangeHint))
    Q_PRIVATE_SLOT(d, void slotSourceLayoutChanged(const QList<QPersistentModelIndex> &, QAbstractItemModel::LayoutChangeHint))
    Q_PRIVATE_SLOT(d, void slotModelAboutToBeReset())
    Q_PRIVATE_SLOT(d, void slotModelReset())

private:
    friend class KConcatenateRowsProxyModelPrivate;
    std::unique_ptr<KConcatenateRowsProxyModelPrivate> const d;
};

#endif

#endif
