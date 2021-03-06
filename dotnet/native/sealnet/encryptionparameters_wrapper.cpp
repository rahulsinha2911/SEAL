// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license.

// SEALNet
#include "sealnet/stdafx.h"
#include "sealnet/encryptionparameters_wrapper.h"
#include "sealnet/utilities.h"

// SEAL
#include "seal/encryptionparams.h"
#include "seal/smallmodulus.h"
#include "seal/util/hash.h"
#include "seal/util/common.h"

using namespace std;
using namespace seal;
using namespace sealnet;

namespace seal
{
    /**
    Enables access to private members of seal::EncryptionParameters.
    */
    struct EncryptionParameters::EncryptionParametersPrivateHelper
    {
        static auto parms_id(const EncryptionParameters &parms)
        {
            return parms.parms_id();
        }
    };
}

SEALNETNATIVE HRESULT SEALCALL EncParams_Create1(uint8_t scheme, void **enc_params)
{
    IfNullRet(enc_params, E_POINTER);

    try
    {
        EncryptionParameters *params = new EncryptionParameters(scheme);
        *enc_params = params;
        return S_OK;
    }
    catch (const invalid_argument&)
    {
        return E_INVALIDARG;
    }
}

SEALNETNATIVE HRESULT SEALCALL EncParams_Create2(void *copy, void **enc_params)
{
    EncryptionParameters *copypt = FromVoid<EncryptionParameters>(copy);
    IfNullRet(copypt, E_POINTER);
    IfNullRet(enc_params, E_POINTER);

    EncryptionParameters *params = new EncryptionParameters(*copypt);
    *enc_params = params;
    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_Destroy(void *thisptr)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);

    delete params;
    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_Set(void *thisptr, void *assign)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    EncryptionParameters *assignpt = FromVoid<EncryptionParameters>(assign);
    IfNullRet(assignpt, E_POINTER);

    *params = *assignpt;
    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_GetPolyModulusDegree(void *thisptr, uint64_t *degree)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(degree, E_POINTER);

    *degree = params->poly_modulus_degree();
    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_SetPolyModulusDegree(void *thisptr, uint64_t degree)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);

    try
    {
        params->set_poly_modulus_degree(degree);
        return S_OK;
    }
    catch (const invalid_argument&)
    {
        return E_INVALIDARG;
    }
}

SEALNETNATIVE HRESULT SEALCALL EncParams_GetCoeffModulus(void *thisptr, uint64_t *length, void **coeffs)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(length, E_POINTER);

    BuildSmallModulusPointers(params->coeff_modulus(), length, coeffs);
    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_SetCoeffModulus(void *thisptr, uint64_t length, void **coeffs)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(coeffs, E_POINTER);

    SmallModulus* *coeff_array = reinterpret_cast<SmallModulus**>(coeffs);
    vector<SmallModulus> coefficients(length);

    for (uint64_t i = 0; i < length; i++)
    {
        coefficients[i] = *coeff_array[i];
    }

    try
    {
        params->set_coeff_modulus(coefficients);
        return S_OK;
    }
    catch (const invalid_argument&)
    {
        return E_INVALIDARG;
    }
}

SEALNETNATIVE HRESULT SEALCALL EncParams_GetScheme(void *thisptr, uint8_t *scheme)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(scheme, E_POINTER);

    *scheme = static_cast<uint8_t>(params->scheme());
    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_GetParmsId(void *thisptr, uint64_t *parms_id)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(parms_id, E_POINTER);

    // We will assume the array is always size hash_block_uint64_count
    auto parmsid = EncryptionParameters::EncryptionParametersPrivateHelper::parms_id(*params);
    for (size_t i = 0; i < util::HashFunction::hash_block_uint64_count; i++)
    {
        parms_id[i] = parmsid[i];
    }

    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_GetPlainModulus(void *thisptr, void **plain_modulus)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(plain_modulus, E_POINTER);

    const auto plainmodulus = &params->plain_modulus();
    *plain_modulus = const_cast<SmallModulus*>(plainmodulus);
    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_SetPlainModulus1(void *thisptr, void *modulus)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    SmallModulus *smallmodulus = FromVoid<SmallModulus>(modulus);
    IfNullRet(smallmodulus, E_POINTER);

    try
    {
        params->set_plain_modulus(*smallmodulus);
        return S_OK;
    }
    catch (const logic_error&)
    {
        return COR_E_INVALIDOPERATION;
    }
}

SEALNETNATIVE HRESULT SEALCALL EncParams_SetPlainModulus2(void *thisptr, uint64_t plain_modulus)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);

    try
    {
        params->set_plain_modulus(plain_modulus);
        return S_OK;
    }
    catch (const logic_error&)
    {
        return COR_E_INVALIDOPERATION;
    }
}

SEALNETNATIVE HRESULT SEALCALL EncParams_Equals(void *thisptr, void *otherptr, bool *result)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    EncryptionParameters *other = FromVoid<EncryptionParameters>(otherptr);
    IfNullRet(other, E_POINTER);
    IfNullRet(result, E_POINTER);

    *result = (*params == *other);
    return S_OK;
}

SEALNETNATIVE HRESULT SEALCALL EncParams_SaveSize(void *thisptr, uint8_t compr_mode, int64_t *result)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(result, E_POINTER);

    try
    {
        *result = static_cast<int64_t>(
            params->save_size(static_cast<compr_mode_type>(compr_mode)));
        return S_OK;
    }
    catch (const invalid_argument &)
    {
        return E_INVALIDARG;
    }
    catch (const logic_error &)
    {
        return COR_E_INVALIDOPERATION;
    }
}

SEALNETNATIVE HRESULT SEALCALL EncParams_Save(void *thisptr, uint8_t *outptr, uint64_t size, uint8_t compr_mode, int64_t *out_bytes)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(outptr, E_POINTER);
    IfNullRet(out_bytes, E_POINTER);

    try
    {
        *out_bytes = util::safe_cast<int64_t>(params->save(
            reinterpret_cast<SEAL_BYTE *>(outptr),
            util::safe_cast<size_t>(size),
            static_cast<compr_mode_type>(compr_mode)));
        return S_OK;
    }
    catch (const invalid_argument &)
    {
        return E_INVALIDARG;
    }
    catch (const logic_error &)
    {
        return COR_E_INVALIDOPERATION;
    }
    catch (const runtime_error &)
    {
        return COR_E_IO;
    }
}

SEALNETNATIVE HRESULT SEALCALL EncParams_Load(void *thisptr, uint8_t *inptr, uint64_t size, int64_t *in_bytes)
{
    EncryptionParameters *params = FromVoid<EncryptionParameters>(thisptr);
    IfNullRet(params, E_POINTER);
    IfNullRet(inptr, E_POINTER);
    IfNullRet(in_bytes, E_POINTER);

    try
    {
        *in_bytes = util::safe_cast<int64_t>(params->load(
            reinterpret_cast<SEAL_BYTE *>(inptr),
            util::safe_cast<size_t>(size)));
        return S_OK;
    }
    catch (const invalid_argument &)
    {
        return E_INVALIDARG;
    }
    catch (const logic_error &)
    {
        return COR_E_INVALIDOPERATION;
    }
    catch (const runtime_error &)
    {
        return COR_E_IO;
    }
}
