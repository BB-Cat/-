
//!! This class probably works okay but it turns out I didn't need it after changing the generic settings of the
//compile functions for shaders.

//#pragma once
//#include <d3d11.h>
//#include "Prerequisites.h"
//
//class ShaderInclude : public ID3DInclude
//{
//public:
//	// ID3DInclude ÇâÓÇµÇƒåpè≥Ç≥ÇÍÇ‹ÇµÇΩ
//	virtual HRESULT __stdcall Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, 
//		LPCVOID* ppData, UINT* pBytes) override;
//	virtual HRESULT __stdcall Close(LPCVOID pData) override;
//
//private:
//	ID3DBlob* blob;
//};