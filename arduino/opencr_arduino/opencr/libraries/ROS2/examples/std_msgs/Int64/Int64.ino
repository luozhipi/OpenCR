#include <ros2.hpp>

#include "std_msgs/Int64.hpp"


#define DEBUG_SERIAL Serial2  
#define RTPS_SERIAL  Serial   //OpenCR USB

void on_topic(ObjectId id, MicroBuffer* serialized_topic, void* args);
static bool is_get_Int64_topic = false;





class Int64PubSub : public ros2::Node
{
public:
  Int64PubSub()
  : Node()
  {
    publisher_ = this->createPublisher<std_msgs::Int64>("Int64");
    publisher_->setPublishInterval(2); // 2 hz
    subscriber_ = this->createSubscriber<std_msgs::Int64>("Int64");
    subscriber_->subscribe(STREAMID_BUILTIN_RELIABLE);
  }

private:  
  void callback()
  {
    if(publisher_->isTimeToPublish())
    {
      callbackInt64Pub();
    }

    if(is_get_Int64_topic)
    {
      subscriber_->subscribe(STREAMID_BUILTIN_RELIABLE);
      is_get_Int64_topic = false;
    }
  }

  void callbackInt64Pub(void)
  {
    std_msgs::Int64 int64_topic;
    int64_topic.data = 1234567891011121314;//(int64_t)millis();

    publisher_->publish(&int64_topic, STREAMID_BUILTIN_RELIABLE);
  }

  ros2::Publisher<std_msgs::Int64>* publisher_;
  ros2::Subscriber<std_msgs::Int64>* subscriber_;
};



void setup() 
{
  DEBUG_SERIAL.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  
  while (!RTPS_SERIAL);

  ros2::init(on_topic);
}

void loop() 
{
  static uint32_t pre_time = millis();
  static bool led_state = false;
  static Int64PubSub Int64Node;

  if(millis() - pre_time > 500)
  {
    pre_time = millis();

    digitalWrite(LED_BUILTIN, led_state);
    led_state = !led_state;
  }

  ros2::spin(&Int64Node);
}


void on_topic(ObjectId id, MicroBuffer* serialized_topic, void* args)
{
  ((void)(args));

  switch(id.data[0])
  {
    case STD_MSGS_INT64_TOPIC:
    {
      std_msgs::Int64 topic;

      topic.deserialize(serialized_topic, &topic);
      DEBUG_SERIAL.println();
      DEBUG_SERIAL.print(" Int64: ");
      DEBUG_SERIAL.print((int32_t)(topic.data >> 32));
      DEBUG_SERIAL.println((int32_t)(topic.data));

      is_get_Int64_topic = true;

      break;
    }

    default:
      break;
  }
}