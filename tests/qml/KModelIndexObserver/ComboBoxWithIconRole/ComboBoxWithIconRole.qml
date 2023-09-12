/*
 *   SPDX-FileCopyrightText: 2023 ivan tkachenko <me@ratijas.tk>
 *
 *   SPDX-License-Identifier: LGPL-2.0-or-later
 */

import QtQml
import QtQuick
import QtQuick.Controls as QQC2
import QtQuick.Layouts
import QtQuick.Templates as T

import org.kde.kirigami as Kirigami
import org.kde.kitemmodels as KItemModels

// More or less a copy of qqc2-desktop-style/qqc2-breeze-style ComboBox but written in pure QML
QQC2.ComboBox {
    id: controlRoot

    property string iconRole

    readonly property string currentIcon: observer.modelData ? observer.modelData[iconRole] : null

    property int iconWidth: Kirigami.Units.iconSizes.sizeForLabels
    property int iconHeight: Kirigami.Units.iconSizes.sizeForLabels

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth
                                + (iconRole !== "" ? iconWidth + Kirigami.Units.smallSpacing : 0)
                                + (indicator.visible ? indicator.implicitWidth + Kirigami.Units.smallSpacing : 0)
                                + Kirigami.Units.largeSpacing * 2
                                + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    implicitContentWidthPolicy: T.ComboBox.WidestText

    KItemModels.KModelIndexObserver {
        id: observer

        sourceModel: controlRoot.model ?? null
        row: controlRoot.currentIndex

        roleNames: controlRoot.iconRole ? [controlRoot.iconRole] : []
    }

    indicator: Kirigami.Icon {
        anchors {
            right: parent.right
            rightMargin: Kirigami.Units.smallSpacing
            verticalCenter: parent.verticalCenter
        }
        implicitWidth: Kirigami.Units.iconSizes.sizeForLabels
        implicitHeight: Kirigami.Units.iconSizes.sizeForLabels
        source: "arrow-down"
        animated: false
    }

    background: Item {
        implicitWidth: Math.max(Kirigami.Units.gridUnit * 5, layout.implicitWidth + layout.anchors.leftMargin + layout.anchors.rightMargin)
        implicitHeight: Math.max(Kirigami.Units.gridUnit * 1.5, layout.implicitHeight + Kirigami.Units.largeSpacing * 2)

        Rectangle {
            anchors.fill: parent
            anchors.margins: 1
            z: -1

            color: controlRoot.down ? Qt.alpha(Kirigami.Theme.focusColor, 0.3) : Kirigami.Theme.backgroundColor
            border.color: controlRoot.hovered || controlRoot.visualFocus || controlRoot.down ? Kirigami.Theme.focusColor : Qt.alpha(Kirigami.Theme.textColor, 0.3)
            border.width: 1
            radius: 3
        }

        RowLayout {
            id: layout

            spacing: Kirigami.Units.smallSpacing
            anchors {
                fill: parent
                leftMargin: Kirigami.Units.largeSpacing
                rightMargin: Kirigami.Units.largeSpacing + (controlRoot.indicator.visible ? controlRoot.indicator.width + Kirigami.Units.smallSpacing : 0)
            }

            Kirigami.Icon {
                visible: controlRoot.iconRole !== ""
                source: controlRoot.currentIcon
                animated: false
                Layout.preferredWidth: controlRoot.iconWidth
                Layout.preferredHeight: controlRoot.iconHeight
                Layout.alignment: Qt.AlignVCenter
            }

            QQC2.Label {
                Layout.fillWidth: true
                Layout.margins: 0
                Layout.preferredHeight: fontMetrics.height
                horizontalAlignment: Text.AlignLeft
                text: controlRoot.currentText
                elide: Text.ElideMiddle
            }

            FontMetrics {
                id: fontMetrics
                font: controlRoot.font
            }
        }
    }

    delegate: QQC2.ItemDelegate {
        width: ListView.view.width
        icon.name: controlRoot.iconRole ? (Array.isArray(controlRoot.model) ? modelData[controlRoot.iconRole] : model[controlRoot.iconRole]) : ""
        icon.width: controlRoot.iconWidth
        icon.height: controlRoot.iconHeight
        text: controlRoot.textRole ? (Array.isArray(controlRoot.model) ? modelData[controlRoot.textRole] : model[controlRoot.textRole]) : modelData
        highlighted: controlRoot.highlightedIndex == index
        property bool separatorVisible: false
        Kirigami.Theme.colorSet: controlRoot.Kirigami.Theme.inherit ? controlRoot.Kirigami.Theme.colorSet : Kirigami.Theme.View
        Kirigami.Theme.inherit: controlRoot.Kirigami.Theme.inherit
    }
}
