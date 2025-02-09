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
