#ifndef day2_hw3_QNODE_HPP_
#define day2_hw3_QNODE_HPP_

#ifndef Q_MOC_RUN
#include <rclcpp/rclcpp.hpp>
#include <std_msgs/msg/int32.hpp>
#include <std_msgs/msg/bool.hpp>
#include <std_msgs/msg/char.hpp>
#include <std_srvs/srv/trigger.hpp>
#endif
#include <QThread>
#include <QString>

class QNode : public QThread
{
  Q_OBJECT
public:
  QNode();
  ~QNode();

  void requestRestart(int index);

protected:
  void run();

private:
  std::shared_ptr<rclcpp::Node> node;

  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr int_sub_;
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr bool_sub_;
  rclcpp::Subscription<std_msgs::msg::Char>::SharedPtr char_sub_;
  rclcpp::Client<std_srvs::srv::Trigger>::SharedPtr restart_clients_[3];

Q_SIGNALS:
  void rosShutDown();
  void intReceived(int value);
  void boolReceived(bool value);
  void charReceived(QString value);
  void serviceResponded(QString message);
};

#endif
