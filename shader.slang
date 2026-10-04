static float2 position[3] = float2[]
(
 float2(0, -0.5),
 float2(0.5, 0.2),
 float2(-0.5, 0.2)
);

static float3 color[3] = float3[]
(
 float3(1.0, 0.0, 0.0),
 float3(0.0, 1.0, 0.0),
 float3(0.0, 0.0, 1.0),
);

struct VertexOutput {
  float3 color;
  float4 sv_position : SV_Position; 
};

[shader("vertex")]
VertexOutput vertexMain(uint vid : SV_VertexID) {
  VertexOutput output;
  output.color = color[vid];
  output.sv_position = float4(position[vid], 0.0, 1.0);
  return output;
}

[shader("fragment")]
float4 fragMain(VertexOutput inVertex) : SV_Target
{
  float3 color = inVertex.color;
  return float4(color, 1.0);
}

