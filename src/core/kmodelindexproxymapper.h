/*
    SPDX-FileCopyrightText: 2010 Klarälvdalens Datakonsult AB, a KDAB Group company <info@kdab.net>
    SPDX-FileContributor: Stephen Kelly <stephen@kdab.com>
    SPDX-FileCopyrightText: 2016 Ableton AG <info@ableton.com>
    SPDX-FileContributor: Stephen Kelly <stephen.kelly@ableton.com>
    SPDX-FileCopyrightText: 2026 Jakob Petsovits <jpetso@petsovits.com>
    SPDX-FileContributor: Jakob Petsovits <jpetso@petsovits.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef KMODELINDEXPROXYMAPPER_H
#define KMODELINDEXPROXYMAPPER_H

#include <QObject>

#include "kitemmodels_export.h"

#include <memory>

class QAbstractItemModel;
class QModelIndex;
class QItemSelection;
class KModelIndexProxyMapperPrivate;

/*!
 * \class KModelIndexProxyMapper
 * \inmodule KItemModels
 * \brief This class facilitates easy mapping of indexes and selections through proxy models.
 *
 * In a complex system of proxy models there can be a need to map indexes and selections between them,
 * and sometimes to do so without knowledge of the path from one model to another.
 *
 * For example,
 *
 * \code
 *     Root model
 *         |
 *       /    \
 *   Proxy 1   Proxy 3
 *      |       |
 *   Proxy 2   Proxy 4
 * \endcode
 *
 * If there is a need to map indexes between proxy 2 and proxy 4, a KModelIndexProxyMapper can be created
 * to facilitate mapping of indexes between them.
 *
 * \code
 *   m_indexMapper = new KModelIndexProxyMapper(proxy2, proxy4, this);
 *
 *  ...
 *
 *   const QModelIndex proxy4Index = m_indexMapper->mapLeftToRight(proxy2->index(0, 0));
 *   Q_ASSERT(proxy4Index.model() == proxy4);
 * \endcode
 *
 * Note that the aim is to achieve black box connections so that there is no need for application code to
 * know the structure of proxy models in the path between left and right and attempt to manually map them.
 *
 * \code
 *     Root model
 *         |
 *   ---------------
 *   |  Black Box  |
 *   ---------------
 *      |       |
 *   Proxy 2   Proxy 4
 * \endcode
 *
 * The isConnected property indicates whether there is a
 * path from the left side to the right side.
 *
 * KModelIndexProxyMapper also works with a simple (but possibly deep) proxy model chain:
 *
 * \code
 *     Root model
 *         |
 *      Proxy 1
 *         |
 *      Proxy 2
 *         |
 *      Proxy 3
 * \endcode
 *
 * Without this class, you may find yourself calling multiple mapping functions. For example,
 *
 * \code
 *   auto proxy3Index = proxy3->index(0, 0);
 *   auto proxy2Index = proxy3->mapToSource(proxy3Index);
 *   auto proxy1Index = proxy2->mapToSource(proxy2Index);
 *   auto rootIndex = proxy1->mapToSource(proxy1Index);
 *   Q_ASSERT(rootIndex.model() == rootModel);
 *
 *   proxy1Index = proxy1->mapFromSource(rootIndex);
 *   proxy2Index = proxy2->mapFromSource(proxy1Index);
 *   proxy3Index = proxy3->mapFromSource(proxy2Index);
 *   Q_ASSERT(proxy3Index.model() == proxy3);
 * \endcode
 *
 * This can be simplified to a single method call:
 *
 * \code
 *   m_indexMapper = new KModelIndexProxyMapper(proxy3, rootModel, this);
 *
 *   auto rootIndex = m_indexMapper->mapLeftToRight(proxy3->index(0, 0));
 *   Q_ASSERT(rootIndex.model() == rootModel);
 *
 *   auto proxy3Index = m_indexMapper->mapRightToLeft(rootIndex);
 *   Q_ASSERT(proxy3Index.model() == proxy3);
 * \endcode
 */
class KITEMMODELS_EXPORT KModelIndexProxyMapper : public QObject
{
    Q_OBJECT

    /*!
     * \property KModelIndexProxyMapper::isConnected
     *
     * Indicates whether there is a chain that can be followed from leftModel to rightModel.
     *
     * This value can change if the sourceModel of an intermediate proxy is changed.
     */
    Q_PROPERTY(bool isConnected READ isConnected NOTIFY isConnectedChanged)

    /*!
     * \property KModelIndexProxyMapper::leftModel
     *
     * If a new pair of models is assigned, the proxy chain between both will be recalculated.
     *
     * \sa isConnected
     * \since 6.30
     */
    Q_PROPERTY(const QAbstractItemModel *leftModel READ leftModel WRITE setLeftModel NOTIFY leftModelChanged)

    /*!
     * \property KModelIndexProxyMapper::rightModel
     *
     * If a new pair of models is assigned, the proxy chain between both needs to be recalculated.
     *
     * \sa isConnected
     * \since 6.30
     */
    Q_PROPERTY(const QAbstractItemModel *rightModel READ rightModel WRITE setRightModel NOTIFY rightModelChanged)

public:
    /*!
     * Constructor
     */
    KModelIndexProxyMapper(const QAbstractItemModel *leftModel, const QAbstractItemModel *rightModel, QObject *parent = nullptr);

    /*!
     * Disconnected default constructor: set leftModel and rightModel to make it functional
     */
    KModelIndexProxyMapper(QObject *parent = nullptr);

    ~KModelIndexProxyMapper() override;

    /*!
     * Maps the \a index from the left model to the right model.
     */
    Q_INVOKABLE QModelIndex mapLeftToRight(const QModelIndex &index) const;

    /*!
     * Maps the \a index from the right model to the left model.
     */
    Q_INVOKABLE QModelIndex mapRightToLeft(const QModelIndex &index) const;

    /*!
     * Maps the \a selection from the left model to the right model.
     */
    Q_INVOKABLE QItemSelection mapSelectionLeftToRight(const QItemSelection &selection) const;

    /*!
     * Maps the \a selection from the right model to the left model.
     */
    Q_INVOKABLE QItemSelection mapSelectionRightToLeft(const QItemSelection &selection) const;

    bool isConnected() const;

    //! \since 6.30
    const QAbstractItemModel *leftModel() const;
    //! \since 6.30
    const QAbstractItemModel *rightModel() const;

    //! \since 6.30
    void setLeftModel(const QAbstractItemModel *);
    //! \since 6.30
    void setRightModel(const QAbstractItemModel *);

Q_SIGNALS:
    void isConnectedChanged();

    //! \since 6.30
    void leftModelChanged();

    //! \since 6.30
    void rightModelChanged();

private:
    //@cond PRIVATE
    Q_DECLARE_PRIVATE(KModelIndexProxyMapper)
    std::unique_ptr<KModelIndexProxyMapperPrivate> const d_ptr;
    //@endcond
};

#endif
