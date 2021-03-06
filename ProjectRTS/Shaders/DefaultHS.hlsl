/*cbuffer TessellationBuffer
{
    float tessellationAmount;
    float3 padding;
};*/

struct VertexOut
{
    float4 position  : POSITION;
    float4 color : COLOR;
};

struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
};

struct HullOutputType
{
    float4 position : POSITION;
    float4 color : COLOR;
};

ConstantOutputType ColorPatchConstantFunction(InputPatch<VertexOut, 3> inputPatch, uint patchId : SV_PrimitiveID)
{
    ConstantOutputType output;


    // Set the tessellation factors for the three edges of the triangle.
    output.edges[0] = 1.f;
    output.edges[1] = 1.f;
    output.edges[2] = 1.f;

    // Set the tessellation factor for tessallating inside the triangle.
    output.inside = 1.f;

    return output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]

HullOutputType main(InputPatch<VertexOut, 3> patch, uint pointId : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HullOutputType output;


    // Set the position for this control point as the output position.
    output.position = patch[pointId].position;

    // Set the input color as the output color.
    output.color = patch[pointId].color;

    return output;
}