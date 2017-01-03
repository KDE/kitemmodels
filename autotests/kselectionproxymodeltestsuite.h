/*
    Copyright (c) 2009 Stephen Kelly <steveire@gmail.com>

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

#ifndef SELECTIONPROXYMODELTEST_H
#define SELECTIONPROXYMODELTEST_H

#include "proxymodeltest.h"

#include "kselectionproxymodel.h"
#include "modelselector.h"

class SelectionProxyModelTest : public ProxyModelTest
{
    Q_OBJECT
public:
    SelectionProxyModelTest(QObject *parent = 0)
        : ProxyModelTest(parent),
          m_selectionModel(0),
          m_modelSelector(0)
    {
    }

    void setTestData(ModelSelector *modelSelector)
    {
        disconnectTestSignals(modelSelector);
        m_modelSelector = modelSelector;
        connectTestSignals(m_modelSelector);
    }

protected:
    /* reimp */ QAbstractProxyModel *getProxy() Q_DECL_OVERRIDE
    {
        Q_ASSERT(sourceModel());
        m_selectionModel = new QItemSelectionModel(sourceModel());

        m_modelSelector->setWatchedModel(sourceModel());
        m_modelSelector->setSelectionModel(m_selectionModel);
        m_modelSelector->setWatch(true);

        m_proxyModel = new KSelectionProxyModel(m_selectionModel, this);
        QVariant filterBehaviour = m_modelSelector->property("filterBehaviour");
        if (!filterBehaviour.isNull()) {
            m_proxyModel->setFilterBehavior(static_cast<KSelectionProxyModel::FilterBehavior>(filterBehaviour.toInt()));
        }
        return m_proxyModel;
    }

private Q_SLOTS:
    void cleanupTestCase()
    {
        doCleanupTestCase();
        delete m_modelSelector;
        m_modelSelector = 0;
    }

    void cleanup()
    {
        doCleanup();
        if (m_modelSelector->selectionModel()) {
            m_modelSelector->selectionModel()->clearSelection();
        }
        m_modelSelector->setWatch(false);
    }

private:
    QItemSelectionModel *m_selectionModel;
    KSelectionProxyModel *m_proxyModel;
    ModelSelector *m_modelSelector;
};

template<typename SelectorStrategy, KSelectionProxyModel::FilterBehavior _filterBehaviour>
class TestData : public SelectorStrategy
{
public:
    TestData(ProxyModelTest *proxyModelTest)
        : SelectorStrategy(proxyModelTest)
    {
        SelectorStrategy::setProperty("filterBehaviour", QVariant(int(_filterBehaviour)));
    }

    /* reimp */ void testInsertWhenEmptyData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testInsertWhenEmptyData();
    }
    /* reimp */ void testInsertInRootData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testInsertInRootData();
    }
    /* reimp */ void testInsertInTopLevelData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testInsertInTopLevelData();
    }
    /* reimp */ void testInsertInSecondLevelData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testInsertInSecondLevelData();
    }

    /* reimp */ void testRemoveFromRootData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testRemoveFromRootData();
    }
    /* reimp */ void testRemoveFromTopLevelData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testRemoveFromTopLevelData();
    }
    /* reimp */ void testRemoveFromSecondLevelData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testRemoveFromSecondLevelData();
    }

    /* reimp */ void testMoveFromRootData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testMoveFromRootData();
    }
    /* reimp */ void testMoveFromTopLevelData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testMoveFromTopLevelData();
    }
    /* reimp */ void testMoveFromSecondLevelData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testMoveFromSecondLevelData();
    }

    /* reimp */ void testModifyInRootData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testModifyInRootData();
    }
    /* reimp */ void testModifyInTopLevelData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testModifyInTopLevelData();
    }
    /* reimp */ void testModifyInSecondLevelData() Q_DECL_OVERRIDE
    {
        SelectorStrategy::testModifyInSecondLevelData();
    }
};

/**
  @brief Selects only one item in the source model which does not change
*/
template< int num1 = 0,
          int num2 = 0,
          int num3 = 0,
          int num4 = 0,
          int num5 = 0>
