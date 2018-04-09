@echo off
if not exist Build mkdir Build

cd Include\XWorld
..\..\protoc\protoc.exe --cpp_out=. xworld.proto xconfig.proto xtabfile.proto
move *.cc ..\..\XWorld

cd ..\..\

protoc\ProtoGen\protogen.exe -i:Include-Common\CloudAtlas\xrpc.proto -o:Build\xrpc.cs -ns:XProtocol
protoc\ProtoGen\protogen.exe -i:Include\XWorld\xworld.proto -o:Build\xworld.cs -ns:XProtocol
protoc\ProtoGen\protogen.exe -i:Include\XWorld\xtabfile.proto -o:Build\xtabfile.cs -ns:XProtocol

C:\Windows\Microsoft.NET\Framework\v3.5\csc.exe /noconfig "/r:C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v3.5\Profile\Unity Subset v3.5\mscorlib.dll" "/r:protoc/CoreOnly/ios/protobuf-net.dll" -nostdlib "/out:Build\XProtocol.dll" "/r:C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v3.5\Profile\Unity Subset v3.5\System.dll" "/r:C:\Program Files (x86)\Reference Assemblies\Microsoft\Framework\.NETFramework\v3.5\Profile\Unity Subset v3.5\System.Core.dll" /nologo /warn:4 /optimize+ /t:library "Build\xrpc.cs" "Build\xworld.cs" "Build\xtabfile.cs" /fullpaths /utf8output

copy protoc\CoreOnly\ios\protobuf-net.dll Build\
protoc\Precompile\precompile.exe Build\XProtocol.dll -o:Build\XProtocolSerializer.dll -t:XProtocolPrecompile

copy Build\XProtocol.dll Client\Assets\Plugins\protobuf-net\
copy Build\XProtocolSerializer.dll Client\Assets\Plugins\protobuf-net\

del Build\*.cs
del Build\protobuf-net.dll
del Build\XProtocolSerializer.dll
del Build\XProtocol.dll

::pause
