#include "DXUT.h"
#include "RenderObject.h"
#include "SDKmesh.h"
#include "SDKmisc.h"

CRenderObject::CRenderObject()
{
	m_pVB = NULL;
	m_pIB = NULL;
	m_dwNumIndices = 0;
	m_dwNumVertices = 0;
	m_pDecl = NULL;
	m_pMeshSysMemory = NULL;
	m_pEffect = NULL;
	m_pColorTexture = NULL;

	m_TextureSize = 512;
	m_pRTTTexture = NULL;
	m_pRTTInterface = NULL;
	m_pRTTSurface = NULL;

	m_vBaryCentric = D3DXVECTOR3(0.0, 0.0, 0.0);
	D3DXMatrixIdentity(&m_matWorldRTT);

}

CRenderObject::~CRenderObject()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pColorTexture);

	SAFE_RELEASE(m_pRTTTexture);
	SAFE_RELEASE(m_pRTTInterface);
	SAFE_RELEASE(m_pRTTSurface);

}

HRESULT CRenderObject::LoadResource(TCHAR * FileName)
{
	//打开文件
	DWORD uByte;
	//查看文件是否存在
	HANDLE theHandle = CreateFile(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == theHandle)
	{
		return E_FAIL;
	}
	//检查文件头
	SSHADERMODULEFILEHEAD sFileHead;
	BOOL bReturn = ReadFile(theHandle, (LPVOID)& sFileHead, sizeof(SSHADERMODULEFILEHEAD), &uByte, NULL);
	if (NULL == bReturn)
	{
		CloseHandle(theHandle);
		return E_FAIL;
	}
	int comp = strcmp(FILEHEADTAG, sFileHead.szHead);
	if (NULL != comp)
	{
		CloseHandle(theHandle);
		return E_FAIL;
	}
	//获取资源个数
	m_iResNum = sFileHead.s_iResourceFileNum;
	m_pResourceInfo = new SRESOURCEINFO[m_iResNum];
	if (NULL == m_pResourceInfo)
	{
		return E_FAIL;
	}
	//读取资源信息
	bReturn = ReadFile(theHandle, (LPVOID)m_pResourceInfo, sizeof(SRESOURCEINFO) * m_iResNum, &uByte, NULL);
	if (NULL == bReturn)
	{
		CloseHandle(theHandle);
		return E_FAIL;
	}
	CloseHandle(theHandle);
	return S_OK;
}

//利用资源类型索引得到文件名
TCHAR * CRenderObject::GetResourceName(RESTYPE enuResType)
{
	TCHAR* szReturn = NULL;
	for (int i = 0; i < m_iResNum; i++)
	{
		if (enuResType == m_pResourceInfo[i].s_enumResType)
		{
			szReturn = m_pResourceInfo[i].s_szFileName;
		}
	}
	return szReturn;
}

//渲染到纹理
HRESULT CRenderObject::RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice, D3DXVECTOR3 vPosition, float fTime)
{
	HRESULT hr;
	//计算3个矩阵
	D3DXVECTOR3 vUpVec(0.0, 1.0, 0.0);
	D3DXMatrixLookAtLH(&m_matViewRTT, &vPosition, &m_vBaryCentric, &vUpVec);
	D3DXHANDLE RTTTechHandle = m_pEffect->GetTechniqueByName("RenderToTextureShader");
	hr = m_pEffect->SetTechnique(RTTTechHandle);
	if (FAILED(hr))
	{
		return hr;
	}
	//D3DXMatrixRotationY(&m_matWorldRTT, fTime);

	D3DXMatrixIdentity(&m_matWorldRTT);
	D3DXHANDLE matWorldHandle = m_pEffect->GetParameterByName(0, "g_matWorldRTT");
	m_pEffect->SetMatrix(matWorldHandle, &m_matWorldRTT);
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matViewRTT");
	m_pEffect->SetMatrix(matViewHandle, &m_matViewRTT);
	D3DXHANDLE matProjectHandle = m_pEffect->GetParameterByName(0, "g_matProjectRTT");
	m_pEffect->SetMatrix(matProjectHandle, &m_matProjectRTT);
	//不能在此清除，否则不能渲染到纹理
	m_pRTTInterface->BeginScene(m_pRTTSurface, NULL);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	pd3dDevice->SetVertexDeclaration(m_pDecl);
	pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(RENDEROBJECT_D3DVERTEX));
	pd3dDevice->SetIndices(m_pIB);
	UINT uPasses;
	m_pEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE);
	for (UINT iPass = 0; iPass < uPasses; iPass++)
	{
		m_pEffect->BeginPass(iPass);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, m_dwNumIndices);
		m_pEffect->EndPass();
	}
	m_pEffect->End();
	m_pRTTInterface->EndScene(0);
	return hr;
}

