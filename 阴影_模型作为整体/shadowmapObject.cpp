#include "DXUT.h"
#include "shadowmapObject.h"

CShadowMapObject::CShadowMapObject()
{
	//m_pMeshSysMemoryVector.clear();
	_meshVector.clear();
	m_pEffect = NULL;

	m_TextureSize = 512;
	m_pRTTTexture = NULL;
	m_pRTTInterface = NULL;
	m_pRTTSurface = NULL;

	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProject);
	D3DXMatrixIdentity(&m_matViewRTT);
	D3DXMatrixIdentity(&m_matProjectRTT);

	_bInitShadowMap = false;
	_meshVector.clear();
	_wstrFXName = L"";

}

CShadowMapObject::~CShadowMapObject()
{
	SAFE_RELEASE(m_pEffect);

	SAFE_RELEASE(m_pRTTTexture);
	SAFE_RELEASE(m_pRTTInterface);
	SAFE_RELEASE(m_pRTTSurface);
	_meshVector.clear();
	

}

bool CShadowMapObject::getBInitShadowMap()
{
	return _bInitShadowMap;
}

void CShadowMapObject::setBInitShadowMap(bool bInitShadowMap)
{
	_bInitShadowMap = bInitShadowMap;
}

void CShadowMapObject::initShadowMap(
	std::vector<std::string> strModelNameVec,			//ģ����������
	std::vector<D3DXMATRIXA16> matWorldVec,				//������������
	std::vector<std::string> strShadowModelNameVec,		//����Ӱ����������
	std::wstring wstrFXName,
	IDirect3DDevice9 * pd3dDevice)
{
	this->setFXFileName( wstrFXName);
	this->initShaderFile(pd3dDevice);
	this->createRTT(pd3dDevice);
	this->loadModelVector(pd3dDevice, strModelNameVec, matWorldVec, strShadowModelNameVec);
}

void CShadowMapObject::setFXFileName(std::wstring wstrFXName)
{
	_wstrFXName = wstrFXName;
}

