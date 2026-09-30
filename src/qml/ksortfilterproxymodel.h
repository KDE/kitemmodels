/*
 *   SPDX-FileCopyrightText: 2010 Marco Martin <mart@kde.org>
 *   SPDX-FileCopyrightText: 2019 David Edmundson <davidedmundson@kde.org>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#ifndef KSORTFILTERPROXYMODEL_H
#define KSORTFILTERPROXYMODEL_H

#include <QAbstractItemModel>
#include <QJSValue>
#include <QList>
#include <QQmlParserStatus>
#include <QSortFilterProxyModel>
#include <qqmlregistration.h>

#include <array>

/*!
 * \qmltype KSortFilterProxyModel
 * \inqmlmodule org.kde.kitemmodels
 * \nativetype QSortFilterProxyModel
 * \brief Filter and sort an existing \l QAbstractItemModel.
 *
 * For example, filter and sort a list by its name role:
 *
 * \qml
 * import QtQuick
 * import org.kde.kitemmodels
 *
 * ListView {
 *     width: 200
 *     height: 200
 *     model: KSortFilterProxyModel {
 *         sourceModel: ListModel {
 *             ListElement { name: "Alice" }
 *             ListElement { name: "Bob" }
 *             ListElement { name: "Charlie" }
 *         }
 *         filterRoleName: "name"
 *         filterString: "li"
 *         filterCaseSensitivity: Qt.CaseInsensitive
 *         sortRoleName: "name"
 *         sortCaseSensitivity: Qt.CaseInsensitive
 *         sortColumn: 0
 *     }
 *     delegate: Text { text: name }
 * }
 * \endqml
 *
 * \since 5.67
 */
class KSortFilterProxyModel : public QSortFilterProxyModel, public QQmlParserStatus
{
    Q_OBJECT
    QML_ELEMENT
    Q_INTERFACES(QQmlParserStatus)

    /*!
     * \qmlproperty var KSortFilterProxyModel::sourceModel
     * The model whose rows are filtered and sorted.
     */

    /*!
     * \qmlproperty bool KSortFilterProxyModel::dynamicSortFilter
     * \default true
     * Whether the proxy updates its sorting and filtering when sourceModel
     * changes.
     */

    /*!
     * \qmlproperty int KSortFilterProxyModel::filterKeyColumn
     * \default 0
     * The source column used for filtering. A value of -1 checks all columns.
     */

    /*!
     * \qmlproperty int KSortFilterProxyModel::filterRole
     * \default Qt.DisplayRole
     * The source role ID used for filtering. Setting this also updates
     * filterRoleName.
     */

    /*!
     * \qmlproperty var KSortFilterProxyModel::filterRegularExpression
     * The regular expression used to filter source rows. Setting filterString
     * replaces it with a fixed-string filter.
     */

    /*!
     * \qmlproperty Qt::CaseSensitivity KSortFilterProxyModel::filterCaseSensitivity
     * \default Qt.CaseSensitive
     * The case sensitivity used when filtering strings.
     */

    /*!
     * \qmlproperty int KSortFilterProxyModel::sortRole
     * \default Qt.DisplayRole
     * The source role ID used for sorting. Setting this also updates
     * sortRoleName.
     */

    /*!
     * \qmlproperty Qt::CaseSensitivity KSortFilterProxyModel::sortCaseSensitivity
     * \default Qt.CaseSensitive
     * The case sensitivity used when sorting strings.
     */

    /*!
     * \qmlproperty bool KSortFilterProxyModel::isSortLocaleAware
     * \default false
     * Whether string sorting follows the current locale.
     */

    /*!
     * \qmlproperty bool KSortFilterProxyModel::recursiveFilteringEnabled
     * \default false
     * Whether a parent remains visible when one of its descendants matches
     * the filter.
     */

    /*!
     * \qmlproperty bool KSortFilterProxyModel::autoAcceptChildRows
     * \default false
     * Whether all children of an accepted row remain visible, even if they
     * do not match the filter themselves.
     */

