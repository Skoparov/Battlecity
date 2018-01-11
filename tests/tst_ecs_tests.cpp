#include <QtTest>

#include "../battlecity/ecs/framework/world.h"

class component_1{};

struct component_2
{
    component_2( int d ) noexcept : data( d ){}
    int data{ 0 };
};

class unused_component{};

struct test_event
{
    test_event( int d ) noexcept : data( d ){}
    int data{ 0 };
};

class test_system : public ecs::system,
                    public ecs::event_callback< test_event >
{
public:
    test_system( ecs::world& world ) : ecs::system( world ){}

    void init() override{ data = 1; }
    void clean() override{ data = 0; }
    bool tick() override{ ++data; return true; }
    void on_event( const test_event& e ){ data = e.data; }

    int data{ 0 };

    static constexpr int data_default_val{ 0 };
    static constexpr int data_upon_init{ 1 };
};

class ecs_tests : public QObject
{
    Q_OBJECT

private slots:
    void entity_tests();
    void world_tests();

private:
    void add_components( ecs::entity& e );

private:
    int m_component2_data{ 42 };
};


template< typename exception_type >
std::string create_fail_message( const std::string& failed_func_name, const exception_type& e )
{
    return failed_func_name + " failed with exception: " + e.what();
}

void ecs_tests::entity_tests()
{
    ecs::world world;
    ecs::entity& e = world.create_entity();
    QVERIFY( e.get_state() == ecs::entity_state::ok );
    QVERIFY( e.get_id() != INVALID_NUMERIC_ID );


    add_components( e );

    // Check components' presence
    {
        QVERIFY( e.has_component< component_1 >() );
        QVERIFY( e.has_component< component_1 >() );
        QVERIFY( e.has_component< component_2 >() );
        bool comps_check{ e.has_components< component_1, component_2 >() }; // QVerify won't accept this one
        QVERIFY( comps_check );

        QVERIFY( !e.has_component< unused_component>() );
    }

    // Check get_component(s)
    QVERIFY_EXCEPTION_THROWN( e.get_component< unused_component >(), std::exception );

    try
    {
        const ecs::entity& const_e = e;

        {
            component_1& comp_1 = e.get_component< component_1 >();
            component_2& comp_2 = e.get_component< component_2 >();
            const component_1& const_comp_1 = const_e.get_component< component_1 >();
            const component_2& const_comp_2 = const_e.get_component< component_2 >();

            QVERIFY( comp_2.data == m_component2_data );
            QVERIFY( const_comp_2.data == m_component2_data );
        }
        {
            auto components = e.get_components< component_1, component_2 >();
            auto const_components = const_e.get_components< component_1, component_2 >();
            component_1& comp_1 = std::get< 0 >( components );
            const component_1& const_comp_1 = std::get< 0 >( const_components );
            component_2& comp_2 = std::get< 1 >( components );
            const component_2& const_comp_2 = std::get< 1 >( const_components );

            QVERIFY( comp_2.data == m_component2_data );
            QVERIFY( const_comp_2.data == m_component2_data );
        }
    }
    catch( const std::exception& e )
    {
        std::string fail_message{ create_fail_message( "get_component(s)", e ) };
        QFAIL( fail_message.c_str() );
    }

    // Check apply_to
    {
        bool called{ false };
        e.apply_to< component_1 >( [&]( ecs::entity&, component_1& )
        {
            called = true;
            return true;
        } );

        QVERIFY( called );
        called = false;
        int value{ m_component2_data * 2 };

        e.apply_to< component_1, component_2 >( [&]( ecs::entity&, component_1&, component_2& c )
        {
            called = true;
            c.data = value;
            return true;
        } );

        QVERIFY( called );
        QVERIFY( e.get_component< component_2 >().data == value );
    }

    // Check remove_component
    {
        e.remove_component< component_1 >();
        QVERIFY( !e.has_component< component_1 >() );
        QVERIFY( e.has_component< component_2 >() );

        e.add_component< component_1 >();
        e.remove_components< component_1, component_2 >();
        QVERIFY( !e.has_component< component_1 >() );
        QVERIFY( !e.has_component< component_2 >() );
        bool comps_present{ e.has_components< component_1, component_2 >() };
        QVERIFY( !comps_present );
    }
}

