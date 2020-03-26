#ifndef LBVH_HPP
#define LBVH_HPP

#include "acceleration/acceleration_abstract.hpp"

class LBVH:public Acceleration{
private:
    struct Node{
        Node(){
            child[0] = nullptr;
            child[1] = nullptr;
            primitive = nullptr;
        }
        ~Node(){
            delete child[0];
            delete child[1];
        }
        AABB aabb;
        Node* child[2];
        
        //for leaf node
        Primitive* primitive;
        uint64_t morton;
    };
    Node* m_root;
public:
    LBVH();
    ~LBVH();
    void Construct(const std::vector<Primitive*>& primitives) override;
    bool Traversal(Hitpoint& hp,const Ray& ray) const override;
private:
    void CalMortonCode(std::vector<Node*>& leaves);
    void SortLeafNodes(std::vector<Node*>& leaves);
    LBVH::Node* CreateNodeTree(const std::vector<Node*>& leaves,size_t begin,size_t end);
    void CreateInternalAABB(Node* node);
    bool Traversal(Hitpoint& hp,const Ray& ray,const Node* node) const;
    void PrintAABBRange(const Node* node);
};

#endif // LBVH_HPP