class ImmediateSelectionStrategy : public ModelSelector
{
public:
    ImmediateSelectionStrategy(ProxyModelTest *parent = 0)
        : ModelSelector(parent)
    {
        if (num1 > 0) {
            m_selectedRows << num1;
        }
        if (num2 > 0) {
            m_selectedRows << num2;
        }
        if (num3 > 0) {
            m_selectedRows << num3;
        }
        if (num4 > 0) {
            m_selectedRows << num4;
        }
        if (num5 > 0) {
            m_selectedRows << num5;
        }
    }
};

/**
  For testing the proxy when it has no selection.
*/
typedef ImmediateSelectionStrategy<0, 0, 0, 0, 0> NoSelectionStrategy;

#define SELECTIONPROXYTESTDATA(SelectionStrategy, num1, num2, num3, num4, num5, Type) \
    new TestData<SelectionStrategy<num1, num2, num3, num4, num5>, Type>(proxyModelTestClass)

#define SELECTIONPROXYTESTCONFIG(SelectionStrategy, num1, num2, num3, num4, num5, Type) \
            << "  FilterBehavior:    " << #Type << "\n" \
            << "  SelectionStrategy: " << #SelectionStrategy << (num1 > 0 ? #num1 : "") \
            << (num2 > 0 ? #num2 : "") \
            << (num3 > 0 ? #num3 : "") \
            << (num4 > 0 ? #num4 : "") \
            << (num5 > 0 ? #num5 : "") << "\n"; \

#define SELECTIONPROXYMODELTEST(SelectionStrategy, num1, num2, num3, num4, num5, Type, IntermediateProxy, LazyPersistence) \
    PROXYMODELTEST_CUSTOM(SELECTIONPROXYTESTDATA(SelectionStrategy, num1, num2, num3, num4, num5, Type), IntermediateProxy, LazyPersistence, SELECTIONPROXYTESTCONFIG(SelectionStrategy, num1, num2, num3, num4, num5, Type))

#define SELECTIONCOMPLETETEST(SelectionStrategy, num1, num2, num3, num4, num5, Type) \
    SELECTIONPROXYMODELTEST(SelectionStrategy, num1, num2, num3, num4, num5, Type, DynamicTree, ImmediatePersistence) \
    SELECTIONPROXYMODELTEST(SelectionStrategy, num1, num2, num3, num4, num5, Type, IntermediateProxy, ImmediatePersistence) \
    SELECTIONPROXYMODELTEST(SelectionStrategy, num1, num2, num3, num4, num5, Type, DynamicTree, LazyPersistence) \
    SELECTIONPROXYMODELTEST(SelectionStrategy, num1, num2, num3, num4, num5, Type, IntermediateProxy, LazyPersistence) \

#define SIMPLETESTDATA(SelectionStrategy, Type) \
    new TestData<SelectionStrategy, Type>(proxyModelTestClass)

#define SELECTIONPROXYMODELSIMPLETEST(SelectionStrategy, Type) \
    PROXYMODELTEST_CUSTOM(SIMPLETESTDATA(SelectionStrategy, Type), DynamicTree, LazyPersistence, SELECTIONPROXYTESTCONFIG(SelectionStrategy, 0, 0, 0, 0, 0, Type))

#define SELECTIONCOMPLETETEST1(SelectionStrategy, num1, Type) \
    SELECTIONCOMPLETETEST(SelectionStrategy, num1, 0, 0, 0, 0, Type) \

#define SELECTIONCOMPLETETEST2(SelectionStrategy, num1, num2, Type) \
    SELECTIONCOMPLETETEST(SelectionStrategy, num1, num2, 0, 0, 0, Type) \

#define SELECTIONCOMPLETETEST3(SelectionStrategy, num1, num2, num3, Type) \
    SELECTIONCOMPLETETEST(SelectionStrategy, num1, num2, num3, 0, 0, Type) \

#define SELECTIONCOMPLETETEST4(SelectionStrategy, num1, num2, num3, num4, Type) \
    SELECTIONCOMPLETETEST(SelectionStrategy, num1, num2, num3, num4, 0, Type) \

#endif
