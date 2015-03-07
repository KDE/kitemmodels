/*
 * This file is part of the proxy model test suite.
 *
 * Copyright 2015 Stephen Kelly <steveire@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301  USA
 */

#include "selectioninqmlwidget.h"

#include <QSplitter>
#include <QTreeView>
#include <QQuickWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QtQml>

#include "dynamictreemodel.h"
#include "dynamictreewidget.h"
#include "kselectionproxymodel.h"

SelectionInQmlWidget::SelectionInQmlWidget(QWidget *parent): QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout(this);
    QSplitter *splitter = new QSplitter(this);
    layout->addWidget(splitter);

    m_rootModel = new DynamicTreeModel(this);

    new DynamicTreeWidget(m_rootModel, splitter);

    QTreeView *selectionTree = new QTreeView(splitter);
    selectionTree->setSelectionMode(QAbstractItemView::ExtendedSelection);
    selectionTree->setModel(m_rootModel);
    selectionTree->expandAll();

    qmlRegisterType<KSelectionProxyModel>("KF5ItemModels", 1, 0, "SelectionProxyModel");

    QQuickWidget *quickView = new QQuickWidget(splitter);

    quickView->engine()->rootContext()->setContextProperty("_model", m_rootModel);
    quickView->engine()->rootContext()->setContextProperty("_selectionModel", selectionTree->selectionModel());

    quickView->setSource(QUrl(SRC_DIR "/selection.qml"));
}
