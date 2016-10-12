/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2000, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 *******************************************************************************/

#include <limits.h>
#include <float.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include "compile/Compilation.hpp"
#include "compile/CompilationTypes.hpp"
#include "compile/Method.hpp"
#include "env/jittypes.h"
#include "gtest/gtest.h"
#include "il/DataTypes.hpp"
#include "ilgen/BinaryOpIlInjector.hpp"
#include "ilgen/ChildlessUnaryOpIlInjector.hpp"
#include "ilgen/CmpBranchOpIlInjector.hpp"
#include "ilgen/IlGeneratorMethodDetails_inlines.hpp"
#include "ilgen/StoreOpIlInjector.hpp"
#include "ilgen/TernaryOpIlInjector.hpp"
#include "ilgen/TypeDictionary.hpp"
#include "ilgen/UnaryOpIlInjector.hpp"
#include "tests/CallIlInjector.hpp"
#include "tests/IndirectLoadIlInjector.hpp"
#include "tests/IndirectStoreIlInjector.hpp"

#include "tests/OpCodesTest.hpp"

extern "C" uint8_t *compileMethod(TR::IlGeneratorMethodDetails &, TR_Hotness, int32_t &);

namespace TestCompiler
{
//common variables definitions
const int64_t OpCodesTest::LONG_NEG = -9;
const int64_t OpCodesTest::LONG_POS = 9;
const int64_t OpCodesTest::LONG_MAXIMUM = LONG_MAX;
const int64_t OpCodesTest::LONG_MINIMUM = LONG_MIN;
const int64_t OpCodesTest::LONG_ZERO = 0;

const int32_t OpCodesTest::INT_NEG = -9;
const int32_t OpCodesTest::INT_POS = 9;
const int32_t OpCodesTest::INT_MAXIMUM = INT_MAX;
const int32_t OpCodesTest::INT_MINIMUM = INT_MIN;
const int32_t OpCodesTest::INT_ZERO = 0;

const int16_t OpCodesTest::SHORT_NEG = -9;
const int16_t OpCodesTest::SHORT_POS = 9;
const int16_t OpCodesTest::SHORT_MAXIMUM = SHRT_MAX;
const int16_t OpCodesTest::SHORT_MINIMUM = SHRT_MIN;
const int16_t OpCodesTest::SHORT_ZERO = 0;

const int8_t OpCodesTest::BYTE_NEG = -9;
const int8_t OpCodesTest::BYTE_POS = 9;
const int8_t OpCodesTest::BYTE_MAXIMUM = CHAR_MAX;
const int8_t OpCodesTest::BYTE_MINIMUM = CHAR_MIN;
const int8_t OpCodesTest::BYTE_ZERO = 0;

const double OpCodesTest::DOUBLE_MINIMUM = - DBL_MAX;
const double OpCodesTest::DOUBLE_MAXIMUM = DBL_MAX;
const double OpCodesTest::DOUBLE_POS = 9.34;
const double OpCodesTest::DOUBLE_NEG = -9.34;
const double OpCodesTest::DOUBLE_ZERO = 0;

const float OpCodesTest::FLOAT_MINIMUM = - FLT_MAX;
const float OpCodesTest::FLOAT_MAXIMUM = FLT_MAX;
const float OpCodesTest::FLOAT_POS = 9.31;
const float OpCodesTest::FLOAT_NEG = -9.31;
const float OpCodesTest::FLOAT_ZERO = 0;

const uint8_t OpCodesTest::UBYTE_POS = 9;
const uint8_t OpCodesTest::UBYTE_MAXIMUM = UCHAR_MAX;
const uint8_t OpCodesTest::UBYTE_MINIMUM = 0;

const uint16_t OpCodesTest::USHORT_POS = 9;
const uint16_t OpCodesTest::USHORT_MAXIMUM = USHRT_MAX;
const uint16_t OpCodesTest::USHORT_MINIMUM = 0;

const uint32_t OpCodesTest::UINT_POS = 9;
const uint32_t OpCodesTest::UINT_MAXIMUM = UINT_MAX;
const uint32_t OpCodesTest::UINT_MINIMUM = 0;

const uint64_t OpCodesTest::ULONG_POS = 9;
const uint64_t OpCodesTest::ULONG_MAXIMUM = ULONG_MAX;
const uint64_t OpCodesTest::ULONG_MINIMUM = 0;

const int8_t OpCodesTest::BYTE_PLACEHOLDER_1 = 50;
const int8_t OpCodesTest::BYTE_PLACEHOLDER_2 = 70;
const int8_t OpCodesTest::BYTE_PLACEHOLDER_3 = 80;

const int16_t OpCodesTest::SHORT_PLACEHOLDER_1 = 100;
const int16_t OpCodesTest::SHORT_PLACEHOLDER_2 = 200;
const int16_t OpCodesTest::SHORT_PLACEHOLDER_3 = 300;

const int32_t OpCodesTest::INT_PLACEHOLDER_1 = 100;
const int32_t OpCodesTest::INT_PLACEHOLDER_2 = 200;
const int32_t OpCodesTest::INT_PLACEHOLDER_3 = 300;

const int64_t OpCodesTest::LONG_PLACEHOLDER_1 = 100;
const int64_t OpCodesTest::LONG_PLACEHOLDER_2 = 200;
const int64_t OpCodesTest::LONG_PLACEHOLDER_3 = 300;

const float OpCodesTest::FLOAT_PLACEHOLDER_1 = 100.11;
const float OpCodesTest::FLOAT_PLACEHOLDER_2 = 200.12;
const float OpCodesTest::FLOAT_PLACEHOLDER_3 = 300.13;

const double OpCodesTest::DOUBLE_PLACEHOLDER_1 = 100.21;
const double OpCodesTest::DOUBLE_PLACEHOLDER_2 = 200.22;
const double OpCodesTest::DOUBLE_PLACEHOLDER_3 = 300.23;

const uintptrj_t OpCodesTest::ADDRESS_PLACEHOLDER_1 = (uintptrj_t)100;
const uintptrj_t OpCodesTest::ADDRESS_PLACEHOLDER_2 = (uintptrj_t)200;
const uintptrj_t OpCodesTest::ADDRESS_PLACEHOLDER_3 = (uintptrj_t)300;

TR::DataType OpCodesTest::_argTypesUnaryByte[_numberOfUnaryArgs] = {TR::Int8};
TR::DataType OpCodesTest::_argTypesUnaryShort[_numberOfUnaryArgs] = {TR::Int16};
TR::DataType OpCodesTest::_argTypesUnaryInt[_numberOfUnaryArgs] = {TR::Int32};
TR::DataType OpCodesTest::_argTypesUnaryLong[_numberOfUnaryArgs] = {TR::Int64};
TR::DataType OpCodesTest::_argTypesUnaryFloat[_numberOfUnaryArgs] = {TR::Float};
TR::DataType OpCodesTest::_argTypesUnaryDouble[_numberOfUnaryArgs] = {TR::Double};
TR::DataType OpCodesTest::_argTypesUnaryAddress[_numberOfUnaryArgs] = {TR::Address};

TR::DataType OpCodesTest::_argTypesBinaryByte[_numberOfBinaryArgs] = {TR::Int8, TR::Int8};
TR::DataType OpCodesTest::_argTypesBinaryShort[_numberOfBinaryArgs] = {TR::Int16, TR::Int16};
TR::DataType OpCodesTest::_argTypesBinaryInt[_numberOfBinaryArgs] = {TR::Int32, TR::Int32};
TR::DataType OpCodesTest::_argTypesBinaryLong[_numberOfBinaryArgs] = {TR::Int64, TR::Int64};
TR::DataType OpCodesTest::_argTypesBinaryFloat[_numberOfBinaryArgs] = {TR::Float, TR::Float};
TR::DataType OpCodesTest::_argTypesBinaryDouble[_numberOfBinaryArgs] = {TR::Double, TR::Double};
TR::DataType OpCodesTest::_argTypesBinaryAddress[_numberOfBinaryArgs] = {TR::Address, TR::Address};

TR::DataType OpCodesTest::_argTypesTernaryByte[_numberOfTernaryArgs] = {TR::Int32,TR::Int8, TR::Int8};
TR::DataType OpCodesTest::_argTypesTernaryShort[_numberOfTernaryArgs] = {TR::Int32,TR::Int16, TR::Int16};
TR::DataType OpCodesTest::_argTypesTernaryInt[_numberOfTernaryArgs] = {TR::Int32,TR::Int32, TR::Int32};
TR::DataType OpCodesTest::_argTypesTernaryLong[_numberOfTernaryArgs] = {TR::Int32,TR::Int64, TR::Int64};
TR::DataType OpCodesTest::_argTypesTernaryFloat[_numberOfTernaryArgs] = {TR::Int32,TR::Float, TR::Float};
TR::DataType OpCodesTest::_argTypesTernaryDouble[_numberOfTernaryArgs] = {TR::Int32,TR::Double, TR::Double};
TR::DataType OpCodesTest::_argTypesTernaryAddress[_numberOfTernaryArgs] = {TR::Int32,TR::Address, TR::Address};

TR::DataType OpCodesTest::_argTypesBinaryAddressByte[_numberOfBinaryArgs] = {TR::Address, TR::Int32};
TR::DataType OpCodesTest::_argTypesBinaryAddressShort[_numberOfBinaryArgs] = {TR::Address,TR::Int16};
TR::DataType OpCodesTest::_argTypesBinaryAddressInt[_numberOfBinaryArgs] = {TR::Address, TR::Int32};
TR::DataType OpCodesTest::_argTypesBinaryAddressLong[_numberOfBinaryArgs] = {TR::Address, TR::Int64};
TR::DataType OpCodesTest::_argTypesBinaryAddressFloat[_numberOfBinaryArgs] = {TR::Address, TR::Float};
TR::DataType OpCodesTest::_argTypesBinaryAddressDouble[_numberOfBinaryArgs] = {TR::Address, TR::Double};
TR::DataType OpCodesTest::_argTypesBinaryAddressAddress[_numberOfBinaryArgs] = {TR::Address, TR::Address};

//Neg
signatureCharB_B_testMethodType  * OpCodesTest::_bNeg = 0;
signatureCharS_S_testMethodType  * OpCodesTest::_sNeg = 0;
signatureCharI_I_testMethodType  * OpCodesTest::_iNeg = 0;
signatureCharJ_J_testMethodType  * OpCodesTest::_lNeg = 0;
signatureCharD_D_testMethodType  * OpCodesTest::_dNeg = 0;
signatureCharF_F_testMethodType  * OpCodesTest::_fNeg = 0;

//Convert Group
signatureCharI_J_testMethodType  * OpCodesTest::_i2l = 0;
signatureCharI_F_testMethodType  * OpCodesTest::_i2f = 0;
signatureCharI_D_testMethodType  * OpCodesTest::_i2d = 0;
signatureCharI_B_testMethodType  * OpCodesTest::_i2b = 0;
signatureCharI_S_testMethodType  * OpCodesTest::_i2s = 0;
unsignedSignatureCharI_J_testMethodType  * OpCodesTest::_iu2l = 0;
unsignedSignatureCharI_F_testMethodType  * OpCodesTest::_iu2f = 0;
unsignedSignatureCharI_D_testMethodType  * OpCodesTest::_iu2d = 0;

signatureCharJ_I_testMethodType  * OpCodesTest::_l2i = 0;
signatureCharJ_F_testMethodType  * OpCodesTest::_l2f = 0;
signatureCharJ_D_testMethodType  * OpCodesTest::_l2d = 0;
signatureCharJ_B_testMethodType  * OpCodesTest::_l2b = 0;
signatureCharJ_S_testMethodType  * OpCodesTest::_l2s = 0;
unsignedSignatureCharJ_F_testMethodType  * OpCodesTest::_lu2f = 0;
unsignedSignatureCharJ_D_testMethodType  * OpCodesTest::_lu2d = 0;

signatureCharF_I_testMethodType  * OpCodesTest::_f2i = 0;
signatureCharF_J_testMethodType  * OpCodesTest::_f2l = 0;
signatureCharF_D_testMethodType  * OpCodesTest::_f2d = 0;
signatureCharF_B_testMethodType  * OpCodesTest::_f2b = 0;
signatureCharF_S_testMethodType  * OpCodesTest::_f2s = 0;

signatureCharD_I_testMethodType  * OpCodesTest::_d2i = 0;
signatureCharD_J_testMethodType  * OpCodesTest::_d2l = 0;
signatureCharD_F_testMethodType  * OpCodesTest::_d2f = 0;
signatureCharD_B_testMethodType  * OpCodesTest::_d2b = 0;
signatureCharD_S_testMethodType  * OpCodesTest::_d2s = 0;

signatureCharS_I_testMethodType  * OpCodesTest::_s2i = 0;
signatureCharS_J_testMethodType  * OpCodesTest::_s2l = 0;
signatureCharS_F_testMethodType  * OpCodesTest::_s2f = 0;
signatureCharS_D_testMethodType  * OpCodesTest::_s2d = 0;
signatureCharS_B_testMethodType  * OpCodesTest::_s2b = 0;
unsignedSignatureCharS_I_testMethodType  * OpCodesTest::_su2i = 0;
unsignedSignatureCharS_J_testMethodType  * OpCodesTest::_su2l = 0;
unsignedSignatureCharS_F_testMethodType  * OpCodesTest::_su2f = 0;
unsignedSignatureCharS_D_testMethodType  * OpCodesTest::_su2d = 0;

signatureCharB_I_testMethodType  * OpCodesTest::_b2i = 0;
signatureCharB_J_testMethodType  * OpCodesTest::_b2l = 0;
signatureCharB_F_testMethodType  * OpCodesTest::_b2f = 0;
signatureCharB_D_testMethodType  * OpCodesTest::_b2d = 0;
signatureCharB_S_testMethodType  * OpCodesTest::_b2s = 0;
unsignedSignatureCharB_I_testMethodType  * OpCodesTest::_bu2i = 0;
unsignedSignatureCharB_J_testMethodType  * OpCodesTest::_bu2l = 0;
unsignedSignatureCharB_F_testMethodType  * OpCodesTest::_bu2f = 0;
unsignedSignatureCharB_D_testMethodType  * OpCodesTest::_bu2d = 0;
unsignedSignatureCharB_S_testMethodType  * OpCodesTest::_bu2s = 0;

//Abs
signatureCharI_I_testMethodType  * OpCodesTest::_iAbs = 0;
signatureCharJ_J_testMethodType  * OpCodesTest::_lAbs = 0;
signatureCharD_D_testMethodType  * OpCodesTest::_dAbs = 0;
signatureCharF_F_testMethodType  * OpCodesTest::_fAbs = 0;

//Load
signatureCharI_I_testMethodType  *OpCodesTest::_iLoad = 0;
signatureCharJ_J_testMethodType  *OpCodesTest::_lLoad = 0;
signatureCharD_D_testMethodType  *OpCodesTest::_dLoad = 0;
signatureCharF_F_testMethodType  *OpCodesTest::_fLoad = 0;
signatureCharB_B_testMethodType  *OpCodesTest::_bLoad = 0;
signatureCharS_S_testMethodType  *OpCodesTest::_sLoad = 0;

//Store
signatureCharI_I_testMethodType * OpCodesTest::_iStore = 0;
signatureCharJ_J_testMethodType * OpCodesTest::_lStore = 0;
signatureCharB_B_testMethodType * OpCodesTest::_bStore = 0;
signatureCharS_S_testMethodType * OpCodesTest::_sStore = 0;
signatureCharD_D_testMethodType * OpCodesTest::_dStore = 0;
signatureCharF_F_testMethodType * OpCodesTest::_fStore = 0;

//Return
signatureCharI_I_testMethodType  * OpCodesTest::_iReturn = 0;
signatureCharJ_J_testMethodType  * OpCodesTest::_lReturn = 0;
signatureCharD_D_testMethodType  * OpCodesTest::_dReturn = 0;
signatureCharF_F_testMethodType  * OpCodesTest::_fReturn = 0;

//direct call
signatureCharI_I_testMethodType  * OpCodesTest::_iCall = 0;
TR::ResolvedMethod * OpCodesTest::_int32Compilee;
signatureCharI_I_testMethodType  * OpCodesTest::_int32CompiledMethod = 0;
signatureCharJ_J_testMethodType  * OpCodesTest::_lCall = 0;
TR::ResolvedMethod * OpCodesTest::_int64Compilee;
signatureCharJ_J_testMethodType  * OpCodesTest::_int64CompiledMethod = 0;
signatureCharD_D_testMethodType  * OpCodesTest::_dCall = 0;
TR::ResolvedMethod * OpCodesTest::_doubleCompilee;
signatureCharD_D_testMethodType  * OpCodesTest::_doubleCompiledMethod = 0;
signatureCharF_F_testMethodType  * OpCodesTest::_fCall = 0;
TR::ResolvedMethod * OpCodesTest::_floatCompilee;
signatureCharF_F_testMethodType  * OpCodesTest::_floatCompiledMethod = 0;

//indirect load
signatureCharL_I_testMethodType  * OpCodesTest::_iLoadi = 0;
signatureCharL_J_testMethodType  * OpCodesTest::_lLoadi = 0;
signatureCharL_D_testMethodType  * OpCodesTest::_dLoadi = 0;
signatureCharL_F_testMethodType  * OpCodesTest::_fLoadi = 0;
signatureCharL_B_testMethodType  * OpCodesTest::_bLoadi = 0;
signatureCharL_S_testMethodType  * OpCodesTest::_sLoadi = 0;
signatureCharL_L_testMethodType  * OpCodesTest::_aLoadi = 0;

//indirect store
signatureCharLI_I_testMethodType  * OpCodesTest::_iStorei = 0;
signatureCharLJ_J_testMethodType  * OpCodesTest::_lStorei = 0;
signatureCharLD_D_testMethodType  * OpCodesTest::_dStorei = 0;
signatureCharLF_F_testMethodType  * OpCodesTest::_fStorei = 0;
signatureCharLB_B_testMethodType  * OpCodesTest::_bStorei = 0;
signatureCharLS_S_testMethodType  * OpCodesTest::_sStorei = 0;
signatureCharLL_L_testMethodType  * OpCodesTest::_aStorei = 0;

//Integer Arithmetic
signatureCharBB_B_testMethodType  * OpCodesTest::_bAdd = 0;
signatureCharBB_B_testMethodType  * OpCodesTest::_bSub = 0;
signatureCharBB_B_testMethodType  * OpCodesTest::_bMul = 0;
signatureCharBB_B_testMethodType  * OpCodesTest::_bDiv = 0;
signatureCharBB_B_testMethodType  * OpCodesTest::_bRem = 0;

signatureCharSS_S_testMethodType  * OpCodesTest::_sAdd = 0;
signatureCharSS_S_testMethodType  * OpCodesTest::_sSub = 0;
signatureCharSS_S_testMethodType  * OpCodesTest::_sMul = 0;
signatureCharSS_S_testMethodType  * OpCodesTest::_sDiv = 0;
signatureCharSS_S_testMethodType  * OpCodesTest::_sRem = 0;

signatureCharII_I_testMethodType  * OpCodesTest::_iDiv = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_iMul = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_iRem = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_iAdd = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_iSub = 0;
unsignedSignatureCharII_I_testMethodType  * OpCodesTest::_iuRem = 0;
unsignedSignatureCharII_I_testMethodType  * OpCodesTest::_iuDiv = 0;
unsignedSignatureCharII_I_testMethodType  * OpCodesTest::_iuMul = 0;

signatureCharJJ_J_testMethodType  * OpCodesTest::_lAdd = 0;
signatureCharJJ_J_testMethodType  * OpCodesTest::_lSub = 0;
signatureCharJJ_J_testMethodType  * OpCodesTest::_lMul = 0;
signatureCharJJ_J_testMethodType  * OpCodesTest::_lDiv = 0;
signatureCharJJ_J_testMethodType  * OpCodesTest::_lRem = 0;
unsignedSignatureCharJJ_J_testMethodType  * OpCodesTest::_luDiv = 0;

//Float Arithmetic
signatureCharFF_F_testMethodType  * OpCodesTest::_fAdd = 0;
signatureCharFF_F_testMethodType  * OpCodesTest::_fSub = 0;
signatureCharFF_F_testMethodType  * OpCodesTest::_fMul = 0;
signatureCharFF_F_testMethodType  * OpCodesTest::_fDiv = 0;
signatureCharFF_F_testMethodType  * OpCodesTest::_fRem = 0;

//Shift, Rol
signatureCharII_I_testMethodType  * OpCodesTest::_iShl = 0;
signatureCharJJ_J_testMethodType  * OpCodesTest::_lShl = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_iShr = 0;
signatureCharJJ_J_testMethodType  * OpCodesTest::_lShr = 0;
unsignedSignatureCharII_I_testMethodType  * OpCodesTest::_iuShr = 0;
unsignedSignatureCharJJ_J_testMethodType  * OpCodesTest::_luShr = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_iRol = 0;
signatureCharJJ_J_testMethodType  * OpCodesTest::_lRol = 0;
signatureCharSS_S_testMethodType  * OpCodesTest::_sShl = 0;
signatureCharBB_B_testMethodType  * OpCodesTest::_bShl = 0;
signatureCharSS_S_testMethodType  * OpCodesTest::_sShr = 0;
signatureCharBB_B_testMethodType  * OpCodesTest::_bShr = 0;
unsignedSignatureCharSS_S_testMethodType  * OpCodesTest::_suShr = 0;
unsignedSignatureCharBB_B_testMethodType  * OpCodesTest::_buShr = 0;

//TR::Double
signatureCharDD_D_testMethodType  * OpCodesTest::_dAdd = 0;
signatureCharDD_D_testMethodType  * OpCodesTest::_dSub = 0;
signatureCharDD_D_testMethodType  * OpCodesTest::_dDiv = 0;
signatureCharDD_D_testMethodType  * OpCodesTest::_dMul = 0;
signatureCharDD_D_testMethodType  * OpCodesTest::_dRem = 0;

//Bitwise
signatureCharII_I_testMethodType * OpCodesTest::_iAnd = 0;
signatureCharJJ_J_testMethodType * OpCodesTest::_lAnd = 0;
signatureCharII_I_testMethodType * OpCodesTest::_iOr = 0;
signatureCharJJ_J_testMethodType * OpCodesTest::_lOr = 0;
signatureCharII_I_testMethodType * OpCodesTest::_iXor = 0;
signatureCharJJ_J_testMethodType * OpCodesTest::_lXor = 0;
signatureCharSS_S_testMethodType * OpCodesTest::_sAnd = 0;
signatureCharSS_S_testMethodType * OpCodesTest::_sOr = 0;
signatureCharSS_S_testMethodType * OpCodesTest::_sXor = 0;
signatureCharBB_B_testMethodType * OpCodesTest::_bAnd = 0;
signatureCharBB_B_testMethodType * OpCodesTest::_bOr = 0;
signatureCharBB_B_testMethodType * OpCodesTest::_bXor = 0;

//Compare
signatureCharII_I_testMethodType  * OpCodesTest::_iCmpeq = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_lCmpeq = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_dCmpeq = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_fCmpeq = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_sCmpeq = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_bCmpeq = 0;

signatureCharII_I_testMethodType  * OpCodesTest::_iCmpne = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_lCmpne = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_dCmpne = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_fCmpne = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_sCmpne = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_bCmpne = 0;

signatureCharII_I_testMethodType  * OpCodesTest::_iCmplt = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_lCmplt = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_dCmplt = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_fCmplt = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_sCmplt = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_bCmplt = 0;

signatureCharII_I_testMethodType  * OpCodesTest::_iCmpgt = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_lCmpgt = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_dCmpgt = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_fCmpgt = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_sCmpgt = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_bCmpgt = 0;

signatureCharII_I_testMethodType  * OpCodesTest::_iCmple = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_lCmple = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_dCmple = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_fCmple = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_sCmple = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_bCmple = 0;

signatureCharII_I_testMethodType  * OpCodesTest::_iCmpge = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_lCmpge = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_dCmpge = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_fCmpge = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_sCmpge = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_bCmpge = 0;

unsignedCompareSignatureCharII_I_testMethodType  * OpCodesTest::_iuCmpeq = 0;
unsignedCompareSignatureCharII_I_testMethodType  * OpCodesTest::_iuCmpne = 0;
unsignedCompareSignatureCharII_I_testMethodType  * OpCodesTest::_iuCmplt = 0;
unsignedCompareSignatureCharII_I_testMethodType  * OpCodesTest::_iuCmpge = 0;
unsignedCompareSignatureCharII_I_testMethodType  * OpCodesTest::_iuCmpgt = 0;
unsignedCompareSignatureCharII_I_testMethodType  * OpCodesTest::_iuCmple = 0;
unsignedCompareSignatureCharJJ_I_testMethodType  * OpCodesTest::_luCmpeq = 0;
unsignedCompareSignatureCharJJ_I_testMethodType  * OpCodesTest::_luCmpne = 0;
unsignedCompareSignatureCharJJ_I_testMethodType  * OpCodesTest::_luCmplt = 0;
unsignedCompareSignatureCharJJ_I_testMethodType  * OpCodesTest::_luCmpge = 0;
unsignedCompareSignatureCharJJ_I_testMethodType  * OpCodesTest::_luCmpgt = 0;
unsignedCompareSignatureCharJJ_I_testMethodType  * OpCodesTest::_luCmple = 0;
unsignedCompareSignatureCharBB_I_testMethodType  * OpCodesTest::_buCmpeq = 0;
unsignedCompareSignatureCharBB_I_testMethodType  * OpCodesTest::_buCmpne = 0;
unsignedCompareSignatureCharBB_I_testMethodType  * OpCodesTest::_buCmplt = 0;
unsignedCompareSignatureCharBB_I_testMethodType  * OpCodesTest::_buCmpge = 0;
unsignedCompareSignatureCharBB_I_testMethodType  * OpCodesTest::_buCmpgt = 0;
unsignedCompareSignatureCharBB_I_testMethodType  * OpCodesTest::_buCmple = 0;
unsignedCompareSignatureCharSS_I_testMethodType  * OpCodesTest::_suCmpeq = 0;
unsignedCompareSignatureCharSS_I_testMethodType  * OpCodesTest::_suCmpne = 0;
unsignedCompareSignatureCharSS_I_testMethodType  * OpCodesTest::_suCmplt = 0;
unsignedCompareSignatureCharSS_I_testMethodType  * OpCodesTest::_suCmpge = 0;
unsignedCompareSignatureCharSS_I_testMethodType  * OpCodesTest::_suCmpgt = 0;
unsignedCompareSignatureCharSS_I_testMethodType  * OpCodesTest::_suCmple = 0;

signatureCharJJ_I_testMethodType * OpCodesTest::_lCmp = 0;
signatureCharFF_I_testMethodType * OpCodesTest::_fCmpl = 0;
signatureCharFF_I_testMethodType * OpCodesTest::_fCmpg = 0;
signatureCharDD_I_testMethodType * OpCodesTest::_dCmpl = 0;
signatureCharDD_I_testMethodType * OpCodesTest::_dCmpg = 0;

//CompareBranch
signatureCharII_I_testMethodType  * OpCodesTest::_ifIcmpeq = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_ifIcmpne = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_ifIcmpgt = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_ifIcmplt = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_ifIcmpge = 0;
signatureCharII_I_testMethodType  * OpCodesTest::_ifIcmple = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_ifLcmpeq = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_ifLcmpne = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_ifLcmpgt = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_ifLcmplt = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_ifLcmpge = 0;
signatureCharJJ_I_testMethodType  * OpCodesTest::_ifLcmple = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_ifFcmpeq = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_ifFcmpne = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_ifFcmpgt = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_ifFcmplt = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_ifFcmpge = 0;
signatureCharFF_I_testMethodType  * OpCodesTest::_ifFcmple = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_ifDcmpeq = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_ifDcmpne = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_ifDcmpgt = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_ifDcmplt = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_ifDcmpge = 0;
signatureCharDD_I_testMethodType  * OpCodesTest::_ifDcmple = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_ifScmpeq = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_ifScmpne = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_ifScmpgt = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_ifScmplt = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_ifScmpge = 0;
signatureCharSS_I_testMethodType  * OpCodesTest::_ifScmple = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_ifBcmpeq = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_ifBcmpne = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_ifBcmpgt = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_ifBcmplt = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_ifBcmpge = 0;
signatureCharBB_I_testMethodType  * OpCodesTest::_ifBcmple = 0;
unsignedCompareSignatureCharII_I_testMethodType * OpCodesTest::_ifIuCmpeq = 0;
unsignedCompareSignatureCharII_I_testMethodType * OpCodesTest::_ifIuCmpne = 0;
unsignedCompareSignatureCharII_I_testMethodType * OpCodesTest::_ifIuCmplt = 0;
unsignedCompareSignatureCharII_I_testMethodType * OpCodesTest::_ifIuCmpge = 0;
unsignedCompareSignatureCharII_I_testMethodType * OpCodesTest::_ifIuCmpgt = 0;
unsignedCompareSignatureCharII_I_testMethodType * OpCodesTest::_ifIuCmple = 0;
unsignedCompareSignatureCharJJ_I_testMethodType * OpCodesTest::_ifLuCmpeq = 0;
unsignedCompareSignatureCharJJ_I_testMethodType * OpCodesTest::_ifLuCmpne = 0;
unsignedCompareSignatureCharJJ_I_testMethodType * OpCodesTest::_ifLuCmplt = 0;
unsignedCompareSignatureCharJJ_I_testMethodType * OpCodesTest::_ifLuCmpge = 0;
unsignedCompareSignatureCharJJ_I_testMethodType * OpCodesTest::_ifLuCmpgt = 0;
unsignedCompareSignatureCharJJ_I_testMethodType * OpCodesTest::_ifLuCmple = 0;
unsignedCompareSignatureCharBB_I_testMethodType * OpCodesTest::_ifBuCmpeq = 0;
unsignedCompareSignatureCharBB_I_testMethodType * OpCodesTest::_ifBuCmpne = 0;
unsignedCompareSignatureCharBB_I_testMethodType * OpCodesTest::_ifBuCmplt = 0;
unsignedCompareSignatureCharBB_I_testMethodType * OpCodesTest::_ifBuCmpge = 0;
unsignedCompareSignatureCharBB_I_testMethodType * OpCodesTest::_ifBuCmpgt = 0;
unsignedCompareSignatureCharBB_I_testMethodType * OpCodesTest::_ifBuCmple = 0;
unsignedCompareSignatureCharSS_I_testMethodType * OpCodesTest::_ifSuCmpeq = 0;
unsignedCompareSignatureCharSS_I_testMethodType * OpCodesTest::_ifSuCmpne = 0;
unsignedCompareSignatureCharSS_I_testMethodType * OpCodesTest::_ifSuCmplt = 0;
unsignedCompareSignatureCharSS_I_testMethodType * OpCodesTest::_ifSuCmpge = 0;
unsignedCompareSignatureCharSS_I_testMethodType * OpCodesTest::_ifSuCmpgt = 0;
unsignedCompareSignatureCharSS_I_testMethodType * OpCodesTest::_ifSuCmple = 0;

//Ternary operators
signatureCharIBB_B_testMethodType * OpCodesTest::_bternary = 0;
signatureCharISS_S_testMethodType * OpCodesTest::_sternary = 0;
signatureCharIII_I_testMethodType * OpCodesTest::_iternary = 0;
signatureCharIJJ_J_testMethodType * OpCodesTest::_lternary = 0;
signatureCharIFF_F_testMethodType * OpCodesTest::_fternary = 0;
signatureCharIDD_D_testMethodType * OpCodesTest::_dternary = 0;

//Address opcodes
signatureCharL_L_testMethodType * OpCodesTest::_acall = 0;
TR::ResolvedMethod * OpCodesTest::_addressCompilee;
signatureCharL_L_testMethodType  * OpCodesTest::_addressCompiledMethod = 0;

signatureCharL_L_testMethodType * OpCodesTest::_aload = 0;
signatureCharL_L_testMethodType * OpCodesTest::_astore = 0;
signatureCharL_L_testMethodType * OpCodesTest::_areturn = 0;
signatureCharL_B_testMethodType * OpCodesTest::_a2b = 0;
signatureCharL_S_testMethodType * OpCodesTest::_a2s = 0;
signatureCharL_I_testMethodType * OpCodesTest::_a2i = 0;
signatureCharL_J_testMethodType * OpCodesTest::_a2l = 0;
signatureCharB_L_testMethodType * OpCodesTest::_b2a = 0;
signatureCharS_L_testMethodType * OpCodesTest::_s2a = 0;
signatureCharI_L_testMethodType * OpCodesTest::_i2a = 0;
signatureCharJ_L_testMethodType * OpCodesTest::_l2a = 0;
unsignedSignatureCharB_L_testMethodType * OpCodesTest::_bu2a = 0;
unsignedSignatureCharS_L_testMethodType * OpCodesTest::_su2a = 0;
unsignedSignatureCharI_L_testMethodType * OpCodesTest::_iu2a = 0;
unsignedSignatureCharJ_L_testMethodType * OpCodesTest::_lu2a = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_acmpeq = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_acmpne = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_acmplt = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_acmpge = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_acmple = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_acmpgt = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_ifacmpeq = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_ifacmpne = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_ifacmplt = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_ifacmpge = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_ifacmple = 0;
signatureCharLL_I_testMethodType * OpCodesTest::_ifacmpgt = 0;
signatureCharILL_L_testMethodType * OpCodesTest::_aternary = 0;


void
OpCodesTest::allocateTestData()
   {
   }

void
OpCodesTest::deallocateTestData()
   {
   }

void
OpCodesTest::compileTestMethods()
   {
   }

uint8_t *
OpCodesTest::compileOpCodeMethod(int32_t opCodeArgsNum,
      TR::ILOpCodes opCode,
      char * resolvedMethodName,
      TR::DataType * argTypes,
      TR::DataType returnType,
      int32_t & returnCode,
      uint16_t numArgs,
      ...)
   {
   TR_ASSERT((numArgs % 2) == 0, "Must be called with zero or an even args, numChildArgs = %d", numArgs);
   if ((numArgs % 2) != 0)
      {
      fprintf(stderr, "Error: numArgs must be called with zero or an even args, numArgs is %d", numArgs);
      exit(-1);
      }

   OpIlInjector * opCodeInjector = 0;
   TR::ILOpCode op(opCode);

   TR::TypeDictionary types;

   CmpBranchOpIlInjector cmpBranchIlnjector(&types, this, opCode);
   BinaryOpIlInjector opCodeBinaryInjector(&types, this, opCode);
   UnaryOpIlInjector opCodeUnaryInjector(&types, this, opCode);
   TernaryOpIlInjector ternaryOpIlInjector(&types, this, opCode);
   ChildlessUnaryOpIlInjector childlessUnaryOpIlInjector(&types, this, opCode);
   StoreOpIlInjector storeOpIlInjector(&types, this, opCode);
   IndirectLoadIlInjector indirectLoadIlInjector(&types, this, opCode);
   IndirectStoreIlInjector indirectStoreIlInjector(&types, this, opCode);

   if (op.isBooleanCompare() && op.isBranch())
      {
      opCodeInjector = &cmpBranchIlnjector;
      }
   else if (op.isTernary())
      {
      opCodeInjector = &ternaryOpIlInjector;
      }
   else if (op.isStoreIndirect())
      {
      opCodeInjector = &indirectStoreIlInjector;
      }
   else if (op.isLoadIndirect())
      {
      opCodeInjector = &indirectLoadIlInjector;
      }
   else if (((op.isLoadVar() || op.isLoadConst()) && !op.isIndirect()) || op.isReturn() )
      {
      opCodeInjector = &childlessUnaryOpIlInjector;
      }
   else if (op.isStore() && !op.isStoreIndirect())
      {
      opCodeInjector = &storeOpIlInjector;
      }

   else
      {
      if (2 == opCodeArgsNum)
         {
         opCodeInjector = &opCodeBinaryInjector;
         }
      else
         {
         opCodeInjector = &opCodeUnaryInjector;
         }
      }

   TR::IlType **argIlTypes = new TR::IlType*[numArgs];
   for (uint32_t a=0;a < opCodeArgsNum;a++)
      argIlTypes[a] = types.PrimitiveType(argTypes[a]);

   if (numArgs != 0)
      {
      va_list args;
      va_start(args, numArgs);
      for (int32_t i = 0; i < numArgs; i = i + 2)
         {

         uint32_t pos = va_arg(args, uint32_t);
         void * value = va_arg(args, void *);

         switch (argTypes[pos - 1])
             {
             case TR::Int8:
                {
                int8_t *int8Value = (int8_t *) value;
                opCodeInjector->bconstParm(pos, *int8Value);
                break;
                }
             case TR::Int16:
                {
                int16_t * int16Value = (int16_t *) value;
                opCodeInjector->sconstParm(pos, *int16Value);
                break;
                }
             case TR::Int32:
                {
                int32_t * int32Value = (int32_t *) value;
                opCodeInjector->iconstParm(pos, *int32Value);
                break;
                }
             case TR::Int64:
                {
                int64_t * int64Value = (int64_t *) value;
                opCodeInjector->lconstParm(pos, *int64Value);
                break;
                }
             case TR::Float:
                {
                float * floatValue = (float *) value;
                opCodeInjector->fconstParm(pos, *floatValue);
                break;
                }
             case TR::Double:
                {
                double * doubleValue = (double *) value;
                opCodeInjector->dconstParm(pos, *doubleValue);
                break;
                }
             case TR::Address:
                {
                uintptrj_t * addressValue = (uintptrj_t *) value;
                opCodeInjector->aconstParm(pos, *addressValue);
                break;
                }
             default:
                TR_ASSERT(0, "Wrong dataType or not supported dataType");
             }
          }
      va_end(args);
      }
   TR::ResolvedMethod opCodeCompilee(__FILE__, LINETOSTR(__LINE__), resolvedMethodName, opCodeArgsNum, argIlTypes, types.PrimitiveType(returnType), 0, opCodeInjector);
   TR::IlGeneratorMethodDetails opCodeDetails(&opCodeCompilee);
   uint8_t *startPC= compileMethod(opCodeDetails, warm, returnCode);
   return startPC;
   }

uint8_t *
OpCodesTest::compileDirectCallOpCodeMethod(int32_t opCodeArgsNum,
      TR::ILOpCodes opCodeCompilee,
      TR::ILOpCodes opCode,
      char * compileeResolvedMethodName,
      char * testResolvedMethodName,
      TR::DataType * argTypes,
      TR::DataType returnType,
      int32_t & returnCode)
   {
   TR::TypeDictionary types;
   ChildlessUnaryOpIlInjector functionIlInjector(&types, this, opCodeCompilee);

   TR::IlType **argIlTypes = new TR::IlType*[opCodeArgsNum];
   for (int32_t i=0;i < opCodeArgsNum;i++)
      argIlTypes[i] = types.PrimitiveType(argTypes[i]);

   TR::ResolvedMethod functionCompilee(__FILE__, LINETOSTR(__LINE__), compileeResolvedMethodName, opCodeArgsNum, argIlTypes, types.PrimitiveType(returnType), 0, &functionIlInjector);
   TR::IlGeneratorMethodDetails functionDetails(&functionCompilee);
   switch (returnType)
      {
      case TR::Int32:
         _int32Compilee = &functionCompilee;
         _int32CompiledMethod = (signatureCharI_I_testMethodType *) (compileMethod(functionDetails, warm, returnCode));
         functionCompilee.setEntryPoint((void *)_int32CompiledMethod);
         break;
      case TR::Int64:
         _int64Compilee = &functionCompilee;
         _int64CompiledMethod = (signatureCharJ_J_testMethodType *) (compileMethod(functionDetails, warm, returnCode));
         functionCompilee.setEntryPoint((void *)_int64CompiledMethod);
         break;
      case TR::Double:
         _doubleCompilee = &functionCompilee;
         _doubleCompiledMethod = (signatureCharD_D_testMethodType *) (compileMethod(functionDetails, warm, returnCode));
         functionCompilee.setEntryPoint((void *)_doubleCompiledMethod);
         break;
      case TR::Float:
         _floatCompilee = &functionCompilee;
         _floatCompiledMethod = (signatureCharF_F_testMethodType *) (compileMethod(functionDetails, warm, returnCode));
         functionCompilee.setEntryPoint((void *)_floatCompiledMethod);
         break;
      case TR::Address:
         _addressCompilee = &functionCompilee;
         _addressCompiledMethod = (signatureCharL_L_testMethodType *) (compileMethod(functionDetails, warm, returnCode));
         functionCompilee.setEntryPoint((void *)_addressCompiledMethod);
         break;
      default:
         TR_ASSERT(0, "compilee dataType should be int32, int64, double, float or address");
      }

   CallIlInjector callIlInjector(&types, this, opCode);
   TR::ResolvedMethod callCompilee(__FILE__, LINETOSTR(__LINE__), testResolvedMethodName, opCodeArgsNum, argIlTypes, types.PrimitiveType(returnType), 0, &callIlInjector);
   TR::IlGeneratorMethodDetails callDetails(&callCompilee);
   uint8_t *startPC = compileMethod(callDetails, warm, returnCode);
   return startPC;
   }

void
OpCodesTest::addUnsupportedOpCodeTest(int32_t opCodeArgsNum,
      TR::ILOpCodes opCode,
      char * resolvedMethodName,
      TR::DataType * argTypes,
      TR::DataType returnType)
   {
   int32_t returnCode = 0;
   compileOpCodeMethod(opCodeArgsNum, opCode, resolvedMethodName, argTypes, returnType, returnCode);
   EXPECT_EQ(COMPILATION_IL_GEN_FAILURE, returnCode) << resolvedMethodName << " is " << returnCode << ", expected is " << COMPILATION_IL_GEN_FAILURE;
   }

TR::ResolvedMethod *
OpCodesTest::resolvedMethod(TR::DataType dataType)
   {
   switch (dataType)
      {
      case TR::Int32:
         return _int32Compilee;
      case TR::Int64:
         return _int64Compilee;
      case TR::Double:
         return _doubleCompilee;
      case TR::Float:
         return _floatCompilee;
      case TR::Address:
         return _addressCompilee;
      case TR::NoType:
      default:
         TR_ASSERT(0, "compilee dataType should be int32, in64, double, float or address");
         return NULL;
      }
   }

void
OpCodesTest::compileIntegerArithmeticTestMethods()
   {
   int32_t rc = 0;
   _iAdd = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iadd, "iAdd", _argTypesBinaryInt, TR::Int32, rc));
   _iSub = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::isub, "iSub", _argTypesBinaryInt, TR::Int32, rc));
   _iDiv = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::idiv, "iDiv", _argTypesBinaryInt, TR::Int32, rc));
   _iMul = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::imul, "iMul", _argTypesBinaryInt, TR::Int32, rc));
   _iRem = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::irem, "iRem", _argTypesBinaryInt, TR::Int32, rc));
   }

