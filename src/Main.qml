import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

ApplicationWindow {
    id: win
    width: 680
    height: 620
    minimumWidth: 520
    minimumHeight: 460
    visible: true
    title: qsTr("Frost — Window Glass Settings")
    color: "#0e0e10"

    Material.theme: Material.Dark
    Material.accent: theme.accent
    Material.roundedScale: theme.radius

    property bool helpVisible: false
    property bool fineTuningOpen: false

    Shortcut { sequence: "?"; context: Qt.ApplicationShortcut; onActivated: win.helpVisible = !win.helpVisible }
    Shortcut { sequence: "Q"; context: Qt.ApplicationShortcut; onActivated: Qt.quit() }
    Shortcut { sequence: "Esc"; context: Qt.ApplicationShortcut; onActivated: win.helpVisible ? (win.helpVisible = false) : Qt.quit() }
    Shortcut { sequence: "R"; context: Qt.ApplicationShortcut; onActivated: backend.resetDefaults() }

    Item {
        anchors.fill: parent
        LayoutMirroring.enabled: Qt.application.layoutDirection === Qt.RightToLeft
        LayoutMirroring.childrenInherit: true

        ScrollView {
            id: scrollView
            anchors.fill: parent
            contentWidth: availableWidth
            clip: true

            ColumnLayout {
                width: parent.width - 48
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 16

                Item { height: 8 }

                // Top Header
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12

                    Label {
                        text: qsTr("Frost")
                        font.pixelSize: 22
                        font.bold: true
                        color: "white"
                    }

                    Label {
                        text: qsTr("Window Glass & Opacity")
                        font.pixelSize: 14
                        color: "#9e9e9e"
                        Layout.alignment: Qt.AlignBaseline
                    }

                    Item { Layout.fillWidth: true }

                    Button {
                        text: qsTr("Reset (R)")
                        flat: true
                        onClicked: backend.resetDefaults()
                    }

                    Button {
                        text: "?"
                        flat: true
                        Layout.preferredWidth: 36
                        onClicked: win.helpVisible = !win.helpVisible
                    }
                }

                // Opacity Group
                Rectangle {
                    Layout.fillWidth: true
                    height: opacityCol.implicitHeight + 28
                    color: "#18181c"
                    radius: Math.max(4, theme.radius)
                    border.color: "#282830"
                    border.width: 1

                    ColumnLayout {
                        id: opacityCol
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: qsTr("Window Opacity")
                                font.pixelSize: 15
                                font.bold: true
                                color: "white"
                            }
                            Item { Layout.fillWidth: true }
                            CheckBox {
                                id: syncCheck
                                text: qsTr("Link active & inactive")
                                checked: backend.syncOpacity
                                onToggled: backend.setSyncOpacity(checked)
                            }
                        }

                        // Active Opacity
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: qsTr("Focused Window Opacity")
                                    color: "#d0d0d0"
                                    font.pixelSize: 13
                                }
                                Item { Layout.fillWidth: true }
                                Label {
                                    text: Math.round(backend.activeOpacity * 100) + "%"
                                    color: theme.accent
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                            Slider {
                                Layout.fillWidth: true
                                from: 0.20
                                to: 1.00
                                stepSize: 0.01
                                value: backend.activeOpacity
                                onMoved: backend.setActiveOpacity(value)
                            }
                        }

                        // Inactive Opacity
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            opacity: syncCheck.checked ? 0.45 : 1.0
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: qsTr("Unfocused Window Opacity")
                                    color: "#d0d0d0"
                                    font.pixelSize: 13
                                }
                                Item { Layout.fillWidth: true }
                                Label {
                                    text: Math.round(backend.inactiveOpacity * 100) + "%"
                                    color: theme.accent
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                            Slider {
                                Layout.fillWidth: true
                                enabled: !syncCheck.checked
                                from: 0.20
                                to: 1.00
                                stepSize: 0.01
                                value: backend.inactiveOpacity
                                onMoved: backend.setInactiveOpacity(value)
                            }
                        }
                    }
                }

                // Blur Controls Group
                Rectangle {
                    Layout.fillWidth: true
                    height: blurCol.implicitHeight + 28
                    color: "#18181c"
                    radius: Math.max(4, theme.radius)
                    border.color: "#282830"
                    border.width: 1

                    ColumnLayout {
                        id: blurCol
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 12

                        Label {
                            text: qsTr("Blur Intensity")
                            font.pixelSize: 15
                            font.bold: true
                            color: "white"
                        }

                        // Blur Size
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: qsTr("Blur Radius (Size)")
                                    color: "#d0d0d0"
                                    font.pixelSize: 13
                                }
                                Item { Layout.fillWidth: true }
                                Label {
                                    text: backend.blurSize + " px"
                                    color: theme.accent
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                            Slider {
                                Layout.fillWidth: true
                                from: 1
                                to: 15
                                stepSize: 1
                                value: backend.blurSize
                                onMoved: backend.setBlurSize(value)
                            }
                        }

                        // Blur Passes
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 4
                            RowLayout {
                                Layout.fillWidth: true
                                Label {
                                    text: qsTr("Blur Depth (Passes)")
                                    color: "#d0d0d0"
                                    font.pixelSize: 13
                                }
                                Item { Layout.fillWidth: true }
                                Label {
                                    text: backend.blurPasses + " " + (backend.blurPasses === 1 ? qsTr("pass") : qsTr("passes"))
                                    color: theme.accent
                                    font.pixelSize: 13
                                    font.bold: true
                                }
                            }
                            Slider {
                                Layout.fillWidth: true
                                from: 1
                                to: 4
                                stepSize: 1
                                value: backend.blurPasses
                                onMoved: backend.setBlurPasses(value)
                            }
                        }
                    }
                }

                // Fine Tuning Group (Collapsible)
                Rectangle {
                    Layout.fillWidth: true
                    height: fineCol.implicitHeight + 28
                    color: "#18181c"
                    radius: Math.max(4, theme.radius)
                    border.color: "#282830"
                    border.width: 1

                    ColumnLayout {
                        id: fineCol
                        anchors.fill: parent
                        anchors.margins: 14
                        spacing: 10

                        RowLayout {
                            Layout.fillWidth: true
                            Label {
                                text: qsTr("Fine Tuning (Noise, Vibrancy, Contrast)")
                                font.pixelSize: 15
                                font.bold: true
                                color: "white"
                            }
                            Item { Layout.fillWidth: true }
                            Button {
                                text: win.fineTuningOpen ? qsTr("Collapse ▲") : qsTr("Expand ▼")
                                flat: true
                                onClicked: win.fineTuningOpen = !win.fineTuningOpen
                            }
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            visible: win.fineTuningOpen
                            spacing: 10

                            // Noise
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                RowLayout {
                                    Layout.fillWidth: true
                                    Label { text: qsTr("Frosted Noise (Grain)"); color: "#b0b0b0"; font.pixelSize: 12 }
                                    Item { Layout.fillWidth: true }
                                    Label { text: (backend.blurNoise).toFixed(3); color: theme.accent; font.pixelSize: 12 }
                                }
                                Slider {
                                    Layout.fillWidth: true
                                    from: 0.00
                                    to: 0.10
                                    stepSize: 0.005
                                    value: backend.blurNoise
                                    onMoved: backend.setBlurNoise(value)
                                }
                            }

                            // Vibrancy
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                RowLayout {
                                    Layout.fillWidth: true
                                    Label { text: qsTr("Vibrancy (Color Pop)"); color: "#b0b0b0"; font.pixelSize: 12 }
                                    Item { Layout.fillWidth: true }
                                    Label { text: (backend.blurVibrancy).toFixed(2); color: theme.accent; font.pixelSize: 12 }
                                }
                                Slider {
                                    Layout.fillWidth: true
                                    from: 0.00
                                    to: 1.00
                                    stepSize: 0.05
                                    value: backend.blurVibrancy
                                    onMoved: backend.setBlurVibrancy(value)
                                }
                            }

                            // Contrast
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                RowLayout {
                                    Layout.fillWidth: true
                                    Label { text: qsTr("Contrast"); color: "#b0b0b0"; font.pixelSize: 12 }
                                    Item { Layout.fillWidth: true }
                                    Label { text: (backend.blurContrast).toFixed(2); color: theme.accent; font.pixelSize: 12 }
                                }
                                Slider {
                                    Layout.fillWidth: true
                                    from: 0.50
                                    to: 2.00
                                    stepSize: 0.05
                                    value: backend.blurContrast
                                    onMoved: backend.setBlurContrast(value)
                                }
                            }

                            // Brightness
                            ColumnLayout {
                                Layout.fillWidth: true
                                spacing: 2
                                RowLayout {
                                    Layout.fillWidth: true
                                    Label { text: qsTr("Brightness"); color: "#b0b0b0"; font.pixelSize: 12 }
                                    Item { Layout.fillWidth: true }
                                    Label { text: (backend.blurBrightness).toFixed(2); color: theme.accent; font.pixelSize: 12 }
                                }
                                Slider {
                                    Layout.fillWidth: true
                                    from: 0.50
                                    to: 1.50
                                    stepSize: 0.05
                                    value: backend.blurBrightness
                                    onMoved: backend.setBlurBrightness(value)
                                }
                            }
                        }
                    }
                }

                // Footer Status
                RowLayout {
                    Layout.fillWidth: true
                    spacing: 8

                    Label {
                        text: backend.statusMessage.length > 0 ? "● " + backend.statusMessage : qsTr("● Live changes synced")
                        color: backend.statusMessage.length > 0 ? theme.accent : "#707070"
                        font.pixelSize: 12
                    }

                    Item { Layout.fillWidth: true }

                    Label {
                        text: qsTr("Press ? for shortcuts")
                        color: "#606060"
                        font.pixelSize: 11
                    }
                }

                Item { height: 12 }
            }
        }

        // Help Overlay Modal
        Rectangle {
            anchors.fill: parent
            visible: win.helpVisible
            color: "#d0000000"

            MouseArea {
                anchors.fill: parent
                onClicked: win.helpVisible = false
            }

            Rectangle {
                width: Math.min(420, parent.width - 48)
                height: helpCol.implicitHeight + 36
                anchors.centerIn: parent
                color: "#1c1c22"
                radius: Math.max(6, theme.radius)
                border.color: theme.accent
                border.width: 1

                ColumnLayout {
                    id: helpCol
                    anchors.fill: parent
                    anchors.margins: 18
                    spacing: 12

                    Label {
                        text: qsTr("Keyboard Shortcuts")
                        font.pixelSize: 18
                        font.bold: true
                        color: "white"
                    }

                    Rectangle { Layout.fillWidth: true; height: 1; color: "#303038" }

                    RowLayout {
                        Label { text: "R"; font.bold: true; color: theme.accent; font.pixelSize: 13; Layout.preferredWidth: 60 }
                        Label { text: qsTr("Reset to Omarchy glass defaults"); color: "#e0e0e0"; font.pixelSize: 13 }
                    }

                    RowLayout {
                        Label { text: "Q"; font.bold: true; color: theme.accent; font.pixelSize: 13; Layout.preferredWidth: 60 }
                        Label { text: qsTr("Quit application"); color: "#e0e0e0"; font.pixelSize: 13 }
                    }

                    RowLayout {
                        Label { text: "?"; font.bold: true; color: theme.accent; font.pixelSize: 13; Layout.preferredWidth: 60 }
                        Label { text: qsTr("Toggle this help dialog"); color: "#e0e0e0"; font.pixelSize: 13 }
                    }

                    RowLayout {
                        Label { text: "Esc"; font.bold: true; color: theme.accent; font.pixelSize: 13; Layout.preferredWidth: 60 }
                        Label { text: qsTr("Close help or quit"); color: "#e0e0e0"; font.pixelSize: 13 }
                    }

                    Item { height: 8 }

                    Button {
                        text: qsTr("Close")
                        Layout.alignment: Qt.AlignRight
                        onClicked: win.helpVisible = false
                    }
                }
            }
        }
    }
}
