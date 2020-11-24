//--------------------------------------------------------------------------------------
// �ļ���FileFX.fx
// ȫ������
//--------------------------------------------------------------------------------------

float g_fFar = 40;

texture  g_ColorTexture;            // ��ɫ����

float    g_fTime;                   // ����ʱ��
float4x4 g_matWorld;                // �������
float4x4 g_matView;                 // �۲����
float4x4 g_matProject;              // ͶӰ����

float4x4 g_matWorldNormalInverseTranspose;
float4	 g_vEyePosition;			//�۲���λ��
float4 	 g_vLightPosition;			//�ƹ�λ��	
//
float4x4 g_matWorldRTT;				//��Ⱦ������ʹ�õ��������
float4x4 g_matViewRTT;				//��Ⱦ������ʹ�õĹ۲����
float4x4 g_matProjectRTT;			//��Ⱦ������ʹ�õ�ͶӰ����

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
    float4 Position   : POSITION;  
	float3 Normal 	  :	NORMAL;
    float2 TextureUV  : TEXCOORD0;  
  };

struct VS_OUTPUTDS
  {
    float4 Position   : POSITION;   
    float3 PositionInTex  : TEXCOORD0;
    float3 NormalInTex  : TEXCOORD1;
    float Depth  : TEXCOORD2;  
    
  };
  
// --------------------- Default Render -----------------------
VS_OUTPUTDS RenderDepthVS( VS_INPUT In )
  {
    VS_OUTPUTDS Output = (VS_OUTPUTDS) 0;
    float4x4 matWorldView = mul( g_matWorldRTT ,g_matViewRTT );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProjectRTT );
	Output.Position = mul(In.Position, matWorldViewProject);
    Output.PositionInTex = Output.Position;
	Output.NormalInTex = In.Normal;
	Output.Depth = Output.Position.z;

    return Output;    
  }

float4 RenderDepthPS( VS_OUTPUTDS In ) :COLOR0
{
	float fDepth = In.Depth / g_fFar;
	return fDepth;
}

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique RenderDepthTech
  {
    pass P0
      {
        //FillMode            = WireFrame;
        VertexShader = compile vs_2_0 RenderDepthVS( );
        PixelShader  = compile ps_2_0 RenderDepthPS( );
      }
  }
