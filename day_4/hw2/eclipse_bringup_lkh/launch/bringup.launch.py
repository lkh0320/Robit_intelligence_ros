from launch import LaunchDescription
from launch.actions import RegisterEventHandler
from launch.event_handlers import OnProcessExit
from launch.substitutions import Command, FindExecutable, PathJoinSubstitution
from launch_ros.actions import Node
from launch_ros.parameter_descriptions import ParameterValue
from launch_ros.substitutions import FindPackageShare


def generate_launch_description():
    robot_description = ParameterValue(
        Command([
            FindExecutable(name='xacro'), ' ',
            PathJoinSubstitution(
                [FindPackageShare('robot_description'), 'urdf', 'eclipse.xacro']),
            ' with_base:=true with_arm:=true with_camera_tower:=true',
            ' use_mock_base:=true use_mock_arm:=true use_mock_ct:=true',
        ]),
        value_type=str,
    )

    controllers_yaml = PathJoinSubstitution(
        [FindPackageShare('eclipse_bringup_lkh'), 'config', 'controllers.yaml'])
    rviz_config = PathJoinSubstitution(
        [FindPackageShare('eclipse_bringup_lkh'), 'config', 'eclipse_bringup.rviz'])

    robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        parameters=[{'robot_description': robot_description}],
        output='screen',
    )

    control_node = Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[controllers_yaml],
        arguments=['--ros-args', '--log-level', 'diff_drive_controller:=error'],
        output='screen',
    )

    joint_state_broadcaster_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=['joint_state_broadcaster'],
        output='screen',
    )

    controller_spawner = Node(
        package='controller_manager',
        executable='spawner',
        arguments=[
            'camera_tower_controller',
            'arm_controller',
            'gripper_controller',
            'flipper_controller',
            'diff_drive_controller',
        ],
        output='screen',
    )

    rviz = Node(
        package='rviz2',
        executable='rviz2',
        arguments=['-d', rviz_config],
        output='screen',
    )

    return LaunchDescription([
        robot_state_publisher,
        control_node,
        joint_state_broadcaster_spawner,
        RegisterEventHandler(OnProcessExit(
            target_action=joint_state_broadcaster_spawner,
            on_exit=[controller_spawner],
        )),
        rviz,
    ])
