//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
//--------------------------------------------------------------------------------------

texture3D g_VolumeTexture;

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;
  
sampler MeshVolumeSampler = sampler_state
  {
    Texture = <g_VolumeTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_INPUT
  {
    float4 Position   	: POSITION;   	//顶点位置
    float2 TextureUV  	: TEXCOORD0; 	//纹理坐标 	
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   	//变换后的顶点位置
	float4 my3d:		TEXCOORD1;
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
	Output.my3d = Output.Position;
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
	//天空颜色
	float4 sky = float4(0.0, 0.0, 1.0, 1.0 );
	//云彩颜色
	float4 cloudColor = float4(1.0, 1.0, 1.0, 1.0 );
	//将xy坐标按某速度移动
	In.my3d.xy += g_fTime * 3.7;
	In.my3d.z += g_fTime * 4.8;
	//返回红色
	float noisy = tex3D(MeshVolumeSampler, In.my3d.xyz / 115.0 ).r;
	float lrp = noisy * 4 - 1.0;
	//在天空和云彩之间插值
    return lerp( cloudColor, sky, saturate(lrp));
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
		FillMode = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
