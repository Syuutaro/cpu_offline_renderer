#ifndef SAHBVH_HPP
#define SAHBVH_HPP

#include "acceleration/acceleration_abstract.hpp"

class SAHBVH:public Acceleration{
private:
    struct Node{
        Node(){
            child[0] = nullptr;
            child[1] = nullptr;
        }
        ~Node(){
            delete child[0];
            delete child[1];
        }
        AABB aabb;
        Node* child[2];
            
        //for leaf node
        std::vector<Primitive*> primitives;
    };
    Node* m_root;
public:
    SAHBVH();
    ~SAHBVH();
    void Construct(const std::vector<Primitive*>& primitives) override;
    bool Traversal(Hitpoint& hp,const Ray& ray) const override;
private:
    void SortLeafNodes(std::vector<Node*>& leaves,int axis);
    SAHBVH::Node* CreateNodeTree(std::vector<Node*>& leaves);
    bool Traversal(Hitpoint& hp,const Ray& ray,const Node* node) const;
    void PrintAABBRange(const Node* node);
};


#endif // SAHBVH_HPP
