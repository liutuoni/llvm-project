//===-- DynamicRegisterInfo.h -----------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_TARGET_DYNAMICREGISTERINFO_H
#define LLDB_TARGET_DYNAMICREGISTERINFO_H

#include <map>
#include <vector>

#include "lldb/Utility/ConstString.h"
#include "lldb/Utility/StructuredData.h"
#include "lldb/lldb-private.h"

namespace lldb_private {

class DynamicRegisterInfo {
protected:
  DynamicRegisterInfo(DynamicRegisterInfo &) = default;
  DynamicRegisterInfo &operator=(DynamicRegisterInfo &) = default;

public:
  DynamicRegisterInfo() = default;

  DynamicRegisterInfo(const lldb_private::StructuredData::Dictionary &dict,
                      const lldb_private::ArchSpec &arch);

  virtual ~DynamicRegisterInfo() = default;

  DynamicRegisterInfo(DynamicRegisterInfo &&info);
  DynamicRegisterInfo &operator=(DynamicRegisterInfo &&info);

  size_t SetRegisterInfo(const lldb_private::StructuredData::Dictionary &dict,
                         const lldb_private::ArchSpec &arch);

  void AddRegister(lldb_private::RegisterInfo reg_info,
                   lldb_private::ConstString &set_name);

  // Add a new register and cross-link it via invalidate_regs with other
  // registers sharing its value_regs.
  void AddSupplementaryRegister(lldb_private::RegisterInfo reg_info,
                                lldb_private::ConstString &set_name);

  void Finalize(const lldb_private::ArchSpec &arch);

  size_t GetNumRegisters() const;

  size_t GetNumRegisterSets() const;

  size_t GetRegisterDataByteSize() const;

  const lldb_private::RegisterInfo *GetRegisterInfoAtIndex(uint32_t i) const;

  lldb_private::RegisterInfo *GetRegisterInfoAtIndex(uint32_t i);

  const lldb_private::RegisterSet *GetRegisterSet(uint32_t i) const;

  uint32_t GetRegisterSetIndexByName(lldb_private::ConstString &set_name,
                                     bool can_create);

  uint32_t ConvertRegisterKindToRegisterNumber(uint32_t kind,
                                               uint32_t num) const;

  const lldb_private::RegisterInfo *GetRegisterInfo(uint32_t kind,
                                                    uint32_t num) const;

  void Dump() const;

  void Clear();

  bool IsReconfigurable();

  const lldb_private::RegisterInfo *
  GetRegisterInfo(llvm::StringRef reg_name) const;

  typedef std::vector<lldb_private::RegisterInfo> reg_collection;
  llvm::iterator_range<reg_collection::const_iterator> registers() const {
    return llvm::iterator_range<reg_collection::const_iterator>(m_regs);
  }

protected:
  // Classes that inherit from DynamicRegisterInfo can see and modify these
  typedef std::vector<lldb_private::RegisterSet> set_collection;
  typedef std::vector<uint32_t> reg_num_collection;
  typedef std::vector<reg_num_collection> set_reg_num_collection;
  typedef std::vector<lldb_private::ConstString> name_collection;
  typedef std::map<uint32_t, reg_num_collection> reg_to_regs_map;

  llvm::Expected<uint32_t> ByteOffsetFromSlice(uint32_t index,
                                               llvm::StringRef slice_str,
                                               lldb::ByteOrder byte_order);
  llvm::Expected<uint32_t> ByteOffsetFromComposite(
      uint32_t index, lldb_private::StructuredData::Array &composite_reg_list,
      lldb::ByteOrder byte_order);
  llvm::Expected<uint32_t> ByteOffsetFromRegInfoDict(
      uint32_t index, lldb_private::StructuredData::Dictionary &reg_info_dict,
      lldb::ByteOrder byte_order);

  void MoveFrom(DynamicRegisterInfo &&info);

  void ConfigureOffsets();

  reg_collection m_regs;
  set_collection m_sets;
  set_reg_num_collection m_set_reg_nums;
  name_collection m_set_names;
  reg_to_regs_map m_value_regs_map;
  reg_to_regs_map m_invalidate_regs_map;
  size_t m_reg_data_byte_size = 0u; // The number of bytes required to store
                                    // all registers
  bool m_finalized = false;
  bool m_is_reconfigurable = false;
};

} // namespace lldb_private

#endif // LLDB_TARGET_DYNAMICREGISTERINFO_H
