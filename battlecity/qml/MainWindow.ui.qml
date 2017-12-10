import QtQuick 2.3
import QtQuick.Dialogs 1.1

Rectangle
{
    property alias mouseArea: mouseArea

    width: game_map.width
    height: game_map.height
    color: "gray"

    MouseArea
    {
        id: mouseArea
        anchors.fill: parent
        Item
        {
            Grid
            {
                id: game_map
                columns: map_interface.rows_num
                rows: map_interface.columns_num
                spacing: 0

                Repeater
                {
                    model: map_interface.tiles
                    Tile{}
                }

                Repeater
                {
                    model: map_interface.player_bases
                    PlayerBase{}
                }
            }
        }
    }
}

