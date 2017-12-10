import QtQuick 2.3

Item
{
    id: player_base
    x: modelData.pos_x
    y: modelData.pos_y
    width: modelData.width
    height: modelData.height

    Image
    {
        id: player_base_image
        anchors.centerIn: parent
        source: modelData.image_path
        rotation: modelData.rotation
    }
}
