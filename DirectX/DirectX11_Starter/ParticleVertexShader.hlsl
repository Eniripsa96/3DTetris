cbuffer perModel : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;
	float4 lightDirection;
	float4 color;
	float4 camPos;
};

struct VertexInput
{
	float3 initialPos	: POSITION;
	float3 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float age			: AGE;
	//unsigned int type	: TYPE;
};

struct VertexOutput
{
	float4 initialPos	: SV_POSITION;
	//float3 initialVel	: VELOCITY;
	//float2 size			: SIZE;
	//float age : AGE;
	//unsigned int type : TYPE;
	float4 camPos		: POSITION;
	matrix viewProj		: MATRIX;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;
	output.initialPos = float4(input.initialPos, 1.0f);
	//output.initialVel = input.initialPos;
	//output.size = input.size;
	//output.age = input.age;
	//output.type = input.type;
	output.camPos = camPos;
	//output.view = view;
	output.viewProj = mul(view, projection);

	return output;
}