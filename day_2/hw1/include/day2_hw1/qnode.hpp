#ifndef day2_hw1_QNODE_HPP_
#define day2_hw1_QNODE_HPP_

#ifndef Q_MOC_RUN
#include <rclcpp/rclcpp.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/srv/set_pen.hpp>
#include <std_srvs/srv/empty.hpp>
#endif
#include <QThread>

class QNode : public QThread
{
  Q_OBJECT
public:
  QNode();
  ~QNode();

  void publishCmdVel(double linear, double angular);
  void setPen(int r, int g, int b, int width);
  void clearScreen();

protected:
  void run();

private:
  std::shared_ptr<rclcpp::Node> node;

  rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_pub_;
  rclcpp::Subscription<geometry_msgs::msg::Twist>::SharedPtr cmd_vel_sub_;
  rclcpp::Client<turtlesim::srv::SetPen>::SharedPtr set_pen_client_;
  rclcpp::Client<std_srvs::srv::Empty>::SharedPtr clear_client_;

Q_SIGNALS:
  void rosShutDown();
  void cmdVelReceived(double linear, double angular);
};

#endif
