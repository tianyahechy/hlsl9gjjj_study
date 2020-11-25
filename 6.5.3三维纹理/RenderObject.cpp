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
	
	//判断是否有模型文件，并从中读取文件名
	TCHAR * szFile = GetResourceName(ENU_MODELX);
	LPD3DXMESH pMeshSysMemClone = NULL;
	hr = D3DXLoadMeshFromX(szFile, D3DXMESH_SYSTEMMEM, pd3dDevice, NULL, NULL, NULL, NULL, &pMeshSysMemClone);
	if (FAILED(hr))
	{
		return hr;
	}

	//以新的方式创建顶点流
	D3DVERTEXELEMENT9 decl[3] =
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

	//获取效果文件名
	szFile = GetResourceName(ENU_FXFILE);
	LPD3DXBUFFER pCode = NULL;
	//装入顶点着色器
	DWORD dwFlag = D3DXSHADER_DEBUG | D3DXSHADER_SKIPOPTIMIZATION;
	hr = D3DXCreateEffectFromFile(pd3dDevice, szFile, NULL, NULL, dwFlag, NULL, &m_pEffect, &pCode);
	if (FAILED(hr))
	{
		::MessageBoxA(0, (char*)pCode->GetBufferPointer(), 0, 0);
		pCode->Release();
		return hr;
	}
	
	//装入颜色纹理
	szFile = GetResourceName(ENU_VOLUMEMAP);
	hr = D3DXCreateVolumeTextureFromFile(pd3dDevice, szFile, &m_pVolumeTexture);
	return hr;
}

HRESULT CRenderObject::OnResetDevice()
{
	HRESULT hr = S_OK;
	
	//设置使用哪种渲染方式和哪个纹理
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

