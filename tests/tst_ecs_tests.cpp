#include <QtTest>

#include "../battlecity/ecs/framework/world.h"

class component_1{};

struct component_2
{
    component_2( int d ) noexcept : data( d ){}
    int data{ 0 };
};

class unused_component{};

class ecs_tests : public QObject
{
    Q_OBJECT

private slots:
    void entity_tests();

private:
    void add_components( ecs::entity& e );

private:
    int m_component2_data{ 42 };
};

void ecs_tests::entity_tests()
{
    ecs::world world;
    ecs::entity& e = world.create_entity();
    QVERIFY( e.get_state() == ecs::entity_state::ok );
    QVERIFY( e.get_id() != INVALID_NUMERIC_ID );


    add_components( e );

    // Check components' presence
    QVERIFY( e.has_component< component_1 >() );
    QVERIFY( e.has_component< component_1 >() );
    QVERIFY( e.has_component< component_2 >() );
    bool comps_check{ e.has_components< component_1, component_2 >() }; // QVerify won't accept this one
    QVERIFY( comps_check );

    QVERIFY( !e.has_component< unused_component>() );

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
        QFAIL( "get_component(s) failed" );
    }

    // Check apply_to
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

    // Check remove_component
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

void ecs_tests::add_components( ecs::entity& e )
{
    e.add_component< component_1 >();
    e.add_component< component_2 >( m_component2_data );
}

QTEST_APPLESS_MAIN(ecs_tests)

#include "tst_ecs_tests.moc"
