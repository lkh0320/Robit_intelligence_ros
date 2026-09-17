# Day3_hw1

이규환 2026406017

## 1. 개요 (Overview)
Lifecycle 노드로 가짜 IMU 센서 노드(fake_imu)와 그 데이터를 감시하는 워치독 노드(imu_watchdog)를 만들고, 실제 시간과 sim time(rosbag 재생)에서 워치독이 올바르게 동작하는지 확인하는 프로그램이다.

## 2. 체크포인트 (checkpoint)

### 2.1 fake_imu: 상태별로 ros2 topic hz /imu 확인

Unconfigured
![](etc/fake_imu_unconfigured.png)

Inactive
![](etc/fake_imu_inactive.png)

Active
![](etc/fake_imu_active.png)

Finalized
![](etc/fake_imu_finalized.png)

Unconfigured, Inactive에서는 /imu가 발행되지 않고(does not appear to be published yet), activate 이후에만 약 50Hz로 발행된다. shutdown하면 on_shutdown from [active] 로그와 함께 Finalized [4] 상태가 된다.

### 2.2 워치독 뼈대: 전이 로그, 파라미터 FAILURE 확인

전이 로그
![](etc/watchdog_transition_log.png)

configure, activate 시 on_configure / on_activate 로그가 출력되고, transition_event 토픽에서 inactive → activating → active 전이를 확인할 수 있다. fake_imu가 없는 상태라 [NO DATA]가 한 번 출력된다.

파라미터 FAILURE
![](etc/watchdog_param_failure.png)

timeout_sec를 0으로 두고 configure하면 `timeout_sec must be > 0` 에러와 함께 Transitioning failed가 나고, configuring → unconfigured로 되돌아간다.

### 2.3 실제 시간에서 두 노드 연동
![](etc/realtime_link_rqt_graph.png)

두 노드를 모두 Active로 만들면 rqt_graph에서 /fake_imu → /imu → /imu_watchdog → /imu_checked 연결을 확인할 수 있고, /imu_checked가 50Hz로 재발행된다.

### 2.4 fake_imu deactivate / offset으로 고장 재현

deactivate로 고장
![](etc/fault_deactivate.png)

fake_imu를 deactivate하면 새 데이터가 안 들어와서 [STALE]의 age가 계속 늘어난다. 다시 activate하면 [RECOVERED] age = 0.01 s가 출력된다.

offset으로 고장 (지연 데이터)
![](etc/fault_offset.png)

stamp_offset_sec를 1.0으로 올리면 /imu는 계속 들어오지만 [STALE] age = 1.01 s로 고정되고, /imu_checked 재발행이 멈춘다(hz의 max가 크게 늘어남). 0.0으로 되돌리면 [RECOVERED]가 출력된다.

### 2.5 고장을 넣으며 ros2 bag record /imu로 공백 bag 녹화
![](etc/bag_record_info.png)

imu_gap_bag 으로 저장됨. 약 60초 동안 정상 → deactivate → activate → offset 1.0 → offset 0.0 순서로 고장을 넣으며 녹화했고, 메시지는 2427개이다.

### 2.6 워치독만 sim time으로 켜고 ros2 bag play --clock 재생, 일시정지·2배속 확인

일시정지
![](etc/simtime_pause.png)

[STALE] 구간에서 일시정지하면 /clock이 멈춰서 create_timer도 멈추고 [STALE] 로그가 더 찍히지 않는다. 재개하면 같은 흐름으로 이어지다가 [RECOVERED]가 출력된다.

2배속
![](etc/simtime_rate_2x.png)

2배속으로 재생하면 sim time이 2배로 흘러서 로그 간격이 0.1초로 줄고, age도 한 줄에 0.2초씩 늘어난다.

### 2.7 워치독만 wall timer로 바꿔 일시정지 결과 비교
![](etc/walltimer_pause.png)

use_wall_timer:=true로 실행(timer = wall)하고 [STALE] 구간에서 일시정지하면, now()가 멈춰서 age는 0.87 s 그대로지만 wall timer는 실제 시간으로 계속 돌아서 같은 [STALE]이 계속 출력된다. 2.6과 달리 재생이 멈춰도 검사가 계속 돈다.

### 2.8 launch 실행
![](etc/launch_auto_active.png)

launch 한 번으로 fake_imu와 imu_watchdog 모두 configure → activate까지 자동으로 진행되고, 두 노드 모두 active [3] 상태이며 /imu_checked가 50Hz로 발행된다.

## 3. 추가적인 설명

강의자료의 lifecycle_talker 구조를 바탕으로 헤더파일과 소스파일을 분리해서 fake_imu와 imu_watchdog를 만들었다. fake_imu는 Active일 때만 create_timer로 /imu를 발행하고, stamp = now() − stamp_offset_sec로 지연 데이터를 만든다. imu_watchdog는 /imu 구독과 검사 타이머를 on_activate에서 만들고 on_deactivate에서 해제하며, LifecyclePublisher인 /imu_checked만 상태를 자동으로 따른다. 수신한 스탬프와 현재 시간의 차이(age)를 Duration으로 비교해서 timeout_sec 이하인 신선한 데이터만 /imu_checked로 재발행하고, 검사 타이머에서 [NO DATA] [STALE] [RECOVERED] 로그를 출력한다. 2.7 비교를 코드 수정 없이 하려고 use_wall_timer 파라미터를 추가했고, launch는 OnStateTransition 이벤트로 inactive가 되면 activate를 요청하도록 했다.
