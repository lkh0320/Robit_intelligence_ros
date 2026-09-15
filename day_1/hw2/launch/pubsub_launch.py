from launch import LaunchDescription
from launch_ros.actions import Node


def generate_launch_description():
    """ros2 launch가 자동으로 호출하는 함수. 실행할 노드 목록을 반환한다."""
    return LaunchDescription([
        # Node() 하나 = 실행할 노드 하나
        Node(
            package='hw2',            # hw2 패키지에 있는
            executable='publisher',   # publisher 실행 파일을
            name='publisher',         # 이 노드 이름으로
            output='screen',          # 로그는 터미널에 출력
        ),
        Node(
            package='hw2',
            executable='subscriber',
            name='subscriber',
            output='screen',
        ),
    ])