void ecs_tests::world_tests()
{
    ecs::world world;
    ecs::entity& entity_one_comp = world.create_entity();
    entity_one_comp.add_component< component_2 >( m_component2_data );
    ecs::entity& entity_two_comps = world.create_entity();
    add_components( entity_two_comps );

    // check entity_present
    {
        QVERIFY( world.entity_present( entity_one_comp.get_id() ) );
        QVERIFY( world.entity_present( entity_two_comps.get_id() ) );
    }

    // check get_entity
    {
        ecs::entity& e_get = world.get_entity( entity_one_comp.get_id() );
        QVERIFY( &entity_one_comp == &e_get );
    }

    // check get_entities_with_components
    try
    {
        auto entities_with_component2 = world.get_entities_with_components< component_2 >();
        QVERIFY( entities_with_component2.size() == 2 );
        bool first_present{ false };
        bool second_present{ false };
        for( ecs::entity* e : entities_with_component2 )
        {
            if( e->get_id() == entity_one_comp.get_id() )
            {
                first_present = true;
            }
            if( e->get_id() == entity_two_comps.get_id() )
            {
                second_present = true;
            }
        }

        QVERIFY( first_present );
        QVERIFY( second_present );

        auto entities_two_comps = world.get_entities_with_components< component_1, component_2 >();
        QVERIFY( entities_two_comps.size() == 1 );
        QVERIFY( entities_two_comps.front()->get_id() == entity_two_comps.get_id() );
    }
    catch( const std::exception& e )
    {
        std::string fail_message{ create_fail_message( "get_entities_with_components", e ) };
        QFAIL( fail_message.c_str() );
    }

    // check for_each_with
    try
    {
        uint64_t called{ 0 };
        world.for_each_with< component_1 >( [ & ]( ecs::entity& e, component_1& )
        {
            ++called;
            return true;
        } );

        QVERIFY( called == 1 );

        called = 0;
        int value{ m_component2_data * 2 };
        world.for_each_with< component_2 >( [ & ]( ecs::entity& e, component_2& comp )
        {
            ++called;
            comp.data = value;
            return true;
        } );

        QVERIFY( called == 2 );
        QVERIFY( entity_one_comp.get_component< component_2 >().data == value );
        QVERIFY( entity_two_comps.get_component< component_2 >().data == value );

        called = 0;
        world.for_each_with< component_2 >( [ & ]( ecs::entity& e, component_2& comp )
        {
            ++called;
            comp.data = m_component2_data;
            return false;
        } );

        QVERIFY( called == 1 );
        QVERIFY( entity_one_comp.get_component< component_2 >().data == m_component2_data ||
                 entity_two_comps.get_component< component_2 >().data == m_component2_data );
    }
    catch( const std::exception& e )
    {
        std::string fail_message{ create_fail_message( "for_each_with", e ) };
        QFAIL( fail_message.c_str() );
    }

    // check remove_entity
    world.schedule_remove_entity( entity_one_comp.get_id() );
    QVERIFY( world.entity_present( entity_one_comp.get_id() ) );
    world.tick();
    QVERIFY( !world.entity_present( entity_one_comp.get_id() ) );

    world.remove_entity( entity_two_comps.get_id() );
    QVERIFY( !world.entity_present( entity_two_comps.get_id() ) );

    // check systems handling
    test_system system1{ world };
    test_system system2{ world };
    world.add_system( system1 );
    world.add_system( system2 );
    QVERIFY( system1.data == test_system::data_upon_init );
    QVERIFY( system2.data == test_system::data_upon_init );

    world.tick();
    QVERIFY( system1.data == test_system::data_upon_init + 1 );
    QVERIFY( system2.data == test_system::data_upon_init + 1 );

    // check events
    test_event event{ m_component2_data };
    world.subscribe< test_event >( system1 );
    world.emit_event( event );
    QVERIFY( system1.data == event.data );

    system1.data = test_system::data_default_val;
    world.unsubscribe< test_event >( system1 );
    world.emit_event( event );
    QVERIFY( system1.data == test_system::data_default_val );

    // check reset() and clean()
    {
        ecs::entity& temp_entity = world.create_entity();
        world.reset();
        QVERIFY( !world.entity_present( temp_entity.get_id() ) );
        QVERIFY( system1.data == test_system::data_default_val );
        QVERIFY( system2.data == test_system::data_default_val );
    }

    {
        ecs::entity& temp_entity = world.create_entity();
        world.clean();
        world.tick();
        QVERIFY( !world.entity_present( temp_entity.get_id() ) );
        QVERIFY( system1.data == test_system::data_default_val );
        QVERIFY( system2.data == test_system::data_default_val );
    }

    // check systems' removal
    world.add_system( system1 );
    world.add_system( system2 );

    world.schedule_remove_system( system1 );
    world.tick();
    QVERIFY( system1.data == test_system::data_upon_init );
    QVERIFY( system2.data == test_system::data_upon_init + 1 );

    world.remove_system( system2 );
    world.tick();
    QVERIFY( system1.data == test_system::data_upon_init );
    QVERIFY( system2.data == test_system::data_upon_init + 1 );
}

void ecs_tests::add_components( ecs::entity& e )
{
    e.add_component< component_1 >();
    e.add_component< component_2 >( m_component2_data );
}

QTEST_APPLESS_MAIN(ecs_tests)

#include "tst_ecs_tests.moc"
