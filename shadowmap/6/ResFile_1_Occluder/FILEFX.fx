//--------------------------------------------------------------------------------------
// �ļ���FileFX.fx
// ȫ������
//--------------------------------------------------------------------------------------

#define SHADOW_EPSILON 0.00005f

texture  g_ColorTexture;            // ��ɫ����
texture  g_RenderToTexture;         // ��Ⱦ��������������

float    g_fTime;                   // ����ʱ��
float4x4 g_matWorld;                // �������
float4x4 g_matView;                 // �۲����
float4x4 g_matProject;              // ͶӰ����
//
float4x4 g_matWorldRTT;
float4x4 g_matViewRTT;
float4x4 g_matProjectRTT;

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshRTTSampler = sampler_state
  {
    Texture = <g_RenderToTexture>;
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
    float2 TextureUV  : TEXCOORD0;  
  };

struct VS_OUTPUT
  {
    float4 Position   : POSITION;   
    float2 TextureUV  : TEXCOORD0;  
    
  };

struct VS_OUTPUTScene
{
    //�����û��۲�ռ��еĵ��λ��
    float4 Position : POSITION;
    //ģ����������
    float2 TextureUV : TEXCOORD0;
    //����λ�ڵƹ�ռ��еĵ��λ��
    float4 PositionInLight : TEXCOORD1;
   
};

// --------------------- Default Render -----------------------
VS_OUTPUTScene RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUTScene Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
    //������任����ԴΪ�۲�������ռ�
    float4x4 matLightWorldView = mul( g_matWorldRTT ,g_matViewRTT );
    float4x4 matLightWorldViewProject = mul( matLightWorldView ,g_matProjectRTT );
    Output.PositionInLight = mul( In.Position, matLightWorldViewProject );

    return Output;    
  }

float4 RenderScenePS( VS_OUTPUTScene In ) :COLOR0
  { 
    
    //ͶӰ����תΪ��������,�ӣ�-1��1��ת������0��1��
    float2 texUV = 0.5 * In.PositionInLight.xy / In.PositionInLight.w + float2(0.5,0.5);
    texUV.y = 1.0f - texUV.y;
    
     float compareZ = In.PositionInLight.z /  In.PositionInLight.w;
    // float compareZ = In.PositionInLight.z / 100;
     float theZ = tex2D(MeshRTTSampler,texUV)  ;
     float fShadow = (theZ  + SHADOW_EPSILON <compareZ)? 0.0f: 1.0f;
     if(fShadow < 0.5)
        return float4(1.0,0.0,0.0,1.0);
     else
        return float4(0.0,1.0,0.0,1.0);
     //float4 vColor = tex2D( MeshTextureSampler, In.TextureUV ) * fShadow;
    // return vColor;
  }

// --------------------- RenderToTexture ---------------------
VS_OUTPUT RenderToTextureVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorldRTT ,g_matViewRTT );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProjectRTT );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = Output.Position.zw;
    //
    return Output;    
  }

//-----------------------------------------------------------
// ��Ȼ�����
float4 RenderToTexturePS( VS_OUTPUT In ) :COLOR
  { 
      return  In.TextureUV.x / In.TextureUV.y ;
     // return  In.TextureUV.x /100;
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        //FillMode            = WireFrame;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }

technique RenderDepthToTextureShader
  {
    pass P0
      {
        ZEnable      = true;
        ZWriteEnable = true;
        ZFunc        = Less;
        
        VertexShader = compile vs_2_0 RenderToTextureVS( );
        PixelShader  = compile ps_2_0 RenderToTexturePS( );
      }
  }
