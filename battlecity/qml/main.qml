import QtQuick 2.3
import QtQuick.Window 2.0

Window
{
    visible: true
    width: main_window.width
    height: main_window.height

    MainWindow
    {
        id: main_window
        anchors.centerIn: parent
    }
}
