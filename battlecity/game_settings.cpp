#include "game_settings.h"

#include <QFile>
#include <QXmlStreamReader>

static constexpr auto tag_fps = "Fps";
static constexpr auto tag_tile_size = "TileSize";
static constexpr auto tag_tank_size = "TankSize";
static constexpr auto tag_projectile_size = "ProjectileSize";
static constexpr auto tag_player_base_size = "PlayerBaseSize";
static constexpr auto tag_tank_health = "TankHealth";
static constexpr auto tag_wall_health = "WallHealth";
static constexpr auto tag_iron_wall_health = "IronWallHealth";
static constexpr auto tag_player_base_health = "PlayerBaseHealth";
static constexpr auto tag_player_lives = "PlayerLives";
static constexpr auto tag_enemies_number = "EnemiesNum";
static constexpr auto tag_respawn_delay_ms = "RespawnDelayMs";
static constexpr auto tag_base_kills_to_win = "BaseKillsToWin";
static constexpr auto tag_tank_speed = "TankSpeed";
static constexpr auto tag_projectile_speed = "ProjectileSpeed";
static constexpr auto tag_projectile_damage = "ProjectileDamage";
static constexpr auto tag_turret_cooldown_ms = "TurretCooldownMs";
static constexpr auto tag_ai_chance_to_fire = "AiChanceToFire";
static constexpr auto tag_explosion_animation_data = "ExplosionAnimation";
static constexpr auto tag_respawn_animation_data = "RespawnAnimation";
static constexpr auto tag_shield_animation_data = "ShieldAnimation";
static constexpr auto tag_animation_frame_num = "FrameNum";
static constexpr auto tag_animation_frame_rate = "FrameRate";
static constexpr auto tag_animation_loops_num = "LoopsNum";
static constexpr auto tag_animation_duration_ms = "DurationMs";
static constexpr auto tag_respawn_shield_timeout = "ShieldRespawnTimeoutMs";

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

void game_settings::set_tank_size( const QSize& size ) noexcept
{
    m_tank_size = size;
}

const QSize& game_settings::get_tank_size() const noexcept
{
    return m_tank_size;
}

void game_settings::set_projectile_size( const QSize& size ) noexcept
{
    m_projectile_size = size;
}

const QSize& game_settings::get_projectile_size() const noexcept
{
    return m_projectile_size;
}

void game_settings::set_player_base_size( const QSize& size ) noexcept
{
    m_player_base_size = size;
}

const QSize& game_settings::get_player_base_size() const noexcept
{
    return m_player_base_size;
}

void game_settings::set_tank_health( const uint32_t health ) noexcept
{
    m_tank_health = health;
}

uint32_t game_settings::get_tank_health() const noexcept
{
    return m_tank_health;
}

void game_settings::set_wall_health( const uint32_t health ) noexcept
{
    m_wall_health = health;
}

uint32_t game_settings::get_wall_health() const noexcept
{
    return m_wall_health;
}

void game_settings::set_iron_wall_health( const uint32_t health ) noexcept
{
    m_iron_wall_health = health;
}

uint32_t game_settings::get_iron_wall_health() const noexcept
{
    return m_iron_wall_health;
}

void game_settings::set_player_base_health( const uint32_t health ) noexcept
{
    m_base_health = health;
}

uint32_t game_settings::get_player_base_health() const noexcept
{
    return m_base_health;
}

void game_settings::set_player_lives( const uint32_t lives ) noexcept
{
    m_player_lives = lives;
}

uint32_t game_settings::get_player_lives() const noexcept
{
    return m_player_lives;
}

void game_settings::set_enemies_number( const uint32_t number ) noexcept
{
    m_enemies_number = number;
}

uint32_t game_settings::get_enemies_number() const noexcept
{
    return m_enemies_number;
}

void game_settings::set_respawn_delay_ms( const uint32_t delay ) noexcept
{
    m_respawn_delay_ms = delay;
}

uint32_t game_settings::get_respawn_delay_ms() const noexcept
{
    return m_respawn_delay_ms;
}

void game_settings::set_base_kills_to_win( const uint32_t kills ) noexcept
{
    m_base_kills_to_win = kills;
}

uint32_t game_settings::get_base_kills_to_win() const noexcept
{
    return m_base_kills_to_win;
}

void game_settings::set_tank_speed( uint32_t speed ) noexcept
{
    m_tank_speed = speed;
}

uint32_t game_settings::get_tank_speed() const noexcept
{
    return m_tank_speed;
}

void game_settings::set_projectile_speed( uint32_t speed ) noexcept
{
    m_projectile_speed = speed;
}

uint32_t game_settings::get_projectile_speed() const noexcept
{
    return m_projectile_speed;
}

