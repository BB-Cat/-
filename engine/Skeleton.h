//#pragma once
//#include <vector>
//#include <fbxsdk.h> 
//using namespace fbxsdk;
//
//struct Joint {
//    int m_parent_index;
//    const char* m_name;
//    FbxAMatrix m_global_bindpose_inverse;
//    //Keyframe* mAnimation;
//    FbxNode* m_node;
//
//    Joint() :
//        m_node(nullptr)
//        //     mAnimation(nullptr)
//    {
//        m_global_bindpose_inverse.SetIdentity();
//        m_parent_index = -1;
//    }
//
//    ~Joint()
//    {
//        //while (mAnimation)
//        //{
//        //    Keyframe* temp = mAnimation->mNext;
//        //    delete mAnimation;
//        //    mAnimation = temp;
//        //}
//    }
//};
//
//struct Skeleton {
//    std::vector<Joint> m_joints;
//};