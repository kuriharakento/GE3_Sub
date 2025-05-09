//#include "CollisionManager.h"
//
//CollisionManager& CollisionManager::GetInstance() {
//    static CollisionManager instance;
//    return instance;
//}
//
//void CollisionManager::Register(GameObject* obj) {
//    gameObjects_.push_back(obj);
//}
//
//void CollisionManager::Unregister(GameObject* obj) {
//    gameObjects_.erase(std::remove(gameObjects_.begin(), gameObjects_.end(), obj), gameObjects_.end());
//}
//
//void CollisionManager::CheckAllCollisions() {
//    std::unordered_map<CollisionPair, bool, CollisionPairHash> currentCollisions;
//
//    for (size_t i = 0; i < gameObjects_.size(); ++i) {
//        auto* objA = gameObjects_[i];
//        auto compA = objA->GetCollisionComponent();
//        if (!compA) continue;
//
//        for (size_t j = i + 1; j < gameObjects_.size(); ++j) {
//            auto* objB = gameObjects_[j];
//            if (objA == objB) continue;
//
//            auto compB = objB->GetCollisionComponent();
//            if (compB == nullptr) continue;
//
//            auto colliderA = compA->GetCollider();
//            auto colliderB = compB->GetCollider();
//            if (!colliderA || !colliderB) continue;
//
//            CollisionPair pair{ objA, objB };
//            bool isColliding = colliderA->CheckCollision(colliderB.get());
//
//            if (isColliding) {
//                currentCollisions[pair] = true;
//
//                if (previousCollisions_.find(pair) == previousCollisions_.end()) {
//                    compA->OnCollisionEnter(objB);
//                    compB->OnCollisionEnter(objA);
//                }
//                else {
//                    compA->OnCollisionStay(objB);
//                    compB->OnCollisionStay(objA);
//                }
//            }
//            else {
//                if (previousCollisions_.find(pair) != previousCollisions_.end()) {
//                    compA->OnCollisionExit(objB);
//                    compB->OnCollisionExit(objA);
//                }
//            }
//        }
//    }
//
//    previousCollisions_ = std::move(currentCollisions);
//}
