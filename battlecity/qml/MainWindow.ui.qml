import QtQuick 2.3
import QtQuick.Dialogs 1.1

Rectangle
{
    property alias mouseArea: mouseArea

    width: board_grid.width
    height: board_grid.height
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
                id: board_grid
                columns: map_interface.rows_num
                rows: map_interface.columns_num
                spacing: 0
                Repeater
                {
                    model: map_interface.tiles
                    tile{}
                }
            }
        }
    }
}

