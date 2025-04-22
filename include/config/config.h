// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

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
#define CATEGORY_DRONE (1u << 5)

//方块填充颜色
#define BOLCK_FILL_GREEN 0x99cc33ff
#define BOLCK_FILL_BLUE 0x3399ccff
#define BOLCK_FILL_PURPLE 0x9966ccff
#define BOLCK_FILL_ORANGE 0xff9900ff
//方块边框颜色
#define BOLCK_STROKE_GREEN 0x336600ff
#define BOLCK_STROKE_BLUE 0x336699ff
#define BOLCK_STROKE_PURPLE 0x663399ff
#define BOLCK_STROKE_ORANGE 0xcc6600ff
//TODO: 各类配置项放不同配置文件中，避免重复编译不相关的cpp，对于可动态配置的项，采用文件读取的方式