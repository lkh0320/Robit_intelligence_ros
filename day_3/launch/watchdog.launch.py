from launch import LaunchDescription
from launch.actions import EmitEvent, RegisterEventHandler
from launch.events import matches_action
from launch_ros.actions import LifecycleNode
from launch_ros.event_handlers import OnStateTransition
from launch_ros.events.lifecycle import ChangeState
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
    fake_imu = LifecycleNode(
        package='hw1',
        executable='fake_imu',
        name='fake_imu',
        namespace='',
        output='screen',
        parameters=[{'rate_hz': 50.0, 'stamp_offset_sec': 0.0}],
    )

    imu_watchdog = LifecycleNode(
        package='hw1',
        executable='imu_watchdog',
        name='imu_watchdog',
        namespace='',
        output='screen',
        parameters=[{'timeout_sec': 0.5, 'check_rate_hz': 5.0, 'use_wall_timer': False}],
    )

    return LaunchDescription([
        activate_when_inactive(fake_imu),
        activate_when_inactive(imu_watchdog),
        fake_imu,
        imu_watchdog,
        configure_event(fake_imu),
        configure_event(imu_watchdog),
    ])
