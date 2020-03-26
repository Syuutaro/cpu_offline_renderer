#include "acceleration/sahbvh.hpp"

SAHBVH::SAHBVH():m_root(nullptr){}

SAHBVH::~SAHBVH(){
    delete m_root;
}

void SAHBVH::Construct(const std::vector<Primitive*>& primitives){
    //check root node
    if (m_root != nullptr) return;

    //check primitives size
    if (primitives.size() == 0){
        return;
    }

    //create leaf nodes
    std::vector<Node*> leaves(primitives.size());
    for (size_t i = 0;i < primitives.size();i++){
        leaves[i] = new Node();
        
        //set aabb
        primitives[i]->CreateAABB(leaves[i]->aabb);
        
        //set primitive
        leaves[i]->primitives.push_back(primitives[i]);
    }

    //create node tree
    m_root = CreateNodeTree(leaves);

    //release leaves
    for (size_t i = 0;i < leaves.size();i++){
        delete leaves[i];
    }
}

bool SAHBVH::Traversal(Hitpoint& hp,const Ray& ray) const{
    if (m_root != nullptr){
        return Traversal(hp,ray,m_root);
    }else{
        return false;
    }
}


//merge sort
void SAHBVH::SortLeafNodes(std::vector<Node*>& leaves,int axis){

    //leaves size
    const size_t leaves_size = leaves.size();
    
    //leaves size == 0 or 1
    if (leaves_size == 0 || leaves_size == 1){
        return;
    }
    
    //dividing index
    const size_t di = 0.5*(leaves_size-1);
    
    //create children
    std::vector<Node*> child1(di+1);
    std::vector<Node*> child2(leaves_size-di-1);
    for (size_t i = 0;i < leaves_size;i++){
        if (i <= di){
            child1[i] = leaves[i];
        }else{
            child2[i-di-1] = leaves[i];
        }
    }
  
    //sort children
    SortLeafNodes(child1,axis);
    SortLeafNodes(child2,axis);
    
    //merge children
    size_t cur_child1_idx = 0;
    size_t cur_child2_idx = 0;
    const size_t child1_size = child1.size();
    const size_t child2_size = child2.size();
    for (size_t i = 0;i < leaves_size;i++){
        if (cur_child1_idx < child1_size && cur_child2_idx < child2_size){
            FLOAT center1[3];
            FLOAT center2[3];
            child1[cur_child1_idx]->aabb.CalCenter(center1);
            child2[cur_child2_idx]->aabb.CalCenter(center2);
            
            if (axis == 0){
                if (center1[0] <= center2[0]){
                    leaves[i] = child1[cur_child1_idx];
                    cur_child1_idx++;
                }else{
                    leaves[i] = child2[cur_child2_idx];
                    cur_child2_idx++;
                }
            }else if (axis == 1){
                if (center1[1] <= center2[1]){
                    leaves[i] = child1[cur_child1_idx];
                    cur_child1_idx++;
                }else{
                    leaves[i] = child2[cur_child2_idx];
                    cur_child2_idx++;
                }
            }else{
                if (center1[2] <= center2[2]){
                    leaves[i] = child1[cur_child1_idx];
                    cur_child1_idx++;
                }else{
                    leaves[i] = child2[cur_child2_idx];
                    cur_child2_idx++;
                }
            }
        }else if (cur_child1_idx < child1_size){
            leaves[i] = child1[cur_child1_idx];
            cur_child1_idx++;
        }else if (cur_child2_idx < child2_size){
            leaves[i] = child2[cur_child2_idx];
            cur_child2_idx++;
        }else{
            break;
        }
    }
}

