from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, EmitEvent, RegisterEventHandler
from launch.events import matches_action
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import LifecycleNode
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState
from launch_ros.parameter_descriptions import ParameterValue
from lifecycle_msgs.msg import Transition


def configure_event(node):
    return EmitEvent(event=ChangeState(
        lifecycle_node_matcher=matches_action(node),
        transition_id=Transition.TRANSITION_CONFIGURE,
    ))


def activate_when_inactive(node):
    return RegisterEventHandler(OnStateTransition(
        target_lifecycle_node=node,
        goal_state='inactive',
        entities=[EmitEvent(event=ChangeState(
            lifecycle_node_matcher=matches_action(node),
            transition_id=Transition.TRANSITION_ACTIVATE,
        ))],
    ))


def generate_launch_description():
    args = [
        DeclareLaunchArgument(
            'watchdog',
            default_value='imu_watchdog',
            description='imu_watchdog | imu_watchdog_sync | imu_watchdog_mt',
        ),
        DeclareLaunchArgument('check_rate_hz', default_value='2.0'),
        DeclareLaunchArgument('state_period_sec', default_value='1.0'),
        DeclareLaunchArgument('use_separate_group', default_value='true'),
        DeclareLaunchArgument('log_age', default_value='true'),
    ]

    fake_imu = LifecycleNode(
        package='day4_hw1',
        executable='fake_imu',
        name='fake_imu',
        namespace='',
        output='screen',
        parameters=[{'rate_hz': 50.0, 'stamp_offset_sec': 0.0}],
    )

    imu_watchdog = LifecycleNode(
        package='day4_hw1',
        executable=LaunchConfiguration('watchdog'),
        name='imu_watchdog',
        namespace='',
        output='screen',
        parameters=[{
            'timeout_sec': 0.5,
            'use_wall_timer': False,
            'check_rate_hz': ParameterValue(
                LaunchConfiguration('check_rate_hz'), value_type=float),
            'state_period_sec': ParameterValue(
                LaunchConfiguration('state_period_sec'), value_type=float),
            'use_separate_group': ParameterValue(
                LaunchConfiguration('use_separate_group'), value_type=bool),
            'log_age': ParameterValue(
                LaunchConfiguration('log_age'), value_type=bool),
        }],
    )

    return LaunchDescription(args + [
        activate_when_inactive(fake_imu),
        activate_when_inactive(imu_watchdog),
        fake_imu,
        imu_watchdog,
        configure_event(fake_imu),
        configure_event(imu_watchdog),
    ])
