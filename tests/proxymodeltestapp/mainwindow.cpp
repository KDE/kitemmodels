/*
    This file is part of the proxy model test suite.

    SPDX-FileCopyrightText: 2009 Stephen Kelly <steveire@gmail.com>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "mainwindow.h"

#include <QTabWidget>

#include "dynamictreemodel.h"

#include "breadcrumbswidget.h"
#include "breadcrumbnavigationwidget.h"
#include "breadcrumbdirectionwidget.h"
#include "checkablewidget.h"
#include "descendantpmwidget.h"
#include "selectionpmwidget.h"
// #include "statesaverwidget.h"
#include "proxymodeltestwidget.h"
#include "proxyitemselectionwidget.h"
#include "descendantqmltree.h"
#ifdef QT_SCRIPT_LIB
#include "reparentingpmwidget.h"
#endif
#include "lessthanwidget.h"
#include "matchcheckingwidget.h"
#include "kidentityproxymodelwidget.h"
#ifdef QT_QUICKWIDGETS_LIB
#include "selectioninqmlwidget.h"
#endif

MainWindow::MainWindow() : QMainWindow()
{

    QTabWidget *tabWidget = new QTabWidget(this);

    tabWidget->addTab(new MatchCheckingWidget(), QStringLiteral("Match Checking PM"));
    tabWidget->addTab(new DescendantProxyModelWidget(), QStringLiteral("descendant PM"));
    tabWidget->addTab(new SelectionProxyWidget(), QStringLiteral("selection PM"));
#ifdef QT_QUICKWIDGETS_LIB
    tabWidget->addTab(new SelectionInQmlWidget(), QStringLiteral("selection PM in QML"));
#endif
    tabWidget->addTab(new KIdentityProxyModelWidget(), QStringLiteral("Identity PM"));
    tabWidget->addTab(new CheckableWidget(), QStringLiteral("Checkable"));
    tabWidget->addTab(new BreadcrumbsWidget(), QStringLiteral("Breadcrumbs"));
    tabWidget->addTab(new BreadcrumbNavigationWidget(), QStringLiteral("Breadcrumb Navigation"));
    tabWidget->addTab(new BreadcrumbDirectionWidget(), QStringLiteral("Breadcrumb Direction"));
    tabWidget->addTab(new ProxyItemSelectionWidget(), QStringLiteral("Proxy Item selection"));
#ifdef QT_SCRIPT_LIB
    tabWidget->addTab(new ReparentingProxyModelWidget(), QStringLiteral("reparenting PM"));
#endif
    tabWidget->addTab(new DescendantQmlTreeWidget(), QStringLiteral("QML Trees"));
    tabWidget->addTab(new LessThanWidget(), QStringLiteral("Less Than"));
    tabWidget->addTab(new ProxyModelTestWidget(), QStringLiteral("Proxy Model Test"));
//   tabWidget->addTab(new StateSaverWidget(), "State Saver Test");

    setCentralWidget(tabWidget);
}

MainWindow::~MainWindow()
{
}