HRESULT CShadowMapObject::initShaderFile(IDirect3DDevice9 * pd3dDevice)
{
	//װ��Ч���ļ�
	LPD3DXBUFFER pCode = NULL;
	HRESULT hr = D3DXCreateEffectFromFile(pd3dDevice, _wstrFXName.c_str(), NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, &pCode);
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
HRESULT CShadowMapObject::loadModelVector(
	IDirect3DDevice9* pd3dDevice,
	std::vector<std::string> strModelNameVec,	//ģ����������
	std::vector<D3DXMATRIXA16> matWorldVec,		//������������
	std::vector<std::string> strShadowModelNameVec		//����Ӱ����������
)
{
	HRESULT hr = S_OK;
	int meshSize = strModelNameVec.size();
	if ( 0 == meshSize)
	{
		return hr;
	}
	_meshVector.resize(meshSize);
	for (int i = 0; i < meshSize; i++)
	{
		myMesh theMesh;
		theMesh.setMatWorld(matWorldVec[i]);
		theMesh.setModelName(strModelNameVec[i]);
		bool bFindShadow = false;
		for (int j = 0; j < strShadowModelNameVec.size(); j++)
		{
			if (strModelNameVec[i] == strShadowModelNameVec[j])
			{
				bFindShadow = true;
				break;
			}
		}
		theMesh.setGenerateShadowMap(bFindShadow);
		_meshVector[i] = theMesh;
		_meshVector[i].loadMeshFromModelName(pd3dDevice, strModelNameVec[i]);
	}

	return hr;
}
void CShadowMapObject::RenderToTexture(
	LPDIRECT3DDEVICE9 pd3dDevice, 
	D3DXMATRIXA16 * pView, 
	D3DXMATRIXA16 * pProject, 
	D3DXMATRIXA16 * pViewRTT, 
	D3DXMATRIXA16 * pProjectRTT)
{
	this->setFirstCameraMatrixVector( pView, pProject);
	this->setLightMatrixVector(pViewRTT, pProjectRTT);
	this->RenderToTexture(pd3dDevice);
}
HRESULT CShadowMapObject:: createRTT(IDirect3DDevice9 * pd3dDevice)
{
	HRESULT hr = S_OK;

	//���ݸ�ʽ���������ٴ�����ҳ�洴����Ⱦ�ӿ�
	//�����Ǵ���������ȡ�������ԣ�ȡ��0��󴴽�������Ⱦ�ӿ�
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
	//��������
	D3DXHANDLE textureHandle = m_pEffect->GetParameterByName(0, "g_RenderToTexture");
	m_pEffect->SetTexture(textureHandle, m_pRTTTexture);
	return hr;

}

void CShadowMapObject::renderObjectVector(IDirect3DDevice9 * pd3dDevice)
{
	D3DXHANDLE defaultHandle = m_pEffect->GetTechniqueByName("DefaultTech");
	m_pEffect->SetTechnique(defaultHandle);
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matView");
	m_pEffect->SetMatrix(matViewHandle, &m_matView);
	D3DXHANDLE matProjectHandle = m_pEffect->GetParameterByName(0, "g_matProject");
	m_pEffect->SetMatrix(matProjectHandle, &m_matProject);
	
	UINT cPasses;
	m_pEffect->Begin(&cPasses, 0);
	for (int i = 0; i < _meshVector.size(); i++)
	{
		D3DXHANDLE matWorldHandle = m_pEffect->GetParameterByName(0, "g_matWorld");
		D3DXMATRIXA16 matWorld = _meshVector[i].getMatWorld();
		m_pEffect->SetMatrix(matWorldHandle, &matWorld);
		for (int iPass = 0; iPass < cPasses; iPass++)
		{
			m_pEffect->BeginPass(iPass);

			_meshVector[i].RenderToScene(pd3dDevice, m_pEffect);
			
			//m_pEffect->CommitChanges();
			m_pEffect->EndPass();
		}
	}
	m_pEffect->End();
}

//��Ⱦ������
HRESULT CShadowMapObject::RenderToTexture(LPDIRECT3DDEVICE9 pd3dDevice)
{
	HRESULT hr;
	//����3������
	D3DXHANDLE RTTTechHandle = m_pEffect->GetTechniqueByName("RenderDepthToTextureShader");
	hr = m_pEffect->SetTechnique(RTTTechHandle);
	if (FAILED(hr))
	{
		return hr;
	}
	
	D3DXHANDLE matViewHandle = m_pEffect->GetParameterByName(0, "g_matViewRTT");
	m_pEffect->SetMatrix(matViewHandle, &m_matViewRTT);
	D3DXHANDLE matProjectHandle = m_pEffect->GetParameterByName(0, "g_matProjectRTT");
	m_pEffect->SetMatrix(matProjectHandle, &m_matProjectRTT);
	//�����ڴ��������������Ⱦ������
	m_pRTTInterface->BeginScene(m_pRTTSurface, NULL);
	pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.0f, 0);
	UINT uPasses;
	m_pEffect->Begin(&uPasses, D3DXFX_DONOTSAVESTATE);			
	for (UINT iPass = 0; iPass < uPasses; iPass++)
	{	
		m_pEffect->BeginPass(iPass);
		for (int i = 0; i < _meshVector.size(); i++)
		{
			//�����������Ӱ���򷵻�
			bool bGenerateShadowMap = _meshVector[i].getBGenerateShadowMap();
			if (!bGenerateShadowMap)
			{
				continue;
			}
			D3DXHANDLE matWorldHandle = m_pEffect->GetParameterByName(0, "g_matWorldRTT");
			D3DXMATRIXA16 matWorld = _meshVector[i].getMatWorld();
			m_pEffect->SetMatrix(matWorldHandle, &matWorld);	
			m_pEffect->CommitChanges();
			_meshVector[i].RenderForDepth(pd3dDevice);
			
		}
		m_pEffect->EndPass();
	}
	m_pEffect->End();
	m_pRTTInterface->EndScene(0);
	return hr;
}

void CShadowMapObject::setFirstCameraMatrixVector(D3DXMATRIXA16 * pView, D3DXMATRIXA16 * pProject)
{
	m_matView = *pView;
	m_matProject = *pProject;
}

void CShadowMapObject::setLightMatrixVector(D3DXMATRIXA16 * pViewRTT, D3DXMATRIXA16 * pProjectRTT)
{
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
	SAFE_RELEASE(m_pEffect);

	SAFE_RELEASE(m_pRTTSurface);
	SAFE_RELEASE(m_pRTTInterface);
	SAFE_RELEASE(m_pRTTTexture);
	_meshVector.clear();
}
