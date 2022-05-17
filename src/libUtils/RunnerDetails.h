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
#ifndef ZILLIQA_RUNNERDETAILS_H
#define ZILLIQA_RUNNERDETAILS_H

#include <string>

struct RunnerDetails {
  std::string   m_from;
  std::string   m_to;
  std::string   m_code;
  std::string   m_data;
};

#endif  // ZILLIQA_RUNNERDETAILS_H