HRESULT CRenderObject::SetTexture(LPDIRECT3DTEXTURE9 pTexture)
{
	D3DXHANDLE textureHandle = m_pEffect->GetParameterByName(0, "g_RenderToTexture");
	HRESULT hr = m_pEffect->SetTexture(textureHandle, pTexture);
	return hr;
}

HRESULT CRenderObject::LoadMesh(IDirect3DDevice9 * pd3dDevice, WCHAR * wstrFileName, ID3DXMesh ** ppMesh)
{
	ID3DXMesh * pMesh = NULL;
	WCHAR wstr[MAX_PATH];
	DXUTFindDXSDKMediaFileCch(wstr, MAX_PATH, wstrFileName);
	D3DXLoadMeshFromXW(wstr, D3DXMESH_MANAGED, pd3dDevice, NULL, NULL, NULL, NULL, &pMesh);
	DWORD * rgdwAdjacency = NULL;
	if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
	{
		ID3DXMesh* pTempMesh;
		pMesh->CloneMeshFVF(pMesh->GetOptions(), pMesh->GetFVF() | D3DFVF_NORMAL, pd3dDevice, &pTempMesh);
		D3DXComputeNormals(pTempMesh, NULL);
		SAFE_RELEASE(pMesh);
		pMesh = pTempMesh;
	}
	rgdwAdjacency = new DWORD[pMesh->GetNumFaces() * 3];
	if (NULL == rgdwAdjacency)
	{
		return E_OUTOFMEMORY;
	}
	pMesh->ConvertPointRepsToAdjacency(NULL, rgdwAdjacency);
	pMesh->OptimizeInplace(D3DXMESHOPT_VERTEXCACHE, rgdwAdjacency, NULL, NULL, NULL);
	delete[]rgdwAdjacency;
	*ppMesh = pMesh;
	
	return S_OK;
}

void CRenderObject::Init()
{
}

