%VULKAN_SDK%\Bin\glslc.exe .\TestFrag.frag -o .\TestFrag.spv
%VULKAN_SDK%\Bin\glslc.exe .\TestVert.vert -o .\TestVert.spv

%VULKAN_SDK%\Bin\glslc.exe .\VertexShader.vert -o .\VertexShaderVert.spv
%VULKAN_SDK%\Bin\glslc.exe .\FragmentShader.frag -o .\FragmentShaderFrag.spv

%VULKAN_SDK%\Bin\glslc.exe .\BackgroundBoxShader.vert -o .\BackgroundBoxShaderVert.spv
%VULKAN_SDK%\Bin\glslc.exe .\BackgroundBoxShader.frag -o .\BackgroundBoxShaderFrag.spv