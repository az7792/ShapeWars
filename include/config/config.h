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


enum class MyCategories // uint32
{
     PLAYER = 0x00000001,
     CAMERA = 0x00000002,
     // BULLET = 0x00000004,
     // RESOURCE_BLOCK = 0x00000008,
     // BORDER_WALL = 0x00000010,
};