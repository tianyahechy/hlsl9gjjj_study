#include "DXUT.h"
#include "shadowmapObject.h"
#include "SDKmesh.h"
#include "SDKmisc.h"

CShadowMapObject::CShadowMapObject()
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
	D3DXMatrixIdentity(&m_matWorld);
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProject);
	D3DXMatrixIdentity(&m_matWorldRTT);
	D3DXMatrixIdentity(&m_matViewRTT);
	D3DXMatrixIdentity(&m_matProjectRTT);

	_wstrModelName = L"";
	_wstrImgName = L"";
	_wstrFXName = L"";

}

CShadowMapObject::~CShadowMapObject()
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

void CShadowMapObject::Init()
{
}

HRESULT CShadowMapObject::OnCreateDevice(IDirect3DDevice9 * pd3dDevice)
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

HRESULT CShadowMapObject::OnResetDevice(IDirect3DDevice9 * pd3dDevice, float fAspectRatio)
{
	HRESULT hr = S_OK;

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
	//设置纹理
	D3DXHANDLE textureHandle = m_pEffect->GetParameterByName(0, "g_RenderToTexture");
	m_pEffect->SetTexture(textureHandle, m_pRTTTexture);
	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_ColorTexture");
	m_pEffect->SetTexture(colorTextureHandle, m_pColorTexture);

	return hr;
}

void CShadowMapObject::OnFrameMove(double fTime)
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

void CShadowMapObject::OnFrameRender(IDirect3DDevice9 * pd3dDevice)
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

//渲染到纹理
HRESULT CShadowMapObject::RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr;
	//计算3个矩阵
	D3DXHANDLE RTTTechHandle = m_pEffect->GetTechniqueByName("RenderDepthToTextureShader");
	hr = m_pEffect->SetTechnique(RTTTechHandle);
	if (FAILED(hr))
	{
		return hr;
	}
	
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
	pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(SHADOWMAPOBJECT_D3DVERTEX));
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

void CShadowMapObject::setFirstCameraMatrixVector(D3DXMATRIXA16 * pWorld, D3DXMATRIXA16 * pView, D3DXMATRIXA16 * pProject)
{
	m_matWorld = *pWorld;
	m_matView = *pView;
	m_matProject = *pProject;
}

void CShadowMapObject::setLightMatrixVector(D3DXMATRIXA16 * pWorldRTT, D3DXMATRIXA16 * pViewRTT, D3DXMATRIXA16 * pProjectRTT)
{
	m_matWorldRTT = *pWorldRTT;
	m_matViewRTT = *pViewRTT;
	m_matProjectRTT = *pProjectRTT;
}

void CShadowMapObject::OnLostDevice()
{
	if (m_pEffect)
	{
		m_pEffect->OnLostDevice();
	}
	SAFE_RELEASE(m_pRTTSurface);
	SAFE_RELEASE(m_pRTTInterface);
	SAFE_RELEASE(m_pRTTTexture);
}

void CShadowMapObject::OnDestroyDevice()
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

void CShadowMapObject::setFile(std::wstring wstrModelName, std::wstring wstrImgName, std::wstring wstrFXName)
{
	_wstrModelName = wstrModelName;
	_wstrImgName = wstrImgName;
	_wstrFXName = wstrFXName;
}
