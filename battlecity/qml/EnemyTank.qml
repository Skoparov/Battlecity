import QtQuick 2.3

Item
{
    id: enemy_tank
    x: modelData.pos_x
    y: modelData.pos_y
    width: modelData.width
    height: modelData.height

    Image
    {
        id: enemy_tank_image
        anchors.centerIn: parent
        source: modelData.image_path
        rotation: modelData.rotation
        visible: modelData.visible
    }
}
