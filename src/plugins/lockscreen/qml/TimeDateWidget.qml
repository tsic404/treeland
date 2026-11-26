// Copyright (C) 2023 ComixHe <heyuming@uniontech.com>.
// SPDX-License-Identifier: Apache-2.0 OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
import QtQuick
import QtQuick.Controls
import Treeland
import LockScreen

Control {
    id: timedataWidget

    property date currentDate: new Date()
    property var currentLocale: regionFormat.locale

    RegionFormatProvider {
        id: regionFormat
    }

    Timer {
        interval: 1000
        running: true
        repeat: true
        triggeredOnStart: true

        onTriggered: {
            currentDate = new Date()
        }
    }

    Text {
        id: timeText
        text: regionFormat.shortTimeFormat !== ""
              ? currentDate.toLocaleTimeString(currentLocale, regionFormat.shortTimeFormat)
              : currentDate.toLocaleTimeString(currentLocale)
        font.weight: Font.Light
        font.pixelSize: 80
        color: palette.windowText

        anchors {
            left: parent.left
            leftMargin: 20
            top: parent.top
        }
    }

    Text {
        id: dateText
        text: regionFormat.shortDateFormat !== ""
              ? currentDate.toLocaleDateString(currentLocale, regionFormat.shortDateFormat)
              : currentDate.toLocaleDateString(currentLocale)
        font.weight: Font.Normal
        font.pixelSize: 20
        color: palette.windowText

        anchors {
            left: parent.left
            leftMargin: 20
            top: timeText.bottom
        }
    }
}