SAHBVH::Node* SAHBVH::CreateNodeTree(std::vector<Node*>& leaves){
    //leaves size
    const size_t leaves_size = leaves.size();
    
    //leaves size == 0
    if (leaves_size == 0){
        std::cout << "sahbvh.cpp:SAHBVH::CreateNodeTree:size of leaves is 0" << "\n";
        std::terminate();
    }
    
    //leaves size == 1
    if (leaves_size == 1){
        Node* node = new Node();
        *node = *leaves[0];
        return node;
    }
    
    //time of intersection test(room for improvement)
    const FLOAT T_aabb = 0.1;
    const FLOAT T_prim = 1;
    
    //best cost,best axis,best dividing index
    int best_cost = T_prim*leaves_size;
    int best_axis = -1;
    size_t best_di = 0;
    
    //for each axis
    for (int a = 0;a < 3;a++){
        //sort leaves in current axis
        SortLeafNodes(leaves,a);
        
        //merged aabb from left and right
        std::vector<AABB> merged_aabb_left(leaves_size);
        std::vector<AABB> merged_aabb_right(leaves_size);
        
        //sweep from left
        merged_aabb_left[0] = leaves[0]->aabb;
        for (size_t i = 1;i < leaves_size;i++){
            merged_aabb_left[i].Merge(merged_aabb_left[i-1],leaves[i]->aabb);
        }
        
        //sweep from right
        merged_aabb_right[leaves_size-1] = leaves[leaves_size-1]->aabb;
        for (size_t i = leaves_size-1;i >= 1;i--){
            //merge
            if (i < leaves_size-1){
                merged_aabb_right[i].Merge(merged_aabb_right[i+1],leaves[i]->aabb);
            }
            
            //check sah cost
            const FLOAT A_S = merged_aabb_left[leaves_size-1].CalSurfaceArea();
            const FLOAT A_S1 = merged_aabb_left[i-1].CalSurfaceArea();
            const FLOAT A_S2 = merged_aabb_right[i].CalSurfaceArea();
            const FLOAT sah_cost = 2*T_aabb+T_prim/A_S*(A_S1*i+A_S2*(leaves_size-i));
            if (sah_cost < best_cost){
                best_cost = sah_cost;
                best_axis = a;
                best_di = i-1;
            }
        }
    }
    
    //division or not
    if (best_axis == -1){
        Node* node = new Node();
        *node = *leaves[0];
        node->primitives.resize(leaves_size);
        for (size_t i = 1;i < leaves_size;i++){
            node->primitives[i] = leaves[i]->primitives[0];
            node->aabb.Merge(node->aabb,leaves[i]->aabb);
        }
        return node;
    }else{
        //sort leaves in best axis
        SortLeafNodes(leaves,best_axis);
        
        //division
        std::vector<Node*> child1(best_di+1);
        std::vector<Node*> child2(leaves_size-best_di-1);
        for (size_t i = 0;i < leaves_size;i++){
            if (i <= best_di){
                child1[i] = leaves[i];
            }else{
                child2[i-best_di-1] = leaves[i];
            }
        }
        
        //create node
        Node* node = new Node();
        node->child[0] = CreateNodeTree(child1);
        node->child[1] = CreateNodeTree(child2);
        node->aabb.Merge(node->child[0]->aabb,node->child[1]->aabb);
        return node;
    }
}

bool SAHBVH::Traversal(Hitpoint& hp,const Ray& ray,const Node* node) const{
    //null check
    if (node == nullptr){
        std::cout << "sahbvh.cpp:SAHBVH::Traversal:node is nullptr" << "\n";
        std::terminate();
    }

    //parent node
    if (!node->aabb.Hit(ray)) return false;

    //child node
    if (node->primitives.size() == 0){
        if (node->child[0] != nullptr && node->child[1] != nullptr){
            Hitpoint hp1,hp2;
            bool result1 = Traversal(hp1,ray,node->child[0]);
            bool result2 = Traversal(hp2,ray,node->child[1]);
            if (result1 && result2){
                hp = (hp1.t < hp2.t)? hp1:hp2;
                return true;
            }else if (result1){
                hp = hp1;
                return true;
            }else if (result2){
                hp = hp2;
                return true;
            }else{
                return false;
            }
        }else{
            std::cout << "sahbvh.cpp:SAHBVH::Traversal:internal node must have two child nodes" << "\n";
            std::terminate();
        }
    }else{
        //hit primitive or not
        Hitpoint hp1,hp2;
        hp1.t = -1;
        for (size_t i = 0;i < node->primitives.size();i++){
            if (node->primitives[i]->Hit(hp2,ray)){
                if (hp1.t == -1){
                    hp1 = hp2;
                }else{
                    if (hp2.t < hp1.t){
                        hp1 = hp2;
                    }
                }
            }
        }
        if (hp1.t != -1){
            hp = hp1;
            return true;
        }else{
            return false;
        }
    }
}


void SAHBVH::PrintAABBRange(const Node* node){
    if (node == nullptr){
        return;
    }
    
    //print parent node
    FLOAT range_x[2],range_y[2],range_z[2];
    node->aabb.GetRange(range_x,range_y,range_z);
    if (node->primitives.size() == 0){
        std::cout << "node type:internal" << "\n";
    }else{
        std::cout << "node type:leaf" << "\n";
    }
    std::cout << "node address:" << node << "\n";
    std::cout << "range x:" << range_x[0] << " " << range_x[1] << "\n";
    std::cout << "range y:" << range_y[0] << " " << range_y[1] << "\n";
    std::cout << "range z:" << range_z[0] << " " << range_z[1] << "\n";
    std::cout << "\n";
    
    //print child nodes
    PrintAABBRange(node->child[0]);
    PrintAABBRange(node->child[1]);
}
