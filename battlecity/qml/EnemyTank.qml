import QtQuick 2.3

Item
{
    id: enemy_tank
    x: modelData.pos_x
    y: modelData.pos_y
    width: modelData.width
    height: modelData.height

    AnimatedSprite
    {
        id: enemy_tank_image
        anchors.centerIn: parent
        width: modelData.width
        height: modelData.height
        source: modelData.image_path
        frameCount: 2
        frameRate: 30
        rotation: modelData.rotation
        visible: modelData.visible
    }
}
