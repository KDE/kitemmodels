/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts

import org.kde.kirigami as Kirigami

QQC2.ApplicationWindow {
    id: root

    width: 310
    height: 400
    visible: true

    title: "ComboBox with icon role"

    component ComboListModel : ListModel {
        function row(text: string, iconName: string) {
            append({ text, value: count, iconName });
        }
    }

    property ComboListModel selectedModel

    ComboListModel {
        id: applicationsModel

        Component.onCompleted: {
            row("Ark", "ark");
            row("Mail", "internet-mail");
            row("Fonts", "kfontview");
            row("Discover", "plasmadiscover");

            root.selectedModel = this;
        }
    }

    ComboListModel {
        id: actionsModel

        Component.onCompleted: {
            row("brightness", "brightness-high-symbolic");
            row("align-right", "align-horizontal-right-out-symbolic");
            row("archive", "archive-insert-symbolic");
            row("bookmark", "bookmark-add-symbolic");
        }
    }

    QQC2.Pane {
        anchors.centerIn: parent
        width: 200

        ColumnLayout {
            spacing: Kirigami.Units.largeSpacing
            anchors.horizontalCenter: parent.horizontalCenter

            QQC2.Frame {
                Layout.fillWidth: true

                ColumnLayout {
                    spacing: Kirigami.Units.largeSpacing

                    QQC2.ButtonGroup {
                        id: modelSelectorGroup

                        onClicked: button => {
                            switch (button) {
                            case applicationRadio:
                                selectedModel = applicationsModel;
                                break;
                            case actionsRadio:
                                selectedModel = actionsModel;
                                break;
                            }
                        }
                    }

                    QQC2.RadioButton {
                        id: applicationRadio
                        checked: true
                        text: "Applications"
                        Layout.fillWidth: true
                        QQC2.ButtonGroup.group: modelSelectorGroup
                    }

                    QQC2.RadioButton {
                        id: actionsRadio
                        text: "Actions"
                        Layout.fillWidth: true
                        QQC2.ButtonGroup.group: modelSelectorGroup
                    }
                }
            }

            ComboBoxWithIconRole {
                model: root.selectedModel

                textRole: "text"
                valueRole: "value"
                iconRole: "iconName"

                Layout.fillWidth: true
            }

            QQC2.ComboBox {
                model: root.selectedModel

                textRole: "text"
                valueRole: "value"

                Layout.fillWidth: true
            }
        }
    }
}
