#include "acceleration/lbvh.hpp"

LBVH::LBVH():m_root(nullptr){}

LBVH::~LBVH(){
    delete m_root;
}

void LBVH::Construct(const std::vector<Primitive*>& primitives){
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
        leaves[i]->primitive = primitives[i];
    }

    //calculate morton code
    CalMortonCode(leaves);

    //sort leaf nodes
    SortLeafNodes(leaves);
    
    //create node tree
    m_root = CreateNodeTree(leaves,0,leaves.size()-1);

    //create internal AABB
    CreateInternalAABB(m_root);
}

bool LBVH::Traversal(Hitpoint& hp,const Ray& ray) const{
    if (m_root != nullptr){
        return Traversal(hp,ray,m_root);
    }else{
        return false;
    }
}


void LBVH::CalMortonCode(std::vector<Node*>& leaves){
    //check leaves size
    if (leaves.size() == 0){
        std::cout << "lbvh.cpp:LBVH::CalMortonCode:size of leaves is 0" << "\n";
        std::terminate();
    }
    
    //world aabb
    AABB world = leaves[0]->aabb;
    for (size_t i = 1;i < leaves.size();i++){
        world.Merge(world,leaves[i]->aabb);
    }
    
    //world range
    FLOAT world_range_x[2],world_range_y[2],world_range_z[2];
    world.GetRange(world_range_x,world_range_y,world_range_z);

    //world size
    FLOAT world_size[3];
    world.CalSize(world_size);
    
    //morton code
    for (size_t i = 0;i < leaves.size();i++){
        //cell id
        uint16_t cell_id[3] = {0};
        
        //center
        FLOAT center[3];
        leaves[i]->aabb.CalCenter(center);

        //axis x
        if (world_size[0] != 0){
            cell_id[0] = UINT16_MAX*(center[0]-world_range_x[0])/world_size[0];
        }

        //axis y
        if (world_size[1] != 0){
            cell_id[1] = UINT16_MAX*(center[1]-world_range_y[0])/world_size[1];
        }

        //axis z
        if (world_size[2] != 0){
            cell_id[2] = UINT16_MAX*(center[2]-world_range_z[0])/world_size[2];
        }

        //morton code
        leaves[i]->morton = morton_code(cell_id[0],cell_id[1],cell_id[2]);
    }
}

void LBVH::SortLeafNodes(std::vector<Node*>& leaves){
    //create buffer
    std::vector<Node*>* buf1 = new std::vector<Node*>(leaves.size());
    std::vector<Node*>* buf2 = new std::vector<Node*>(leaves.size());
    *buf1 = leaves;

    //radix sort using morton code
    for (uint8_t bit = 0;bit < 64;bit++){
        //count bit number 0
        size_t count = 0;
        for (size_t i = 0;i < leaves.size();i++){
            if (bit_number((*buf1)[i]->morton,bit) == 0){
                count++;
            }
        }

        //reorder
        size_t offset[2] = {0};
        for (size_t i = 0;i < leaves.size();i++){
            if (bit_number((*buf1)[i]->morton,bit) == 0){
                size_t index = offset[0];
                (*buf2)[index] = (*buf1)[i];
                offset[0]++;
            }else{
                size_t index = count+offset[1];
                (*buf2)[index] = (*buf1)[i];
                offset[1]++;
            }
        }

        //swap buffer
        std::vector<Node*>* tmp = buf1;
        buf1 = buf2;
        buf2 = tmp;
    }

    //reorder leaves
    leaves = *buf1;

    //delete buffer
    delete buf1;
    delete buf2;
}

LBVH::Node* LBVH::CreateNodeTree(const std::vector<Node*>& leaves,size_t begin,size_t end){
    //error
    if (begin < 0 || begin >= leaves.size() ||
        end < 0   || end >= leaves.size()   ||
        begin > end)
    {
        std::cout << "lbvh.cpp:LBVH::CreateNodeTree:begin and end are in invalid range" << "\n";
        std::terminate();
    }
    
    //begin == end
    if (begin == end){
        return leaves[begin];
    }
    
    //begin < end
    //initial dividing index
    size_t di = begin;
    
    //initial common bit length
    uint8_t cbl = common_bit_length(leaves[begin+0]->morton,leaves[begin+1]->morton);
    
    //search first differing bit of morton code
    for (size_t i = begin+1;i < end;i++){
        uint8_t tmp = common_bit_length(leaves[i]->morton,leaves[i+1]->morton);
        if (cbl != tmp){
            di = i;
            break;
        }
    }
    
    //create node
    Node* node = new Node();
    node->child[0] = CreateNodeTree(leaves,begin,di);
    node->child[1] = CreateNodeTree(leaves,di+1,end);
    return node;
}

void LBVH::CreateInternalAABB(Node* node){
    //null check
    if (node == nullptr){
        std::cout << "lbvh.cpp:LBVH::CreateInternalAABB:node is nullptr" << "\n";
        std::terminate();
    }
    
    //create internal aabb
    if (node->primitive == nullptr){
        if (node->child[0] != nullptr && node->child[1] != nullptr){
            CreateInternalAABB(node->child[0]);
            CreateInternalAABB(node->child[1]);
            node->aabb.Merge(node->child[0]->aabb,node->child[1]->aabb);
        }else{
            std::cout << "lbvh.cpp:LBVH::CreateInternalAABB:internal node must have two child nodes" << "\n";
            std::terminate();
        }
    }
}

bool LBVH::Traversal(Hitpoint& hp,const Ray& ray,const Node* node) const{
    //null check
    if (node == nullptr){
        std::cout << "bvh.cpp:LBVH::Traversal:node is nullptr" << "\n";
        std::terminate();
    }

    //parent node
    if (!node->aabb.Hit(ray)) return false;

    //child node
    if (node->primitive == nullptr){
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
            std::cout << "lbvh.cpp:LBVH::Traversal:internal node must have two child nodes" << "\n";
            std::terminate();
        }
    }else{
        //hit primitive or not
        if (node->primitive->Hit(hp,ray)){
            return true;
        }else{
            return false;
        }
    }
}


void LBVH::PrintAABBRange(const Node* node){
    if (node == nullptr){
        return;
    }
    
    //print parent node
    FLOAT range_x[2],range_y[2],range_z[2];
    node->aabb.GetRange(range_x,range_y,range_z);
    if (node->primitive == nullptr){
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