HRESULT CRenderObject::OnCreateDevice(IDirect3DDevice9 * pd3dDevice, TCHAR * FileName)
{
	HRESULT hr = S_OK;
	//装入.shm文件
	hr = LoadResource(FileName);
	if (FAILED(hr))
	{
		return hr;
	}
	//判断是否有次模型文件，并从中读取文件名
	//TCHAR * szFile = GetResourceName(ENU_MODELX);
	TCHAR* szFile = L"D:\\lifei\\test\\hlsl高级精讲\\SimpleSample\\ResFile_1\\model.x";
	LPD3DXMESH pMeshSysMemClone = NULL;
	hr = D3DXLoadMeshFromX(szFile, D3DXMESH_SYSTEMMEM, pd3dDevice, NULL, NULL, NULL, NULL, &pMeshSysMemClone);
	if (FAILED(hr))
	{
		return hr;
	}
	//以新的方式创建顶点流
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	//因为装入的mesh的顶点结构不是我们需要的，因此按照我们需要的格式复制一个
	hr = pd3dDevice->CreateVertexDeclaration(decl, &m_pDecl);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = pMeshSysMemClone->CloneMesh(D3DXMESH_MANAGED, decl, pd3dDevice, &m_pMeshSysMemory);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = m_pMeshSysMemory->GetIndexBuffer(&m_pIB);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = m_pMeshSysMemory->GetVertexBuffer(&m_pVB);
	if (FAILED(hr))
	{
		return hr;
	}
	m_dwNumIndices = m_pMeshSysMemory->GetNumFaces();
	m_dwNumVertices = m_pMeshSysMemory->GetNumVertices();
	SAFE_RELEASE(pMeshSysMemClone);
	
	//获取模型范围和中心点位置
	D3DXVECTOR3* pData;
	hr = m_pMeshSysMemory->LockVertexBuffer(0, (LPVOID*)&pData);
	if (FAILED(hr))
	{
		return hr;
	}
	DWORD dwStride = D3DXGetFVFVertexSize(m_pMeshSysMemory->GetFVF());
	hr = D3DXComputeBoundingSphere(pData,m_dwNumVertices, dwStride,&m_vCenter, &m_fObjectRadius );
	if (FAILED(hr))
	{
		return hr;
	}
	hr = m_pMeshSysMemory->UnlockVertexBuffer();
	if (FAILED(hr))
	{
		return hr;
	}
	//装入效果文件
	szFile = L"D:\\lifei\\test\\hlsl高级精讲\\SimpleSample\\ResFile_1\\7_3_2.txt";
	LPD3DXBUFFER pCode = NULL;
	hr = D3DXCreateEffectFromFile(pd3dDevice, szFile, NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, &pCode);
	if (pCode)
	{
		::MessageBoxA(0, (char*)pCode->GetBufferPointer(), 0, 0);		
		pCode->Release();
	}
	if (FAILED(hr))
	{		
		return hr;
	}
	//装入纹理
	szFile = L"D:\\lifei\\test\\hlsl高级精讲\\SimpleSample\\ResFile_1\\colormap.jpg";
	hr = D3DXCreateTextureFromFileEx(pd3dDevice, szFile, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,D3DX_DEFAULT, 0,
		NULL, NULL, &m_pColorTexture);
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
}

HRESULT CRenderObject::OnResetDevice(IDirect3DDevice9 * pd3dDevice)
{
	HRESULT hr = S_OK;
	//设置使用哪种渲染方式和哪个纹理
	D3DXHANDLE defaultTechHandle = m_pEffect->GetTechniqueByName("DefaultTech");
	hr = m_pEffect->SetTechnique(defaultTechHandle);
	if (FAILED(hr))
	{
		return hr;
	}
	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_ColorTexture");
	hr = m_pEffect->SetTexture(colorTextureHandle, m_pColorTexture);
	if (FAILED(hr))
	{
		return hr;
	}
	//根据格式创建纹理，再从纹理页面创建渲染接口
	//流程是创建纹理，获取纹理属性，取第0层后创建纹理渲染接口
	hr = D3DXCreateTexture(pd3dDevice, m_TextureSize, m_TextureSize, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRTTTexture);
	if (FAILED(hr))
	{
		hr = D3DXCreateTexture(pd3dDevice, m_TextureSize, m_TextureSize, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_pRTTTexture);
		if (FAILED(hr))
		{
			return hr;
		}
	}
	D3DSURFACE_DESC desc;
	hr = m_pRTTTexture->GetSurfaceLevel(0, &m_pRTTSurface);
	hr = m_pRTTSurface->GetDesc(&desc);
	hr = D3DXCreateRenderToSurface(pd3dDevice, desc.Width, desc.Height, desc.Format, TRUE, D3DFMT_D24X8, &m_pRTTInterface);
	if (FAILED(hr))
	{
		return hr;
	}
	//投影矩阵
	D3DXMatrixPerspectiveFovLH(&m_matProjectRTT, D3DX_PI / 4, 1, 1.0f, 100.0f);
	return hr;
}


