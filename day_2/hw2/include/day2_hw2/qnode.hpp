#ifndef day2_hw2_QNODE_HPP_
#define day2_hw2_QNODE_HPP_

#ifndef Q_MOC_RUN
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>
#include <turtlesim/srv/teleport_absolute.hpp>
#include <turtlesim/srv/set_pen.hpp>
#endif
#include <QThread>

class QNode : public QThread
{
  Q_OBJECT
public:
  QNode();
  ~QNode();

  void publishCmdVel(double linear, double angular);
  void teleport(double x, double y, double theta);
  void setPen(int r, int g, int b, int width, bool off);

protected:
  void run();

private:
  std::shared_ptr<rclcpp::Node> node;

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
  rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr pose_sub_;
  rclcpp::Client<turtlesim::srv::TeleportAbsolute>::SharedPtr teleport_client_;
  rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr set_pen_client_;

Q_SIGNALS:
  void rosShutDown();
  void poseReceived(double x, double y, double theta);
};

#endif
