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

	_wstrModelName = L"";
	_wstrImgName = L"";
	_wstrFXName = L"";

	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProject);

}

CRenderObject::~CRenderObject()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pColorTexture);

}

void CRenderObject::Init()
{
}

HRESULT CRenderObject::OnCreateDevice(IDirect3DDevice9 * pd3dDevice)
{
	HRESULT hr = S_OK;
	LPD3DXMESH pMeshSysMemClone = NULL;
	hr = D3DXLoadMeshFromX(_wstrModelName.c_str(), D3DXMESH_SYSTEMMEM, pd3dDevice, NULL, NULL, NULL, NULL, &pMeshSysMemClone);
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
	//改变位置
	for (size_t i = 0; i < m_dwNumVertices; i++)
	{
		if (pData[i].y > 3.0)
		{
			pData[i].y = 3.0;
		}
	}
	hr = m_pMeshSysMemory->UnlockVertexBuffer();
	if (FAILED(hr))
	{
		return hr;
	}
	//装入效果文件
	LPD3DXBUFFER pCode = NULL;
	hr = D3DXCreateEffectFromFile(pd3dDevice, _wstrFXName.c_str(), NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, &pCode);
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
	hr = D3DXCreateTextureFromFileEx(pd3dDevice, _wstrImgName.c_str(), D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT,D3DX_DEFAULT, 0,
		NULL, NULL, &m_pColorTexture);
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
}

HRESULT CRenderObject::OnResetDevice(IDirect3DDevice9 * pd3dDevice, float fAspectRatio)
{
	HRESULT hr = S_OK;

	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_ColorTexture");
	m_pEffect->SetTexture(colorTextureHandle, m_pColorTexture);

	return hr;
}

void CRenderObject::OnFrameMove(double fTime)
{
	D3DXHANDLE matWorldHandle = m_pEffect->GetParameterByName(0, "g_matWorld");
	m_pEffect->SetMatrix(matWorldHandle, &m_matWorld);
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matView");
	m_pEffect->SetMatrix(matViewHandle, &m_matView);
	D3DXHANDLE matProjectHandle = m_pEffect->GetParameterByName(0, "g_matProject");
	m_pEffect->SetMatrix(matProjectHandle, &m_matProject);
	D3DXHANDLE fTimeHandle = m_pEffect->GetParameterByName(0, "g_fTime");
	m_pEffect->SetFloat(fTimeHandle, (float)fTime);
}

void CRenderObject::OnFrameRender(IDirect3DDevice9 * pd3dDevice)
{
	D3DXHANDLE defaultHandle = m_pEffect->GetTechniqueByName("DefaultTech");
	m_pEffect->SetTechnique(defaultHandle);
	pd3dDevice->SetVertexDeclaration(m_pDecl);
	pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(SHADOWMAPOBJECT_D3DVERTEX));
	pd3dDevice->SetIndices(m_pIB);
	UINT cPasses;
	m_pEffect->Begin(&cPasses, 0);
	for (int iPass = 0; iPass < cPasses; iPass++)
	{
		m_pEffect->BeginPass(iPass);
		pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, m_dwNumVertices, 0, m_dwNumIndices);
		m_pEffect->EndPass();
	}
	m_pEffect->End();
}

void CRenderObject::setFirstCameraMatrixVector(D3DXMATRIXA16 * pWorld, D3DXMATRIXA16 * pView, D3DXMATRIXA16 * pProject)
{
	m_matWorld = *pWorld;
	m_matView = *pView;
	m_matProject = *pProject;
}

void CRenderObject::OnLostDevice()
{
	if (m_pEffect)
	{
		m_pEffect->OnLostDevice();
	}

}

void CRenderObject::OnDestroyDevice()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pColorTexture);
}

void CRenderObject::setFile(std::wstring wstrModelName, std::wstring wstrImgName, std::wstring wstrFXName)
{
	_wstrModelName = wstrModelName;
	_wstrImgName = wstrImgName;
	_wstrFXName = wstrFXName;
}
