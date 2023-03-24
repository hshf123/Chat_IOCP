pushd %~dp0
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Enum.proto
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Struct.proto
protoc.exe -I=./ --cpp_out=./ --csharp_out=./ ./Protocol.proto

GenPackets.exe --path=./Protocol.proto --output=ClientPacketHandler --recv=C_ --send=S_
GenPackets.exe --path=./Protocol.proto --output=ServerPacketHandler --recv=S_ --send=C_
IF ERRORLEVEL 1 PAUSE

XCOPY /Y ENum.pb.h "../../../Server"
XCOPY /Y ENum.pb.cc "../../../Server"
XCOPY /Y Struct.pb.h "../../../Server"
XCOPY /Y Struct.pb.cc "../../../Server"
XCOPY /Y Protocol.pb.h "../../../Server"
XCOPY /Y Protocol.pb.cc "../../../Server"
XCOPY /Y ClientPacketHandler.h "../../../Server"

XCOPY /Y ENum.pb.h "../../../DummyClient"
XCOPY /Y ENum.pb.cc "../../../DummyClient"
XCOPY /Y ENum.cs "../../../CSDummyClient/Protocol"
XCOPY /Y Struct.pb.h "../../../DummyClient"
XCOPY /Y Struct.pb.cc "../../../DummyClient"
XCOPY /Y Struct.cs "../../../CSDummyClient/Protocol"
XCOPY /Y Protocol.pb.h "../../../DummyClient"
XCOPY /Y Protocol.pb.cc "../../../DummyClient"
XCOPY /Y Protocol.cs "../../../CSDummyClient/Protocol"
XCOPY /Y ServerPacketHandler.h "../../../DummyClient"
XCOPY /Y ServerPacketHandler.cs "../../../CSDummyClient/Packet"

DEL /Q /F *pb.h
DEL /Q /F *.pb.cc
DEL /Q /F *.h
DEL /Q /F *.cs

PAUSE