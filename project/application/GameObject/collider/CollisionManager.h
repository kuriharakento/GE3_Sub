//#pragma once
//#include <memory>
//#include <vector>
//
//#include "application/GameObject/component/collision/CollisionComponent.h"
//
//// 衝突ペアを扱う構造体
//struct CollisionPair {
//    GameObject* a;
//    GameObject* b;
//
//    bool operator==(const CollisionPair& other) const {
//        return (a == other.a && b == other.b) || (a == other.b && b == other.a);
//    }
//};
//
//struct CollisionPairHash {
//    std::size_t operator()(const CollisionPair& pair) const {
//        auto ha = std::hash<GameObject*>()(pair.a);
//        auto hb = std::hash<GameObject*>()(pair.b);
//        return ha ^ hb;
//    }
//};
//
//class CollisionManager {
//public:
//	// シングルトンインスタンスを取得
//    static CollisionManager& GetInstance();
//
//    // GameObjectを登録
//    void Register(GameObject* obj);
//    void Unregister(GameObject* obj);
//    // 当たり判定のチェック
//    void CheckAllCollisions();
//
//private:
//    std::vector<GameObject*> gameObjects_;  // 登録されたGameObject
//    std::unordered_map<CollisionPair, bool, CollisionPairHash> previousCollisions_; // 前フレームの衝突情報
//
//private: // シングルトンインスタンス
//	static CollisionManager* instance_;
//
//	CollisionManager() = default;
//    ~CollisionManager() = default;
//    CollisionManager(const CollisionManager&) = delete;
//    CollisionManager& operator=(const CollisionManager&) = delete;
//};
