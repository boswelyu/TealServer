cc_binary(
    name = 'game_svr',
    srcs = [
        "main.cpp",
        "game_server.cpp",
        "client_msg_handler.cpp",
        "iomn_menu.cpp",
    ],

    incs = [
        '../../build64_release/proto',
        '../../common/base',
        '../../common/network',
        '../../common/extend',
    ],

    deps = [
        '//proto:proto_msg',
        '//common/base:base',
        '//common/network:network',
        '#pthread',
    ],
)