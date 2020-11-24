//--------------------------------------------------------------------------------------
// 文件：FileFX.fx
// 全部物体
//--------------------------------------------------------------------------------------

float g_fFar = 40;

texture  g_ColorTexture;            // 颜色纹理

float    g_fTime;                   // 传入时间
float4x4 g_matWorld;                // 世界矩阵
float4x4 g_matView;                 // 观察矩阵
float4x4 g_matProject;              // 投影矩阵

float4x4 g_matWorldNormalInverseTranspose;
float4	 g_vEyePosition;			//观察者位置
float4 	 g_vLightPosition;			//灯光位置	
//
float4x4 g_matWorldRTT;				//渲染到纹理使用的世界矩阵
float4x4 g_matViewRTT;				//渲染到纹理使用的观察矩阵
float4x4 g_matProjectRTT;			//渲染到纹理使用的投影矩阵

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
