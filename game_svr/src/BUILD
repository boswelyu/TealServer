cc_binary(
    name = 'game_svr',
    srcs = [
        "main.cpp",
        "game_server.cpp",
        "client_msg_handler.cpp",
        "iomn_menu.cpp",
    ],

    incs = [
        '../../common/base',
        '../../common/network',
    ],

    deps = [
        '//common/base:base',
        '//common/network:network',
        '#pthread',
    ],
)