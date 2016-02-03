/*
    Copyright (C) 2010 Klarälvdalens Datakonsult AB,
        a KDAB Group company, info@kdab.net,
        author Stephen Kelly <stephen@kdab.com>
    Copyright (c) 2016 Ableton AG <info@ableton.com>
        Author Stephen Kelly <stephen.kelly@ableton.com>

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

#include "klinkitemselectionmodel.h"

#include "kmodelindexproxymapper.h"

#include <QItemSelection>
#include <QDebug>

class KLinkItemSelectionModelPrivate
{
public:
    KLinkItemSelectionModelPrivate(KLinkItemSelectionModel *proxySelectionModel)
        : q_ptr(proxySelectionModel),
          m_linkedItemSelectionModel(Q_NULLPTR),
          m_ignoreCurrentChanged(false),
          m_indexMapper(Q_NULLPTR)
    {
        QObject::connect(q_ptr, &QItemSelectionModel::currentChanged, q_ptr,
            [this](const QModelIndex& idx) { slotCurrentChanged(idx); } );

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        QObject::connect(q_ptr, &QItemSelectionModel::modelChanged, q_ptr, [this] {
            reinitializeIndexMapper();
        });
#endif
    }

    Q_DECLARE_PUBLIC(KLinkItemSelectionModel)
    KLinkItemSelectionModel *const q_ptr;

    bool assertSelectionValid(const QItemSelection &selection) const
    {
        Q_FOREACH (const QItemSelectionRange &range, selection) {
            if (!range.isValid()) {
                qDebug() << selection;
            }
            Q_ASSERT(range.isValid());
        }
        return true;
    }

    void reinitializeIndexMapper()
    {
        delete m_indexMapper;
        m_indexMapper = Q_NULLPTR;
        if (!q_ptr->model()
                || !m_linkedItemSelectionModel
                || !m_linkedItemSelectionModel->model()) {
            return;
        }
        m_indexMapper = new KModelIndexProxyMapper(
            q_ptr->model(),
            m_linkedItemSelectionModel->model(),
            q_ptr);
        const QItemSelection mappedSelection = m_indexMapper->mapSelectionRightToLeft(m_linkedItemSelectionModel->selection());
        q_ptr->QItemSelectionModel::select(mappedSelection, QItemSelectionModel::ClearAndSelect);
    }

    void sourceSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);
    void sourceCurrentChanged(const QModelIndex &current);
    void slotCurrentChanged(const QModelIndex &current);

    QItemSelectionModel *m_linkedItemSelectionModel;
    bool m_ignoreCurrentChanged;
    KModelIndexProxyMapper * m_indexMapper;
};

KLinkItemSelectionModel::KLinkItemSelectionModel(QAbstractItemModel *model, QItemSelectionModel *proxySelector, QObject *parent)
    : QItemSelectionModel(model, parent),
      d_ptr(new KLinkItemSelectionModelPrivate(this))
{
    setLinkedItemSelectionModel(proxySelector);
}

KLinkItemSelectionModel::~KLinkItemSelectionModel()
{
    delete d_ptr;
}

QItemSelectionModel *KLinkItemSelectionModel::linkedItemSelectionModel() const
{
    Q_D(const KLinkItemSelectionModel);
    return d->m_linkedItemSelectionModel;
}

void KLinkItemSelectionModel::setLinkedItemSelectionModel(QItemSelectionModel *selectionModel)
{
    Q_D(KLinkItemSelectionModel);
    if (d->m_linkedItemSelectionModel != selectionModel) {

        if (d->m_linkedItemSelectionModel) {
            disconnect(d->m_linkedItemSelectionModel);
        }

        d->m_linkedItemSelectionModel = selectionModel;

        if (d->m_linkedItemSelectionModel) {
            connect(d->m_linkedItemSelectionModel, SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(sourceSelectionChanged(QItemSelection,QItemSelection)));
            connect(d->m_linkedItemSelectionModel, SIGNAL(currentChanged(QModelIndex,QModelIndex)), SLOT(sourceCurrentChanged(QModelIndex)));

#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            connect(d->m_linkedItemSelectionModel, &QItemSelectionModel::modelChanged, this, [this] {
                d_ptr->reinitializeIndexMapper();
            });
#endif
        }
        d->reinitializeIndexMapper();
        Q_EMIT linkedItemSelectionModelChanged();
    }
}

void KLinkItemSelectionModel::select(const QModelIndex &index, QItemSelectionModel::SelectionFlags command)
{
    Q_D(KLinkItemSelectionModel);
    // When an item is removed, the current index is set to the top index in the model.
    // That causes a selectionChanged signal with a selection which we do not want.
    if (d->m_ignoreCurrentChanged) {
        return;
    }
    // Do *not* replace next line with: QItemSelectionModel::select(index, command)
    //
    // Doing so would end up calling KLinkItemSelectionModel::select(QItemSelection, QItemSelectionModel::SelectionFlags)
    //
    // This is because the code for QItemSelectionModel::select(QModelIndex, QItemSelectionModel::SelectionFlags) looks like this:
    // {
    //     QItemSelection selection(index, index);
    //     select(selection, command);
    // }
    // So it calls KLinkItemSelectionModel overload of
    // select(QItemSelection, QItemSelectionModel::SelectionFlags)
    //
    // When this happens and the selection flags include Toggle, it causes the
    // selection to be toggled twice.
    QItemSelectionModel::select(QItemSelection(index, index), command);
    if (index.isValid()) {
        d->m_linkedItemSelectionModel->select(d->m_indexMapper->mapSelectionLeftToRight(QItemSelection(index, index)), command);
    } else {
        d->m_linkedItemSelectionModel->clearSelection();
    }
}

// QAbstractProxyModel::mapSelectionFromSource creates invalid ranges to we filter
// those out manually in a loop. Hopefully fixed in Qt 4.7.2, so we ifdef it out.
// http://qt.gitorious.org/qt/qt/merge_requests/2474
// http://qt.gitorious.org/qt/qt/merge_requests/831
#if QT_VERSION < 0x040702
#define RANGE_FIX_HACK
#endif

#ifdef RANGE_FIX_HACK
static QItemSelection klink_removeInvalidRanges(const QItemSelection &selection)
{
    QItemSelection result;
    Q_FOREACH (const QItemSelectionRange &range, selection) {
        if (!range.isValid()) {
            continue;
        }
        result << range;
    }
    return result;
}
#endif

void KLinkItemSelectionModel::select(const QItemSelection &selection, QItemSelectionModel::SelectionFlags command)
{
    Q_D(KLinkItemSelectionModel);
    d->m_ignoreCurrentChanged = true;
#ifdef RANGE_FIX_HACK
    QItemSelection _selection = klink_removeInvalidRanges(selection);
#else
    QItemSelection _selection = selection;
#endif
    QItemSelectionModel::select(_selection, command);
    Q_ASSERT(d->assertSelectionValid(_selection));
    QItemSelection mappedSelection = d->m_indexMapper->mapSelectionLeftToRight(_selection);
    Q_ASSERT(d->assertSelectionValid(mappedSelection));
    d->m_linkedItemSelectionModel->select(mappedSelection, command);
    d->m_ignoreCurrentChanged = false;
}

void KLinkItemSelectionModelPrivate::slotCurrentChanged(const QModelIndex &current)
{
    const QModelIndex mappedCurrent = m_indexMapper->mapLeftToRight(current);
    if (!mappedCurrent.isValid()) {
        return;
    }
    m_linkedItemSelectionModel->setCurrentIndex(mappedCurrent, QItemSelectionModel::NoUpdate);
}

void KLinkItemSelectionModelPrivate::sourceSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_Q(KLinkItemSelectionModel);
#ifdef RANGE_FIX_HACK
    QItemSelection _selected = klink_removeInvalidRanges(selected);
    QItemSelection _deselected = klink_removeInvalidRanges(deselected);
#else
    QItemSelection _selected = selected;
    QItemSelection _deselected = deselected;
#endif
    Q_ASSERT(assertSelectionValid(_selected));
    Q_ASSERT(assertSelectionValid(_deselected));
    const QItemSelection mappedDeselection = m_indexMapper->mapSelectionRightToLeft(_deselected);
    const QItemSelection mappedSelection = m_indexMapper->mapSelectionRightToLeft(_selected);

    q->QItemSelectionModel::select(mappedDeselection, QItemSelectionModel::Deselect);
    q->QItemSelectionModel::select(mappedSelection, QItemSelectionModel::Select);
}

void KLinkItemSelectionModelPrivate::sourceCurrentChanged(const QModelIndex &current)
{
    Q_Q(KLinkItemSelectionModel);
    const QModelIndex mappedCurrent = m_indexMapper->mapRightToLeft(current);
    if (!mappedCurrent.isValid()) {
        return;
    }
    q->setCurrentIndex(mappedCurrent, QItemSelectionModel::NoUpdate);
}

#include "moc_klinkitemselectionmodel.cpp"
