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
#include "libData/AccountData/Account.h"
#include "libData/AccountData/TransactionReceipt.h"
#include "libPersistence/ContractStorage.h"
#include "libUtils/EvmCallParameters.h"
#include "libUtils/EvmJsonResponse.h"

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

Json::Value EvmUtils::GetCreateContractJson(EvmCallParameters& params) {
  Json::Value arr_ret(Json::arrayValue);

  arr_ret.append(params.m_owner);
  arr_ret.append(params.m_contract);
  // The next two parameters come directly from the user in the code and init
  // struct
  //
  arr_ret.append(params.m_code);
  arr_ret.append(GetDataFromItemData(params.m_data));
  arr_ret.append("00");
  arr_ret.append(Json::Value::UInt64(params.m_available_gas));

  params.m_data = GetDataFromItemData(params.m_data);

  return arr_ret;
}

Json::Value EvmUtils::GetCallContractJson(const EvmCallParameters& params) {
  Json::Value arr_ret(Json::arrayValue);

  arr_ret.append(params.m_owner);
  arr_ret.append(params.m_contract);
  arr_ret.append(params.m_code);
  arr_ret.append(params.m_data);
  arr_ret.append("00");
  arr_ret.append(Json::Value::UInt64(params.m_available_gas));

  return arr_ret;
}

bool EvmUtils::EvmUpdateContractStateAndAccount(
    Account* contractAccount, evmproj::ApplyInstructions& op) {
  if (op.OperationType() == "modify") {
    if (op.isResetStorage()) {
      contractAccount->SetStorageRoot(dev::h256());
    }

    /* useful for debug
    std::map<std::string, bytes> myMap;
    std::set<std::string> myIndices;
    Contract::ContractStorage::GetContractStorage().FetchUpdatedStateValuesForAddress(Address(op.Address()),myMap,myIndices,false);

    std::cout << "map for address " << op.Address() << " has " << myMap.size()
    << " entries " << std::endl; for (const auto& iter:myMap){ std::cout << "key
    " << iter.first << " value " <<
    DataConversion::CharArrayToString(iter.second) << endl;

    }

    myMap.clear();

    Contract::ContractStorage::GetContractStorage().FetchStateDataForContract(myMap,
                                   Address(op.Address()));

    std::cout << "map for address " << op.Address() << " has " << myMap.size()
    << " entries " << std::endl; for (const auto& iter:myMap){ std::cout << "key
    " << iter.first << " value " <<
    DataConversion::CharArrayToString(iter.second) << endl;

    }
    */

    if (op.Code().size() > 0)
      contractAccount->SetCode(DataConversion::StringToCharArray(op.Code()));

    for (const auto& it : op.Storage()) {
      if (!Contract::ContractStorage::GetContractStorage().UpdateStateValue(
              Address(op.Address()),
              DataConversion::StringToCharArray(it.Key()), 0,
              DataConversion::StringToCharArray(it.Value()), 0)) {
        return false;
      }
    }

    if (op.Balance().size()) {
      contractAccount->SetBalance(uint128_t(op.Balance()));
    }

    if (op.Nonce().size()) {
      contractAccount->SetNonce(std::stoull(op.Nonce()));
    }

  } else if (op.OperationType() == "delete") {
    // TODO process deletion of account
  }
  return true;
}

uint64_t EvmUtils::UpdateGasRemaining(TransactionReceipt& receipt,
                                      INVOKE_TYPE invoke_type,
                                      uint64_t& oldValue, uint64_t newValue) {
  uint64_t cost{0};

  if (newValue > 0) oldValue = std::min(oldValue, newValue);

  // Create has already been charged before we were invoked.
  if (invoke_type == RUNNER_CREATE) return oldValue;

  cost = CONTRACT_INVOKE_GAS;

  if (oldValue > cost) {
    oldValue -= cost;
  } else {
    oldValue = 0;
    receipt.AddError(NO_GAS_REMAINING_FOUND);
  }
  LOG_GENERAL(INFO, "gasRemained: " << oldValue);

  return oldValue;
}