void
OpCodesTest::compileMemoryOperationTestMethods()
   {
   int32_t rc = 0;

   _iLoad = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::iload, "iLoad", _argTypesUnaryInt, TR::Int32, rc));
   _lLoad = (signatureCharJ_J_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::lload, "lLoad", _argTypesUnaryLong, TR::Int64, rc));
   _dLoad = (signatureCharD_D_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::dload, "dLoad", _argTypesUnaryDouble, TR::Double, rc));
   _fLoad = (signatureCharF_F_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::fload, "fLoad", _argTypesUnaryFloat, TR::Float, rc));

   _iStore = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::istore, "iStore", _argTypesUnaryInt, TR::Int32, rc));

   _iLoadi = (signatureCharL_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::iloadi, "iLoadi", _argTypesUnaryAddress, TR::Int32, rc));
   _lLoadi = (signatureCharL_J_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::lloadi, "lLoadi", _argTypesUnaryAddress, TR::Int64, rc));
   _dLoadi = (signatureCharL_D_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::dloadi, "dLoadi", _argTypesUnaryAddress, TR::Double, rc));
   _fLoadi = (signatureCharL_F_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::floadi, "fLoadi", _argTypesUnaryAddress, TR::Float, rc));
   _bLoadi = (signatureCharL_B_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::bloadi, "bLoadi", _argTypesUnaryAddress, TR::Int8, rc));
   _sLoadi = (signatureCharL_S_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::sloadi, "sLoadi", _argTypesUnaryAddress, TR::Int16, rc));
   _aLoadi = (signatureCharL_L_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::aloadi, "aLoadi", _argTypesUnaryAddress, TR::Address, rc));

   }

