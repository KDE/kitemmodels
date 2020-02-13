/*
    This file is part of the proxy model test suite.

    SPDX-FileCopyrightText: 2009 Stephen Kelly <steveire@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef RECURSIVE_FILTER_PM_WIDGET_H
#define RECURSIVE_FILTER_PM_WIDGET_H

#include <QWidget>
#include <QRegExp>

#include "krecursivefilterproxymodel.h"

class QTreeView;
class QLineEdit;
class QLabel;
class QPushButton;

class DynamicTreeModel;

class KRecursiveFilterProxyModelSubclass : public KRecursiveFilterProxyModel
{
    Q_OBJECT
public:
    KRecursiveFilterProxyModelSubclass(QObject *parent = nullptr)
        : KRecursiveFilterProxyModel(parent)
    {

    }

    bool acceptRow(int sourceRow, const QModelIndex &parent_index) const override
    {
        static const int column = 0;
        QModelIndex srcIndex = sourceModel()->index(sourceRow, column, parent_index);
        return srcIndex.data().toString().contains(m_regExp);
    }

    void setRegExp(const QRegExp &re)
    {
        layoutAboutToBeChanged();
        m_regExp = re;
        invalidateFilter();
        layoutChanged();
    }

private:
    QRegExp m_regExp;
};

class RecursiveFilterProxyWidget : public QWidget
{
    Q_OBJECT
public:
    RecursiveFilterProxyWidget(QWidget *parent = nullptr);

protected Q_SLOTS:
    void reset();

private:
    DynamicTreeModel *m_rootModel;
    KRecursiveFilterProxyModel *m_recursive;
    KRecursiveFilterProxyModelSubclass *m_recursiveSubclass;

    QLineEdit *m_lineEdit;
    QLabel *m_label;
};

#endif

