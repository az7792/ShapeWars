// SPDX-FileCopyrightText: 2025 Xuefei Ai
// SPDX-License-Identifier: MIT

class EntityManager {
     static entityTypeToLevel = [2, 0, 1]; // 实体类型到渲染层级的映射
     constructor() {
          this.SparseSet = [new SparseSet(), new SparseSet(), new SparseSet()]; //[资源方块，玩家，子弹] 分层渲染，越小越先渲染
          this.deadEntities = []; //待删除的实体列表,用于播放动画
     }
     //清空实体管理器
     clear() {
          for (let i = 0; i < this.SparseSet.length; i++) {
               this.SparseSet[i].clear();
          }
          this.deadEntities = [];
     }

     //添加实体
     addEntityByDataView(dataView, offset) {
          let entityId = readEntityIDIndex(dataView, offset);//实体id的index部分
          let entityType = readTypeID(dataView, offset);//实体类型
          if (entityType == CATEGORY_PLAYER) {
               this.addEntity(EntityManager.entityTypeToLevel[entityType], entityId, PlayerEntity.create(dataView, offset));
          } else if (entityType == CATEGORY_BLOCK) {
               this.addEntity(EntityManager.entityTypeToLevel[entityType], entityId, PolygonEntity.create(dataView, offset));
          } else if (entityType == CATEGORY_BULLET) {
               this.addEntity(EntityManager.entityTypeToLevel[entityType], entityId, BulletEntity.create(dataView, offset));
          }
     }

     //移出实体
     removeEntityByDataView(dataView, offset) {
          let entityId = readEntityIDIndex(dataView, offset);//实体id的index部分
          for (let i = 0; i < this.SparseSet.length; i++) {
               this.SparseSet[i].try_remove(entityId);
          }
     }

     //删除实体
     deleteEntityByDataView(dataView, offset) {
          let entityId = readEntityIDIndex(dataView, offset);//实体id的index部分
          for (let i = 0; i < this.SparseSet.length; i++) {
               if (this.SparseSet[i].has(entityId)) {
                    let entity = this.SparseSet[i].get(entityId);
                    this.deadEntities.push(entity);
                    this.SparseSet[i].remove(entityId);
                    entity.initDeadStatus();
                    break;
               }
          }
     }

     //更新实体
     updateEntityByDataView(dataView, offset) {
          let entityId = readEntityIDIndex(dataView, offset);//实体id的index部分
          let entityType = dataView.getUint8(offset.value);//实体类型
          offset.value += 1;
          this.SparseSet[EntityManager.entityTypeToLevel[entityType]].get(entityId).update(dataView, offset);
     }

     getEntity(id) {
          for (let i = 0; i < this.SparseSet.length; i++) {
               if (this.SparseSet[i].has(id))
                    return this.SparseSet[i].get(id);
          }
     }


     addEntity(level, id, entity) { this.SparseSet[level].add(id, entity); }
     removeEntity(level, id) {
          console.log(level, id);

          this.SparseSet[level].remove(id);
     }
     updateEntity(level, id, entity) { this.SparseSet[level].get(id) = entity; }//实际更像是替换

     update(deltaTime) {
          for (let i = 0; i < this.SparseSet.length; i++) {
               for (let j = 0; j < this.SparseSet[i].dense.length; j++) {
                    this.SparseSet[i].dense[j].entity.showMe(deltaTime);
               }
          }
          let currentTime = Date.now();
          for (let i = this.deadEntities.length - 1; i >= 0; i--) {
               let entity = this.deadEntities[i];
               if (entity.deadTime + entity.deadAnimationDuration < currentTime) {
                    [this.deadEntities[i], this.deadEntities[this.deadEntities.length - 1]] = [this.deadEntities[this.deadEntities.length - 1], this.deadEntities[i]];
                    this.deadEntities.pop();
               }
               else {
                    entity.showDeadAnimation(currentTime);
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

     clear() {
          this.sparse = [];
          this.dense = [];
     }
}
