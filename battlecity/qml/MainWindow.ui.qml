import QtQuick 2.3
import QtQuick.Dialogs 1.1

Rectangle
{
    property alias mouseArea: mouseArea

    width: board.width + 64
    height: board.height + 128
    color: "gray"

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent
        Item
        {
            x:64
            y:64

            Grid
            {
                id: board
                columns: map_interface.colsCount
                rows: map_interface.rowsCount
                spacing: 0

                Repeater
                {
                    model: map_interface.tiles
                    Tile{}
                }
            }
        }
    }
}

