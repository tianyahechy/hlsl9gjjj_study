//--------------------------------------------------------------------------------------
// �ļ���FileFX.fx
// ȫ������
//--------------------------------------------------------------------------------------

texture  g_ColorTexture;            // ��ɫ����
float    g_fTime;                   // ����ʱ��
float4x4 g_matWorld;                // �������
float4x4 g_matView;                 // �۲����
float4x4 g_matProject;              // ͶӰ����

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   : POSITION;  		//ģ�Ͷ���λ��
    float2 TextureUV  : TEXCOORD0;  	//��������
  };

struct VS_OUTPUT
  {
    float4 Position   	: POSITION;   	//�任���λ��
    float2 TextureUV  	: TEXCOORD0; 	//��������     
  };

// --------------------- Default Render -----------------------
VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output = (VS_OUTPUT) 0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
	float4 vColor = tex2D(MeshTextureSampler, In.TextureUV );
	return vColor;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech 
  {
    pass P0
      {
		ClipPlaneEnable	= 3;
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }


