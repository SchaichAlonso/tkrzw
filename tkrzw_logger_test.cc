/*************************************************************************************************
 * Tests for tkrzw_logger.h
 *
 * Copyright 2020 Google LLC
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file
 * except in compliance with the License.  You may obtain a copy of the License at
 *     https://www.apache.org/licenses/LICENSE-2.0
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
 * either express or implied.  See the License for the specific language governing permissions
 * and limitations under the License.
 *************************************************************************************************/

#include "tkrzw_sys_config.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "tkrzw_lib_common.h"
#include "tkrzw_logger.h"
#include "tkrzw_str_util.h"

using namespace testing;

// Main routine
int main(int argc, char** argv) {
  InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

TEST(LoggerTest, Basic) {
  std::stringstream out1, out2;
  tkrzw::StreamLogger logger(&out1);
  logger.Log(tkrzw::Logger::DEBUG, "debug");
  logger.Log(tkrzw::Logger::INFO, "info");
  logger.LogF(tkrzw::Logger::WARN, "warn: %d", 123);
  logger.LogF(tkrzw::Logger::ERROR, "error: %s", "hello");
  logger.LogCat(tkrzw::Logger::FATAL, "fatal: ", "bye");
  EXPECT_EQ(std::string::npos, out1.str().find(" [DEBUG] debug"));
  EXPECT_NE(std::string::npos, out1.str().find(" [INFO] info"));
  EXPECT_NE(std::string::npos, out1.str().find(" [WARN] warn: 123"));
  EXPECT_NE(std::string::npos, out1.str().find(" [ERROR] error: hello"));
  EXPECT_NE(std::string::npos, out1.str().find(" [FATAL] fatal: bye"));
  logger.SetStream(&out2);
  logger.SetMinLevel(tkrzw::Logger::ERROR);
  logger.SetSeparator(" | ");
  logger.SetDateFormat(tkrzw::BaseLogger::DATE_W3CDTF_MICRO, 0);
  logger.Log(tkrzw::Logger::DEBUG, "debug");
  logger.Log(tkrzw::Logger::INFO, "info");
  logger.LogF(tkrzw::Logger::WARN, "warn: %d", 123);
  logger.LogF(tkrzw::Logger::ERROR, "error: %s", "hello");
  logger.LogCat(tkrzw::Logger::FATAL, "fatal: ", "bye");
  EXPECT_EQ(std::string::npos, out2.str().find("Z | [DEBUG] | debug"));
  EXPECT_EQ(std::string::npos, out2.str().find("Z | [INFO] | info"));
  EXPECT_EQ(std::string::npos, out2.str().find("Z | [WARN] | warn: 123"));
  EXPECT_NE(std::string::npos, out2.str().find("Z | [ERROR] | error: hello"));
  EXPECT_NE(std::string::npos, out2.str().find("Z | [FATAL] | fatal: bye"));
  out1.str("");
  out2.str("");
  logger.SetStream(nullptr);
  logger.SetMinLevel(tkrzw::Logger::DEBUG);
  logger.Log(tkrzw::Logger::ERROR, "error");
  EXPECT_TRUE(out1.str().empty());
  EXPECT_TRUE(out2.str().empty());
  tkrzw::StreamLogger logger2(&out1, tkrzw::Logger::NONE, "|",
                              tkrzw::BaseLogger::DATE_NONE, 0);
  logger.LogCat(tkrzw::Logger::FATAL, "hop", "step");
  EXPECT_TRUE(out1.str().empty());
  logger2.SetMinLevel(tkrzw::Logger::FATAL);
  logger2.LogCat(tkrzw::Logger::FATAL, "hop", "step");
  EXPECT_EQ("[FATAL]|hopstep", tkrzw::StrStripLine(out1.str()));
}

TEST(LoggerTest, ParseLevelStr) {
  EXPECT_EQ(tkrzw::Logger::NONE, tkrzw::Logger::ParseLevelStr(""));
  EXPECT_EQ(tkrzw::Logger::DEBUG, tkrzw::Logger::ParseLevelStr("debug"));
  EXPECT_EQ(tkrzw::Logger::INFO, tkrzw::Logger::ParseLevelStr("INFO"));
  EXPECT_EQ(tkrzw::Logger::WARN, tkrzw::Logger::ParseLevelStr("Warn"));
  EXPECT_EQ(tkrzw::Logger::ERROR, tkrzw::Logger::ParseLevelStr("error"));
  EXPECT_EQ(tkrzw::Logger::FATAL, tkrzw::Logger::ParseLevelStr("FATAL"));
}

TEST(LoggerTest, ParseDateFormatStr) {
  EXPECT_EQ(tkrzw::BaseLogger::DATE_NONE, tkrzw::BaseLogger::ParseDateFormatStr(""));
  EXPECT_EQ(tkrzw::BaseLogger::DATE_SIMPLE,
            tkrzw::BaseLogger::ParseDateFormatStr("date_simple"));
  EXPECT_EQ(tkrzw::BaseLogger::DATE_SIMPLE_MICRO,
            tkrzw::BaseLogger::ParseDateFormatStr("simple_micro"));
  EXPECT_EQ(tkrzw::BaseLogger::DATE_W3CDTF,
            tkrzw::BaseLogger::ParseDateFormatStr("DATE_W3CDTF"));
  EXPECT_EQ(tkrzw::BaseLogger::DATE_W3CDTF_MICRO,
            tkrzw::BaseLogger::ParseDateFormatStr("W3CDTF_MICRO"));
  EXPECT_EQ(tkrzw::BaseLogger::DATE_RFC1123,
            tkrzw::BaseLogger::ParseDateFormatStr("RFC1123"));
  EXPECT_EQ(tkrzw::BaseLogger::DATE_EPOCH,
            tkrzw::BaseLogger::ParseDateFormatStr("epoch"));
  EXPECT_EQ(tkrzw::BaseLogger::DATE_EPOCH_MICRO,
            tkrzw::BaseLogger::ParseDateFormatStr("DATE_epoch_MICRO"));
}

// END OF FILE