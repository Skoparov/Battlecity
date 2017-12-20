import QtQuick 2.5
import QtQuick.Dialogs 1.1

Rectangle
{
    property alias mouseArea: mouse_area

    width: game_map.width + side_bar.width
    height: game_map.height
    color: "#333333"

    MouseArea
    {
        id: mouse_area
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

                Repeater
                {
                    model: map_interface.explosions
                    Explosion{}
                }
            }
        }

        Rectangle
        {
            id: announcenement_back
            color: "white"
            opacity: 0.7
            width: announcenement_text.width + 10
            height: announcenement_text.height + 10
            visible: map_interface.announcement_visible
            anchors.centerIn: parent

            Text
            {
                id: announcenement_text
                text: map_interface.announcement_text
                font.family: "Comic Sans"
                font.pointSize: 18
                font.capitalization: Font.AllUppercase
                font.bold: true
                color: "black"
                anchors.centerIn: parent
            }
        }
    }

    SideBar
    {
        id: side_bar
        x: game_map.width
        height: parent.height
    }

    Keys.onPressed:
    {
        if( event.key === Qt.Key_P )
        {
            map_interface.pause_resume()
            event.accepted = true;
        }
    }
}

