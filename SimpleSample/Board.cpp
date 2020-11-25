#include "DXUT.h"
#include "Board.h"

CBoard::CBoard()
{
	m_pVB = NULL;
	m_pIB = NULL;
	m_dwNumIndices = 0;
	m_dwNumVertices = 0;
	m_pDecl = NULL;
	m_pEffect = NULL;
}

CBoard::~CBoard()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	m_dwNumIndices = 0;
	m_dwNumVertices = 0;
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pEffect);
}

HRESULT CBoard::CreateSquare(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr = S_OK;

	//���������ι�4����
	m_dwNumIndices = 4;
	m_dwNumVertices = 4;
	//��������������
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
	//�������㻺����
	BOARD_D3DVERTEX * pVertices;
	hr = pd3dDevice->CreateVertexBuffer(m_dwNumVertices * sizeof(BOARD_D3DVERTEX), 0, 0, D3DPOOL_DEFAULT, &m_pVB, NULL);
	if (FAILED(hr))
	{
		return hr;
	}
	hr = m_pVB->Lock(0, 0, (void**)& pVertices, 0);
	if (FAILED(hr))
	{
		return hr;
	}
	//��XZƽ��
	//��
	pVertices[0].x = -10;
	pVertices[0].y = 10;
	pVertices[0].z = 0;
	pVertices[0].w = 1;
	pVertices[0].color = D3DCOLOR_ARGB(0x70, 0xFF, 0, 0 ); //͸������
	pVertices[0].u = 0;
	pVertices[0].v = 0;
	//��
	pVertices[1].x = 10;
	pVertices[1].y = 10;
	pVertices[1].z = 0;
	pVertices[1].w = 1;
	pVertices[1].color = D3DCOLOR_ARGB(0x70, 0x0, 0xFF, 0); //͸������
	pVertices[1].u = 1;
	pVertices[1].v = 0;

	//��
	pVertices[2].x = -10;
	pVertices[2].y = -10;
	pVertices[2].z = 0;
	pVertices[2].w = 1;
	pVertices[2].color = D3DCOLOR_ARGB(0x70, 0x0, 0,0xFF); //͸������
	pVertices[2].u = 0;
	pVertices[2].v = 1;

	//��
	pVertices[3].x = 10;
	pVertices[3].y = -10;
	pVertices[3].z = 0;
	pVertices[3].w = 1;
	pVertices[3].color = D3DCOLOR_ARGB(0x70, 0xFF, 0xFF, 0); //͸������
	pVertices[3].u = 1;
	pVertices[3].v = 1;

	hr = m_pVB->Unlock();
	if (FAILED(hr))
	{
		return hr;
	}
	return hr;
}

void CBoard::init()
{
}

HRESULT CBoard::OnCreateDevice(IDirect3DDevice9 * pd3dDevice)
{
	HRESULT hr = S_OK;

	//װ������
	hr = D3DXCreateTextureFromFileEx(
		pd3dDevice,
		_szTextureFileName.c_str(),
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
	//���µķ�ʽ����������
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
	//TCHAR* szFile = L"D:\\lifei\\test\\hlsl�߼�����\\SimpleSample\\ResFile_Debug\\2_4_3_1.txt";
	LPD3DXBUFFER pCode = NULL;
	hr = D3DXCreateEffectFromFile(pd3dDevice, _szEffectFileName.c_str(), NULL, NULL, 0, NULL, &m_pEffect, &pCode);
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

void CBoard::OnFrameRender(IDirect3DDevice9* pd3dDevice, LPDIRECT3DTEXTURE9 pTexture)
{
	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "texColorTextureIn");
	m_pEffect->SetTexture(colorTextureHandle, pTexture);
	HRESULT hr = pd3dDevice->SetVertexDeclaration(m_pDecl);
	hr = pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(BOARD_D3DVERTEX));
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

void CBoard::OnFrameRender(IDirect3DDevice9 * pd3dDevice, D3DXMATRIXA16 * pWorld, D3DXMATRIXA16 * pView, D3DXMATRIXA16 * pProject, double fTime)
{
	D3DXHANDLE defaultHandle = m_pEffect->GetTechniqueByName("Default");
	m_pEffect->SetTechnique(defaultHandle);

	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_CubeTexture");
	 m_pEffect->SetTexture(colorTextureHandle, m_pColorTexture);
	
	HRESULT hr = pd3dDevice->SetVertexDeclaration(m_pDecl);
	hr = pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(BOARD_D3DVERTEX));
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

void CBoard::OnLostDevice()
{
	if (m_pEffect)
	{
		m_pEffect->OnLostDevice();
	}
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pVB);
}

void CBoard::OnDestroyDevice()
{
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pColorTexture);
}

void CBoard::setFiles(std::wstring szEffectFileName, std::wstring szTextureFileName)
{
	_szEffectFileName = szEffectFileName;
	_szTextureFileName = szTextureFileName;
}

HRESULT CBoard::OnResetDevice(IDirect3DDevice9 * pd3dDevice)
{
	HRESULT hr = CreateSquare(pd3dDevice);
	if (FAILED(hr))
	{
		return hr;
	}

	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_CubeTexture");
	hr = m_pEffect->SetTexture(colorTextureHandle, m_pColorTexture);
	if (FAILED(hr))
	{
		return hr;
	}
	
}

void CBoard::OnFrameMove()
{
}
