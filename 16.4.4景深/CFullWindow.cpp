#include "DXUT.h"
#include "CFullWindow.h"

CFullWindow::CFullWindow()
{
	m_pVB = NULL;
	m_pIB = NULL;
	m_dwNumIndices = 0;
	m_dwNumVertices = 0;
	m_pDecl = NULL;
	m_pEffect = NULL;
}

CFullWindow::~CFullWindow()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	m_dwNumIndices = 0;
	m_dwNumVertices = 0;
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pEffect);
}

void CFullWindow::init()
{
}

HRESULT CFullWindow::OnCreateDevice(IDirect3DDevice9 * pd3dDevice)
{
	HRESULT hr = S_OK;
	TCHAR szAppPath[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szAppPath);
	//装入纹理
	hr = D3DXCreateTextureFromFileEx(
		pd3dDevice,
		L"ColorMap.bmp",
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		D3DFMT_UNKNOWN,
		D3DPOOL_MANAGED,
		D3DX_DEFAULT,
		D3DX_DEFAULT,
		0,
		NULL,
		NULL,
		&m_pColorTexture
	);
	if (FAILED(hr))
	{
		return hr;
	}
	//以新的方式创建顶点流
	LPD3DXMESH pMeshSysMemClone = NULL;
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 16, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_COLOR, 0 },
		{ 0, 20, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	hr = pd3dDevice->CreateVertexDeclaration(decl, &m_pDecl);
	if (FAILED(hr))
	{
		return hr;
	}
	LPD3DXBUFFER pCode = NULL;
	hr = D3DXCreateEffectFromFile(pd3dDevice, L"FileFx.fx", NULL, NULL, 0, NULL, &m_pEffect, &pCode);
	if (pCode)
	{
		::MessageBoxA(0, (char*)pCode->GetBufferPointer(), 0, 0);
		pCode->Release();
	}
	if (FAILED(hr))
	{
		return hr;
	}
	
	return hr;
}

HRESULT CFullWindow::OnResetDevice(IDirect3DDevice9 * pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc)
{
	HRESULT hr = CreateSquare(pd3dDevice);
	if (FAILED(hr))
	{
		return hr;
	}
	D3DXVECTOR4 v4ScreenSize = D3DXVECTOR4(pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 0.0, 0.0);
	D3DXHANDLE screenHandle = m_pEffect->GetParameterByName(0, "g_v4ScreenSize");
	m_pEffect->SetVector(screenHandle, &v4ScreenSize);
	return hr;

}

void CFullWindow::OnFrameMove()
{
}
void CFullWindow::OnFrameRender(IDirect3DDevice9* pd3dDevice, LPDIRECT3DTEXTURE9 pTexture)
{
	//根据不同的选择，使用不同位置渲染纹理
	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_texTextureIn");
	m_pEffect->SetTexture(colorTextureHandle, pTexture);

	HRESULT hr = pd3dDevice->SetVertexDeclaration(m_pDecl);
	hr = pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(FULLWINDOW_D3DVERTEX));
	hr = pd3dDevice->SetIndices(m_pIB);

	UINT cPasses;
	m_pEffect->Begin(&cPasses, 0);
	for (size_t iPass = 0; iPass < cPasses; iPass++)
	{
		hr = m_pEffect->BeginPass(iPass);
		hr = pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_dwNumVertices, 0, 2);
		hr = m_pEffect->EndPass();
	}
	hr = m_pEffect->End();

}
void CFullWindow::OnFrameRender(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, double fTime)
{
	//根据不同的选择，使用不同位置渲染纹理

	HRESULT hr = pd3dDevice->SetVertexDeclaration(m_pDecl);
	hr = pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(FULLWINDOW_D3DVERTEX));
	hr = pd3dDevice->SetIndices(m_pIB);
	D3DXHANDLE renderHandle = m_pEffect->GetTechniqueByName("DepthOfFieldTech");
	m_pEffect->SetTechnique(renderHandle);

	UINT cPasses;
	m_pEffect->Begin(&cPasses, 0);
	for (size_t iPass = 0; iPass < cPasses; iPass++)
	{
		hr = m_pEffect->BeginPass(iPass);
		hr = pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, m_dwNumVertices, 0, 2);
		hr = m_pEffect->EndPass();
	}
	hr = m_pEffect->End();

}
void CFullWindow::OnLostDevice()
{
	if (m_pEffect)
	{
		m_pEffect->OnLostDevice();
	}
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pVB);
}

void CFullWindow::OnDestroyDevice()
{
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pColorTexture);
}
void CFullWindow::setLightParameter(D3DXVECTOR4 * vPosition)
{
	D3DXHANDLE lightPositionHandle = m_pEffect->GetParameterByName(0, "g_vLightPosition");
	m_pEffect->SetVector(lightPositionHandle, vPosition);
}
HRESULT CFullWindow::CreateSquare(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr = S_OK;

	//两个三角形共4个点
	m_dwNumIndices = 4;
	m_dwNumVertices = 4;
	//创建索引缓存区
	WORD* pIndices;
	hr = pd3dDevice->CreateIndexBuffer(m_dwNumIndices * sizeof(WORD), 0, D3DFMT_INDEX16, D3DPOOL_DEFAULT, &m_pIB, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = m_pIB->Lock(0, 0, (void**)& pIndices, 0);
	if (FAILED(hr))
	{
		return hr;
	}
	pIndices[0] = 0;
	pIndices[1] = 1;
	pIndices[2] = 2;
	pIndices[3] = 3;
	hr = m_pIB->Unlock();
	if (FAILED(hr))
	{
		return hr;
	}
	//创建顶点缓冲区
	FULLWINDOW_D3DVERTEX * pVertices;
	hr = pd3dDevice->CreateVertexBuffer(m_dwNumVertices * sizeof(FULLWINDOW_D3DVERTEX), 0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = m_pVB->Lock(0, 0, (void**)& pVertices, 0);
	if (FAILED(hr))
	{
		return hr;
	}
	//在XZ平面
	//红
	pVertices[0].x = -10;
	pVertices[0].y = 10;
	pVertices[0].z = 0;
	pVertices[0].w = 1;
	pVertices[0].color = D3DCOLOR_ARGB(0x70, 0xFF, 0, 0 ); //透明顶点
	pVertices[0].u = 0;
	pVertices[0].v = 0;
	//绿
	pVertices[1].x = 10;
	pVertices[1].y = 10;
	pVertices[1].z = 0;
	pVertices[1].w = 1;
	pVertices[1].color = D3DCOLOR_ARGB(0x70, 0x0, 0xFF, 0); //透明顶点
	pVertices[1].u = 1;
	pVertices[1].v = 0;

	//蓝
	pVertices[2].x = -10;
	pVertices[2].y = -10;
	pVertices[2].z = 0;
	pVertices[2].w = 1;
	pVertices[2].color = D3DCOLOR_ARGB(0x70, 0x0, 0,0xFF); //透明顶点
	pVertices[2].u = 0;
	pVertices[2].v = 1;

	//白
	pVertices[3].x = 10;
	pVertices[3].y = -10;
	pVertices[3].z = 0;
	pVertices[3].w = 1;
	pVertices[3].color = D3DCOLOR_ARGB(0x70, 0xFF, 0xFF, 0); //透明顶点
	pVertices[3].u = 1;
	pVertices[3].v = 1;

	hr = m_pVB->Unlock();
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
}