void
OpCodesTest::compileUnaryTestMethods()
   {
   int32_t rc = 0;

   _iNeg = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::ineg, "iNeg", _argTypesUnaryInt, TR::Int32, rc));
   _iAbs = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::iabs, "iAbs", _argTypesUnaryInt, TR::Int32, rc));

   _iReturn = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::ireturn, "iReturn", _argTypesUnaryInt, TR::Int32, rc));

   _i2l = (signatureCharI_J_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::i2l, "i2l", _argTypesUnaryInt, TR::Int64, rc));
   _i2b = (signatureCharI_B_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::i2b, "i2b", _argTypesUnaryInt, TR::Int8, rc));
   _i2s = (signatureCharI_S_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::i2s, "i2s", _argTypesUnaryInt, TR::Int16, rc));

   _l2i = (signatureCharJ_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::l2i, "l2i", _argTypesUnaryLong, TR::Int32, rc));
   _l2b = (signatureCharJ_B_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::l2b, "l2b", _argTypesUnaryLong, TR::Int8, rc));
   _l2s = (signatureCharJ_S_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::l2s, "l2s", _argTypesUnaryLong, TR::Int16, rc));

   _f2i = (signatureCharF_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::f2i, "f2i", _argTypesUnaryFloat, TR::Int32, rc));
   _d2i = (signatureCharD_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::d2i, "d2i", _argTypesUnaryDouble, TR::Int32, rc));

   }

void
OpCodesTest::compileShiftOrRolTestMethods()
   {
   int32_t rc = 0;

   _iShl = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ishl, "iShl", _argTypesBinaryInt, TR::Int32, rc));
   _iShr = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ishr, "iShr", _argTypesBinaryInt, TR::Int32, rc));
   _iuShr = (unsignedSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iushr, "iuShr", _argTypesBinaryInt, TR::Int32, rc));
   }

void
OpCodesTest::compileBitwiseMethods()
   {
   int32_t rc;

   _iAnd = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iand, "iAnd", _argTypesBinaryInt, TR::Int32, rc));
   _iOr = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ior, "iOr", _argTypesBinaryInt, TR::Int32, rc));
   _iXor = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ixor, "iXor", _argTypesBinaryInt, TR::Int32, rc));
   }

void
OpCodesTest::compileCompareTestMethods()
   {
   int32_t rc = 0;

   //Compare
   _iCmpeq = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::icmpeq, "iCmpeq", _argTypesBinaryInt, TR::Int32, rc));
   _iCmpne = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::icmpne, "iCmpne", _argTypesBinaryInt, TR::Int32, rc));
   _iCmpgt = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::icmpgt, "iCmpgt", _argTypesBinaryInt, TR::Int32, rc));
   _iCmplt = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::icmplt, "iCmplt", _argTypesBinaryInt, TR::Int32, rc));
   _iCmpge = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::icmpge, "iCmpge", _argTypesBinaryInt, TR::Int32, rc));
   _iCmple = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::icmple, "iCmple", _argTypesBinaryInt, TR::Int32, rc));

   _lCmpne = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lcmpne, "lCmpne", _argTypesBinaryLong, TR::Int32, rc));
   _lCmpgt = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lcmpgt, "lCmpgt", _argTypesBinaryLong, TR::Int32, rc));
   _lCmpge = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lcmpge, "lCmpge", _argTypesBinaryLong, TR::Int32, rc));
   _lCmple = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lcmple, "lCmple", _argTypesBinaryLong, TR::Int32, rc));

   _iuCmplt = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iucmplt, "iuCmplt", _argTypesBinaryInt, TR::Int32, rc));
   _iuCmpgt = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iucmpgt, "iuCmpgt", _argTypesBinaryInt, TR::Int32, rc));
   _iuCmple = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iucmple, "iuCmple", _argTypesBinaryInt, TR::Int32, rc));
   _luCmpeq = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lucmpeq, "luCmpeq", _argTypesBinaryLong, TR::Int32, rc));
   _luCmpne = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lucmpne, "luCmpne", _argTypesBinaryLong, TR::Int32, rc));
   _luCmplt = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lucmplt, "luCmplt", _argTypesBinaryLong, TR::Int32, rc));
   _luCmpge = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lucmpge, "luCmpge", _argTypesBinaryLong, TR::Int32, rc));
   _luCmpgt = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lucmpgt, "luCmpgt", _argTypesBinaryLong, TR::Int32, rc));
   _luCmple = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::lucmple, "luCmple", _argTypesBinaryLong, TR::Int32, rc));

   //CompareAndBranch
   _ifIcmpeq = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ificmpeq, "ifIcmpeq", _argTypesBinaryInt, TR::Int32, rc));
   _ifIcmpne = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ificmpne, "ifIcmpne", _argTypesBinaryInt, TR::Int32, rc));
   _ifIcmpgt = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ificmpgt, "ifIcmpgt", _argTypesBinaryInt, TR::Int32, rc));
   _ifIcmplt = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ificmplt, "ifIcmplt", _argTypesBinaryInt, TR::Int32, rc));
   _ifIcmpge = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ificmpge, "ifIcmpge", _argTypesBinaryInt, TR::Int32, rc));
   _ifIcmple = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ificmple, "ifIcmple", _argTypesBinaryInt, TR::Int32, rc));

   _ifLcmpne = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflcmpne, "ifLcmpne", _argTypesBinaryLong, TR::Int32, rc));
   _ifLcmpge = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflcmpge, "ifLcmpge", _argTypesBinaryLong, TR::Int32, rc));
   _ifLcmple = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflcmple, "ifLcmple", _argTypesBinaryLong, TR::Int32, rc));

   _ifIuCmpeq = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ifiucmpeq, "ifIuCmpeq", _argTypesBinaryInt, TR::Int32, rc));
   _ifIuCmpne = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ifiucmpne, "ifIuCmpne", _argTypesBinaryInt, TR::Int32, rc));
   _ifIuCmplt = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ifiucmplt, "ifIuCmplt", _argTypesBinaryInt, TR::Int32, rc));
   _ifIuCmpge = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ifiucmpge, "ifIuCmpge", _argTypesBinaryInt, TR::Int32, rc));
   _ifIuCmpgt = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ifiucmpgt, "ifIuCmpgt", _argTypesBinaryInt, TR::Int32, rc));
   _ifIuCmple = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::ifiucmple, "ifIuCmple", _argTypesBinaryInt, TR::Int32, rc));
   _ifLuCmpeq = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflucmpeq, "ifLuCmpeq", _argTypesBinaryLong, TR::Int32, rc));
   _ifLuCmpne = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflucmpne, "ifLuCmpne", _argTypesBinaryLong, TR::Int32, rc));
   _ifLuCmplt = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflucmplt, "ifLuCmplt", _argTypesBinaryLong, TR::Int32, rc));
   _ifLuCmpge = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflucmpge, "ifLuCmpge", _argTypesBinaryLong, TR::Int32, rc));
   _ifLuCmpgt = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflucmpgt, "ifLuCmpgt", _argTypesBinaryLong, TR::Int32, rc));
   _ifLuCmple = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iflucmple, "ifLuCmple", _argTypesBinaryLong, TR::Int32, rc));
   }

void
OpCodesTest::compileTernaryTestMethods()
   {
   int32_t rc = 0;

   _iternary = (signatureCharIII_I_testMethodType *) (compileOpCodeMethod(_numberOfTernaryArgs, TR::iternary, "iTernary", _argTypesTernaryInt, TR::Int32, rc));
   }

void
OpCodesTest::compileAddressTestMethods()
   {
   int32_t rc = 0;

   _aload = (signatureCharL_L_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::aload, "aload", _argTypesUnaryAddress, TR::Address, rc));
   _astore = (signatureCharL_L_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::astore, "astore", _argTypesUnaryAddress, TR::Address, rc));
   _areturn = (signatureCharL_L_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::areturn, "areturn", _argTypesUnaryAddress, TR::Address, rc));
   _a2i = (signatureCharL_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::a2i, "a2i", _argTypesUnaryAddress, TR::Int32, rc));

   }

