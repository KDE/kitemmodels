/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

#include "krolenamesmodel.h"
#include <QSignalSpy>
#include <QStandardItemModel>
#include <QTest>

#include <KRoleNamesModel>

#include <memory>
#include <qnamespace.h>

class KRoleNamesModelTest : public QObject
{
    Q_OBJECT

    enum Roles {
        IndexRole = Qt::UserRole,
        TemperatureRole,
    };

private Q_SLOTS:

    void testOwnRoleNames()
    {
        KRoleNamesModel roleNamesModel;
        const auto roleNames = roleNamesModel.roleNames();
        Q_ASSERT(roleNames.count() == 2);
        Q_ASSERT(roleNames[KRoleNamesModel::RoleRole] == "role");
        Q_ASSERT(roleNames[KRoleNamesModel::RoleNameRole] == "roleName");
    }

    void recursiveSet()
    {
        KRoleNamesModel roleNamesModel;
        // Should not cause an infinite recursion
        roleNamesModel.setSourceModel(&roleNamesModel);
        Q_ASSERT(roleNamesModel.rowCount() == 2);
        // Test destructing with self set as a source model.
    }

    void recursiveUnset()
    {
        // Test unsetting self
        KRoleNamesModel roleNamesModel;
        roleNamesModel.setSourceModel(&roleNamesModel);
        Q_ASSERT(roleNamesModel.rowCount() == 2);
        roleNamesModel.setSourceModel(nullptr);
    }

    void reflexive()
    {
        KRoleNamesModel sourceModel;
        KRoleNamesModel roleNamesModel;
        roleNamesModel.setSourceModel(&sourceModel);

        Q_ASSERT(roleNamesModel.rowCount() == 2);

        const auto role0 = roleNamesModel.data(roleNamesModel.index(0), KRoleNamesModel::RoleRole).toInt();
        const auto role1 = roleNamesModel.data(roleNamesModel.index(1), KRoleNamesModel::RoleRole).toInt();
        const auto roleName0 = roleNamesModel.data(roleNamesModel.index(0), KRoleNamesModel::RoleNameRole).toByteArray();
        const auto roleName1 = roleNamesModel.data(roleNamesModel.index(1), KRoleNamesModel::RoleNameRole).toByteArray();

        if (role0 == KRoleNamesModel::RoleRole) {
            Q_ASSERT(role1 == KRoleNamesModel::RoleNameRole);
            Q_ASSERT(roleName0 == "role");
            Q_ASSERT(roleName1 == "roleName");
        } else {
            Q_ASSERT(role0 == KRoleNamesModel::RoleNameRole);
            Q_ASSERT(role1 == KRoleNamesModel::RoleRole);
            Q_ASSERT(roleName0 == "roleName");
            Q_ASSERT(roleName1 == "role");
        }
    }

    void createAndSwap()
    {
        KRoleNamesModel roleNamesModel;
        QSignalSpy changeSpy(&roleNamesModel, &KRoleNamesModel::sourceModelChanged);
        QSignalSpy resetSpy(&roleNamesModel, &QAbstractListModel::modelReset);
        Q_ASSERT(roleNamesModel.sourceModel() == nullptr);

        auto some = createSomeSourceModel();
        roleNamesModel.setSourceModel(&*some);
        Q_ASSERT(roleNamesModel.sourceModel() == &*some);
        Q_ASSERT(roleNamesModel.rowCount() == 2);
        Q_ASSERT(changeSpy.count() == 1);
        Q_ASSERT(resetSpy.count() == 1);

        auto another = createAnotherSourceModel();
        roleNamesModel.setSourceModel(&*another);
        Q_ASSERT(roleNamesModel.sourceModel() == &*another);
        Q_ASSERT(roleNamesModel.rowCount() == 3);
        Q_ASSERT(changeSpy.count() == 2);
        Q_ASSERT(resetSpy.count() == 2);

        another->clear();
        Q_ASSERT(changeSpy.count() == 2);
        Q_ASSERT(resetSpy.count() == 3);

        another.reset();
        Q_ASSERT(roleNamesModel.sourceModel() == nullptr);
        Q_ASSERT(changeSpy.count() == 3);
        Q_ASSERT(resetSpy.count() == 4);
    }

    void procedural()
    {
        KRoleNamesModel roleNamesModel;

        Q_ASSERT(roleNamesModel.role("display") == -1);
        Q_ASSERT(roleNamesModel.roleName(Qt::DisplayRole) == QByteArray());

        auto some = createSomeSourceModel();
        roleNamesModel.setSourceModel(&*some);

        Q_ASSERT(roleNamesModel.role("display") == Qt::DisplayRole);
        Q_ASSERT(roleNamesModel.role("user") == Qt::UserRole);

        Q_ASSERT(roleNamesModel.roleName(Qt::DisplayRole) == "display");
        Q_ASSERT(roleNamesModel.roleName(Qt::UserRole) == "user");

        auto another = createAnotherSourceModel();
        roleNamesModel.setSourceModel(&*another);

        Q_ASSERT(roleNamesModel.role("display") == Qt::DisplayRole);
        Q_ASSERT(roleNamesModel.role("index") == IndexRole);
        Q_ASSERT(roleNamesModel.role("temperature") == TemperatureRole);

        Q_ASSERT(roleNamesModel.roleName(Qt::DisplayRole) == "display");
        Q_ASSERT(roleNamesModel.roleName(IndexRole) == "index");
        Q_ASSERT(roleNamesModel.roleName(TemperatureRole) == "temperature");

        roleNamesModel.setSourceModel(nullptr);

        Q_ASSERT(roleNamesModel.role("display") == -1);
        Q_ASSERT(roleNamesModel.roleName(Qt::DisplayRole) == QByteArray());
    }

private:
    std::unique_ptr<QStandardItemModel> createSomeSourceModel()
    {
        auto testModel = std::make_unique<QStandardItemModel>();
        for (int i = 1; i <= 12; i++) {
            auto entry = new QStandardItem();
            entry->setData(QLocale::c().monthName(i), Qt::DisplayRole);
            entry->setData(i, Qt::UserRole);
            testModel->appendRow(entry);
        }
        testModel->setItemRoleNames({
            {Qt::DisplayRole, "display"},
            {Qt::UserRole, "user"},
        });
        return testModel;
    }

    std::unique_ptr<QStandardItemModel> createAnotherSourceModel()
    {
        auto testModel = std::make_unique<QStandardItemModel>();
        for (int i = 1; i <= 7; i++) {
            auto entry = new QStandardItem();
            entry->setData(QLocale::c().dayName(i), Qt::DisplayRole);
            entry->setData(i, IndexRole);
            entry->setData(42, TemperatureRole);
            testModel->appendRow(entry);
        }
        testModel->setItemRoleNames({
            {Qt::DisplayRole, "display"},
            {IndexRole, "index"},
            {TemperatureRole, "temperature"},
        });
        return testModel;
    }
};

QTEST_GUILESS_MAIN(KRoleNamesModelTest)

#include "krolenamesmodeltest.moc"
