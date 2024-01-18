from grpc_tools import protoc

protoc.main(
(
"",
"-I../../protos",
"--python_out=.",
"--pyi_out=.",
"--grpc_python_out=.",
"../../protos/message.proto",
)
)