/*
 * Copyright (C) 2019 Zilliqa
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

#ifndef ZILLIQA_SRC_LIBDATA_ACCOUNTDATA_ACCOUNTSTORETRIE_H_
#define ZILLIQA_SRC_LIBDATA_ACCOUNTDATA_ACCOUNTSTORETRIE_H_

#include "AccountStoreSC.h"
#include "depends/libDatabase/MemoryDB.h"
#include "depends/libDatabase/OverlayDB.h"

template <class DB, class MAP>
class AccountStoreTrie : public AccountStoreSC<MAP> {
 protected:
  DB m_db;
  dev::SpecificTrieDB<dev::GenericTrieDB<DB>, Address> m_state;
  dev::h256 m_prevRoot;

  // mutex for AccountStore DB related operations
  std::mutex m_mutexDB;
  mutable std::mutex m_mutexTrie;

  AccountStoreTrie();

  bool UpdateStateTrie(const Address& address, const Account& account);
  bool RemoveFromTrie(const Address& address);

 public:
  virtual void Init() override;

  void InitTrie();

  bool Serialize(bytes& dst, unsigned int offset) const override;

  Account* GetAccount(const Address& address) override;

  dev::h256 GetStateRootHash() const;
  dev::h256 GetPrevRootHash() const;
  bool UpdateStateTrieAll();

  void PrintAccountState() override;

  void PrintTrie();
};

#include "AccountStoreTrie.tpp"

#endif  // ZILLIQA_SRC_LIBDATA_ACCOUNTDATA_ACCOUNTSTORETRIE_H_
