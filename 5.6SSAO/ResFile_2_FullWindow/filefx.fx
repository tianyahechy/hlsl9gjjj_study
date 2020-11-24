
//ssao效果
//深度纹理
texture g_texDepthTexture;
//噪声纹理(16个随机向量存储的纹理）
texture g_texNoiseMap;

sampler g_SceneDepthSampler = sampler_state
{
	Texture = <g_texDepthTexture>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
	ADDRESSU = Border;
	ADDRESSV = Border;
};

sampler g_RotSampler4x4 = sampler_state
{
	Texture = <g_texNoiseMap>;
	MinFilter = Linear;
	MagFilter = Linear;
	MipFilter = Linear;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : POSITION;
	float4 color : COLOR0;
	float2 tex : TEXCOORD0;
};

VS_OUTPUT VS_AO(VS_INPUT In)
{
	VS_OUTPUT Out = ( VS_OUTPUT) 0;
	In.pos.x = sign(In.pos.x);
	In.pos.y = sign(In.pos.y);
	Out.pos = float4(In.pos.x, In.pos.y, 0.91, 1 );
	Out.tex = In.tex;
	Out.color = In.color;
	return Out;
};

float4 PS_AO(VS_OUTPUT In) : COLOR0
{
	float4 v4OutColor = float4(0.0, 0.0, 0.0, 1.0 );
	//显示窗口的大小
	const float2 screenSize = float2(640, 410 );
	//远剪裁平面
	const float farClipDist = 70.0f;
	//暂存纹理坐标
	float2 screenTC = In.tex;
	//得到旋转向量,旋转贴到4个屏幕像素上
	float2 rotaitonTC = screenTC * screenSize / 4;
	float3 vRotation = 2 * tex2D(g_RotSampler4x4, rotaitonTC ).rgb - 1;
	//从旋转向量创建矩阵
	float3x3 rotMat;
	float h = 1.0/( 1.0 + vRotation.z );
	rotMat._m00 = h * vRotation.y * vRotation.y + vRotation.z;
	rotMat._m01 = h * vRotation.y * vRotation.x;
	rotMat._m02 = - vRotation.x;
	rotMat._m10 = - h * vRotation.y * vRotation.x;
	rotMat._m11 = -h * vRotation.x * vRotation.x + vRotation.z;
	rotMat._m12 = -vRotation.y;
	rotMat._m20 = vRotation.x;
	rotMat._m21 = vRotation.y;
	rotMat._m22 = vRotation.z;
	//得到当前深度，转换成米
	float fSceneDepthP = tex2D( g_SceneDepthSampler, screenTC ).r * farClipDist;
	//设置影像的范围
	const int nSamplesNum = 16;
	float offsetScale = 0.01;
	const float offsetScaleStep = 1 + 2.4 / nSamplesNum;
	float Accessibility = 0;
	//采样区域和累积
	for(int i = 0; i < (nSamplesNum / 8 ); i++)
	{
		for(int x = -1; x <= 1; x = x+2)
		{
			for(int y = -1; y <= 1; y = y+2)
			{
				for(int z = -1; z <= 1; z = z+2)
				{
					//产生偏移向量
					float3 vOffset = normalize(float3(x,y,z)) * (offsetScale *= offsetScaleStep );
					//通过旋转矩阵旋转偏移向量
					float3 vRotatedOffset = mul(vOffset, rotMat );
					//得到3d坐标的重心位置
					float3 vSamplePos = float3( screenTC, fSceneDepthP);
					//通过偏移向量变换坐标（范围转换和宽度深度值）
					vSamplePos += float3(vRotatedOffset.xy, vRotatedOffset.z * fSceneDepthP * 2 );
					//从场景深度读取深度值，转换成米
					float fSceneDepthS = tex2D(g_SceneDepthSampler, vSamplePos.xy ) * farClipDist;
					//检查两个深度是否接近，则采样点影响像素中心值
					float fRangeIsInvalid = saturate(((fSceneDepthP - fSceneDepthS ) / fSceneDepthS));
					//累加访问值，默认值为0.5
					Accessibility += lerp(fSceneDepthS > vSamplePos.z, 0.5, fRangeIsInvalid );
					
				}
			}
		}
	}
	//得到平均值
	Accessibility = Accessibility / nSamplesNum;
	//放大或进行饱和运算
	return saturate(Accessibility * Accessibility + Accessibility );
	
}

technique techSSAO
{
	pass Pass0
	{
		VertexShader = compile vs_3_0 VS_AO();
		PixelShader = compile ps_3_0 PS_AO();
		CullMode = CCW;
	}
}
