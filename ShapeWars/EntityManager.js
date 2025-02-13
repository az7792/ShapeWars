class EntityManager {
     static entityTypeToLevel = [1, 0, 2]; // 实体类型到渲染层级的映射
     constructor() {
          this.SparseSet = [new SparseSet(), new SparseSet(), new SparseSet()]; //[资源方块，玩家，子弹] 分层渲染，越小越先渲染
     }
     //添加实体
     addEntityByDataView(dataView, offset) {
          let entityId = dataView.getUint32(offset.value, true);//实体id
          offset.value += 4;
          let entityType = readTypeID(dataView, offset);//实体类型
          if (entityType == CATEGORY_PLAYER) {
               this.addEntity(EntityManager.entityTypeToLevel[entityType], entityId, PlayerEntity.create(dataView, offset));
          } else if (entityType == CATEGORY_BLOCK) {
               this.addEntity(EntityManager.entityTypeToLevel[entityType], entityId, PolygonEntity.create(dataView, offset));
          }else if(entityType == CATEGORY_BULLET){
               this.addEntity(EntityManager.entityTypeToLevel[entityType], entityId, BulletEntity.create(dataView, offset));
          }
     }

     //移除实体
     removeEntityByDataView(dataView, offset) {
          let entityId = dataView.getUint32(offset.value, true);//实体id
          offset.value += 4;
          for (let i = 0; i < this.SparseSet.length; i++) {
               this.SparseSet[i].try_remove(entityId);
          }
     }

     //更新实体
     updateEntityByDataView(dataView, offset) {
          let entityId = dataView.getUint32(offset.value, true);//实体id
          offset.value += 4;
          let entityType = dataView.getUint8(offset.value);//实体类型
          offset.value += 1;
          this.SparseSet[EntityManager.entityTypeToLevel[entityType]].get(entityId).update(dataView, offset);
     }


     addEntity(level, id, entity) { this.SparseSet[level].add(id, entity); }
     removeEntity(level, id) {
          console.log(level, id);

          this.SparseSet[level].remove(id);
     }
     updateEntity(level, id, entity) { this.SparseSet[level].get(id) = entity; }//实际更像是替换

     update(currentTime) {
          for (let i = 0; i < this.SparseSet.length; i++) {
               for (let j = 0; j < this.SparseSet[i].dense.length; j++) {
                    this.SparseSet[i].dense[j].entity.showMe(currentTime);
               }
          }
     }
}

class SparseSet {
     constructor() {
          this.sparse = []; // 稀疏数组，用于存储实体 ID 到密集数组索引的映射
          this.dense = [];  // 密集Map，用于存储实体对象
     }

     /**
      * 添加实体对象到稀疏集
      * @param {uint32} id - 实体的唯一标识符
      * @param {Object} entity - 实体对象
      */
     add(id, entity) {
          if (this.has(id)) {
               console.error(`Entity with id ${id} already exists.`);
               return;
          }

          // 在密集数组中添加实体，并在稀疏数组中建立映射
          this.sparse[id] = this.dense.length;
          this.dense.push({ id, entity });
     }

     /**
      * 检查稀疏集中是否存在某个 ID 的实体
      * @param {uint32} id - 实体的唯一标识符
      * @returns {boolean}
      */
     has(id) {
          return this.sparse[id] !== undefined && this.sparse[id] != null && this.sparse[id] < this.dense.length;
     }

     /**
      * 获取某个 ID 对应的实体对象
      * @param {uint32} id - 实体的唯一标识符
      * @returns {Object|null}
      */
     get(id) {
          if (this.has(id)) {
               return this.dense[this.sparse[id]].entity;
          }
          return null;
     }

     /**
      * 删除某个 ID 对应的实体对象
      * @param {uint32} id - 实体的唯一标识符
      */
     remove(id) {
          if (!this.has(id)) {
               console.error(`Entity with id ${id} does not exist.`);
               return;
          }

          const index = this.sparse[id]; // 稀疏数组中的索引
          const lastIndex = this.dense.length - 1;

          // 如果删除的不是最后一个元素，则将最后一个元素移到当前索引位置
          if (index !== lastIndex) {
               const lastEntity = this.dense[lastIndex];
               this.dense[index] = lastEntity;
               this.sparse[lastEntity.id] = index; // 更新稀疏数组的映射
          }

          // 删除最后一个元素，并清理稀疏数组
          this.dense.pop();
          this.sparse[id] = null;
     }

     try_remove(id) {
          if (this.has(id)) {
               this.remove(id);
          }
     }
}