void
OpCodesTest::compileDisabledOpCodesTests()
   {
   int32_t rc = 0;
   //Jazz103 Work item 110364
   _fRem = (signatureCharFF_F_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::frem, "fRem", _argTypesBinaryFloat, TR::Float, rc));
   _dRem = (signatureCharDD_D_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::drem, "dRem", _argTypesBinaryDouble, TR::Double, rc));
   }

void
OpCodesTest::invokeTests()
   {

   }

void
OpCodesTest::invokeIntegerArithmeticTests()
   {
   //TR::Int32
   int32_t rc = 0;
   uint32_t testCaseArrLength = 0;
   char resolvedMethodName [RESOLVED_METHOD_NAME_LENGTH];

   signatureCharII_I_testMethodType * iBinaryCons = 0;
   signatureCharJJ_J_testMethodType * lBinaryCons = 0;

   int32_t intAddArr[][2] =
      {
      INT_ZERO, INT_ZERO,
      INT_NEG, INT_NEG,
      INT_MINIMUM, INT_POS,
      INT_MAXIMUM, INT_MAXIMUM,
      INT_POS, INT_MINIMUM
      };
   int32_t intSubArr[][2] =
      {
      INT_MAXIMUM, INT_MINIMUM,
      INT_NEG, INT_POS,
      INT_POS, INT_MAXIMUM,
      INT_ZERO, INT_NEG,
      INT_MINIMUM, INT_ZERO
      };
   int32_t intMulArr[][2] =
      {
      INT_NEG, INT_MINIMUM,
      INT_ZERO, INT_POS,
      INT_MINIMUM, INT_NEG,
      INT_POS, INT_ZERO,
      INT_MAXIMUM, INT_MINIMUM,
      INT_MINIMUM, INT_MAXIMUM
      };
   int32_t intDivArr[][2] =
      {
      INT_NEG, INT_MAXIMUM,
      INT_POS, INT_POS,
      INT_MAXIMUM, INT_NEG,
      INT_ZERO, INT_MINIMUM
      };
   int32_t intRemArr[][2] =
      {
      INT_MINIMUM, INT_MINIMUM,
      INT_ZERO, INT_MAXIMUM,
      INT_POS, INT_NEG,
      INT_MAXIMUM, INT_POS
      };

   //iadd
   testCaseArrLength = sizeof(intAddArr) / sizeof(intAddArr[0]);
   for(uint32_t i = 0; i < testCaseArrLength; ++i)
      {
      EXPECT_EQ(add(intAddArr[i][0], intAddArr[i][1]), _iAdd(intAddArr[i][0], intAddArr[i][1]));

      sprintf(resolvedMethodName, "iAddConst1_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iadd,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &intAddArr[i][0], 2, &intAddArr[i][1]));
      EXPECT_EQ(add(intAddArr[i][0], intAddArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iAddConst2_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iadd,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &intAddArr[i][0]));
      EXPECT_EQ(add(intAddArr[i][0], intAddArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, intAddArr[i][1]));

      sprintf(resolvedMethodName, "iAddConst3_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::iadd,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &intAddArr[i][1]));
      EXPECT_EQ(add(intAddArr[i][0], intAddArr[i][1]), iBinaryCons(intAddArr[i][0], INT_PLACEHOLDER_2));
      }

   //isub
   testCaseArrLength = sizeof(intSubArr) / sizeof(intSubArr[0]);
   for(uint32_t i = 0; i < testCaseArrLength; ++i)
      {
      EXPECT_EQ(sub(intSubArr[i][0], intSubArr[i][1]), _iSub(intSubArr[i][0], intSubArr[i][1]));

      sprintf(resolvedMethodName, "iSubConst1_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::isub,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &intSubArr[i][0], 2, &intSubArr[i][1]));
      EXPECT_EQ(sub(intSubArr[i][0], intSubArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iSubConst2_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::isub,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &intSubArr[i][0]));
      EXPECT_EQ(sub(intSubArr[i][0], intSubArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, intSubArr[i][1]));

      sprintf(resolvedMethodName, "iSubConst3_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::isub,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &intSubArr[i][1]));
      EXPECT_EQ(sub(intSubArr[i][0], intSubArr[i][1]), iBinaryCons(intSubArr[i][0], INT_PLACEHOLDER_2));
      }

   //imul
   testCaseArrLength = sizeof(intMulArr) / sizeof(intMulArr[0]);
   for(uint32_t i = 0; i < testCaseArrLength; ++i)
      {
      EXPECT_EQ(mul(intMulArr[i][0], intMulArr[i][1]), _iMul(intMulArr[i][0], intMulArr[i][1]));

      sprintf(resolvedMethodName, "iMulConst1_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::imul,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &intMulArr[i][0], 2, &intMulArr[i][1]));
      EXPECT_EQ(mul(intMulArr[i][0], intMulArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iMulConst2_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::imul,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &intMulArr[i][0]));
      EXPECT_EQ(mul(intMulArr[i][0], intMulArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, intMulArr[i][1]));

      sprintf(resolvedMethodName, "iMulConst3_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::imul,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &intMulArr[i][1]));
      EXPECT_EQ(mul(intMulArr[i][0], intMulArr[i][1]), iBinaryCons(intMulArr[i][0], INT_PLACEHOLDER_2));
      }

   //idiv
   //TODO: Use ASSERT_DEATH() to catch Divide by zero which will get "Floating point exception (core dumped)"
   //Test secnario : _iDiv(INT_MIN, 0)
   testCaseArrLength = sizeof(intDivArr) / sizeof(intDivArr[0]);
   for(uint32_t i = 0; i < testCaseArrLength; ++i)
      {
      EXPECT_EQ(div(intDivArr[i][0], intDivArr[i][1]), _iDiv(intDivArr[i][0], intDivArr[i][1]));

      sprintf(resolvedMethodName, "iDivConst1_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::idiv,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &intDivArr[i][0], 2, &intDivArr[i][1]));
      EXPECT_EQ(div(intDivArr[i][0], intDivArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iDivConst2_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::idiv,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &intDivArr[i][0]));
      EXPECT_EQ(div(intDivArr[i][0], intDivArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, intDivArr[i][1]));

      sprintf(resolvedMethodName, "iDivConst3_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::idiv,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &intDivArr[i][1]));
      EXPECT_EQ(div(intDivArr[i][0], intDivArr[i][1]), iBinaryCons(intDivArr[i][0], INT_PLACEHOLDER_2));
      }

   //irem
   //TODO: Use ASSERT_DEATH() to catch Remainder by zero which will get "Floating point exception (core dumped)"
   //Test secnario : _iRem(TR::getMaxSigned<TR::Int32>(), 0) , _Rem(-9, 0)
   testCaseArrLength = sizeof(intRemArr) / sizeof(intRemArr[0]);
   for(uint32_t i = 0; i < testCaseArrLength; ++i)
      {
      EXPECT_EQ(rem(intRemArr[i][0], intRemArr[i][1]), _iRem(intRemArr[i][0], intRemArr[i][1]));

      sprintf(resolvedMethodName, "iRemConst1_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::irem,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &intRemArr[i][0], 2, &intRemArr[i][1]));
      EXPECT_EQ(rem(intRemArr[i][0], intRemArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iRemConst2_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::irem,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &intRemArr[i][0]));
      EXPECT_EQ(rem(intRemArr[i][0], intRemArr[i][1]), iBinaryCons(INT_PLACEHOLDER_1, intRemArr[i][1]));

      sprintf(resolvedMethodName, "iRemConst3_Testcase%d", i);
      iBinaryCons = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(_numberOfBinaryArgs, TR::irem,
            resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &intRemArr[i][1]));
      EXPECT_EQ(rem(intRemArr[i][0], intRemArr[i][1]), iBinaryCons(intRemArr[i][0], INT_PLACEHOLDER_2));
      }
   }

void
OpCodesTest::invokeMemoryOperationTests()
   {
   //iload
   EXPECT_EQ(INT_ZERO, _iLoad(INT_ZERO));
   EXPECT_EQ(INT_NEG, _iLoad(INT_NEG));
   EXPECT_EQ(INT_POS, _iLoad(INT_POS));
   EXPECT_EQ(INT_MAXIMUM, _iLoad(INT_MAXIMUM));
   EXPECT_EQ(INT_MINIMUM, _iLoad(INT_MINIMUM));

   EXPECT_EQ(LONG_ZERO, _lLoad(LONG_ZERO));
   EXPECT_EQ(LONG_NEG, _lLoad(LONG_NEG));
   EXPECT_EQ(LONG_POS, _lLoad(LONG_POS));
   EXPECT_EQ(LONG_MAXIMUM, _lLoad(LONG_MAXIMUM));
   EXPECT_EQ(LONG_MINIMUM, _lLoad(LONG_MINIMUM));

   EXPECT_DOUBLE_EQ(DOUBLE_ZERO, _dLoad(DOUBLE_ZERO));
   EXPECT_DOUBLE_EQ(DOUBLE_NEG, _dLoad(DOUBLE_NEG));
   EXPECT_DOUBLE_EQ(DOUBLE_POS, _dLoad(DOUBLE_POS));
   EXPECT_DOUBLE_EQ(DOUBLE_MAXIMUM, _dLoad(DOUBLE_MAXIMUM));
   EXPECT_DOUBLE_EQ(DOUBLE_MINIMUM, _dLoad(DOUBLE_MINIMUM));

   EXPECT_FLOAT_EQ(FLOAT_ZERO, _fLoad(FLOAT_ZERO));
   EXPECT_FLOAT_EQ(FLOAT_NEG, _fLoad(FLOAT_NEG));
   EXPECT_FLOAT_EQ(FLOAT_POS, _fLoad(FLOAT_POS));
   EXPECT_FLOAT_EQ(FLOAT_MAXIMUM, _fLoad(FLOAT_MAXIMUM));
   EXPECT_FLOAT_EQ(FLOAT_MINIMUM, _fLoad(FLOAT_MINIMUM));

   int32_t intDataArray[] = {INT_NEG, INT_POS, INT_MAXIMUM, INT_MINIMUM, INT_ZERO};
   int16_t shortDataArray[] = {SHORT_NEG, SHORT_POS, SHORT_MAXIMUM, SHORT_MINIMUM, SHORT_ZERO};
   int8_t byteDataArray[] = {BYTE_NEG, BYTE_POS, BYTE_MAXIMUM, BYTE_MINIMUM, BYTE_ZERO};
   int64_t longDataArray[] = {LONG_NEG, LONG_POS, LONG_MAXIMUM, LONG_MINIMUM, LONG_ZERO};
   float floatDataArray[] = {FLOAT_NEG, FLOAT_POS, FLOAT_MAXIMUM, FLOAT_MINIMUM, FLOAT_ZERO};
   double doubleDataArray[] = {DOUBLE_NEG, DOUBLE_POS, DOUBLE_MAXIMUM, DOUBLE_MINIMUM, DOUBLE_ZERO};
   uintptrj_t addressDataArray[] = {(uintptrj_t)&INT_NEG, (uintptrj_t)&LONG_POS, (uintptrj_t)&BYTE_MAXIMUM, (uintptrj_t)&SHORT_MINIMUM, (uintptrj_t)&FLOAT_ZERO};

   int32_t rc = 0;
   uint32_t testCaseNum = 0;
   char resolvedMethodName [RESOLVED_METHOD_NAME_LENGTH];

   signatureCharI_I_testMethodType  *iMemCons = 0;

   //istore
   testCaseNum = sizeof(intDataArray) / sizeof(intDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      sprintf(resolvedMethodName, "iStoreConst%d", i + 1);
      EXPECT_EQ(intDataArray[i], _iStore(intDataArray[i]));
      iMemCons = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::istore, resolvedMethodName, _argTypesUnaryInt, TR::Int32, rc, 2, 1, &(intDataArray[i])));
      EXPECT_EQ(intDataArray[i], iMemCons(INT_PLACEHOLDER_1));
      }

   testCaseNum = sizeof(intDataArray) / sizeof(intDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(intDataArray[i], _iLoadi((uintptrj_t)(&intDataArray[i])));
      }

   testCaseNum = sizeof(shortDataArray) / sizeof(shortDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(shortDataArray[i], _sLoadi((uintptrj_t)(&shortDataArray[i])));
      }

   testCaseNum = sizeof(byteDataArray) / sizeof(byteDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(byteDataArray[i], _bLoadi((uintptrj_t)(&byteDataArray[i])));
      }

   testCaseNum = sizeof(longDataArray) / sizeof(longDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(longDataArray[i], _lLoadi((uintptrj_t)(&longDataArray[i])));
      }

   testCaseNum = sizeof(doubleDataArray) / sizeof(doubleDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(doubleDataArray[i], _dLoadi((uintptrj_t)(&doubleDataArray[i])));
      }

   testCaseNum = sizeof(floatDataArray) / sizeof(floatDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(floatDataArray[i], _fLoadi((uintptrj_t)(&floatDataArray[i])));
      }

   testCaseNum = sizeof(addressDataArray) / sizeof(addressDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(addressDataArray[i], _aLoadi((uintptrj_t)(&addressDataArray[i])));
      }
   }

void
OpCodesTest::invokeShiftOrRolTests()
   {
   const int8_t BYTE_MAX_STEP = 7;
   const uint8_t UBYTE_MAX_STEP = 7;
   const int16_t SHORT_MAX_STEP = 15;
   const uint16_t USHORT_MAX_STEP = 15;
   const int32_t INT_MAX_STEP = 31;
   const uint32_t UINT_MAX_STEP = 31;

   int32_t ishlDataArr[][2] =
      {
      INT_ZERO, INT_ZERO,
      INT_MINIMUM, INT_ZERO,
      INT_NEG, INT_ZERO,
      INT_POS, INT_MAX_STEP
      };
   int32_t ishrDataArr[][2] =
      {
      INT_MAXIMUM, INT_POS,
      INT_MAXIMUM, INT_MAX_STEP,
      INT_MINIMUM, INT_MAX_STEP,
      INT_NEG, INT_POS,
      INT_POS, INT_POS
      };
   uint32_t iushrDataArr[][2] =
      {
      UINT_POS , UINT_MAX_STEP,
      UINT_MINIMUM, UINT_POS,
      UINT_MAXIMUM, UINT_MINIMUM,
      UINT_POS, UINT_POS
      };

   int32_t rc = 0;
   char resolvedMethodName [RESOLVED_METHOD_NAME_LENGTH];
   uint32_t testCaseNum = 0;

   signatureCharII_I_testMethodType  * iShiftOrRolConst = 0;
   unsignedSignatureCharII_I_testMethodType  * iuShiftOrRolConst = 0;

   //ishl
   testCaseNum = sizeof(ishlDataArr) / sizeof(ishlDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(shl(ishlDataArr[i][0], ishlDataArr[i][1]), _iShl(ishlDataArr[i][0], ishlDataArr[i][1]));

      sprintf(resolvedMethodName, "iShlConst1_TestCase%d", i + 1);
      iShiftOrRolConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ishl, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &ishlDataArr[i][0], 2, &ishlDataArr[i][1]));
      EXPECT_EQ(shl(ishlDataArr[i][0], ishlDataArr[i][1]), iShiftOrRolConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iShlConst2_TestCase%d", i + 1);
      iShiftOrRolConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ishl, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &ishlDataArr[i][0]));
      EXPECT_EQ(shl(ishlDataArr[i][0], ishlDataArr[i][1]), iShiftOrRolConst(INT_PLACEHOLDER_1, ishlDataArr[i][1]));

      sprintf(resolvedMethodName, "iShlConst3_TestCase%d", i + 1);
      iShiftOrRolConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ishl, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &ishlDataArr[i][1]));
      EXPECT_EQ(shl(ishlDataArr[i][0], ishlDataArr[i][1]), iShiftOrRolConst(ishlDataArr[i][0], INT_PLACEHOLDER_2));
      }

   //ishr
   testCaseNum = sizeof(ishrDataArr) / sizeof(ishrDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(shr(ishrDataArr[i][0], ishrDataArr[i][1]), _iShr(ishrDataArr[i][0],ishrDataArr[i][1]));

      sprintf(resolvedMethodName, "iShrConst1_TestCase%d", i + 1);
      iShiftOrRolConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ishr, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &ishrDataArr[i][0], 2, &ishrDataArr[i][1]));
      EXPECT_EQ(shr(ishrDataArr[i][0], ishrDataArr[i][1]), iShiftOrRolConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iShrConst2_TestCase%d", i + 1);
      iShiftOrRolConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ishr, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &ishrDataArr[i][0]));
      EXPECT_EQ(shr(ishrDataArr[i][0], ishrDataArr[i][1]), iShiftOrRolConst(INT_PLACEHOLDER_1, ishrDataArr[i][1]));

      sprintf(resolvedMethodName, "iShrConst3_TestCase%d", i + 1);
      iShiftOrRolConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ishr, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &ishrDataArr[i][1]));
      EXPECT_EQ(shr(ishrDataArr[i][0], ishrDataArr[i][1]), iShiftOrRolConst(ishrDataArr[i][0], INT_PLACEHOLDER_2));
      }

   //iushr
   testCaseNum = sizeof(iushrDataArr) / sizeof(iushrDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(shr(iushrDataArr[i][0], iushrDataArr[i][1]), _iuShr(iushrDataArr[i][0],iushrDataArr[i][1]));

      sprintf(resolvedMethodName, "iuShrConst1_TestCase%d", i + 1);
      iuShiftOrRolConst = (unsignedSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iushr, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &iushrDataArr[i][0], 2, &iushrDataArr[i][1]));
      EXPECT_EQ(shr(iushrDataArr[i][0], iushrDataArr[i][1]), iuShiftOrRolConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iuShrConst2_TestCase%d", i + 1);
      iuShiftOrRolConst = (unsignedSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iushr, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &iushrDataArr[i][0]));
      EXPECT_EQ(shr(iushrDataArr[i][0], iushrDataArr[i][1]), iuShiftOrRolConst(INT_PLACEHOLDER_1, iushrDataArr[i][1]));

      sprintf(resolvedMethodName, "iuShrConst3_TestCase%d", i + 1);
      iuShiftOrRolConst = (unsignedSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iushr, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &iushrDataArr[i][1]));
      EXPECT_EQ(shr(iushrDataArr[i][0], iushrDataArr[i][1]), iuShiftOrRolConst(iushrDataArr[i][0], INT_PLACEHOLDER_2));
      }

   }

