/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQml.Models
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami 2 as Kirigami
import org.kde.kitemmodels 1.1 as KItemModels

import org.kde.bluezqt 1.0 as BluezQt
import org.kde.plasma.networkmanagement 0.2 as PlasmaNM

QQC2.ApplicationWindow {
    id: root

    width: 800
    height: 400
    visible: true

    title: {
        const parts = [];
        if (0 <= currentModelsModelIndex && currentModelsModelIndex < modelsModel.count) {
            const name = modelsModel.get(currentModelsModelIndex).name;
            parts.push(name);
        }
        parts.push("KRoleNamesModel Test");
        return parts.join(" — ");
    }

    property int currentModelsModelIndex: -1
    property int currentRowsModelIndex: -1
    property int currentRoleNamesModelIndex: -1

    property QtObject currentModel: null

    onCurrentModelsModelIndexChanged: {
        currentRowsModelIndex = -1;
        currentRoleNamesModelIndex = -1;
        currentModel = modelsModel.get(currentModelsModelIndex)?.model ?? null;
    }

    ListModel {
        id: modelsModel

        Component.onCompleted: {
            append({ model: this, name: "Model of Models", });
            append({ model: bluetoothModel, name: "Bluetooth Devices", });
            append({ model: networkModel, name: "Networks", });
            append({ model: descendantsModel, name: "Descendants", });

            root.currentModelsModelIndex = 0;
        }
    }

    KItemModels.KRoleNamesModel {
        id: roleNamesModel

        sourceModel: root.currentModel
    }

    BluezQt.DevicesModel {
        id: bluetoothModel
    }

    PlasmaNM.NetworkModel {
        id: networkModel
    }

    // This one has interesting role values
    KItemModels.KDescendantsProxyModel {
        id: descendantsModel
    }

    component Heading : ColumnLayout {
        id: heading

        property string text

        spacing: 0
        Layout.fillWidth: true

        Kirigami.Heading {
            text: heading.text
            padding: Kirigami.Units.largeSpacing
            Layout.fillWidth: true
        }

        Kirigami.Separator {
            Layout.fillWidth: true
        }
    }

    component FormLabel : QQC2.Label {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignRight
        horizontalAlignment: Text.AlignRight
    }

    component FormDataLabel : Kirigami.SelectableLabel {
        Layout.alignment: Qt.AlignVCenter | Qt.AlignLeft
        Layout.fillWidth: true
        wrapMode: Text.Wrap
    }

    QQC2.Pane {
        anchors.fill: parent
        padding: 0

        QQC2.SplitView {
            anchors.fill: parent

            QQC2.Pane {
                QQC2.SplitView.fillWidth: false
                QQC2.SplitView.fillHeight: true
                QQC2.SplitView.preferredWidth: 150

                padding: 0

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Heading {
                        text: "Models"
                    }

                    QQC2.ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        Kirigami.Theme.colorSet: Kirigami.Theme.View
                        Kirigami.Theme.inherit: true

                        ListView {
                            currentIndex: root.currentModelsModelIndex

                            onCurrentIndexChanged: {
                                root.currentModelsModelIndex = currentIndex;
                            }

                            model: modelsModel
                            delegate: Kirigami.BasicListItem {
                                required property int index
                                required property string name

                                text: name
                            }
                        }
                    }
                }
            }

            QQC2.Pane {
                QQC2.SplitView.fillWidth: false
                QQC2.SplitView.fillHeight: true
                QQC2.SplitView.preferredWidth: 150

                padding: 0

                ColumnLayout {
                    anchors.fill: parent
                    spacing: 0

                    Heading {
                        text: "Model rows"
                    }

                    QQC2.ScrollView {
                        Layout.fillWidth: true
                        Layout.fillHeight: true

                        ListView {
                            currentIndex: root.currentRowsModelIndex

                            onCurrentIndexChanged: {
                                root.currentRowsModelIndex = currentIndex;
                            }

                            onModelChanged: {
                                root.currentRowsModelIndex = -1;
                                currentIndex = Qt.binding(() => root.currentRowsModelIndex);
                            }

                            model: root.currentModel
                            delegate: Kirigami.BasicListItem {
                                required property int index

                                text: index
                            }
                        }
                    }
                }
            }

            QQC2.SplitView {
                QQC2.SplitView.fillWidth: true
                QQC2.SplitView.fillHeight: true

                orientation: Qt.Vertical

                QQC2.SplitView {
                    QQC2.SplitView.fillWidth: true
                    QQC2.SplitView.fillHeight: true

                    QQC2.Pane {
                        QQC2.SplitView.fillWidth: false
                        QQC2.SplitView.fillHeight: true
                        QQC2.SplitView.preferredWidth: 200

                        padding: 0

                        ColumnLayout {
                            anchors.fill: parent
                            spacing: 0

                            Heading {
                                text: "Model roles"
                            }

                            QQC2.ScrollView {
                                Layout.fillWidth: true
                                Layout.fillHeight: true

                                ListView {
                                    currentIndex: root.currentRoleNamesModelIndex

                                    onCurrentIndexChanged: {
                                        root.currentRoleNamesModelIndex = currentIndex;
                                    }

                                    model: KItemModels.KSortFilterProxyModel {
                                        sourceModel: roleNamesModel
                                        sortRole: "role"
                                    }
                                    delegate: Kirigami.BasicListItem {
                                        required property int index
                                        required property int role
                                        required property string roleName

                                        text: roleName
                                        subtitle: role
                                    }
                                }
                            }
                        }
                    }

                    QQC2.ScrollView {
                        id: detailsView

                        property var data: {
                            let row = "None";
                            let role = "None";
                            let roleName = "None";
                            let data = "None";

                            const model = root.currentModel;
                            if (model && root.currentRowsModelIndex !== -1) {
                                row = root.currentRowsModelIndex;
                            }
                            if (root.currentRoleNamesModelIndex !== -1) {
                                const index = roleNamesModel.index(root.currentRoleNamesModelIndex, 0);
                                role = roleNamesModel.data(index, KItemModels.KRoleNamesModel.RoleRole);
                                roleName = roleNamesModel.data(index, KItemModels.KRoleNamesModel.RoleNameRole);

                                if (model && root.currentRowsModelIndex !== -1) {
                                    const index = model.index(root.currentRowsModelIndex, 0);
                                    data = model.data(index, role);
                                }
                            }

                            return ({
                                row,
                                role,
                                roleName,
                                data,
                            });
                        }

                        QQC2.SplitView.fillWidth: true
                        QQC2.SplitView.fillHeight: true

                        contentWidth: availableWidth

                        ColumnLayout {
                            spacing: 0
                            width: detailsView.availableWidth

                            Heading {
                                text: "Model Data"
                            }

                            GridLayout {
                                Layout.fillWidth: true
                                Layout.margins: Kirigami.Units.largeSpacing

                                columns: 2
                                columnSpacing: Kirigami.Units.largeSpacing

                                FormLabel {
                                    text: "Row:"
                                }
                                FormDataLabel {
                                    text: String(detailsView.data.row)
                                }

                                FormLabel {
                                    text: "Role:"
                                }
                                FormDataLabel {
                                    text: String(detailsView.data.role)
                                }

                                FormLabel {
                                    text: "Role name:"
                                }
                                FormDataLabel {
                                    text: String(detailsView.data.roleName)
                                }

                                FormLabel {
                                    text: "Data:"
                                }
                                FormDataLabel {
                                    text: String(detailsView.data.data)
                                }
                            }
                        }
                    }
                }

                QQC2.ScrollView {
                    id: convertView

                    QQC2.SplitView.fillWidth: true
                    QQC2.SplitView.fillHeight: false
                    QQC2.SplitView.preferredHeight: Kirigami.Units.gridUnit * 10

                    contentWidth: availableWidth

                    ColumnLayout {
                        spacing: 0
                        width: convertView.availableWidth

                        Heading {
                            text: "Manual conversion"
                        }

                        GridLayout {
                            id: convertForm

                            Layout.fillWidth: true
                            Layout.margins: Kirigami.Units.largeSpacing

                            columns: 4
                            columnSpacing: Kirigami.Units.largeSpacing

                            FormLabel {
                                text: "Role:"
                            }
                            QQC2.TextField {
                                id: roleField
                                text: "0"
                                validator: IntValidator {}
                                Layout.preferredWidth: Kirigami.Units.gridUnit * 8
                            }
                            FormLabel {
                                text: "Corresponds to<br/>role name:"
                            }
                            FormDataLabel {
                                Layout.fillWidth: true
                                text: {
                                    const role = Number.parseInt(roleField.text, 10);
                                    if (!Number.isNaN(role)) {
                                        // Simple way to make it reevaluate when sourceModel changes.
                                        void roleNamesModel.sourceModel;
                                        const name = roleNamesModel.roleName(role);
                                        if (name) {
                                            return name;
                                        }
                                    }
                                    return "<unknown>"
                                }
                            }

                            Kirigami.Separator {
                                Layout.fillWidth: true
                                Layout.columnSpan: convertForm.columns
                                Layout.topMargin: Kirigami.Units.largeSpacing
                                Layout.bottomMargin: Kirigami.Units.largeSpacing
                            }

                            FormLabel {
                                text: "Role name:"
                            }
                            QQC2.TextField {
                                id: roleNameField
                                text: ""
                                Layout.preferredWidth: Kirigami.Units.gridUnit * 8
                            }
                            FormLabel {
                                text: "Corresponds to<br/>role:"
                            }
                            FormDataLabel {
                                Layout.fillWidth: true
                                text: {
                                    const roleName = roleNameField.text;
                                    if (roleName) {
                                        // Simple way to make it reevaluate when sourceModel changes.
                                        void roleNamesModel.sourceModel;
                                        const role = roleNamesModel.role(roleName);
                                        if (role !== -1) {
                                            return String(role);
                                        }
                                    }
                                    return "<unknown>";
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
