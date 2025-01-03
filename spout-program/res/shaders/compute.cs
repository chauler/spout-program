#version 430 core

layout(local_size_x = {local_size_x}, local_size_y = {local_size_y}, local_size_z = {local_size_z}) in;

layout(rgba8ui, binding = 0) uniform uimage2D imgOutput;
layout(rgba8ui, binding = 1) uniform uimage2D imgInput; //Processed image with sobel edge data vec3(x, y, angle);

shared uint edges[{local_size_x}*{local_size_y}];
shared uint luminance[{local_size_x}*{local_size_y}];

void main()
{
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);

    uvec4 inValue = imageLoad(imgInput, texelCoord);
    edges[gl_LocalInvocationIndex] = 0; //Initialize to 0, overwrite
    luminance[gl_LocalInvocationIndex] = inValue.w;
    //Check for threshold

    edges[gl_LocalInvocationIndex] = inValue.x;

    if (inValue.x == 0)
    {
        edges[gl_LocalInvocationIndex] = 0;
    }

    memoryBarrierShared();
    barrier();
    //First thread of each group tallies up edges and averages the luminance
    if(gl_LocalInvocationID == 0)
    {
        uint edgeTotals[5] = uint[5](0, 0, 0, 0, 0);
        uint avgLum = 0;
        for (uint i=0; (i<{local_size_x}*{local_size_y}); (i++))
        {
            edgeTotals[edges[i]] += 1;
            avgLum += luminance[i];
        }
        avgLum = avgLum / ({local_size_x}*{local_size_y}); //Must use parentheses around the denominator
        uint edge = 1;
        //if (edgeTotals[1] >= edgeTotals[edge])
        //{
        //    edge = 1;
        //}
        if (edgeTotals[2] > edgeTotals[edge])
        {
            edge = 2;
        }
        if (edgeTotals[3] > edgeTotals[edge])
        {
            edge = 3;
        }
        if (edgeTotals[4] > edgeTotals[edge])
        {
            edge = 4;
        }
        if (edgeTotals[0] > {local_size_x}*{local_size_y}*2/3)
        {
            edge = 0;
        }
        uvec4 color = uvec4(0);
        //avgLum = 255;
        if(edge == 0)
        {
            color = uvec4(4, 0, 0, avgLum);
        }
        else if (edge == 1)
        {
            color = uvec4(3, 0, 0, avgLum);
        }
        else if (edge == 2)
        {
            color = uvec4(0, 0, 0, avgLum);
        }
        else if (edge == 3)
        {
            color = uvec4(1, 0, 0, avgLum);
        }
        else if (edge == 4)
        {
            color = uvec4(2, 0, 0, avgLum);
        }
        else
        {
            color = uvec4(0);
        }
        imageStore(imgOutput, ivec2(int(gl_WorkGroupID.x), int(gl_WorkGroupID.y)), color);
    }
}