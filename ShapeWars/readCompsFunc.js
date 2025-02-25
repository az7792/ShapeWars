/**
 * 读取实体ID的index部分
 * @returns 实体ID的index部分
 */
function readEntityIDIndex(dataView, offset) {
     let entityID = dataView.getUint32(offset.value, true);
     offset.value += 4;
     return entityID & 0x000FFFFF;
}

/**
 * 读取位置
 * @returns [x,y]
 */
function readPosition(dataView, offset) {
     let val = [];
     val[0] = dataView.getFloat32(offset.value, true);//x
     offset.value += 4;
     val[1] = dataView.getFloat32(offset.value, true);//y
     offset.value += 4;
     return val;
}

/**
 * 读取速度
 * @returns [vx,vy]
 */
function readVelocity(dataView, offset) {
     return readPosition(dataView, offset);//[vx,vy]
}

/**
 * 读取角度
 * @returns 角度 
 */
function readAngle(dataView, offset) {
     let angle = dataView.getFloat32(offset.value, true);
     offset.value += 4;
     return angle;
}

/**
 * 读取多边形
 * @returns [边数,半径] 
 */
function readPolygon(dataView, offset) {
     let val = [];
     val[0] = dataView.getUint8(offset.value, true);//边数
     offset.value += 1;
     val[1] = dataView.getFloat32(offset.value, true);//半径
     offset.value += 4;
     return val;
}

/**
 * 读取HP
 * @returns [当前血量,最大血量]
 */
function readHP(dataView, offset) {
     let val = [];
     val[0] = dataView.getInt16(offset.value, true);//当前血量
     offset.value += 2;
     val[1] = dataView.getInt16(offset.value, true);//最大血量
     offset.value += 2;
     return val;
}

/**
 * 读取类型ID
 * @returns 类型ID
 */
function readTypeID(dataView, offset) {
     let typeID = dataView.getUint8(offset.value, true);
     offset.value += 1;
     return typeID;
}


/**
 * 读取碰撞组
 * @returns 碰撞组
 */
function readGroupIndex(dataView, offset) {
     let index = dataView.getInt32(offset.value, true);
     offset.value += 4;
     return index;
}

/**
 * 读取名称
 * @returns 名称
 */
function readName(dataView, offset) {
     let len = dataView.getUint8(offset.value, true);
     offset.value += 1;

     const nameBytes = new Uint8Array(dataView.buffer, offset.value, len);
     offset.value += len;

     return new TextDecoder().decode(nameBytes);//使用 TextDecoder 解码字节流
}

/**
 * 读取填充样式
 * @return [填充颜色,描边颜色]
 */
function readStyle(dataView, offset) {
     let fillColor = dataView.getUint32(offset.value, true);
     offset.value += 4;
     let strokeColor = dataView.getUint32(offset.value, true);
     offset.value += 4;

     return [uint32ToHexRGBA(fillColor), uint32ToHexRGBA(strokeColor)];
}

// 将 Uint32 转换为 RRGGBBAA 字符串
function uint32ToHexRGBA(color) {
     const r = (color >> 24) & 0xFF; // R
     const g = (color >> 16) & 0xFF; // G
     const b = (color >> 8) & 0xFF;  // B
     const a = color & 0xFF;         // A

     // 转换为 2 位 16 进制字符串并拼接
     return `#${r.toString(16).padStart(2, '0')}${g.toString(16).padStart(2, '0')}${b.toString(16).padStart(2, '0')}${a.toString(16).padStart(2, '0')}`;
}


/**
 * 读取得分
 * @returns 分数
 */
function readScore(dataView, offset) {
     let score = dataView.getInt32(offset.value, true);
     offset.value += 4;
     return score;
}

function readStandings(dataView, offset) {
     let len = dataView.getUint8(offset.value, true);
     offset.value += 1;

     while (standings.length < len) {
          standings.push(["", 0]);
     }
     while (standings.length > len) {
          standings.pop();
     }

     for (let i = 0; i < len; i++) {
          standings[i][0] = readName(dataView, offset);
          standings[i][1] = readScore(dataView, offset);
     }
}