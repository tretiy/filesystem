#pragma once
#include <vector>
#include <string>
SAFEARRAY* makeBSTRarray(std::vector<std::wstring>& _strings)
{
	if (_strings.empty())
		return nullptr;

	SAFEARRAYBOUND bounds[] = { { (ULONG)_strings.size(), 0 } }; //Array Contains 2 Elements starting from Index ‘0’ 

	SAFEARRAY* stringsBSTR = SafeArrayCreate(VT_VARIANT, 1, bounds); //Create a one-dimensional SafeArray of variants
	long*  lIndex = new long[_strings.size()];
	long index = 0;
	for (const auto& value : _strings)
	{
		VARIANT var;
		*lIndex = index++; // index of the element being inserted in the array
		var.vt = VT_BSTR; // type of the element being inserted 
		var.bstrVal = SysAllocStringLen(value.data(), (UINT)value.size());
		HRESULT hr = SafeArrayPutElement(stringsBSTR, lIndex, &var); // insert the element 
		std::advance(lIndex, 1);
	}

	return stringsBSTR;
}

VARIANT* makeByteVariant(std::vector<char>& _bytes)
{
	SAFEARRAY FAR*  psarray;
	SAFEARRAYBOUND sabounds[1];

	sabounds[0].lLbound = 0;
	sabounds[0].cElements = (ULONG)_bytes.size();

	long nLbound;

	psarray = SafeArrayCreate(VT_UI1, 1, sabounds);
	if (psarray == NULL)
		return nullptr;

	for (nLbound = 0; nLbound < (long)sabounds[0].cElements; nLbound++) {
		if (FAILED(SafeArrayPutElement(psarray, &nLbound, &_bytes[nLbound]))) {
			SafeArrayDestroy(psarray);
			return nullptr;
		}
	}
	VARIANT* vtResult = new VARIANT();
	VariantInit(vtResult);
	vtResult->vt = VT_ARRAY | VT_UI1;
	vtResult->parray = psarray;
	return vtResult;
}

std::vector<char>* makeByteArray(VARIANT* _bytes)
{
	if (_bytes->vt != (VT_ARRAY | VT_UI1))
		return nullptr;

	auto arraySize = _bytes->parray->rgsabound[0].cElements;
	auto retVec = new std::vector<char>(arraySize);
	long nLbound;
	for (nLbound = 0; nLbound < (long)_bytes->parray->rgsabound[0].cElements; nLbound++)
	{
		if (FAILED(SafeArrayGetElement(_bytes->parray, &nLbound, &(*retVec)[nLbound])))
		{
			delete retVec;
			return nullptr;
		}
	}

	return retVec;
}