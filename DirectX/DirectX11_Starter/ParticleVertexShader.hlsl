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
	float3 initialVel	: VELOCITY;
	float2 size			: SIZE;
	float age : AGE;
	//unsigned int type : TYPE;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;
	output.initialPos = float4(input.initialPos, 1.0f);
	output.initialVel = input.initialVel;
	output.size = input.size;
	output.age = input.age;
	//output.type = input.type;

	return output;
}