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
	m_pVolumeTexture = NULL;
}

CRenderObject::~CRenderObject()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
	SAFE_RELEASE(m_pEffect);	
	SAFE_RELEASE(m_pDecl);
	SAFE_RELEASE(m_pMeshSysMemory);
	SAFE_RELEASE(m_pVolumeTexture);
}

HRESULT CRenderObject::LoadResource(TCHAR * FileName)
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


void CRenderObject::Init()
{
}

HRESULT CRenderObject::OnCreateDevice(IDirect3DDevice9 * pd3dDevice, TCHAR * FileName)
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
	D3DVERTEXELEMENT9 decl[3] =
	{
		{ 0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0 },
		{ 0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0 },
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
	
	//װ����ɫ����
	szFile = GetResourceName(ENU_VOLUMEMAP);
	hr = D3DXCreateVolumeTextureFromFile(pd3dDevice, szFile, &m_pVolumeTexture);
	return hr;
}

HRESULT CRenderObject::OnResetDevice()
{
	HRESULT hr = S_OK;
	
	//����ʹ��������Ⱦ��ʽ���ĸ�����
	D3DXHANDLE defaultTechHandle = m_pEffect->GetTechniqueByName("DefaultTech");
	hr = m_pEffect->SetTechnique(defaultTechHandle);
	if (FAILED(hr))
	{
		return hr;
	}
	D3DXHANDLE volumeTextureHandle = m_pEffect->GetParameterByName(0, "g_VolumeTexture");
	hr = m_pEffect->SetTexture(volumeTextureHandle, m_pVolumeTexture);

	return hr;
}

void CRenderObject::OnFrameMove(IDirect3DDevice9* pd3dDevice,D3DXMATRIXA16 * pWorld, D3DXMATRIXA16 * pView, D3DXMATRIXA16 * pProject, double fTime)
{
	D3DXHANDLE matWorldHandle = m_pEffect->GetParameterByName(0, "g_matWorld");
	m_pEffect->SetMatrix(matWorldHandle, pWorld);
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matView");
	m_pEffect->SetMatrix( matViewHandle, pView);
	D3DXHANDLE matProjectHandle = m_pEffect->GetParameterByName(0, "g_matProject");
	m_pEffect->SetMatrix( matProjectHandle, pProject);
	D3DXHANDLE matTimeHandle = m_pEffect->GetParameterByName(0, "g_fTime");
	m_pEffect->SetFloat( matTimeHandle, fTime);
}

void CRenderObject::OnFrameRender(IDirect3DDevice9 * pd3dDevice)
{
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
	SAFE_RELEASE(m_pVolumeTexture);
}

