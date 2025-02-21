#pragma once

// 组件分页大小
#define ECS_COMPONENT_PAGE_SIZE 128

// WebSocketServer最大连接数
#define MAX_CONNECTED 20

// 监听端口
#define LISTEN_IP "0.0.0.0"

// 监听IP
#define LISTEN_PORT 7792

// box2d 帧率
#define TPS 30

// 实体类型
#define CATEGORY_PLAYER (1u << 0)
#define CATEGORY_BLOCK (1u << 1)
#define CATEGORY_BULLET (1u << 2)
#define CATEGORY_CAMERA (1u << 3)
#define CATEGORY_BORDER_WALL (1u << 4)

//方块填充颜色
#define BOLCK_FILL_GREEN 0x3ff566ff
#define BOLCK_FILL_BLUE 0x4c69a0ff
#define BOLCK_FILL_PURPLE 0xff57e6ff
#define BOLCK_FILL_ORANGE 0xffac4dff
//方块边框颜色
#define BOLCK_STROKE_GREEN 0x238f28ff
#define BOLCK_STROKE_BLUE 0x32467cff
#define BOLCK_STROKE_PURPLE 0xa83c98ff
#define BOLCK_STROKE_ORANGE 0xc7873dff