//创建三个纹理，纹理大小与窗口一致，格式为D3DFMT_A16B16G16R16F
HRESULT CRenderObject::OnResetDeviceRTT(IDirect3DDevice9 * pd3dDevice, const D3DSURFACE_DESC * pBackBufferSurfaceDesc)
{
	HRESULT hr = S_OK;
	/*
	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_ColorTexture");
	hr = m_pEffect->SetTexture(colorTextureHandle, m_pColorTexture);
	if (FAILED(hr))
	{
		return hr;
	}
	pd3dDevice->CreateTexture(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
		D3DPOOL_DEFAULT, &m_pRTTTexturePosition, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	pd3dDevice->CreateTexture(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
		D3DPOOL_DEFAULT, &m_pRTTTextureNormal, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	pd3dDevice->CreateTexture(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 1, D3DUSAGE_RENDERTARGET, D3DFMT_A16B16G16R16F,
		D3DPOOL_DEFAULT, &m_pRTTTextureDepth, NULL);
	if (FAILED(hr))
	{
		return hr;
	}	
	*/
	return hr;
}


void CRenderObject::OnFrameMove(D3DXMATRIXA16 * pWorld, D3DXMATRIXA16 * pView, D3DXMATRIXA16 * pProject, double fTime)
{
	D3DXHANDLE matWorldHandle = m_pEffect->GetParameterByName(0, "g_matWorld");
	m_pEffect->SetMatrix(matWorldHandle, pWorld);
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matView");
	m_pEffect->SetMatrix(matViewHandle, pView);
	D3DXHANDLE matProjectHandle = m_pEffect->GetParameterByName(0, "g_matProject");
	m_pEffect->SetMatrix(matProjectHandle, pProject);
	D3DXHANDLE fTimeHandle = m_pEffect->GetParameterByName(0, "g_fTime");
	m_pEffect->SetFloat(fTimeHandle, (float)fTime);
}

void CRenderObject::OnFrameRender(IDirect3DDevice9 * pd3dDevice)
{
	D3DXHANDLE defaultHandle = m_pEffect->GetTechniqueByName("DefaultTech");
	m_pEffect->SetTechnique(defaultHandle);
	pd3dDevice->SetVertexDeclaration(m_pDecl);
	pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(RENDEROBJECT_D3DVERTEX));
	pd3dDevice->SetIndices(m_pIB);
	UINT cPasses;
	m_pEffect->Begin(&cPasses, 0);
	for (int iPass = 0; iPass < cPasses; iPass++)
	{
		m_pEffect->BeginPass(iPass);
		//m_pMeshSysMemory->DrawSubset(0);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, m_dwNumIndices);
		m_pEffect->EndPass();
	}
	m_pEffect->End();
}

void CRenderObject::OnFrameRender(IDirect3DDevice9 * pd3dDevice, LPDIRECT3DTEXTURE9 pTexture)
{
	D3DXHANDLE RTTHandle = m_pEffect->GetTechniqueByName("DefaultTech");
	m_pEffect->SetTechnique(RTTHandle);
	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_RenderToTexture");
	m_pEffect->SetTexture(colorTextureHandle, pTexture);
	pd3dDevice->SetVertexDeclaration(m_pDecl);
	pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(RENDEROBJECT_D3DVERTEX));
	pd3dDevice->SetIndices(m_pIB);
	UINT cPasses;
	m_pEffect->Begin(&cPasses, 0);
	for (int iPass = 0; iPass < cPasses; iPass++)
	{
		m_pEffect->BeginPass(iPass);
		//m_pMeshSysMemory->DrawSubset(0);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, m_dwNumIndices);
		m_pEffect->EndPass();
	}
	m_pEffect->End();
}

void CRenderObject::OnLostDevice()
{
	if (m_pEffect)
	{
		m_pEffect->OnLostDevice();
	}
	SAFE_RELEASE(m_pRTTSurface);
	SAFE_RELEASE(m_pRTTInterface);
	SAFE_RELEASE(m_pRTTTexture);
}

void CRenderObject::OnDestroyDevice()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pColorTexture);
	SAFE_RELEASE(m_pRTTSurface);
	SAFE_RELEASE(m_pRTTInterface);
	SAFE_RELEASE(m_pRTTTexture);
}
