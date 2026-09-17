from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, EmitEvent, RegisterEventHandler
from launch.events import matches_action
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import LifecycleNode, SetParameter
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState
from lifecycle_msgs.msg import Transition


def generate_launch_description():
    use_wall_timer = LaunchConfiguration('use_wall_timer')

    imu_watchdog = LifecycleNode(
        package='hw1',
        executable='imu_watchdog',
        name='imu_watchdog',
        namespace='',
        output='screen',
        parameters=[{
            'timeout_sec': 0.5,
            'check_rate_hz': 5.0,
            'use_wall_timer': use_wall_timer,
        }],
    )

    configure = EmitEvent(event=ChangeState(
        lifecycle_node_matcher=matches_action(imu_watchdog),
        transition_id=Transition.TRANSITION_CONFIGURE,
    ))

    activate_when_inactive = RegisterEventHandler(OnStateTransition(
        target_lifecycle_node=imu_watchdog,
        goal_state='inactive',
        entities=[EmitEvent(event=ChangeState(
            lifecycle_node_matcher=matches_action(imu_watchdog),
            transition_id=Transition.TRANSITION_ACTIVATE,
        ))],
    ))

    return LaunchDescription([
        DeclareLaunchArgument('use_wall_timer', default_value='false'),
        SetParameter(name='use_sim_time', value=True),
        activate_when_inactive,
        imu_watchdog,
        configure,
    ])
