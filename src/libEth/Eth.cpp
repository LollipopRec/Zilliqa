#include "jsonrpccpp/server.h"

using namespace jsonrpc;

Json::Value populateReceiptHelper(std::string const& txnhash) {

  Json::Value ret;

  ret["transactionHash"] = txnhash;
  ret["blockHash"] = "0x0000000000000000000000000000000000000000000000000000000000000000";
  ret["blockNumber"] = "0x429d3b";
  ret["contractAddress"] = nullptr;
  ret["cumulativeGasUsed"] = "0x64b559";
  ret["from"] = "0x999"; // todo: fill
  ret["gasUsed"] = "0xcaac";
  ret["logs"].append(Json::Value());
  ret["logsBloom"] = "0x00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
  ret["root"] = "0x0000000000000000000000000000000000000000000000000000000000001010";
  ret["status"] = nullptr;
  ret["to"] = "0x888"; // todo: fill
  ret["transactionIndex"] = "0x777"; // todo: fill

  return ret;
}
