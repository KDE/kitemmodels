/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KMODELINDEXOBSERVER_H
#define KMODELINDEXOBSERVER_H

#include <QAbstractItemModel>
#include <qqml.h>

#include <memory>

class QQmlPropertyMap;
class KModelIndexObserverPrivate;

/**
 * @class KModelIndexObserver
 *
 * @brief Observes a single index of a source model and notifies of data
 * changes.
 *
 * This class provides QQmlPropertyMap object property for declarative bindings
 * on model data. It works just like modelData context property inside delegates
 * of Repeater, ListView etc.
 *
 * Index can be set directly as a QModelIndex value, or as combination of row,
 * column and parentIndex properties. By default row is -1, column is 0, and
 * parentIndex is an invalid index. The result is now well-defined if both index
 * and of row, column or parentIndex are bound.
 *
 * Set of observed roles can be limited using roles or roleNames property.
 * Setting one will cause the other one to adjust accordingly. If model is not
 * set or some role/name is not found in mapping, the corresponding value in the
 * other list will be set to -1 for role or an empty string for role name.
 * The object will attempt to refresh roles array when the sourceModel property
 * changes, or the model emits reset signal.
 * By default, arrays are empty, which means all roles are observed.
 *
 * @since 6.2
 */
class KModelIndexObserver : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_ADDED_IN_MINOR_VERSION(2)

    /**
     * The source model whose data is to be observed.
     */
    Q_PROPERTY(QAbstractItemModel *sourceModel READ sourceModel WRITE setSourceModel NOTIFY sourceModelChanged)

    Q_PROPERTY(int row READ row WRITE setRow NOTIFY indexChanged FINAL)
    Q_PROPERTY(int column READ column WRITE setColumn NOTIFY indexChanged FINAL)
    Q_PROPERTY(QModelIndex parentIndex READ parentIndex WRITE setParentIndex NOTIFY indexChanged FINAL)
    Q_PROPERTY(QModelIndex index READ index WRITE setIndex NOTIFY indexChanged FINAL)

    Q_PROPERTY(QList<int> roles READ roles WRITE setRoles RESET resetRoles NOTIFY rolesChanged FINAL)
    Q_PROPERTY(QStringList roleNames READ roleNames WRITE setRoleNames RESET resetRoles NOTIFY rolesChanged FINAL)

    Q_PROPERTY(QQmlPropertyMap *modelData READ modelData NOTIFY modelDataChanged FINAL)

public:
    explicit KModelIndexObserver(QObject *parent = nullptr);
    ~KModelIndexObserver() override;

    QAbstractItemModel *sourceModel() const;
    void setSourceModel(QAbstractItemModel *model);

    int row() const;
    void setRow(int row);

    int column() const;
    void setColumn(int column);

    QModelIndex parentIndex() const;
    void setParentIndex(const QModelIndex &parentIndex);

    QModelIndex index() const;
    void setIndex(const QModelIndex &index);

    QList<int> roles() const;
    void setRoles(const QList<int> &roles);

    QStringList roleNames() const;
    void setRoleNames(const QStringList &roleNames);

    void resetRoles();

    QQmlPropertyMap *modelData() const;

Q_SIGNALS:
    void sourceModelChanged();
    void indexChanged();
    void rolesChanged();
    void modelDataChanged();

private Q_SLOTS:
    void update();
    void updateModelData(const QList<int> &roles = {});
    void rowsInsertedOrRemoved(const QModelIndex &parent, int first, int last);
    void columnsInsertedOrRemoved(const QModelIndex &parent, int first, int last);
    void rowsMoved(const QModelIndex &parent, int first, int last, const QModelIndex &destinationParent, int destinationRow);
    void columnsMoved(const QModelIndex &parent, int first, int last, const QModelIndex &destinationParent, int destinationColumn);
    void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles);
    void sourceModelDestroyed(QObject *source);

private:
    std::unique_ptr<KModelIndexObserverPrivate> const d;
};

#endif