void
OpCodesTest::invokeUnaryTests()
   {
   int64_t longDataArray[] = {LONG_NEG, LONG_POS, LONG_MAXIMUM, LONG_MINIMUM, LONG_ZERO};
   int32_t intDataArray[] = {INT_NEG, INT_POS, INT_MAXIMUM, INT_MINIMUM, INT_ZERO};
   float floatDataArray[] = {FLOAT_NEG, FLOAT_POS, FLOAT_ZERO, FLOAT_MAXIMUM, FLOAT_MINIMUM};
   double doubleDataArray[] = {DOUBLE_NEG, DOUBLE_POS, DOUBLE_ZERO, DOUBLE_MAXIMUM, DOUBLE_MINIMUM};
   uint64_t ulongDataArray[] = {ULONG_POS, ULONG_MAXIMUM, ULONG_MINIMUM};
   uint32_t uintDataArray[] = {UINT_POS, UINT_MAXIMUM, UINT_MINIMUM};

   int16_t shortDataArray[] = {SHORT_NEG, SHORT_POS, SHORT_MAXIMUM, SHORT_MINIMUM, SHORT_ZERO};
   int8_t byteDataArray[] = {BYTE_NEG, BYTE_POS, BYTE_MAXIMUM, BYTE_MINIMUM, BYTE_ZERO};

   uint16_t ushortDataArray[] = {USHORT_POS, USHORT_MAXIMUM, USHORT_MINIMUM};
   uint8_t ubyteDataArray[] = {UBYTE_POS, UBYTE_MAXIMUM, UBYTE_MINIMUM};

   uint32_t testCaseNum = 0;
   char resolvedMethodName [RESOLVED_METHOD_NAME_LENGTH];

   int32_t rc = 0;
   signatureCharI_I_testMethodType  *iUnaryCons = 0;
   signatureCharS_S_testMethodType  *sUnaryCons = 0;
   signatureCharB_B_testMethodType  *bUnaryCons = 0;

   signatureCharI_J_testMethodType * i2lConst = 0;
   signatureCharI_B_testMethodType * i2bConst = 0;
   signatureCharI_S_testMethodType * i2sConst = 0;
   unsignedSignatureCharI_F_testMethodType * iu2fConst = 0;
   unsignedSignatureCharI_D_testMethodType * iu2dConst = 0;

   signatureCharJ_I_testMethodType * l2iConst = 0;
   signatureCharJ_B_testMethodType * l2bConst = 0;
   signatureCharJ_S_testMethodType * l2sConst = 0;
   unsignedSignatureCharJ_F_testMethodType * lu2fConst = 0;
   unsignedSignatureCharJ_D_testMethodType * lu2dConst = 0;

   signatureCharD_I_testMethodType * d2iConst = 0;
   signatureCharF_I_testMethodType * f2iConst = 0;

   //ineg
   testCaseNum = sizeof(intDataArray) / sizeof(intDataArray[0]);
   for (uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(neg(intDataArray[i]), _iNeg(intDataArray[i]));
      sprintf(resolvedMethodName, "iNegConst%d", i + 1);
      iUnaryCons = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::ineg,
            resolvedMethodName, _argTypesUnaryInt, TR::Int32, rc, 2, 1, &intDataArray[i]));
      EXPECT_EQ(neg(intDataArray[i]), iUnaryCons(INT_PLACEHOLDER_1));
      }

   //iabs
   testCaseNum = sizeof(intDataArray) / sizeof(intDataArray[0]);
   for (uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(abs(intDataArray[i]), _iAbs(intDataArray[i]));
      sprintf(resolvedMethodName, "iAbsConst%d", i + 1);
      iUnaryCons = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::iabs,
            resolvedMethodName, _argTypesUnaryInt, TR::Int32, rc, 2, 1, &intDataArray[i]));
      EXPECT_EQ(abs(intDataArray[i]), iUnaryCons(INT_PLACEHOLDER_1));
      }

   //return group
   testCaseNum = sizeof(intDataArray) / sizeof(intDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      sprintf(resolvedMethodName, "iReturnCons%d", i + 1);
      EXPECT_EQ(intDataArray[i], _iReturn(intDataArray[i]));
      iUnaryCons = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::ireturn, resolvedMethodName, _argTypesUnaryInt, TR::Int32, rc, 2, 1, &(intDataArray[i])));
      EXPECT_EQ(intDataArray[i], iUnaryCons(INT_PLACEHOLDER_1));
      }

   //const
   testCaseNum = sizeof(intDataArray) / sizeof(intDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      sprintf(resolvedMethodName, "iConst%d", i + 1);
      iUnaryCons = (signatureCharI_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::iconst, resolvedMethodName, _argTypesUnaryInt, TR::Int32, rc, 2, 1, &(intDataArray[i])));
      EXPECT_EQ(intDataArray[i], iUnaryCons(INT_PLACEHOLDER_1));
      }

   testCaseNum = sizeof(byteDataArray) / sizeof(byteDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      sprintf(resolvedMethodName, "bConst%d", i + 1);
      bUnaryCons = (signatureCharB_B_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::bconst, resolvedMethodName, _argTypesUnaryByte, TR::Int8, rc, 2, 1, &(byteDataArray[i])));
      EXPECT_EQ(byteDataArray[i], bUnaryCons(BYTE_PLACEHOLDER_1));
      }

   testCaseNum = sizeof(shortDataArray) / sizeof(shortDataArray[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      sprintf(resolvedMethodName, "sConst%d", i + 1);
      sUnaryCons = (signatureCharS_S_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::sconst, resolvedMethodName, _argTypesUnaryShort, TR::Int16, rc, 2, 1, &(shortDataArray[i])));
      EXPECT_EQ(shortDataArray[i], sUnaryCons(SHORT_PLACEHOLDER_1));
      }

   //int 2 l,b,s
   testCaseNum = sizeof(intDataArray) / sizeof(intDataArray[0]);
   for (uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(convert(intDataArray[i], BYTE_POS), _i2b(intDataArray[i]));
      EXPECT_EQ(convert(intDataArray[i], SHORT_POS), _i2s(intDataArray[i]));
      EXPECT_EQ(convert(intDataArray[i], LONG_POS), _i2l(intDataArray[i]));

      sprintf(resolvedMethodName, "i2bConst%d", i + 1);
      i2bConst = (signatureCharI_B_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::i2b,
            resolvedMethodName, _argTypesUnaryInt, TR::Int8, rc, 2, 1, &intDataArray[i]));
      EXPECT_EQ(convert(intDataArray[i], BYTE_POS), i2bConst(INT_PLACEHOLDER_1));


      sprintf(resolvedMethodName, "i2sConst%d", i + 1);
      i2sConst = (signatureCharI_S_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::i2s,
            resolvedMethodName, _argTypesUnaryInt, TR::Int16, rc, 2, 1, &intDataArray[i]));
      EXPECT_EQ(convert(intDataArray[i], SHORT_POS), i2sConst(INT_PLACEHOLDER_1));

      sprintf(resolvedMethodName, "i2lConst%d", i + 1);
      i2lConst = (signatureCharI_J_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::i2l,
            resolvedMethodName, _argTypesUnaryInt, TR::Int64, rc, 2, 1, &intDataArray[i]));
      EXPECT_EQ(convert(intDataArray[i], LONG_POS), i2lConst(INT_PLACEHOLDER_1));
      }

   //l 2 i,b,s
   testCaseNum = sizeof(longDataArray) / sizeof(longDataArray[0]);
   for (uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(convert(longDataArray[i], BYTE_POS), _l2b(longDataArray[i]));
      EXPECT_EQ(convert(longDataArray[i], SHORT_POS), _l2s(longDataArray[i]));
      EXPECT_EQ(convert(longDataArray[i], INT_POS), _l2i(longDataArray[i]));

      sprintf(resolvedMethodName, "l2bConst%d", i + 1);
      l2bConst = (signatureCharJ_B_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::l2b,
            resolvedMethodName, _argTypesUnaryLong, TR::Int8, rc, 2, 1, &longDataArray[i]));
      EXPECT_EQ(convert(longDataArray[i], BYTE_POS), l2bConst(LONG_PLACEHOLDER_1));

      sprintf(resolvedMethodName, "l2sConst%d", i + 1);
      l2sConst = (signatureCharJ_S_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::l2s,
            resolvedMethodName, _argTypesUnaryLong, TR::Int16, rc, 2, 1, &longDataArray[i]));
      EXPECT_EQ(convert(longDataArray[i], SHORT_POS), l2sConst(LONG_PLACEHOLDER_1));

      sprintf(resolvedMethodName, "l2iConst%d", i + 1);
      l2iConst = (signatureCharJ_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::l2i,
            resolvedMethodName, _argTypesUnaryLong, TR::Int32, rc, 2, 1, &longDataArray[i]));
      EXPECT_EQ(convert(longDataArray[i], INT_POS), l2iConst(LONG_PLACEHOLDER_1));
      }

   //f2i
   //Temporarily postpone converting FLOAT_MAXIMUM and FLOAT_MINIMUM to int and long.
   //the behavior is undefined since the truncated value of the min and max numbers
   //cannot be represented in the destination types. This operation is lack of helper,
   //which will lead to Segmentation fault, use i < 3 temporarily.
   //When the helper for f2i MAXIMUM and MINIMUM is done, this part should be moved
   //back to the loop above.
   for (uint32_t i = 0; i < 3; ++i)
      {
      EXPECT_EQ(convert(floatDataArray[i], INT_POS), _f2i(floatDataArray[i]));

      sprintf(resolvedMethodName, "f2iConst%d", i + 1);
      f2iConst = (signatureCharF_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::f2i,
            resolvedMethodName, _argTypesUnaryFloat, TR::Int32, rc, 2, 1, &floatDataArray[i]));
      EXPECT_EQ(convert(floatDataArray[i], INT_POS), f2iConst(FLOAT_PLACEHOLDER_1));
      }

   //d2i
   //Temporarily postpone converting DOUBLE_MAXIMUM and DOUBLE_MINIMUM to int and long.
   //the behavior is undefined since the truncated value of the min and max numbers
   //cannot be represented in the destination types. This operation is lack of helper,
   //which will lead to Segmentation fault, use i < 3 temporarily
   //When the helper for d2i MAXIMUM and MINIMUM is done, this part should be moved
   //back to the loop above.
   for (uint32_t i = 0; i < 3; ++i)
      {
      EXPECT_EQ(convert(doubleDataArray[i], INT_POS), _d2i(doubleDataArray[i]));

      sprintf(resolvedMethodName, "d2iConst%d", i + 1);
      d2iConst = (signatureCharD_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::d2i,
            resolvedMethodName, _argTypesUnaryDouble, TR::Int32, rc, 2, 1, &doubleDataArray[i]));
      EXPECT_EQ(convert(doubleDataArray[i], INT_POS), d2iConst(DOUBLE_PLACEHOLDER_1));
      }

   }

//This function is used to test the JIT helper for converting MAXIMUM and MINIMUM from
//float or double to int or long. When JIT helper can handle these conversion properly,
//these tests will be moved back to incokeUnaryTest() function. This function is
//disabled by default. To enable this function, uncomment in google test framework.
void
OpCodesTest::invokeNoHelperUnaryTests()
   {
   int32_t rc = 0;

   _f2i = (signatureCharF_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::f2i, "f2i", _argTypesUnaryFloat, TR::Int32, rc));
   _d2i = (signatureCharD_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::d2i, "d2i", _argTypesUnaryDouble, TR::Int32, rc));

   _f2i(FLOAT_MAXIMUM);
   _f2i(FLOAT_MINIMUM);
   _d2i(DOUBLE_MAXIMUM);
   _d2i(DOUBLE_MINIMUM);
   }

void
OpCodesTest::invokeBitwiseTests()
   {
   int32_t rc = 0;
   char resolvedMethodName [RESOLVED_METHOD_NAME_LENGTH];
   uint32_t testCaseNum = 0;

   int32_t intAndArr [][2] =
      {
      INT_ZERO, INT_NEG,
      INT_POS, INT_MAXIMUM,
      INT_NEG, INT_MINIMUM,
      INT_MAXIMUM, INT_POS,
      INT_MAXIMUM, INT_MINIMUM,
      INT_POS, INT_POS,
      INT_NEG, INT_MAXIMUM
      };
   int32_t intOrArr [][2] =
      {
      INT_MINIMUM, INT_POS
      };
   int32_t intXorArr [][2] =
      {
      INT_MAXIMUM, INT_ZERO,
      INT_MINIMUM, INT_MINIMUM
      };

   signatureCharII_I_testMethodType * iBitwiseConst = 0;

   //iand
   testCaseNum = sizeof(intAndArr) / sizeof(intAndArr[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(tand(intAndArr[i][0], intAndArr[i][1]), _iAnd(intAndArr[i][0], intAndArr[i][1]));

      sprintf(resolvedMethodName, "iAndConst1_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iand, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(intAndArr[i][0]), 2, &(intAndArr[i][1])));
      EXPECT_EQ(tand(intAndArr[i][0], intAndArr[i][1]), iBitwiseConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iAndConst2_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iand, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(intAndArr[i][0])));
      EXPECT_EQ(tand(intAndArr[i][0], intAndArr[i][1]), iBitwiseConst(INT_PLACEHOLDER_1, intAndArr[i][1]));

      sprintf(resolvedMethodName, "iAndConst3_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iand, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(intAndArr[i][1])));
      EXPECT_EQ(tand(intAndArr[i][0], intAndArr[i][1]), iBitwiseConst(intAndArr[i][0], INT_PLACEHOLDER_2));
     }

   //ior
   testCaseNum = sizeof(intOrArr) / sizeof(intOrArr[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(tor(intOrArr[i][0], intOrArr[i][1]), _iOr(intOrArr[i][0], intOrArr[i][1]));

      sprintf(resolvedMethodName, "iOrConst1_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ior, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(intOrArr[i][0]), 2, &(intOrArr[i][1])));
      EXPECT_EQ(tor(intOrArr[i][0], intOrArr[i][1]), iBitwiseConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iOrConst2_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ior, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(intOrArr[i][0])));
      EXPECT_EQ(tor(intOrArr[i][0], intOrArr[i][1]), iBitwiseConst(INT_PLACEHOLDER_1, intOrArr[i][1]));

      sprintf(resolvedMethodName, "iOrConst3_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ior, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(intOrArr[i][1])));
      EXPECT_EQ(tor(intOrArr[i][0], intOrArr[i][1]), iBitwiseConst(intOrArr[i][0], INT_PLACEHOLDER_2));
     }

   //ixor
   testCaseNum = sizeof(intXorArr) / sizeof(intXorArr[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(txor(intXorArr[i][0], intXorArr[i][1]), _iXor(intXorArr[i][0], intXorArr[i][1]));

      sprintf(resolvedMethodName, "iXorConst1_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ixor, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(intXorArr[i][0]), 2, &(intXorArr[i][1])));
      EXPECT_EQ(txor(intXorArr[i][0], intXorArr[i][1]), iBitwiseConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iXorConst2_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ixor, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(intXorArr[i][0])));
      EXPECT_EQ(txor(intXorArr[i][0], intXorArr[i][1]), iBitwiseConst(INT_PLACEHOLDER_1, intXorArr[i][1]));

      sprintf(resolvedMethodName, "iXorConst3_TestCase%d", i + 1);
      iBitwiseConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ixor, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(intXorArr[i][1])));
      EXPECT_EQ(txor(intXorArr[i][0], intXorArr[i][1]), iBitwiseConst(intXorArr[i][0], INT_PLACEHOLDER_2));
     }
   }

void
OpCodesTest::invokeDisabledOpCodesTests()
   {

   //frem
   EXPECT_FLOAT_EQ(remainderf(FLOAT_MINIMUM, FLOAT_MINIMUM), _fRem(FLOAT_MINIMUM, FLOAT_MINIMUM));
   EXPECT_FLOAT_EQ(remainderf(FLOAT_ZERO, FLOAT_MAXIMUM), _fRem(FLOAT_ZERO, FLOAT_MAXIMUM));
   EXPECT_FLOAT_EQ(remainderf(FLOAT_POS, FLOAT_NEG), _fRem(FLOAT_POS, FLOAT_NEG));
   EXPECT_FLOAT_EQ(remainderf(FLOAT_MAXIMUM, FLOAT_POS), _fRem(FLOAT_MAXIMUM, FLOAT_POS));


   //drem
   EXPECT_DOUBLE_EQ(remainder(DOUBLE_MINIMUM, DOUBLE_MINIMUM), _dRem(DOUBLE_MINIMUM, DOUBLE_MINIMUM));
   EXPECT_DOUBLE_EQ(remainder(DOUBLE_ZERO, DOUBLE_MAXIMUM), _dRem(DOUBLE_ZERO, DOUBLE_MAXIMUM));
   EXPECT_DOUBLE_EQ(remainder(DOUBLE_POS, DOUBLE_NEG), _dRem(DOUBLE_POS, DOUBLE_NEG));
   EXPECT_DOUBLE_EQ(remainder(DOUBLE_MAXIMUM, DOUBLE_POS), _dRem(DOUBLE_MAXIMUM, DOUBLE_POS));
   }

void
OpCodesTest::UnsupportedOpCodesTests()
   {
   //bdiv, bmul, brem
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::bdiv, "bDiv", _argTypesBinaryByte, TR::Int8);
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::bmul, "bMul", _argTypesBinaryByte, TR::Int8);
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::brem, "bRem", _argTypesBinaryByte, TR::Int8);

   //sdiv, smul, srem
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::sdiv, "sDiv", _argTypesBinaryShort, TR::Int16);
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::smul, "sMul", _argTypesBinaryShort, TR::Int16);
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::srem, "sRem", _argTypesBinaryShort, TR::Int16);

   //bucmplt, bucmple, bucmpgt, bucmpge
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::bucmplt, "buCmplt", _argTypesBinaryByte, TR::Int32);
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::bucmpge, "buCmpge", _argTypesBinaryByte, TR::Int32);
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::bucmpgt, "buCmpgt", _argTypesBinaryByte, TR::Int32);
   addUnsupportedOpCodeTest(_numberOfBinaryArgs, TR::bucmple, "buCmple", _argTypesBinaryByte, TR::Int32);
   }

