//--------------------------------------------------------------------------------------
// File: BasicHLSL.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;
texture g_NormalTexture;
texture g_SpecularTexture;

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4 	 g_vEyePosition;			//观察者位置
float4 	 g_vLightPosition;			//灯光位置

sampler MeshTextureSampler = sampler_state
  {
    Texture = <g_ColorTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };

sampler MeshNormalSampler = sampler_state
  {
    Texture = <g_NormalTexture>;
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
  };
  
sampler MeshSpecularSampler = sampler_state
  {
    Texture = <g_SpecularTexture>;
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
	float3 Normal		: NORMAL;		//顶点上的法向量
	float3 Tanget		: TANGENT;		//模型顶点的切线坐标
    float2 TextureUV  	: TEXCOORD0; 	//纹理坐标 	
  };


struct VS_OUTPUT
  {
    float4 Position   : POSITION;   	//变换后的顶点位置
    float2 TextureUV  : TEXCOORD1;  	//顶点上的纹理坐标
	float3 vLight	  : TEXCOORD2;		//模型上每个顶点指向光源的方向
	float3 vLookAt	  : TEXCOORD3;		//模型上每个顶点指向观察者的方向
    float3x3 matMTW	  : TEXCOORD4;		//此矩阵是模型法向量变换到世界空间的矩阵,用来将法线纹理的方向变换到世界空间，才能计算光照	   
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Output;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Output.Position = mul( In.Position ,matWorldViewProject );
    Output.TextureUV = In.TextureUV;
	//将世界坐标变换到切线空间的变换矩阵
	float3x3 matModelToWorld = float3x3( mul(In.Tanget, g_matWorld ).xyz, mul( cross(In.Tanget, In.Normal), g_matWorld ).xyz, mul(In.Normal, g_matWorld).xyz );
	Output.matMTW = matModelToWorld;
	//将灯光按世界坐标变换
	float4 Position = mul(In.Position, g_matWorld );
	//每个顶点上的灯光反向
	Output.vLight = g_vLightPosition - Position;
	//每个顶点上指向观察者的方向
	Output.vLookAt = normalize(Position - g_vEyePosition );
	
    return Output;    
  }

float4 RenderScenePS( VS_OUTPUT In ) :COLOR0
  { 
	//灯光衰减因子
	float fScaleDistance = 5 / length(In.vLight);
	//从法线纹理取模型空间的法线
	float3 vNormalInTexture = normalize(2 * tex2D( MeshNormalSampler, 5 * In.TextureUV ) - 1.0 );
	//将纹理坐标转换成空间法向量
	float3 vNormalInWorld = mul(vNormalInTexture, In.matMTW );
	//环境光
	float fAmbient = 0.2f;
	//高光因子
	float4 vSpecularColor = float4( 1.0, 1.0, 1.0, 1.0 );
	float3 fNormal = normalize(vNormalInWorld);
	float fDiffuse = 0.4 * dot(fNormal, normalize(In.vLight)) * fScaleDistance;
	//高光
	float fSpecular = pow(saturate(dot(reflect(normalize(In.vLight),fNormal ), In.vLookAt)), 4 ) * fScaleDistance;
    return tex2D(MeshTextureSampler, In.TextureUV ) * ( fAmbient + fDiffuse) + fSpecular * vSpecularColor * tex2D(MeshSpecularSampler, 5 * In.TextureUV );
  }

//------------------------------------------------------------------------------
// technique
//------------------------------------------------------------------------------
technique DefaultTech
  {
    pass P0
      {
        FillMode            = Solid;
        VertexShader = compile vs_2_0 RenderSceneVS( );
        PixelShader  = compile ps_2_0 RenderScenePS( );
      }
  }
