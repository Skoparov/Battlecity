#include "game_settings.h"

#include <iostream>

#include <QFile>
#include <QXmlStreamReader>

static constexpr auto tag_fps = "Fps";
static constexpr auto tag_tile_size = "TileSize";
static constexpr auto tag_player_health = "PlayerHealth";
static constexpr auto tag_player_base_health = "PlayerBaseHealth";
static constexpr auto tag_player_base_size = "PlayerBaseSize";
static constexpr auto tag_base_kills_to_win = "BaseKillsToWin";
static constexpr auto tag_tank_movement_speed = "TankMoveSpeed";

namespace game
{

void game_settings::set_fps( const uint32_t fps ) noexcept
{
    m_fps = fps;
}

uint32_t game_settings::get_fps() const noexcept
{
    return m_fps;
}

void game_settings::set_tile_size( const QSize& size ) noexcept
{
    m_tile_size = size;
}

const QSize& game_settings::get_tile_size() const noexcept
{
    return m_tile_size;
}

void game_settings::set_player_base_health( const uint32_t health ) noexcept
{
    m_base_health = health;
}

uint32_t game_settings::get_player_base_health() const noexcept
{
    return m_base_health;
}

void game_settings::set_player_base_size( const QSize& size ) noexcept
{
    m_player_base_size = size;
}

const QSize &game_settings::get_player_base_size() const noexcept
{
    return m_player_base_size;
}

void game_settings::set_player_health( const uint32_t health ) noexcept
{
    m_player_health = health;
}

uint32_t game_settings::get_player_health() const noexcept
{
    return m_player_health;
}

void game_settings::set_base_kills_to_win( const uint32_t kills ) noexcept
{
    m_base_kills_to_win = kills;
}

uint32_t game_settings::get_base_kills_to_win() const noexcept
{
    return m_base_kills_to_win;
}

void game_settings::set_tank_movement_speed( uint32_t speed ) noexcept
{
    m_tank_movement_speed = speed;
}

uint32_t game_settings::get_tank_movement_speed() const noexcept
{
    return m_tank_movement_speed;
}

game_settings read_game_settings( const QString& file )
{
    QFile settings_file{ file };
    if ( !settings_file.open( QIODevice::ReadOnly | QIODevice::Text ) )
    {
        throw std::ios_base::failure{ "Failed to open settings file" };
    }

    game_settings settings;

    QXmlStreamReader xml_reader{ &settings_file };

    while( !xml_reader.atEnd() && !xml_reader.hasError() )
    {
        QXmlStreamReader::TokenType token{ xml_reader.readNext() };

        if(token == QXmlStreamReader::StartElement)
        {
            auto name = xml_reader.name();

            if( name == tag_fps )
            {
                settings.set_fps( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_tile_size )
            {
                uint32_t tile_size{ xml_reader.readElementText().toUInt() };
                settings.set_tile_size( QSize( tile_size, tile_size ) );
            }
            else if( name == tag_player_health )
            {
                settings.set_player_health( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_player_base_health )
            {
                settings.set_player_base_health( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_player_base_size )
            {
                uint32_t base_size{ xml_reader.readElementText().toUInt() };
                settings.set_player_base_size( QSize( base_size, base_size ) );
            }
            else if( name == tag_base_kills_to_win )
            {
                settings.set_base_kills_to_win( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_tank_movement_speed )
            {
                settings.set_tank_movement_speed( xml_reader.readElementText().toUInt() );
            }
        }
    }

    if( xml_reader.hasError() )
    {
        throw std::logic_error{ std::string{ "Error parsing settings: " } +
                                xml_reader.errorString().toStdString() };
    }

    xml_reader.clear();
    settings_file.close();

    return settings;
}

}// game
