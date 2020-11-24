#include "DXUT.h"
#include "RenderObjectDS.h"
#include "SDKmesh.h"
#include "SDKmisc.h"

CRenderObjectDS::CRenderObjectDS()
{
	m_pVB = NULL;
	m_pIB = NULL;
	m_dwNumIndices = 0;
	m_dwNumVertices = 0;
	m_pDecl = NULL;
	m_pMeshSysMemory = NULL;
	m_pEffect = NULL;
	m_pColorTexture = NULL;

	m_pRTTTextureDepth = NULL;
	m_pRTTInterfaceDepth = NULL;
	m_pRTTSurfaceDepth = NULL;

}

CRenderObjectDS::~CRenderObjectDS()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pEffect);	
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pColorTexture);

	SAFE_RELEASE(m_pRTTTextureDepth);
	SAFE_RELEASE(m_pRTTInterfaceDepth);
	SAFE_RELEASE(m_pRTTSurfaceDepth);
}

HRESULT CRenderObjectDS::LoadResource(TCHAR * FileName)
{
	//���ļ�
	DWORD uByte;
	//�鿴�ļ��Ƿ����
	HANDLE theHandle = CreateFile(FileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (INVALID_HANDLE_VALUE == theHandle)
	{
		return E_FAIL;
	}
	//����ļ�ͷ
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
	//��ȡ��Դ����
	m_iResNum = sFileHead.s_iResourceFileNum;
	m_pResourceInfo = new SRESOURCEINFO[m_iResNum];
	if (NULL == m_pResourceInfo)
	{
		return E_FAIL;
	}
	//��ȡ��Դ��Ϣ
	bReturn = ReadFile(theHandle, (LPVOID)m_pResourceInfo, sizeof(SRESOURCEINFO) * m_iResNum, &uByte, NULL);
	if (NULL == bReturn)
	{
		CloseHandle(theHandle);
		return E_FAIL;
	}
	CloseHandle(theHandle);
	return S_OK;
}

//������Դ���������õ��ļ���
TCHAR * CRenderObjectDS::GetResourceName(RESTYPE enuResType)
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

void CRenderObjectDS::Init()
{
}

HRESULT CRenderObjectDS::OnCreateDevice(IDirect3DDevice9 * pd3dDevice, TCHAR * FileName)
{
	HRESULT hr = S_OK;
	
	//װ��.shm�ļ�
	hr = LoadResource(FileName);
	if (FAILED(hr))
	{
		return hr;
	}
	
	//�ж��Ƿ���ģ���ļ��������ж�ȡ�ļ���
	TCHAR * szFile = GetResourceName(ENU_MODELX);
	LPD3DXMESH pMeshSysMemClone = NULL;
	hr = D3DXLoadMeshFromX(szFile, D3DXMESH_SYSTEMMEM, pd3dDevice, NULL, NULL, NULL, NULL, &pMeshSysMemClone);
	if (FAILED(hr))
	{
		return hr;
	}

	//���µķ�ʽ����������
	D3DVERTEXELEMENT9 decl[] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0 },
		{ 0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
		D3DDECL_END()
	};
	//��Ϊװ���mesh�Ķ���ṹ����������Ҫ�ģ���˰���������Ҫ�ĸ�ʽ����һ��
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
	
	//��ȡģ�ͷ�Χ�����ĵ�λ��
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

	//��ȡЧ���ļ���
	szFile = GetResourceName(ENU_FXFILE);
	LPD3DXBUFFER pCode = NULL;
	//װ�붥����ɫ��

	DWORD dwFlag = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
	hr = D3DXCreateEffectFromFile(pd3dDevice, szFile, NULL, NULL, dwFlag, NULL, &m_pEffect, &pCode);
	if (FAILED(hr))
	{
		::MessageBoxA(0, (char*)pCode->GetBufferPointer(), 0, 0);
		pCode->Release();
		return hr;
	}
	
	szFile = GetResourceName(ENU_TEXCOLORMAP);
	hr = D3DXCreateTextureFromFileEx(pd3dDevice, szFile, D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT,
		0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0,
		NULL, NULL, &m_pColorTexture);
	if (FAILED(hr))
	{
		return hr;
	}

	return hr;
}

