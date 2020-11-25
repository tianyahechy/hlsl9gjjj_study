//--------------------------------------------------------------------------------------
// �ļ���FileFX.fx
// ȫ������
//--------------------------------------------------------------------------------------

texture  g_ColorTexture;            // ��ɫ����
float    g_fTime;                   // ����ʱ��
float4x4 g_matWorld;                // �������
float4x4 g_matView;                 // �۲����
float4x4 g_matProject;              // ͶӰ����
//
//����۽�ƽ��,����ģ���ϵĸ����㵽�۽�ƽ���λ�ã���ģ���̶�
//��ƽ��
float4 g_vFocalPlane = float4(0.0, 0.0, -1.0, 23.0);
float g_fFactor = 0.07;
const float g_MaxBlurFactor = 5.0f;

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
	float4 Diffuse 		: COLOR0;		//�������ɫ����ģ�����Ӱ�����ALPHA��
    float2 TextureUV  	: TEXCOORD0; 	//�������� 
    
  };


// --------------------- Default Render -----------------------
VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output = (VS_OUTPUT) 0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
	Output.Diffuse.xyz = float3(1.0, 1.0, 1.0);
    //������任����ԴΪ�۲�������ռ�,���ڹ۲�����ϵ�м���ģ������
    float4 v4PositionInView = mul( In.Position ,matWorldView );
	float fBlurFactor = abs(dot(v4PositionInView, g_vFocalPlane ) ) * g_fFactor;
	fBlurFactor = fBlurFactor * fBlurFactor;
	Output.Diffuse.a = min(fBlurFactor, g_MaxBlurFactor);
    Output.TextureUV = In.TextureUV;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
	float4 vColor = tex2D(MeshTextureSampler, In.TextureUV ) * In.Diffuse;
	return vColor;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique RenderToTextureTech 
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }

technique DefaultTech
  {
 
  }