    /*!
     * \qmlproperty string KSortFilterProxyModel::filterString
     * A fixed string used to filter rows by filterRole. Only matching rows are displayed.
     */
    Q_PROPERTY(QString filterString READ filterString WRITE setFilterString NOTIFY filterStringChanged)
    /*!
     * \qmlproperty var KSortFilterProxyModel::filterRowCallback
     *
     * A JavaScript callable that can be used to perform advanced filters on a given row.
     * The callback is passed the source row, and source parent for a given row as arguments
     *
     * The callable's return value is evaluated as boolean to determine
     * whether the row is accepted (true) or filtered out (false). It overrides the default row filter,
     * so filterString and filterRegularExpression have no effect while filterRowCallback is set.
     * Attempts to write a non-callable to this property are silently ignored, but you can set
     * it to null.
     *
     * \qml
     * filterRowCallback: function(source_row, source_parent) {
     *   return sourceModel.data(sourceModel.index(source_row, 0, source_parent), Qt.DisplayRole) == "...";
     * };
     * \endqml
     */
    Q_PROPERTY(QJSValue filterRowCallback READ filterRowCallback WRITE setFilterRowCallback NOTIFY filterRowCallbackChanged)

    /*!
     * \qmlproperty var KSortFilterProxyModel::filterColumnCallback
     *
     * A JavaScript callable that can be used to perform advanced filters on a given column.
     * The callback is passed the source column, and source parent for a given column as arguments.
     *
     * \sa filterRowCallback
     */
    Q_PROPERTY(QJSValue filterColumnCallback READ filterColumnCallback WRITE setFilterColumnCallback NOTIFY filterColumnCallbackChanged)

    /*!
     * \qmlproperty string KSortFilterProxyModel::filterRoleName
     *
     * The name of the source role used for filtering. Use filterRole to set a
     * numerical role ID instead.
     */
    Q_PROPERTY(QString filterRoleName READ filterRoleName WRITE setFilterRoleName NOTIFY filterRoleNameChanged)

    /*!
     * \qmlproperty string KSortFilterProxyModel::sortRoleName
     *
     * The name of the source role used for sorting. Use sortRole to set a
     * numerical role ID instead. An empty name leaves rows unsorted.
     */
    Q_PROPERTY(QString sortRoleName READ sortRoleName WRITE setSortRoleName NOTIFY sortRoleNameChanged)

    /*!
     * \qmlproperty Qt::SortOrder KSortFilterProxyModel::sortOrder
     *
     * One of Qt.AscendingOrder or Qt.DescendingOrder
     */
    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder WRITE setSortOrder NOTIFY sortOrderChanged)

    /*!
     * \qmlproperty int KSortFilterProxyModel::sortColumn
     * \default -1
     *
     * Specify which column should be used for sorting
     * Setting sortRoleName sorts column 0 if no column has been selected.
     */
    Q_PROPERTY(int sortColumn READ sortColumn WRITE setSortColumn NOTIFY sortColumnChanged)

    /*!
     * \qmlproperty int KSortFilterProxyModel::count
     *
     * The number of top level rows.
     */
    Q_PROPERTY(int count READ rowCount NOTIFY rowCountChanged)

public:
    explicit KSortFilterProxyModel(QObject *parent = nullptr);
    ~KSortFilterProxyModel() override;

    void setSourceModel(QAbstractItemModel *sourceModel) override;

    void setFilterRowCallback(const QJSValue &callback);
    QJSValue filterRowCallback() const;

    void setFilterString(const QString &filterString);
    QString filterString() const;

    void setFilterColumnCallback(const QJSValue &callback);
    QJSValue filterColumnCallback() const;

    void setFilterRoleName(const QString &roleName);
    QString filterRoleName() const;

    void setSortRoleName(const QString &roleName);
    QString sortRoleName() const;

    void setSortOrder(const Qt::SortOrder order);
    void setSortColumn(int column);

    void classBegin() override;
    void componentComplete() override;

