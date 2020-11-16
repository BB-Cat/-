//#include "ShaderInclude.h"
//#include <d3dcompiler.h>
//#include <atlstr.h>
//
//HRESULT __stdcall ShaderInclude::Open(D3D_INCLUDE_TYPE IncludeType, LPCSTR pFileName, LPCVOID pParentData, LPCVOID* ppData, UINT* pBytes)
//{
//    CString D(pFileName);
//    D3DReadFileToBlob(D.GetBuffer(), &blob);
//    D.ReleaseBuffer();
//
//    (*ppData) = blob->GetBufferPointer();
//    (*pBytes) = blob->GetBufferSize();
//
//    return S_OK;
//}
//
//HRESULT __stdcall ShaderInclude::Close(LPCVOID pData)
//{
//    blob->Release();
//    return S_OK;
//}