void game_settings::set_projectile_damage( uint32_t damage ) noexcept
{
    m_projectile_damage = damage;
}

uint32_t game_settings::get_projectile_damage() const noexcept
{
    return m_projectile_damage;
}

void game_settings::set_turret_cooldown_ms( uint32_t cooldown_ms ) noexcept
{
    m_turret_cooldown_ms = cooldown_ms;
}

uint32_t game_settings::get_turret_cooldown_ms() const noexcept
{
    return m_turret_cooldown_ms;
}

void game_settings::set_ai_chance_to_fire( float chance_to_fire ) noexcept
{
    m_ai_chance_to_file = chance_to_fire;
}

float game_settings::get_ai_chance_to_fire() const noexcept
{
    return m_ai_chance_to_file;
}

void game_settings::set_powerup_respawn_timeout( const powerup_type& type, uint32_t timeout )
{
    m_powerup_timeouts[ type ] = timeout;
}

uint32_t game_settings::get_powerup_respawn_timeout( const powerup_type& type ) const
{
    return m_powerup_timeouts.at( type );
}

void game_settings::set_animation_data( const animation_type& type, const animation_data& data )
{
    m_animation_data[ type ] = data;
}

const animation_data& game_settings::get_animation_data( const animation_type& type ) const
{
    return m_animation_data.at( type );
}

const std::map< animation_type, animation_data >& game_settings::get_animation_data() const noexcept
{
    return m_animation_data;
}

animation_data read_animation_data( QXmlStreamReader& xml_reader, const QStringRef& start_tag_name )
{
    animation_data data;
    QStringRef name;

    do
    {
        xml_reader.readNextStartElement();
        name = xml_reader.name();

        if( name == tag_animation_frame_rate )
        {
            data.frame_rate = xml_reader.readElementText().toUInt();
        }
        else if( name == tag_animation_frame_num )
        {
            data.frame_num = xml_reader.readElementText().toUInt();
        }
        else if( name == tag_animation_loops_num )
        {
            data.loops = xml_reader.readElementText().toUInt();
        }
        else if( name == tag_animation_duration_ms )
        {
            data.duration = std::chrono::milliseconds{ xml_reader.readElementText().toUInt() };
        }
    }
    while( name != start_tag_name );

    return data;
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

        if( token == QXmlStreamReader::StartElement )
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
            else if( name == tag_tank_size )
            {
                uint32_t tank_size{ xml_reader.readElementText().toUInt() };
                settings.set_tank_size( QSize( tank_size, tank_size ) );
            }
            else if( name == tag_projectile_size )
            {
                uint32_t projectile_size{ xml_reader.readElementText().toUInt() };
                settings.set_projectile_size( QSize( projectile_size, projectile_size ) );
            }
            else if( name == tag_player_base_size )
            {
                uint32_t base_size{ xml_reader.readElementText().toUInt() };
                settings.set_player_base_size( QSize( base_size, base_size ) );
            }
            else if( name == tag_tank_health )
            {
                settings.set_tank_health( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_wall_health )
            {
                settings.set_wall_health( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_iron_wall_health )
            {
                settings.set_iron_wall_health( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_player_base_health )
            {
                settings.set_player_base_health( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_player_lives )
            {
                settings.set_player_lives( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_enemies_number )
            {
                settings.set_enemies_number( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_respawn_delay_ms )
            {
                settings.set_respawn_delay_ms( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_base_kills_to_win )
            {
                settings.set_base_kills_to_win( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_tank_speed )
            {
                settings.set_tank_speed( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_projectile_speed )
            {
                settings.set_projectile_speed( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_projectile_damage )
            {
                settings.set_projectile_damage( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_turret_cooldown_ms )
            {
                settings.set_turret_cooldown_ms( xml_reader.readElementText().toUInt() );
            }
            else if( name == tag_ai_chance_to_fire )
            {
                settings.set_ai_chance_to_fire( xml_reader.readElementText().toFloat() );
            }
            else if( name == tag_explosion_animation_data )
            {
                settings.set_animation_data( animation_type::explosion,
                                             read_animation_data( xml_reader, name ) );
            }
            else if( name == tag_respawn_animation_data )
            {
                settings.set_animation_data( animation_type::respawn,
                                             read_animation_data( xml_reader, name ) );
            }
            else if( name == tag_shield_animation_data )
            {
                settings.set_animation_data( animation_type::shield,
                                             read_animation_data( xml_reader, name ) );
            }
            else if( name == tag_respawn_shield_timeout )
            {
                settings.set_powerup_respawn_timeout( powerup_type::shield,
                                                      xml_reader.readElementText().toUInt() );
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