HRESULT CRenderObjectDS::OnResetDevice(IDirect3DDevice9 * pd3dDevice, const D3DSURFACE_DESC * pBackSurfaceDesc)
{
	HRESULT hr = S_OK;
	
	//����ʹ��������Ⱦ��ʽ���ĸ�����
	//D3DXHANDLE defaultTechHandle = m_pEffect->GetTechniqueByName("RenderToTextureTech");
	//hr = m_pEffect->SetTechnique(defaultTechHandle);
	//if (FAILED(hr))
	//{
	//	return hr;
	//}
	D3DXHANDLE colorTextureHandle = m_pEffect->GetParameterByName(0, "g_ColorTexture");
	hr = m_pEffect->SetTexture(colorTextureHandle, m_pColorTexture);
	if (FAILED(hr))
	{
		return hr;
	}

	float fTextureWidth = pBackSurfaceDesc->Width / 2.0;
	float fTextureHeight = pBackSurfaceDesc->Height / 2.0;
	//��Ⱦ������
	hr = D3DXCreateTexture(pd3dDevice, fTextureWidth, fTextureHeight, 1, D3DUSAGE_RENDERTARGET, D3DFMT_R32F, D3DPOOL_DEFAULT, &m_pRTTTextureDepth);
	if (FAILED(hr))
	{
		return hr;	
	}
	D3DSURFACE_DESC desc;
	hr = m_pRTTTextureDepth->GetSurfaceLevel(0, &m_pRTTSurfaceDepth);
	hr = m_pRTTSurfaceDepth->GetDesc(&desc);
	hr = D3DXCreateRenderToSurface(pd3dDevice, desc.Width, desc.Height, desc.Format, TRUE, D3DFMT_D16, &m_pRTTInterfaceDepth);
	if (FAILED(hr))
	{
		return hr;
	}

	return hr;
}


void CRenderObjectDS::OnFrameMove(IDirect3DDevice9* pd3dDevice, D3DXMATRIXA16 * pWorld, D3DXMATRIXA16 * pView, D3DXMATRIXA16 * pProject, double fTime, D3DXVECTOR4* pvEye)
{
	D3DXHANDLE matWorldHandle = m_pEffect->GetParameterByName(0, "g_matWorld");
	m_pEffect->SetMatrix(matWorldHandle, pWorld);
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matView");
	m_pEffect->SetMatrix(matViewHandle, pView);
	D3DXHANDLE matProjectHandle = m_pEffect->GetParameterByName(0, "g_matProject");
	m_pEffect->SetMatrix(matProjectHandle, pProject);
	D3DXHANDLE matTimeHandle = m_pEffect->GetParameterByName(0, "g_fTime");
	m_pEffect->SetFloat(matTimeHandle, fTime);
	D3DXHANDLE eyePositionHandle = m_pEffect->GetParameterByName(0, "g_vEyePosition");
	m_pEffect->SetVector(eyePositionHandle, pvEye);

	float Determinant;
	D3DXMATRIX matOut;
	D3DXMatrixInverse(&matOut, &Determinant, pWorld);
	D3DXMatrixTranspose(&matOut, &matOut);
	D3DXHANDLE matOutHandle = m_pEffect->GetParameterByName(0, "g_matWorldNormalInverseTranspose");
	m_pEffect->SetMatrix(matOutHandle, &matOut);
	
}

void CRenderObjectDS::OnFrameRender(IDirect3DDevice9 * pd3dDevice)
{
	//D3DXHANDLE renderHandle = m_pEffect->GetTechniqueByName("DefaultTech");
	//m_pEffect->SetTechnique(renderHandle);
	pd3dDevice->SetVertexDeclaration(m_pDecl);
	pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(RENDEROBJECT_D3DVERTEX));
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

HRESULT CRenderObjectDS::RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice, D3DXMATRIXA16* pWorld, D3DXMATRIXA16* pView, D3DXMATRIXA16* pProject, float fTime)
{

	D3DXHANDLE matWorldHandle = m_pEffect->GetParameterByName(0, "g_matWorldRTT");
	m_pEffect->SetMatrix(matWorldHandle, pWorld);
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matViewRTT");
	m_pEffect->SetMatrix(matViewHandle, pView);
	D3DXHANDLE matProjectHandle = m_pEffect->GetParameterByName(0, "g_matProjectRTT");
	m_pEffect->SetMatrix(matProjectHandle, pProject);

	D3DXHANDLE defaultTechHandle = m_pEffect->GetTechniqueByName("RenderDepthTech");
	m_pEffect->SetTechnique(defaultTechHandle);

	m_pRTTInterfaceDepth->BeginScene(m_pRTTSurfaceDepth, NULL);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	pd3dDevice->SetVertexDeclaration(m_pDecl);
	pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(RENDEROBJECT_D3DVERTEXDS));
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
	m_pRTTInterfaceDepth->EndScene(0);
	return S_OK;
}

void CRenderObjectDS::OnLostDevice()
{
	if (m_pEffect)
	{
		m_pEffect->OnLostDevice();
	}

	SAFE_RELEASE(m_pRTTSurfaceDepth);
	SAFE_RELEASE(m_pRTTInterfaceDepth);
	SAFE_RELEASE(m_pRTTTextureDepth);
}

void CRenderObjectDS::OnDestroyDevice()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pEffect);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pColorTexture);
	SAFE_RELEASE(m_pRTTSurfaceDepth);
	SAFE_RELEASE(m_pRTTInterfaceDepth);
	SAFE_RELEASE(m_pRTTTextureDepth);
}

