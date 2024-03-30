#pragma once

#include <stdafx.hpp>

namespace dot::arc
{
    constexpr unsigned int THUMB_FORMAT_COUNT = 19;
    constexpr std::array<unsigned short, THUMB_FORMAT_COUNT> THUMB_INSTRUCTION_FORMAT
    {
        0x0000,                                                                //MoveShiftedRegister
        0x1800,                                                                //AddSubtract
        0x2000,                                                                //MoveCompareAddSubtractImmediate
        0x4000,                                                                //ALUOperations,
        0x4400,                                                                //HiRegisterOperationsBranchExchange,
        0x4800,                                                                //LoadPCRelative
        0x5000,                                                                //LoadStoreRegisterOffset,
        0x5200,                                                                //LoadStoreSignExtended,
        0x6000,                                                                //LoadStoreImmediateOffset
        0x8000,                                                                //LoadStoreHalfword
        0x9000,                                                                //LoadStoreSPRelative
        0xA000,                                                                //LoadAddress,
        0xB000,                                                                //AddOffsetToStackPointer
        0xB400,                                                                //PushPopRegisters
        0xC000,                                                                //MultipleLoadStore
        0xD000,                                                                //ConditionalBranch
        0xDF00,                                                                //SoftwareInterrupt
        0xE000,                                                                //UnconditionalBranch
        0xF000,                                                                //LongBranchWithLink
    };
    constexpr std::array<unsigned short, THUMB_FORMAT_COUNT> THUMB_INSTRUCTION_MASK
    {
        0xE000,                                                                //MoveShiftedRegister
        0xF800,                                                                //AddSubtract
        0xE000,                                                                //MoveCompareAddSubtractImmediate
        0xFC00,                                                                //ALUOperations,
        0xFC00,                                                                //HiRegisterOperationsBranchExchange,
        0xF800,                                                                //LoadPCRelative
        0xF200,                                                                //LoadStoreRegisterOffset,
        0xF200,                                                                //LoadStoreSignExtended,
        0xE000,                                                                //LoadStoreImmediateOffset
        0xF000,                                                                //LoadStoreHalfword
        0xF000,                                                                //LoadStoreSPRelative
        0xF000,                                                                //LoadAddress,
        0xFF00,                                                                //AddOffsetToStackPointer
        0xF600,                                                                //PushPopRegisters
        0xF000,                                                                //MultipleLoadStore
        0xF000,                                                                //ConditionalBranch
        0xFF00,                                                                //SoftwareInterrupt
        0xF800,                                                                //UnconditionalBranch
        0xF000,                                                                //LongBranchWithLink
    };
}
