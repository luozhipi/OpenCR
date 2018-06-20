// Copyright 2016 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/*! 
 * @file Combination.h
 * This header file contains the declaration of the described types in the IDL file.
 *
 * This file was generated by the tool gen.
 */

#ifndef _TEST_MSGS_COMBINATION_HPP_
#define _TEST_MSGS_COMBINATION_HPP_


#include "micrortps.hpp"
#include <topic_config.h>
#include <topic.hpp>

#include "std_msgs/Bool.hpp"
#include "std_msgs/String.hpp"


namespace test_msgs {

class Combination : public ros2::Topic<Combination>
{
public:
  std_msgs::String message;
  std_msgs::Bool   value;

  Combination():
    Topic("Combination", TEST_MSGS_COMBINATION_TOPIC),
    message(),
    value()
  { 
  };

  virtual bool serialize(MicroBuffer* writer, const Combination* topic)
  {
      uint8_t result;

      message.serialize(writer, &topic->message);
      result = writer->error;
      value.serialize(writer, &topic->value);
      result |= writer->error;
      return result == BUFFER_OK;
  }

  virtual bool deserialize(MicroBuffer* reader, Combination* topic)
  {
      uint8_t result;

      message.deserialize(reader, &topic->message);
      result = reader->error;
      value.deserialize(reader, &topic->value);
      result |= reader->error;
      return result == BUFFER_OK;
  }


  virtual bool write(Session* session, ObjectId datawriter_id, StreamId stream_id, Combination* topic)
  {
      bool result = false;

      result = message.write(session, datawriter_id, stream_id, &topic->message);
      result &= value.write(session, datawriter_id, stream_id, &topic->value);

      return result;
  }

};

} // namespace test_msgs


#endif // _TEST_MSGS_COMBINATION_HPP_