public Q_SLOTS:
    /*!
     * \qmlmethod QModelIndex KSortFilterProxyModel::mapToSource(QModelIndex proxyIndex)
     * Returns the source model index corresponding to \a proxyIndex.
     *
     * \sa mapFromSource()
     */

    /*!
     * \qmlmethod QModelIndex KSortFilterProxyModel::mapFromSource(QModelIndex sourceIndex)
     * Returns the proxy model index corresponding to \a sourceIndex.
     * A filtered-out item has no corresponding proxy index.
     *
     * \sa mapToSource()
     */

    /*!
     * \qmlmethod QItemSelection KSortFilterProxyModel::mapSelectionToSource(QItemSelection proxySelection)
     * Returns the source model selection corresponding to \a proxySelection.
     */

    /*!
     * \qmlmethod QItemSelection KSortFilterProxyModel::mapSelectionFromSource(QItemSelection sourceSelection)
     * Returns the proxy model selection corresponding to \a sourceSelection.
     */

    /*!
     * \qmlmethod void KSortFilterProxyModel::setFilterRegularExpression(string pattern)
     * Filters rows using the regular expression \a pattern.
     */

    /*!
     * \qmlmethod void KSortFilterProxyModel::setFilterRegularExpression(var regularExpression)
     * Filters rows using \a regularExpression.
     */

    /*!
     * \qmlmethod void KSortFilterProxyModel::setFilterWildcard(string pattern)
     * Filters rows using the wildcard \a pattern.
     */

    /*!
     * \qmlmethod void KSortFilterProxyModel::setFilterFixedString(string pattern)
     * Filters rows using the literal string \a pattern.
     */

    /*!
     * \qmlmethod void KSortFilterProxyModel::invalidate()
     * Reapplies sorting and filtering to the source model.
     */

    /*!
     * \qmlmethod void KSortFilterProxyModel::invalidateFilter()
     * Invalidates the current filtering.
     *
     * This function should be called if you are implementing custom filtering through
     * filterRowCallback or filterColumnCallback, and your filter parameters have changed.
     *
     * \since 5.70
     */
    void invalidateFilter();

Q_SIGNALS:
    void filterStringChanged();
    void filterRoleNameChanged();
    void sortRoleNameChanged();
    void sortOrderChanged();
    void sortColumnChanged();
    void filterRowCallbackChanged(const QJSValue &);
    void filterColumnCallbackChanged(const QJSValue &);
    void rowCountChanged();

protected:
    int roleNameToId(const QString &name) const;
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    bool filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const override;

protected Q_SLOTS:
    // This method is called whenever we suspect that role names mapping might have gone stale.
    // It must not alter the source of truth for sort/filter properties.
    void syncRoleNames();
    // These methods are dealing with individual pairs of properties. They are
    // called on various occasions, and need to check whether the invocation
    // has been caused by a standalone base type's property change
    // (switching source of truth to role ID) or as a side-effect of a sync.
    void syncSortRoleProperties();
    void syncFilterRoleProperties();

private:
    // conveniently, role ID is the default source of truth, turning it into
    // zero-initialization.
    enum SourceOfTruthForRoleProperty : bool {
        SourceOfTruthIsRoleID = false,
        SourceOfTruthIsRoleName = true,
    };

    bool m_componentCompleted : 1;
    SourceOfTruthForRoleProperty m_sortRoleSourceOfTruth : 1;
    SourceOfTruthForRoleProperty m_filterRoleSourceOfTruth : 1;
    bool m_sortRoleGuard : 1;
    bool m_filterRoleGuard : 1;
    // default role name corresponds to the standard mapping of the default Qt::DisplayRole in QAbstractItemModel::roleNames
    QString m_sortRoleName{QStringLiteral("display")};
    QString m_filterRoleName{QStringLiteral("display")};
    QString m_filterString;
    QJSValue m_filterRowCallback;
    QJSValue m_filterColumnCallback;
    QHash<QString, int> m_roleIds;
    std::array<QMetaObject::Connection, 3> m_sourceModelConnections;
};

#endif
