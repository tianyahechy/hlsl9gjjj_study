#include "DXUT.h"
#include "myMesh.h"

myMesh::myMesh()
{
	g_pMesh = NULL; // Our mesh object in sysmem
	g_pMeshMaterials = NULL; // Materials for our mesh
	g_pMeshTextures = NULL; // Textures for our mesh
	g_dwNumMaterials = 0L;   // Number of mesh materials
	g_bGenerateShadowMap = false;
	D3DXMatrixIdentity(&g_matWorld);
	g_strModelName = "";
}


myMesh::~myMesh()
{
	if (g_pMeshMaterials != NULL)
		delete[] g_pMeshMaterials;
	
	if (g_pMeshTextures)
	{
		for (DWORD i = 0; i < g_dwNumMaterials; i++)
		{
			if (g_pMeshTextures[i])
				g_pMeshTextures[i]->Release();
		}
		delete[] g_pMeshTextures;
	}
	
	if (g_pMesh != NULL)
		g_pMesh->Release();
		
}

HRESULT myMesh::loadMeshFromModelName(LPDIRECT3DDEVICE9 g_pd3dDevice, std::string strModelName)
{
	LPD3DXBUFFER pD3DXMtrlBuffer;

	// Load the mesh from the specified file
	if (FAILED(D3DXLoadMeshFromXA(strModelName.c_str(), D3DXMESH_SYSTEMMEM,
		g_pd3dDevice, NULL,
		&pD3DXMtrlBuffer, NULL, &g_dwNumMaterials,
		&g_pMesh)))
	{
		MessageBox(NULL, L"Could not find tiger.x", L"Meshes.exe", MB_OK);
		return E_FAIL;
	}
	//设置模型所在的目录
	int nPos = strModelName.rfind("\\");
	std::string strDir = strModelName.substr(0, nPos);
	
	// We need to extract the material properties and texture names from the 
	// pD3DXMtrlBuffer
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();
	g_pMeshMaterials = new D3DMATERIAL9[g_dwNumMaterials];
	if (g_pMeshMaterials == NULL)
		return E_OUTOFMEMORY;
	g_pMeshTextures = new LPDIRECT3DTEXTURE9[g_dwNumMaterials];
	if (g_pMeshTextures == NULL)
		return E_OUTOFMEMORY;

	for (DWORD i = 0; i < g_dwNumMaterials; i++)
	{
		// Copy the material
		g_pMeshMaterials[i] = d3dxMaterials[i].MatD3D;

		// Set the ambient color for the material (D3DX does not do this)
		g_pMeshMaterials[i].Ambient = g_pMeshMaterials[i].Diffuse;

		g_pMeshTextures[i] = NULL;
		if (d3dxMaterials[i].pTextureFilename == NULL)
		{
			continue;
		}
		std::string strTexture = strDir + "\\" + d3dxMaterials[i].pTextureFilename;
		D3DXCreateTextureFromFileA(g_pd3dDevice,
			strTexture.c_str(),
			&g_pMeshTextures[i]);
	
	}

	// Done with the material buffer
	pD3DXMtrlBuffer->Release();

	return S_OK;
}

void myMesh::RenderForDepth(LPD3DXEFFECT effect)
{
	for (DWORD i = 0; i < g_dwNumMaterials; i++)
	{
		effect->CommitChanges();
		// Draw the mesh subset
		g_pMesh->DrawSubset(i);
	}

}


void myMesh::RenderToScene(LPDIRECT3DDEVICE9 g_pd3dDevice, LPD3DXEFFECT effect)
{
	D3DXHANDLE colorTextureHandle = effect->GetParameterByName(0, "g_ColorTexture");
	
	for (DWORD i = 0; i < g_dwNumMaterials; i++)
	{
		// Set the material and texture for this subset
		g_pd3dDevice->SetMaterial(&g_pMeshMaterials[i]);
		//g_pd3dDevice->SetTexture(0, g_pMeshTextures[i]);
		effect->SetTexture(colorTextureHandle, g_pMeshTextures[i]);
		effect->CommitChanges();
		// Draw the mesh subset
		g_pMesh->DrawSubset(i);
	}
}

void myMesh::setMatWorld(D3DXMATRIXA16 matWorld)
{
	g_matWorld = matWorld;
}

D3DXMATRIXA16 myMesh::getMatWorld()
{
	return g_matWorld;
}

void myMesh::setGenerateShadowMap(bool generateShadowMap)
{
	g_bGenerateShadowMap = generateShadowMap;
}

bool myMesh::getBGenerateShadowMap()
{
	return g_bGenerateShadowMap;
}

void myMesh::setModelName(std::string strModelName)
{
	g_strModelName = strModelName;
}

std::string myMesh::getModelName()
{
	return g_strModelName;
}

int myMesh::getSubNum()
{
	return g_dwNumMaterials;
}
