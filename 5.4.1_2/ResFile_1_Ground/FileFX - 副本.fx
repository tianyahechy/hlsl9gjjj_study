//--------------------------------------------------------------------------------------
// File: 受光物体.fx
//
//--------------------------------------------------------------------------------------

texture g_ColorTexture;              // Color texture for mesh

float    g_fTime;                   // App's time in seconds
float4x4 g_matWorld;                  // World matrix for object
float4x4 g_matView;
float4x4 g_matProject;

float4x4 g_matWorldNormalInverseTranspose; 
float4   g_vEyePosition;            // 观察者位置
float4   g_vLightPosition ;         // 灯光位置
float4 	 g_vlightDirect;			//灯光照射方向	
// 
float 	 g_fCone = 0.9f;			//光锥大小，可以作为主程序传入

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
    float3 Normal     : NORMAL; 
    float2 TextureUV  : TEXCOORD0;  
  };


struct VS_OUTPUT
  {
    float4 Position   	: POSITION;   
    float3 Normal     	: TEXCOORD0; 
    float2 TextureUV  	: TEXCOORD1;  
    float3 vLight   	: TEXCOORD2;       	// 模型上每个顶点上指向光源的方向    
    float3 vLookAt  	: TEXCOORD3;       	// 模型上每个顶点上指向观察者的方向
    float3 pos		   	: TEXCOORD4;		// 屏幕上每个像素点的空间位置
  };

VS_OUTPUT RenderSceneVS( VS_INPUT In )
  {
    VS_OUTPUT Out = ( VS_OUTPUT )0;
    float4x4 matWorldView = mul( g_matWorld ,g_matView );
    float4x4 matWorldViewProject = mul( matWorldView ,g_matProject );
    Out.Position = mul( In.Position , matWorldViewProject );
    Out.TextureUV = In.TextureUV;
    // 法向量
    Out.Normal = normalize( mul( In.Normal , ( float3x3 )g_matWorldNormalInverseTranspose ));
    // 顶点位置
    float4 Position = mul( In.Position ,g_matWorld );
    //计算每个顶点上的光线方向
    Out.vLight = g_vLightPosition - Position ;
	//每个顶点指向观察者的方向
    Out.vLookAt =  normalize( Position - g_vEyePosition );
	//顶点在完成世界变换后即可，因为需要在世界空间中比较
	Out.pos = Position;
    return Out;
  }

float4 RenderScenePS(VS_OUTPUT In ) : COLOR0
{
	//设定光锥过渡区域
	float MIN = 0.85f;
	//设定光锥过渡区域
	float MAX = 0.9f;
	//光线距离衰减因子
	float fLength = length(In.vLight) * 0.10;
	// 环境光
    float fAmbient = 0.1f;
    // 高光因子
    float4 fSpecularColor  = float4( 1.0 ,  1.0 ,1.0 , 1.0 );
	//计算灯光到顶点的向量
	float3 fDirectInVertex = normalize(In.pos - g_vLightPosition.xyz );
	float fDiffuse = 0;
	float fSpecular = 0;
	//计算每个像素中灯光的照射方向与聚光灯目标方向的夹角
	float fTensity = dot(fDirectInVertex, normalize(g_vlightDirect ));
	//如果夹角大于最小设定值，则可以使用一定的光照计算强度
	if( fTensity > MIN)
	{
		float fTensity = clamp(fTensity, MIN, MAX) - MIN;
		fTensity = 30 * fTensity;
		//漫反射光
		fDiffuse =  fTensity * 0.4* dot(In.Normal, normalize(In.vLight)) / fLength;
		//高光
		fSpecular = fTensity * pow(saturate(dot(reflect(normalize(In.vLight),In.Normal),In.vLookAt)), 10) / fLength;
	}
	//在表示聚光灯外的像素时，漫反射和镜面高光为0
	else
	{
		fDiffuse = 0;
		fSpecular = 0;
	}
    return  tex2D(MeshTextureSampler, In.TextureUV)  * ( fAmbient + fDiffuse + fSpecular * fSpecularColor);
}

technique DefaultTech
{
    pass p0
    {
         FillMode = Solid;
         VertexShader = compile vs_2_0 RenderSceneVS();
         PixelShader = compile ps_2_0 RenderScenePS();
    }
}





