/*
 * Copyright (C) 2022 Zilliqa
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <mutex>
#include <string>
#include <unordered_map>

#include <boost/filesystem.hpp>
#include "EvmUtils.h"

#include "JsonUtils.h"
#include "Logger.h"
#include "common/Constants.h"
#include "libUtils/EvmCallParameters.h"

using namespace std;
using namespace boost::multiprecision;

bool EvmUtils::PrepareRootPathWVersion(const uint32_t& evm_version,
                                       string& root_w_version) {
  root_w_version = EVM_ROOT;
  if (ENABLE_EVM_MULTI_VERSION) {
    root_w_version += '/' + to_string(evm_version);
  }

  if (!boost::filesystem::exists(root_w_version)) {
    LOG_GENERAL(WARNING, "Folder for desired version (" << root_w_version
                                                        << ") doesn't exists");
    return false;
  }

  return true;
}

std::string EvmUtils::GetDataFromItemData(const std::string& itemData) {
  Json::Value root;
  Json::Reader reader;
  std::string reply;
  try {
    if (reader.parse(itemData, root)) {
      std::string testString = root[0]["vname"].asString();
      if (testString != "_evm_version") {
        LOG_GENERAL(WARNING,
                    "Init Parameter does not appear to be formatted correctly "
                        << testString);
      }
      reply = root[1]["data"].asString();
    }
  } catch (const std::exception& e) {
    LOG_GENERAL(WARNING,
                "Exception caught: " << e.what() << " itemData: " << itemData);
  }
  return reply;
}

Json::Value EvmUtils::GetCreateContractJson(EvmCallParameters& details) {
  Json::Value arr_ret(Json::arrayValue);

  arr_ret.append(details.m_from);
  arr_ret.append(details.m_to);
  // The next two parameters come directly from the user in the code and init
  // struct
  //
  arr_ret.append(details.m_code);
  arr_ret.append(GetDataFromItemData(details.m_data));
  arr_ret.append("00");
  arr_ret.append(Json::Value::UInt64(details.m_available_gas));

  details.m_data = GetDataFromItemData(details.m_data);

  return arr_ret;
}

Json::Value EvmUtils::GetCallContractJson(const EvmCallParameters& details) {
  Json::Value arr_ret(Json::arrayValue);

  arr_ret.append(details.m_from);
  arr_ret.append(details.m_to);
  arr_ret.append(details.m_code);
  arr_ret.append(details.m_data);
  arr_ret.append("00");
  arr_ret.append(Json::Value::UInt64(details.m_available_gas));

  return arr_ret;
}
