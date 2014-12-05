struct VertexOutput
{
	float4 initialPos	: SV_POSITION;
	//float3 initialVel	: VELOCITY;
	//float2 size			: SIZE;
	//float age : AGE;
	//unsigned int type : TYPE;
};

struct GSOutput
{
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;		// Not used
	float2 uv			: TEXCOORD0;
	float4 lightDir     : LIGHT;		// Not used
	float4 color        : COLOR;		// Not used
};

[maxvertexcount(4)]
void main(point VertexOutput input[1] /*: SV_POSITION*/, inout TriangleStream<GSOutput> output)
{
	//for (uint i = 0; i < 3; i++)
	//{
		GSOutput element;
		element.position = input[0].initialPos;
		element.uv = float2(0,0);
		element.normal = float3(0,0,0);
		element.lightDir = float4(0,0,0,0);
		element.color = float4(1.0, 0.0, 0.0, 1.0);
	//}

	output.Append(element);
}