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

                Repeater
                {
                    model: map_interface.enemy_tanks
                    EnemyTank{}
                }

                Repeater
                {
                    model: map_interface.player_tanks
                    PlayerTank{}
                }

                Repeater
                {
                    model: map_interface.projectiles
                    Projectile{}
                }
            }
        }

        Text
        {
            text: map_interface.text
            font.family: "Comic Sans"
            font.pointSize: 24
            color: "red"
            visible: map_interface.text_visible
            anchors.centerIn: parent
        }
    }
}