void
OpCodesTest::invokeCompareTests()
   {

   //Compare op codes data array
   int32_t iCmpeqDataArr[][2] =
         {
         INT_MINIMUM, INT_MAXIMUM,
         INT_POS, INT_POS
         };
   int32_t iCmpneDataArr[][2] =
         {
         INT_MAXIMUM, INT_NEG,
         INT_NEG, INT_NEG
         };
   int32_t iCmpgtDataArr[][2] =
         {
         INT_NEG, INT_MINIMUM,
         INT_MINIMUM, INT_NEG
         };
   int32_t iCmpltDataArr[][2] =
         {
         INT_MINIMUM, INT_ZERO,
         INT_ZERO, INT_MINIMUM
         };
   int32_t iCmpgeDataArr[][2] =
         {
         INT_ZERO, INT_POS,
         INT_POS, INT_ZERO
         };
   int32_t iCmpleDataArr[][2] =
         {
         INT_POS, INT_ZERO,
         INT_ZERO, INT_POS
         };
   int64_t lCmpneDataArr[][2] =
         {
         LONG_ZERO, LONG_ZERO,
         LONG_MAXIMUM, LONG_MINIMUM
         };
   int64_t lCmpgtDataArr[][2] =
         {
         LONG_MINIMUM, LONG_ZERO,
         LONG_ZERO, LONG_MINIMUM
         };
   int64_t lCmpgeDataArr[][2] =
         {
         LONG_POS,LONG_NEG,
         LONG_NEG,LONG_POS
         };
   int64_t lCmpleDataArr[][2] =
         {
         LONG_NEG, LONG_POS,
         LONG_POS, LONG_NEG
         };
   uint32_t iuCmpgtDataArr[][2] =
         {
         UINT_POS, UINT_MINIMUM,
         UINT_MINIMUM, UINT_POS,
         UINT_POS, UINT_POS
         };
   uint32_t iuCmpltDataArr[][2] =
         {
         UINT_MAXIMUM, UINT_POS,
         UINT_POS, UINT_MAXIMUM,
         UINT_MINIMUM, UINT_POS,
         UINT_POS, UINT_MINIMUM
         };
   uint32_t iuCmpleDataArr[][2] =
         {
         UINT_MINIMUM, UINT_MINIMUM,
         UINT_POS, UINT_MAXIMUM,
         UINT_MAXIMUM, UINT_POS
         };
   uint64_t luCmpeqDataArr[][2] =
         {
         ULONG_POS, ULONG_MINIMUM,
         ULONG_MINIMUM, ULONG_POS,
         ULONG_MAXIMUM, ULONG_MINIMUM,
         ULONG_MINIMUM, ULONG_MAXIMUM,
         ULONG_MAXIMUM, ULONG_MAXIMUM
         };
   uint64_t luCmpneDataArr[][2] =
         {
         ULONG_MAXIMUM, ULONG_POS,
         ULONG_POS, ULONG_MAXIMUM,
         ULONG_MINIMUM, ULONG_POS,
         ULONG_POS, ULONG_MINIMUM,
         ULONG_MAXIMUM, ULONG_MAXIMUM
         };
   uint64_t luCmpgtDataArr[][2] =
         {
         ULONG_MINIMUM, ULONG_MAXIMUM,
         ULONG_MAXIMUM, ULONG_MINIMUM
         };
   uint64_t luCmpltDataArr[][2] =
         {
         ULONG_MAXIMUM, ULONG_MINIMUM,
         ULONG_MINIMUM, ULONG_MAXIMUM

         };
   uint64_t luCmpgeDataArr[][2] =
         {
         ULONG_POS, ULONG_MINIMUM,
         ULONG_MINIMUM, ULONG_POS

         };
   uint64_t luCmpleDataArr[][2] =
         {
         ULONG_MAXIMUM, ULONG_POS,
         ULONG_POS, ULONG_MAXIMUM
         };

   //CompareAndBranch data array
   int32_t ifIcmpeqDataArr[][2] =
         {
         INT_MINIMUM, INT_MAXIMUM,
         INT_POS, INT_POS
         };
   int32_t ifIcmpneDataArr[][2] =
         {
         INT_MAXIMUM, INT_NEG,
         INT_NEG, INT_NEG
         };
   int32_t ifIcmpgtDataArr[][2] =
         {
         INT_NEG, INT_MINIMUM,
         INT_MINIMUM, INT_NEG
         };
   int32_t ifIcmpltDataArr[][2] =
         {
         INT_MINIMUM, INT_ZERO,
         INT_ZERO, INT_MINIMUM
         };
   int32_t ifIcmpgeDataArr[][2] =
         {
         INT_ZERO, INT_POS,
         INT_POS, INT_ZERO
         };
   int32_t ifIcmpleDataArr[][2] =
         {
         INT_POS, INT_ZERO,
         INT_ZERO, INT_POS
         };
   int64_t ifLcmpneDataArr[][2] =
         {
         LONG_ZERO, LONG_ZERO,
         LONG_MAXIMUM, LONG_MINIMUM
         };
   int64_t ifLcmpgeDataArr[][2] =
         {
         LONG_POS,LONG_NEG,
         LONG_NEG,LONG_POS
         };
   int64_t ifLcmpleDataArr[][2] =
         {
         LONG_NEG, LONG_POS,
         LONG_POS, LONG_NEG
         };
   uint32_t ifIuCmpeqDataArr[][2] =
         {
         UINT_POS, UINT_MINIMUM,
         UINT_MINIMUM, UINT_POS,
         UINT_MAXIMUM, UINT_MINIMUM,
         UINT_MINIMUM, UINT_MAXIMUM,
         UINT_MAXIMUM, UINT_MAXIMUM
         };
   uint32_t ifIuCmpneDataArr[][2] =
         {
         UINT_MAXIMUM, UINT_POS,
         UINT_POS, UINT_MAXIMUM,
         UINT_MINIMUM, UINT_POS,
         UINT_POS, UINT_MINIMUM,
         UINT_MAXIMUM, UINT_MAXIMUM
         };
   uint32_t ifIuCmpgtDataArr[][2] =
         {
         UINT_MAXIMUM, UINT_MAXIMUM,
         UINT_MAXIMUM, UINT_POS
         };
   uint32_t ifIuCmpltDataArr[][2] =
         {
         UINT_MINIMUM, UINT_POS,
         UINT_POS, UINT_MINIMUM
         };
   uint32_t ifIuCmpgeDataArr[][2] =
         {
         UINT_POS, UINT_MINIMUM,
         UINT_MINIMUM, UINT_POS
         };
   uint32_t ifIuCmpleDataArr[][2] =
         {
         UINT_POS, UINT_MINIMUM,
         UINT_MINIMUM, UINT_POS
         };
   uint64_t ifLuCmpeqDataArr[][2] =
         {
         ULONG_POS, ULONG_MINIMUM,
         ULONG_MINIMUM, ULONG_POS,
         ULONG_MAXIMUM, ULONG_MINIMUM,
         ULONG_MINIMUM, ULONG_MAXIMUM,
         ULONG_MAXIMUM, ULONG_MAXIMUM
         };
   uint64_t ifLuCmpneDataArr[][2] =
         {
         ULONG_MAXIMUM, ULONG_POS,
         ULONG_POS, ULONG_MAXIMUM,
         ULONG_MINIMUM, ULONG_POS,
         ULONG_POS, ULONG_MINIMUM,
         ULONG_MAXIMUM, ULONG_MAXIMUM
         };
   uint64_t ifLuCmpgtDataArr[][2] =
         {
         ULONG_MINIMUM, ULONG_MINIMUM,
         ULONG_MAXIMUM, ULONG_POS
         };
   uint64_t ifLuCmpltDataArr[][2] =
         {
         ULONG_POS,ULONG_MAXIMUM,
         ULONG_MAXIMUM, ULONG_POS
         };
   uint64_t ifLuCmpgeDataArr[][2] =
         {
         ULONG_MAXIMUM, ULONG_POS,
         ULONG_POS,ULONG_MAXIMUM
         };
   uint64_t ifLuCmpleDataArr[][2] =
         {
         ULONG_MAXIMUM, ULONG_MAXIMUM,
         ULONG_POS, ULONG_MINIMUM
         };

   int32_t rc = 0;
   int32_t testCaseNum = 0;
   char resolvedMethodName [RESOLVED_METHOD_NAME_LENGTH];

   signatureCharII_I_testMethodType * iCompareConst = 0;
   signatureCharJJ_I_testMethodType * lCompareConst = 0;
   unsignedCompareSignatureCharII_I_testMethodType * iuCompareConst = 0;
   unsignedCompareSignatureCharJJ_I_testMethodType * luCompareConst = 0;


   //iCompare
   testCaseNum = sizeof(iCmpeqDataArr) / sizeof(iCmpeqDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareEQ(iCmpeqDataArr[i][0], iCmpeqDataArr[i][1]), _iCmpeq(iCmpeqDataArr[i][0], iCmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpeqConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iCmpeqDataArr[i][0]), 2, &(iCmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(iCmpeqDataArr[i][0], iCmpeqDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iCmpeqConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iCmpeqDataArr[i][0])));
      EXPECT_EQ(compareEQ(iCmpeqDataArr[i][0], iCmpeqDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, iCmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpeqConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iCmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(iCmpeqDataArr[i][0], iCmpeqDataArr[i][1]), iCompareConst(iCmpeqDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(iCmpneDataArr) / sizeof(iCmpneDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareNE(iCmpneDataArr[i][0], iCmpneDataArr[i][1]), _iCmpne(iCmpneDataArr[i][0], iCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpneConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iCmpneDataArr[i][0]), 2, &(iCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(iCmpneDataArr[i][0], iCmpneDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iCmpneConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iCmpneDataArr[i][0])));
      EXPECT_EQ(compareNE(iCmpneDataArr[i][0], iCmpneDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, iCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpneConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(iCmpneDataArr[i][0], iCmpneDataArr[i][1]), iCompareConst(iCmpneDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(iCmpgtDataArr) / sizeof(iCmpgtDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGT(iCmpgtDataArr[i][0], iCmpgtDataArr[i][1]), _iCmpgt(iCmpgtDataArr[i][0], iCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpgtConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iCmpgtDataArr[i][0]), 2, &(iCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(iCmpgtDataArr[i][0], iCmpgtDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iCmpgtConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iCmpgtDataArr[i][0])));
      EXPECT_EQ(compareGT(iCmpgtDataArr[i][0], iCmpgtDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, iCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpgtConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(iCmpgtDataArr[i][0], iCmpgtDataArr[i][1]), iCompareConst(iCmpgtDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(iCmpltDataArr) / sizeof(iCmpltDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLT(iCmpltDataArr[i][0], iCmpltDataArr[i][1]), _iCmplt(iCmpltDataArr[i][0], iCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpltConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iCmpltDataArr[i][0]), 2, &(iCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(iCmpltDataArr[i][0], iCmpltDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iCmpltConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iCmpltDataArr[i][0])));
      EXPECT_EQ(compareLT(iCmpltDataArr[i][0], iCmpltDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, iCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpltConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(iCmpltDataArr[i][0], iCmpltDataArr[i][1]), iCompareConst(iCmpltDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(iCmpgeDataArr) / sizeof(iCmpgeDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGE(iCmpgeDataArr[i][0], iCmpgeDataArr[i][1]), _iCmpge(iCmpgeDataArr[i][0], iCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpgeConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iCmpgeDataArr[i][0]), 2, &(iCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(iCmpgeDataArr[i][0], iCmpgeDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iCmpgeConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iCmpgeDataArr[i][0])));
      EXPECT_EQ(compareGE(iCmpgeDataArr[i][0], iCmpgeDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, iCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpgeConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(iCmpgeDataArr[i][0], iCmpgeDataArr[i][1]), iCompareConst(iCmpgeDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(iCmpleDataArr) / sizeof(iCmpleDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLE(iCmpleDataArr[i][0], iCmpleDataArr[i][1]), _iCmple(iCmpleDataArr[i][0], iCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpleConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iCmpleDataArr[i][0]), 2, &(iCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(iCmpleDataArr[i][0], iCmpleDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iCmpleConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iCmpleDataArr[i][0])));
      EXPECT_EQ(compareLE(iCmpleDataArr[i][0], iCmpleDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, iCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "iCmpleConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::icmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(iCmpleDataArr[i][0], iCmpleDataArr[i][1]), iCompareConst(iCmpleDataArr[i][0], INT_PLACEHOLDER_2));
      }

   //lCompare
   testCaseNum = sizeof(lCmpneDataArr) / sizeof(lCmpneDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareNE(lCmpneDataArr[i][0], lCmpneDataArr[i][1]), _lCmpne(lCmpneDataArr[i][0], lCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "lCmpneConst1_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(lCmpneDataArr[i][0]), 2, &(lCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(lCmpneDataArr[i][0], lCmpneDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

#ifndef TR_TARGET_POWER
      sprintf(resolvedMethodName, "lCmpneConst2_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(lCmpneDataArr[i][0])));
      EXPECT_EQ(compareNE(lCmpneDataArr[i][0], lCmpneDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, lCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "lCmpneConst3_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(lCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(lCmpneDataArr[i][0], lCmpneDataArr[i][1]), lCompareConst(lCmpneDataArr[i][0], LONG_PLACEHOLDER_2));
#endif
      }

   testCaseNum = sizeof(lCmpgtDataArr) / sizeof(lCmpgtDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGT(lCmpgtDataArr[i][0], lCmpgtDataArr[i][1]), _lCmpgt(lCmpgtDataArr[i][0], lCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "lCmpgtConst1_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(lCmpgtDataArr[i][0]), 2, &(lCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(lCmpgtDataArr[i][0], lCmpgtDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

#ifndef TR_TARGET_POWER
      sprintf(resolvedMethodName, "lCmpgtConst2_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(lCmpgtDataArr[i][0])));
      EXPECT_EQ(compareGT(lCmpgtDataArr[i][0], lCmpgtDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, lCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "lCmpgtConst3_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(lCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(lCmpgtDataArr[i][0], lCmpgtDataArr[i][1]), lCompareConst(lCmpgtDataArr[i][0], LONG_PLACEHOLDER_2));
#endif
      }

   testCaseNum = sizeof(lCmpgeDataArr) / sizeof(lCmpgeDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGE(lCmpgeDataArr[i][0], lCmpgeDataArr[i][1]), _lCmpge(lCmpgeDataArr[i][0], lCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "lCmpgeConst1_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(lCmpgeDataArr[i][0]), 2, &(lCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(lCmpgeDataArr[i][0], lCmpgeDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "lCmpgeConst2_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(lCmpgeDataArr[i][0])));
      EXPECT_EQ(compareGE(lCmpgeDataArr[i][0], lCmpgeDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, lCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "lCmpgeConst3_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(lCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(lCmpgeDataArr[i][0], lCmpgeDataArr[i][1]), lCompareConst(lCmpgeDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(lCmpleDataArr) / sizeof(lCmpleDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLE(lCmpleDataArr[i][0], lCmpleDataArr[i][1]), _lCmple(lCmpleDataArr[i][0], lCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "lCmpleConst1_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(lCmpleDataArr[i][0]), 2, &(lCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(lCmpleDataArr[i][0], lCmpleDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "lCmpleConst2_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(lCmpleDataArr[i][0])));
      EXPECT_EQ(compareLE(lCmpleDataArr[i][0], lCmpleDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, lCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "lCmpleConst3_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lcmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(lCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(lCmpleDataArr[i][0], lCmpleDataArr[i][1]), lCompareConst(lCmpleDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   //iuCompare
   testCaseNum = sizeof(iuCmpgtDataArr) / sizeof(iuCmpgtDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGT(iuCmpgtDataArr[i][0], iuCmpgtDataArr[i][1]), _iuCmpgt(iuCmpgtDataArr[i][0], iuCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "iuCmpgtConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iuCmpgtDataArr[i][0]), 2, &(iuCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(iuCmpgtDataArr[i][0], iuCmpgtDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iuCmpgtConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iuCmpgtDataArr[i][0])));
      EXPECT_EQ(compareGT(iuCmpgtDataArr[i][0], iuCmpgtDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, iuCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "iuCmpgtConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iuCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(iuCmpgtDataArr[i][0], iuCmpgtDataArr[i][1]), iuCompareConst(iuCmpgtDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(iuCmpltDataArr) / sizeof(iuCmpltDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLT(iuCmpltDataArr[i][0], iuCmpltDataArr[i][1]), _iuCmplt(iuCmpltDataArr[i][0], iuCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "iuCmpltConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iuCmpltDataArr[i][0]), 2, &(iuCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(iuCmpltDataArr[i][0], iuCmpltDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iuCmpltConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iuCmpltDataArr[i][0])));
      EXPECT_EQ(compareLT(iuCmpltDataArr[i][0], iuCmpltDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, iuCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "iuCmpltConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iuCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(iuCmpltDataArr[i][0], iuCmpltDataArr[i][1]), iuCompareConst(iuCmpltDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(iuCmpleDataArr) / sizeof(iuCmpleDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLE(iuCmpleDataArr[i][0], iuCmpleDataArr[i][1]), _iuCmple(iuCmpleDataArr[i][0], iuCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "iuCmpleConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(iuCmpleDataArr[i][0]), 2, &(iuCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(iuCmpleDataArr[i][0], iuCmpleDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "iuCmpleConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(iuCmpleDataArr[i][0])));
      EXPECT_EQ(compareLE(iuCmpleDataArr[i][0], iuCmpleDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, iuCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "iuCmpleConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iucmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(iuCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(iuCmpleDataArr[i][0], iuCmpleDataArr[i][1]), iuCompareConst(iuCmpleDataArr[i][0], INT_PLACEHOLDER_2));
      }

   //luCompare
   testCaseNum = sizeof(luCmpeqDataArr) / sizeof(luCmpeqDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareEQ(luCmpeqDataArr[i][0], luCmpeqDataArr[i][1]), _luCmpeq(luCmpeqDataArr[i][0], luCmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpeqConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpeq, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(luCmpeqDataArr[i][0]), 2, &(luCmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(luCmpeqDataArr[i][0], luCmpeqDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "luCmpeqConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpeq, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(luCmpeqDataArr[i][0])));
      EXPECT_EQ(compareEQ(luCmpeqDataArr[i][0], luCmpeqDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, luCmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpeqConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpeq, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(luCmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(luCmpeqDataArr[i][0], luCmpeqDataArr[i][1]), luCompareConst(luCmpeqDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(luCmpneDataArr) / sizeof(luCmpneDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareNE(luCmpneDataArr[i][0], luCmpneDataArr[i][1]), _luCmpne(luCmpneDataArr[i][0], luCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpneConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(luCmpneDataArr[i][0]), 2, &(luCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(luCmpneDataArr[i][0], luCmpneDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "luCmpneConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(luCmpneDataArr[i][0])));
      EXPECT_EQ(compareNE(luCmpneDataArr[i][0], luCmpneDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, luCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpneConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(luCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(luCmpneDataArr[i][0], luCmpneDataArr[i][1]), luCompareConst(luCmpneDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(luCmpgtDataArr) / sizeof(luCmpgtDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGT(luCmpgtDataArr[i][0], luCmpgtDataArr[i][1]), _luCmpgt(luCmpgtDataArr[i][0], luCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpgtConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(luCmpgtDataArr[i][0]), 2, &(luCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(luCmpgtDataArr[i][0], luCmpgtDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "luCmpgtConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(luCmpgtDataArr[i][0])));
      EXPECT_EQ(compareGT(luCmpgtDataArr[i][0], luCmpgtDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, luCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpgtConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(luCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(luCmpgtDataArr[i][0], luCmpgtDataArr[i][1]), luCompareConst(luCmpgtDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(luCmpltDataArr) / sizeof(luCmpltDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLT(luCmpltDataArr[i][0], luCmpltDataArr[i][1]), _luCmplt(luCmpltDataArr[i][0], luCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpltConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmplt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(luCmpltDataArr[i][0]), 2, &(luCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(luCmpltDataArr[i][0], luCmpltDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "luCmpltConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmplt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(luCmpltDataArr[i][0])));
      EXPECT_EQ(compareLT(luCmpltDataArr[i][0], luCmpltDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, luCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpltConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmplt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(luCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(luCmpltDataArr[i][0], luCmpltDataArr[i][1]), luCompareConst(luCmpltDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(luCmpgeDataArr) / sizeof(luCmpgeDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGE(luCmpgeDataArr[i][0], luCmpgeDataArr[i][1]), _luCmpge(luCmpgeDataArr[i][0], luCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpgeConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(luCmpgeDataArr[i][0]), 2, &(luCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(luCmpgeDataArr[i][0], luCmpgeDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "luCmpgeConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(luCmpgeDataArr[i][0])));
      EXPECT_EQ(compareGE(luCmpgeDataArr[i][0], luCmpgeDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, luCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpgeConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(luCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(luCmpgeDataArr[i][0], luCmpgeDataArr[i][1]), luCompareConst(luCmpgeDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(luCmpleDataArr) / sizeof(luCmpleDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLE(luCmpleDataArr[i][0], luCmpleDataArr[i][1]), _luCmple(luCmpleDataArr[i][0], luCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpleConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(luCmpleDataArr[i][0]), 2, &(luCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(luCmpleDataArr[i][0], luCmpleDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "luCmpleConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(luCmpleDataArr[i][0])));
      EXPECT_EQ(compareLE(luCmpleDataArr[i][0], luCmpleDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, luCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "luCmpleConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::lucmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(luCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(luCmpleDataArr[i][0], luCmpleDataArr[i][1]), luCompareConst(luCmpleDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   //ifiCompare
   testCaseNum = sizeof(ifIcmpeqDataArr) / sizeof(ifIcmpeqDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareEQ(ifIcmpeqDataArr[i][0], ifIcmpeqDataArr[i][1]), _ifIcmpeq(ifIcmpeqDataArr[i][0], ifIcmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpeqConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIcmpeqDataArr[i][0]), 2, &(ifIcmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(ifIcmpeqDataArr[i][0], ifIcmpeqDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIcmpeqConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIcmpeqDataArr[i][0])));
      EXPECT_EQ(compareEQ(ifIcmpeqDataArr[i][0], ifIcmpeqDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, ifIcmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpeqConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIcmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(ifIcmpeqDataArr[i][0], ifIcmpeqDataArr[i][1]), iCompareConst(ifIcmpeqDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIcmpneDataArr) / sizeof(ifIcmpneDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareNE(ifIcmpneDataArr[i][0], ifIcmpneDataArr[i][1]), _ifIcmpne(ifIcmpneDataArr[i][0], ifIcmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpneConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIcmpneDataArr[i][0]), 2, &(ifIcmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(ifIcmpneDataArr[i][0], ifIcmpneDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIcmpneConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIcmpneDataArr[i][0])));
      EXPECT_EQ(compareNE(ifIcmpneDataArr[i][0], ifIcmpneDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, ifIcmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpneConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIcmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(ifIcmpneDataArr[i][0], ifIcmpneDataArr[i][1]), iCompareConst(ifIcmpneDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIcmpgtDataArr) / sizeof(ifIcmpgtDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGT(ifIcmpgtDataArr[i][0], ifIcmpgtDataArr[i][1]), _ifIcmpgt(ifIcmpgtDataArr[i][0], ifIcmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpgtConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIcmpgtDataArr[i][0]), 2, &(ifIcmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(ifIcmpgtDataArr[i][0], ifIcmpgtDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIcmpgtConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIcmpgtDataArr[i][0])));
      EXPECT_EQ(compareGT(ifIcmpgtDataArr[i][0], ifIcmpgtDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, ifIcmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpgtConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIcmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(ifIcmpgtDataArr[i][0], ifIcmpgtDataArr[i][1]), iCompareConst(ifIcmpgtDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIcmpltDataArr) / sizeof(ifIcmpltDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLT(ifIcmpltDataArr[i][0], ifIcmpltDataArr[i][1]), _ifIcmplt(ifIcmpltDataArr[i][0], ifIcmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpltConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIcmpltDataArr[i][0]), 2, &(ifIcmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(ifIcmpltDataArr[i][0], ifIcmpltDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIcmpltConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIcmpltDataArr[i][0])));
      EXPECT_EQ(compareLT(ifIcmpltDataArr[i][0], ifIcmpltDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, ifIcmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpltConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIcmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(ifIcmpltDataArr[i][0], ifIcmpltDataArr[i][1]), iCompareConst(ifIcmpltDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIcmpgeDataArr) / sizeof(ifIcmpgeDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGE(ifIcmpgeDataArr[i][0], ifIcmpgeDataArr[i][1]), _ifIcmpge(ifIcmpgeDataArr[i][0], ifIcmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpgeConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIcmpgeDataArr[i][0]), 2, &(ifIcmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(ifIcmpgeDataArr[i][0], ifIcmpgeDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIcmpgeConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIcmpgeDataArr[i][0])));
      EXPECT_EQ(compareGE(ifIcmpgeDataArr[i][0], ifIcmpgeDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, ifIcmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpgeConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIcmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(ifIcmpgeDataArr[i][0], ifIcmpgeDataArr[i][1]), iCompareConst(ifIcmpgeDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIcmpleDataArr) / sizeof(ifIcmpleDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLE(ifIcmpleDataArr[i][0], ifIcmpleDataArr[i][1]), _ifIcmple(ifIcmpleDataArr[i][0], ifIcmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpleConst1_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIcmpleDataArr[i][0]), 2, &(ifIcmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(ifIcmpleDataArr[i][0], ifIcmpleDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIcmpleConst2_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIcmpleDataArr[i][0])));
      EXPECT_EQ(compareLE(ifIcmpleDataArr[i][0], ifIcmpleDataArr[i][1]), iCompareConst(INT_PLACEHOLDER_1, ifIcmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIcmpleConst3_TestCase%d", i + 1);
      iCompareConst = (signatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ificmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIcmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(ifIcmpleDataArr[i][0], ifIcmpleDataArr[i][1]), iCompareConst(ifIcmpleDataArr[i][0], INT_PLACEHOLDER_2));
      }

   //iflCompare
   testCaseNum = sizeof(ifLcmpneDataArr) / sizeof(ifLcmpneDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareNE(ifLcmpneDataArr[i][0], ifLcmpneDataArr[i][1]), _ifLcmpne(ifLcmpneDataArr[i][0], ifLcmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLcmpneConst1_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLcmpneDataArr[i][0]), 2, &(ifLcmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(ifLcmpneDataArr[i][0], ifLcmpneDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

#ifndef TR_TARGET_POWER
      sprintf(resolvedMethodName, "ifLcmpneConst2_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLcmpneDataArr[i][0])));
      EXPECT_EQ(compareNE(ifLcmpneDataArr[i][0], ifLcmpneDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, ifLcmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLcmpneConst3_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLcmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(ifLcmpneDataArr[i][0], ifLcmpneDataArr[i][1]), lCompareConst(ifLcmpneDataArr[i][0], LONG_PLACEHOLDER_2));
#endif
      }

   testCaseNum = sizeof(ifLcmpgeDataArr) / sizeof(ifLcmpgeDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGE(ifLcmpgeDataArr[i][0], ifLcmpgeDataArr[i][1]), _ifLcmpge(ifLcmpgeDataArr[i][0], ifLcmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLcmpgeConst1_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLcmpgeDataArr[i][0]), 2, &(ifLcmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(ifLcmpgeDataArr[i][0], ifLcmpgeDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifLcmpgeConst2_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLcmpgeDataArr[i][0])));
      EXPECT_EQ(compareGE(ifLcmpgeDataArr[i][0], ifLcmpgeDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, ifLcmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLcmpgeConst3_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLcmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(ifLcmpgeDataArr[i][0], ifLcmpgeDataArr[i][1]), lCompareConst(ifLcmpgeDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifLcmpleDataArr) / sizeof(ifLcmpleDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLE(ifLcmpleDataArr[i][0], ifLcmpleDataArr[i][1]), _ifLcmple(ifLcmpleDataArr[i][0], ifLcmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLcmpleConst1_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLcmpleDataArr[i][0]), 2, &(ifLcmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(ifLcmpleDataArr[i][0], ifLcmpleDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifLcmpleConst2_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLcmpleDataArr[i][0])));
      EXPECT_EQ(compareLE(ifLcmpleDataArr[i][0], ifLcmpleDataArr[i][1]), lCompareConst(LONG_PLACEHOLDER_1, ifLcmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLcmpleConst3_TestCase%d", i + 1);
      lCompareConst = (signatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflcmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLcmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(ifLcmpleDataArr[i][0], ifLcmpleDataArr[i][1]), lCompareConst(ifLcmpleDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   //ifiuCompare
   testCaseNum = sizeof(ifIuCmpeqDataArr) / sizeof(ifIuCmpeqDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareEQ(ifIuCmpeqDataArr[i][0], ifIuCmpeqDataArr[i][1]), _ifIuCmpeq(ifIuCmpeqDataArr[i][0], ifIuCmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpeqConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIuCmpeqDataArr[i][0]), 2, &(ifIuCmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(ifIuCmpeqDataArr[i][0], ifIuCmpeqDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIuCmpeqConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIuCmpeqDataArr[i][0])));
      EXPECT_EQ(compareEQ(ifIuCmpeqDataArr[i][0], ifIuCmpeqDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, ifIuCmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpeqConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpeq, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIuCmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(ifIuCmpeqDataArr[i][0], ifIuCmpeqDataArr[i][1]), iuCompareConst(ifIuCmpeqDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIuCmpneDataArr) / sizeof(ifIuCmpneDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareNE(ifIuCmpneDataArr[i][0], ifIuCmpneDataArr[i][1]), _ifIuCmpne(ifIuCmpneDataArr[i][0], ifIuCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpneConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIuCmpneDataArr[i][0]), 2, &(ifIuCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(ifIuCmpneDataArr[i][0], ifIuCmpneDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIuCmpneConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIuCmpneDataArr[i][0])));
      EXPECT_EQ(compareNE(ifIuCmpneDataArr[i][0], ifIuCmpneDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, ifIuCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpneConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpne, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIuCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(ifIuCmpneDataArr[i][0], ifIuCmpneDataArr[i][1]), iuCompareConst(ifIuCmpneDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIuCmpgtDataArr) / sizeof(ifIuCmpgtDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGT(ifIuCmpgtDataArr[i][0], ifIuCmpgtDataArr[i][1]), _ifIuCmpgt(ifIuCmpgtDataArr[i][0], ifIuCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpgtConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIuCmpgtDataArr[i][0]), 2, &(ifIuCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(ifIuCmpgtDataArr[i][0], ifIuCmpgtDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIuCmpgtConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIuCmpgtDataArr[i][0])));
      EXPECT_EQ(compareGT(ifIuCmpgtDataArr[i][0], ifIuCmpgtDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, ifIuCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpgtConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpgt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIuCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(ifIuCmpgtDataArr[i][0], ifIuCmpgtDataArr[i][1]), iuCompareConst(ifIuCmpgtDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIuCmpltDataArr) / sizeof(ifIuCmpltDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLT(ifIuCmpltDataArr[i][0], ifIuCmpltDataArr[i][1]), _ifIuCmplt(ifIuCmpltDataArr[i][0], ifIuCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpltConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIuCmpltDataArr[i][0]), 2, &(ifIuCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(ifIuCmpltDataArr[i][0], ifIuCmpltDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIuCmpltConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIuCmpltDataArr[i][0])));
      EXPECT_EQ(compareLT(ifIuCmpltDataArr[i][0], ifIuCmpltDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, ifIuCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpltConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmplt, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIuCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(ifIuCmpltDataArr[i][0], ifIuCmpltDataArr[i][1]), iuCompareConst(ifIuCmpltDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIuCmpgeDataArr) / sizeof(ifIuCmpgeDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGE(ifIuCmpgeDataArr[i][0], ifIuCmpgeDataArr[i][1]), _ifIuCmpge(ifIuCmpgeDataArr[i][0], ifIuCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpgeConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIuCmpgeDataArr[i][0]), 2, &(ifIuCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(ifIuCmpgeDataArr[i][0], ifIuCmpgeDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIuCmpgeConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIuCmpgeDataArr[i][0])));
      EXPECT_EQ(compareGE(ifIuCmpgeDataArr[i][0], ifIuCmpgeDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, ifIuCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpgeConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmpge, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIuCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(ifIuCmpgeDataArr[i][0], ifIuCmpgeDataArr[i][1]), iuCompareConst(ifIuCmpgeDataArr[i][0], INT_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifIuCmpleDataArr) / sizeof(ifIuCmpleDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLE(ifIuCmpleDataArr[i][0], ifIuCmpleDataArr[i][1]), _ifIuCmple(ifIuCmpleDataArr[i][0], ifIuCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpleConst1_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 4, 1, &(ifIuCmpleDataArr[i][0]), 2, &(ifIuCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(ifIuCmpleDataArr[i][0], ifIuCmpleDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifIuCmpleConst2_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 1, &(ifIuCmpleDataArr[i][0])));
      EXPECT_EQ(compareLE(ifIuCmpleDataArr[i][0], ifIuCmpleDataArr[i][1]), iuCompareConst(INT_PLACEHOLDER_1, ifIuCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "ifIuCmpleConst3_TestCase%d", i + 1);
      iuCompareConst = (unsignedCompareSignatureCharII_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::ifiucmple, resolvedMethodName, _argTypesBinaryInt, TR::Int32, rc, 2, 2, &(ifIuCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(ifIuCmpleDataArr[i][0], ifIuCmpleDataArr[i][1]), iuCompareConst(ifIuCmpleDataArr[i][0], INT_PLACEHOLDER_2));
      }

   //ifluCompare

   testCaseNum = sizeof(ifLuCmpeqDataArr) / sizeof(ifLuCmpeqDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareEQ(ifLuCmpeqDataArr[i][0], ifLuCmpeqDataArr[i][1]), _ifLuCmpeq(ifLuCmpeqDataArr[i][0], ifLuCmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpeqConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpeq, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLuCmpeqDataArr[i][0]), 2, &(ifLuCmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(ifLuCmpeqDataArr[i][0], ifLuCmpeqDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifLuCmpeqConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpeq, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLuCmpeqDataArr[i][0])));
      EXPECT_EQ(compareEQ(ifLuCmpeqDataArr[i][0], ifLuCmpeqDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, ifLuCmpeqDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpeqConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpeq, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLuCmpeqDataArr[i][1])));
      EXPECT_EQ(compareEQ(ifLuCmpeqDataArr[i][0], ifLuCmpeqDataArr[i][1]), luCompareConst(ifLuCmpeqDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifLuCmpneDataArr) / sizeof(ifLuCmpneDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareNE(ifLuCmpneDataArr[i][0], ifLuCmpneDataArr[i][1]), _ifLuCmpne(ifLuCmpneDataArr[i][0], ifLuCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpneConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLuCmpneDataArr[i][0]), 2, &(ifLuCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(ifLuCmpneDataArr[i][0], ifLuCmpneDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifLuCmpneConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLuCmpneDataArr[i][0])));
      EXPECT_EQ(compareNE(ifLuCmpneDataArr[i][0], ifLuCmpneDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, ifLuCmpneDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpneConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpne, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLuCmpneDataArr[i][1])));
      EXPECT_EQ(compareNE(ifLuCmpneDataArr[i][0], ifLuCmpneDataArr[i][1]), luCompareConst(ifLuCmpneDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifLuCmpgtDataArr) / sizeof(ifLuCmpgtDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGT(ifLuCmpgtDataArr[i][0], ifLuCmpgtDataArr[i][1]), _ifLuCmpgt(ifLuCmpgtDataArr[i][0], ifLuCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpgtConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLuCmpgtDataArr[i][0]), 2, &(ifLuCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(ifLuCmpgtDataArr[i][0], ifLuCmpgtDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifLuCmpgtConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLuCmpgtDataArr[i][0])));
      EXPECT_EQ(compareGT(ifLuCmpgtDataArr[i][0], ifLuCmpgtDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, ifLuCmpgtDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpgtConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpgt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLuCmpgtDataArr[i][1])));
      EXPECT_EQ(compareGT(ifLuCmpgtDataArr[i][0], ifLuCmpgtDataArr[i][1]), luCompareConst(ifLuCmpgtDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifLuCmpltDataArr) / sizeof(ifLuCmpltDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLT(ifLuCmpltDataArr[i][0], ifLuCmpltDataArr[i][1]), _ifLuCmplt(ifLuCmpltDataArr[i][0], ifLuCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpltConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmplt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLuCmpltDataArr[i][0]), 2, &(ifLuCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(ifLuCmpltDataArr[i][0], ifLuCmpltDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifLuCmpltConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmplt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLuCmpltDataArr[i][0])));
      EXPECT_EQ(compareLT(ifLuCmpltDataArr[i][0], ifLuCmpltDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, ifLuCmpltDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpltConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmplt, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLuCmpltDataArr[i][1])));
      EXPECT_EQ(compareLT(ifLuCmpltDataArr[i][0], ifLuCmpltDataArr[i][1]), luCompareConst(ifLuCmpltDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifLuCmpgeDataArr) / sizeof(ifLuCmpgeDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareGE(ifLuCmpgeDataArr[i][0], ifLuCmpgeDataArr[i][1]), _ifLuCmpge(ifLuCmpgeDataArr[i][0], ifLuCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpgeConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLuCmpgeDataArr[i][0]), 2, &(ifLuCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(ifLuCmpgeDataArr[i][0], ifLuCmpgeDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifLuCmpgeConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLuCmpgeDataArr[i][0])));
      EXPECT_EQ(compareGE(ifLuCmpgeDataArr[i][0], ifLuCmpgeDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, ifLuCmpgeDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpgeConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmpge, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLuCmpgeDataArr[i][1])));
      EXPECT_EQ(compareGE(ifLuCmpgeDataArr[i][0], ifLuCmpgeDataArr[i][1]), luCompareConst(ifLuCmpgeDataArr[i][0], LONG_PLACEHOLDER_2));
      }

   testCaseNum = sizeof(ifLuCmpleDataArr) / sizeof(ifLuCmpleDataArr[0]);
   for(uint32_t i = 0; i < testCaseNum; ++i)
      {
      EXPECT_EQ(compareLE(ifLuCmpleDataArr[i][0], ifLuCmpleDataArr[i][1]), _ifLuCmple(ifLuCmpleDataArr[i][0], ifLuCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpleConst1_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 4, 1, &(ifLuCmpleDataArr[i][0]), 2, &(ifLuCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(ifLuCmpleDataArr[i][0], ifLuCmpleDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, LONG_PLACEHOLDER_2));

      sprintf(resolvedMethodName, "ifLuCmpleConst2_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 1, &(ifLuCmpleDataArr[i][0])));
      EXPECT_EQ(compareLE(ifLuCmpleDataArr[i][0], ifLuCmpleDataArr[i][1]), luCompareConst(LONG_PLACEHOLDER_1, ifLuCmpleDataArr[i][1]));

      sprintf(resolvedMethodName, "ifLuCmpleConst3_TestCase%d", i + 1);
      luCompareConst = (unsignedCompareSignatureCharJJ_I_testMethodType *) (compileOpCodeMethod(
            _numberOfBinaryArgs, TR::iflucmple, resolvedMethodName, _argTypesBinaryLong, TR::Int32, rc, 2, 2, &(ifLuCmpleDataArr[i][1])));
      EXPECT_EQ(compareLE(ifLuCmpleDataArr[i][0], ifLuCmpleDataArr[i][1]), luCompareConst(ifLuCmpleDataArr[i][0], LONG_PLACEHOLDER_2));
      }
   }

void
OpCodesTest::invokeTernaryTests()
   {
   int32_t rc = 0;
   char resolvedMethodName [RESOLVED_METHOD_NAME_LENGTH];
   uint32_t testCaseNum = 0;
   uint32_t testCaseNumCheck = 0;

   int32_t iternaryChild1Arr[] =
      {
      INT_MAXIMUM, INT_MAXIMUM, INT_MINIMUM, INT_POS, INT_ZERO, INT_MINIMUM, INT_POS, INT_ZERO, INT_NEG,
      INT_NEG, INT_NEG, INT_ZERO, INT_POS, INT_NEG, INT_ZERO, INT_MINIMUM, INT_MAXIMUM, INT_POS
      };

   int32_t intArr[][2] =
      {
      INT_NEG, INT_MINIMUM,
      INT_NEG, INT_POS,
      INT_NEG, INT_POS,
      INT_MAXIMUM, INT_ZERO,
      INT_ZERO, INT_POS,
      INT_ZERO, INT_POS,
      INT_ZERO, INT_MAXIMUM,
      INT_POS, INT_NEG,
      INT_POS, INT_MINIMUM,
      INT_MAXIMUM, INT_POS,
      INT_MINIMUM, INT_ZERO,
      INT_MINIMUM, INT_MAXIMUM,
      INT_NEG, INT_MAXIMUM,
      INT_ZERO, INT_NEG,
      INT_POS, INT_ZERO,
      INT_MAXIMUM, INT_NEG,
      INT_ZERO, INT_MINIMUM,
      INT_MINIMUM, INT_POS
      };

   testCaseNum = sizeof(iternaryChild1Arr) / sizeof(iternaryChild1Arr[0]);
   testCaseNumCheck = sizeof(intArr) / sizeof(intArr[0]);
   TR_ASSERT( (testCaseNum > 0) && (testCaseNum == testCaseNumCheck), "There is problem in iternary input array");
   signatureCharIII_I_testMethodType * iTernaryConst = 0;
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      sprintf(resolvedMethodName, "iTernaryConst%d", i + 1);
      EXPECT_EQ(ternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]), _iternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]));

      iTernaryConst = (signatureCharIII_I_testMethodType *) (compileOpCodeMethod(_numberOfTernaryArgs, TR::iternary,
            resolvedMethodName, _argTypesTernaryInt, TR::Int32, rc, 6, 1, &iternaryChild1Arr[i], 2, &intArr[i][0], 3, &intArr[i][1]));
      EXPECT_EQ(ternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]), iTernaryConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2, INT_PLACEHOLDER_3));

      iTernaryConst = (signatureCharIII_I_testMethodType *) (compileOpCodeMethod(_numberOfTernaryArgs, TR::iternary,
            resolvedMethodName, _argTypesTernaryInt, TR::Int32, rc, 4, 1, &iternaryChild1Arr[i], 2, &intArr[i][0]));
      EXPECT_EQ(ternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]), iTernaryConst(INT_PLACEHOLDER_1, INT_PLACEHOLDER_2, intArr[i][1]));

      iTernaryConst = (signatureCharIII_I_testMethodType *) (compileOpCodeMethod(_numberOfTernaryArgs, TR::iternary,
            resolvedMethodName, _argTypesTernaryInt, TR::Int32, rc, 4, 1, &iternaryChild1Arr[i], 3, &intArr[i][1]));
      EXPECT_EQ(ternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]), iTernaryConst(INT_PLACEHOLDER_1, intArr[i][0], INT_PLACEHOLDER_3));

      iTernaryConst = (signatureCharIII_I_testMethodType *) (compileOpCodeMethod(_numberOfTernaryArgs, TR::iternary,
            resolvedMethodName, _argTypesTernaryInt, TR::Int32, rc, 4, 2, &intArr[i][0], 3, &intArr[i][1]));
      EXPECT_EQ(ternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]), iTernaryConst(iternaryChild1Arr[i], INT_PLACEHOLDER_2, INT_PLACEHOLDER_3));

      iTernaryConst = (signatureCharIII_I_testMethodType *) (compileOpCodeMethod(_numberOfTernaryArgs, TR::iternary,
            resolvedMethodName, _argTypesTernaryInt, TR::Int32, rc, 2, 1, &iternaryChild1Arr[i]));
      EXPECT_EQ(ternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]), iTernaryConst(INT_PLACEHOLDER_1, intArr[i][0], intArr[i][1]));

      iTernaryConst = (signatureCharIII_I_testMethodType *) (compileOpCodeMethod(_numberOfTernaryArgs, TR::iternary,
            resolvedMethodName, _argTypesTernaryInt, TR::Int32, rc, 2, 2, &intArr[i][0]));
      EXPECT_EQ(ternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]), iTernaryConst(iternaryChild1Arr[i], INT_PLACEHOLDER_1, intArr[i][1]));

      iTernaryConst = (signatureCharIII_I_testMethodType *) (compileOpCodeMethod(_numberOfTernaryArgs, TR::iternary,
            resolvedMethodName, _argTypesTernaryInt, TR::Int32, rc, 2, 3, &intArr[i][1]));
      EXPECT_EQ(ternary(iternaryChild1Arr[i], intArr[i][0], intArr[i][1]), iTernaryConst(iternaryChild1Arr[i], intArr[i][0], INT_PLACEHOLDER_1));
      }
   }

void
OpCodesTest::invokeAddressTests()
   {
   int32_t rc = 0;

   uintptrj_t aUnaryDataArr[] =
      {
      (uintptrj_t) &INT_POS,
      (uintptrj_t) &INT_MAXIMUM,
      (uintptrj_t) &INT_ZERO,
      (uintptrj_t) &LONG_POS,
      (uintptrj_t) &LONG_MAXIMUM,
      (uintptrj_t) &LONG_ZERO
      };

   uint32_t testCaseNum = 0;
   char resolvedMethodName [RESOLVED_METHOD_NAME_LENGTH];
   signatureCharL_L_testMethodType *aUnaryCons = 0;
   signatureCharL_I_testMethodType *a2iConst = 0;

   testCaseNum = sizeof(aUnaryDataArr) / sizeof(aUnaryDataArr[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(aUnaryDataArr[i], _aload(aUnaryDataArr[i]));
      }

   testCaseNum = sizeof(aUnaryDataArr) / sizeof(aUnaryDataArr[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      sprintf(resolvedMethodName, "aConst%d", i + 1);
      aUnaryCons = (signatureCharL_L_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::aconst, resolvedMethodName, _argTypesUnaryAddress, TR::Address, rc, 2, 1, &(aUnaryDataArr[i])));
      EXPECT_EQ(aUnaryDataArr[i], aUnaryCons(ADDRESS_PLACEHOLDER_1));
      }

   testCaseNum = sizeof(aUnaryDataArr) / sizeof(aUnaryDataArr[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(aUnaryDataArr[i], _areturn(aUnaryDataArr[i]));

      sprintf(resolvedMethodName, "aReturnConst%d", i + 1);
      aUnaryCons = (signatureCharL_L_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::areturn, resolvedMethodName, _argTypesUnaryAddress, TR::Address, rc, 2, 1, &(aUnaryDataArr[i])));
      EXPECT_EQ(aUnaryDataArr[i], aUnaryCons(ADDRESS_PLACEHOLDER_1));
      }

   testCaseNum = sizeof(aUnaryDataArr) / sizeof(aUnaryDataArr[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(aUnaryDataArr[i], _astore(aUnaryDataArr[i]));

      sprintf(resolvedMethodName, "aStoreConst%d", i + 1);
      aUnaryCons = (signatureCharL_L_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::astore, resolvedMethodName, _argTypesUnaryAddress, TR::Address, rc, 2, 1, &(aUnaryDataArr[i])));
      EXPECT_EQ(aUnaryDataArr[i], aUnaryCons(ADDRESS_PLACEHOLDER_1));
      }

   testCaseNum = sizeof(aUnaryDataArr) / sizeof(aUnaryDataArr[0]);
   for (int32_t i = 0 ; i < testCaseNum ; i++)
      {
      EXPECT_EQ(convert(aUnaryDataArr[i], INT_POS), _a2i(aUnaryDataArr[i]));

      sprintf(resolvedMethodName, "a2iConst%d", i + 1);
      a2iConst = (signatureCharL_I_testMethodType *) (compileOpCodeMethod(_numberOfUnaryArgs, TR::a2i, resolvedMethodName, _argTypesUnaryAddress, TR::Int32, rc, 2, 1, &aUnaryDataArr[i]));
      EXPECT_EQ(convert(aUnaryDataArr[i], INT_POS), a2iConst(ADDRESS_PLACEHOLDER_1));
      }
   }

} // namespace TestCompiler

//groups by testname
TEST(JITCrossPlatformsOpCodesTest, UnaryTest)
   {
   ::TestCompiler::OpCodesTest unaryTest;
   unaryTest.compileUnaryTestMethods();
   unaryTest.invokeUnaryTests();
   }

TEST(JITCrossPlatformsOpCodesTest, IntegerArithmeticTest)
   {
   ::TestCompiler::OpCodesTest integerArithmeticTest;
   integerArithmeticTest.compileIntegerArithmeticTestMethods();
   integerArithmeticTest.invokeIntegerArithmeticTests();
   }

TEST(JITCrossPlatformsOpCodesTest, MemoryOperationTest)
   {
   ::TestCompiler::OpCodesTest memoryOperationTest;
   memoryOperationTest.compileMemoryOperationTestMethods();
   memoryOperationTest.invokeMemoryOperationTests();
   }

TEST(JITCrossPlatformsOpCodesTest, ShiftOrRolTest)
   {
   ::TestCompiler::OpCodesTest shiftOrRolTest;
   shiftOrRolTest.compileShiftOrRolTestMethods();
   shiftOrRolTest.invokeShiftOrRolTests();
   }

TEST(JITCrossPlatformsOpCodesTest, BitwiseTest)
   {
   ::TestCompiler::OpCodesTest bitwiseTest;
   bitwiseTest.compileBitwiseMethods();
   bitwiseTest.invokeBitwiseTests();
   }

TEST(JITCrossPlatformsOpCodesTest, CompareTest)
   {
   ::TestCompiler::OpCodesTest compareTest;
   compareTest.compileCompareTestMethods();
   compareTest.invokeCompareTests();
   }

TEST(JITCrossPlatformsOpCodesTest, TernaryTest)
   {
   ::TestCompiler::OpCodesTest ternaryTest;
   ternaryTest.compileTernaryTestMethods();
   ternaryTest.invokeTernaryTests();
   }

TEST(JITCrossPlatformsOpCodesTest, AddressTest)
   {
   ::TestCompiler::OpCodesTest addressTest;
   addressTest.compileAddressTestMethods();
   addressTest.invokeAddressTests();
   }

TEST(JITCrossPlatformsOpCodesTest, UnsupportedOpCodesTest)
   {
   ::TestCompiler::OpCodesTest unsupportedOpcodesTest;
   unsupportedOpcodesTest.UnsupportedOpCodesTests();
   }

TEST(JITCrossPlatformsOpCodesTest, DISABLED_OpCodesTests)
   {
   //Jazz103 Work item 110364
   //To temporarily enable "DISABLED" test, append " --gtest_also_run_disabled_tests" in the command line.
   ::TestCompiler::OpCodesTest disabledOpCodesTest;
   disabledOpCodesTest.compileDisabledOpCodesTests();
   disabledOpCodesTest.invokeDisabledOpCodesTests();
   }


TEST(JITCrossPlatformsOpCodesTest, DISABLED_UnaryTest)
   {
   //Jazz103 Work Item 110363
   //This defect is related to 97974: Separate group to temporarily disable crashed (will work on) testcases
   //Please move this test and recover f2i testcase number from 3 to 5.
   ::TestCompiler::OpCodesTest disabledUnaryTest;
   disabledUnaryTest.invokeNoHelperUnaryTests();
   }

