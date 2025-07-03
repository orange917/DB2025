#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstring>

// 简化的结构定义
struct IxPageHdr {
    int next_free_page_no;
    int parent;
    int num_key;
    bool is_leaf;
    int prev_leaf;
    int next_leaf;
};

struct IxFileHdr {
    int tot_len_;
    int first_free_page_no_;
    int num_pages_;
    int root_page_;
    int col_num_;
    int col_tot_len_;
    int btree_order_;
    int keys_size_;
    int first_leaf_;
    int last_leaf_;
};

const int PAGE_SIZE = 4096;
const int IX_FILE_HDR_PAGE = 0;

void print_node_keys(char* page_data, int col_tot_len, const std::string& prefix = "") {
    IxPageHdr* page_hdr = reinterpret_cast<IxPageHdr*>(page_data);
    char* keys = page_data + sizeof(IxPageHdr);
    
    std::cout << prefix << "Node (leaf=" << page_hdr->is_leaf 
              << ", size=" << page_hdr->num_key 
              << ", parent=" << page_hdr->parent << "): ";
    
    for (int i = 0; i < page_hdr->num_key; i++) {
        if (i > 0) std::cout << ", ";
        // 假设key是int类型
        int key_val = *(int*)(keys + i * col_tot_len);
        std::cout << key_val;
    }
    std::cout << std::endl;
    
    // 检查有序性
    bool ordered = true;
    for (int i = 1; i < page_hdr->num_key; i++) {
        int prev_key = *(int*)(keys + (i-1) * col_tot_len);
        int curr_key = *(int*)(keys + i * col_tot_len);
        if (prev_key >= curr_key) {
            std::cout << prefix << "ERROR: NOT ordered at position " << i 
                      << " (prev=" << prev_key << ", curr=" << curr_key << ")" << std::endl;
            ordered = false;
        }
    }
    
    if (ordered && page_hdr->num_key > 1) {
        std::cout << prefix << "✓ Node is properly ordered" << std::endl;
    } else if (page_hdr->num_key <= 1) {
        std::cout << prefix << "- Node has <= 1 key (trivially ordered)" << std::endl;
    }
}

void traverse_btree_file(const std::string& index_file, int page_no, int col_tot_len, int depth = 0) {
    std::string prefix(depth * 2, ' ');
    
    std::ifstream file(index_file, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Cannot open index file: " << index_file << std::endl;
        return;
    }
    
    // 读取页面
    char page_data[PAGE_SIZE];
    file.seekg(page_no * PAGE_SIZE);
    file.read(page_data, PAGE_SIZE);
    
    if (!file.good()) {
        std::cout << prefix << "Error reading page " << page_no << std::endl;
        file.close();
        return;
    }
    
    IxPageHdr* page_hdr = reinterpret_cast<IxPageHdr*>(page_data);
    
    std::cout << prefix << "=== Page " << page_no << " ===" << std::endl;
    print_node_keys(page_data, col_tot_len, prefix);
    
    if (!page_hdr->is_leaf) {
        // 内部节点，递归遍历子节点
        char* keys = page_data + sizeof(IxPageHdr);
        int* rids = reinterpret_cast<int*>(keys + (page_hdr->num_key + 1) * col_tot_len);
        
        std::cout << prefix << "Child pages: ";
        for (int i = 0; i <= page_hdr->num_key; i++) {
            if (i > 0) std::cout << ", ";
            std::cout << rids[i];
        }
        std::cout << std::endl;
        
        // 递归遍历子节点
        for (int i = 0; i <= page_hdr->num_key; i++) {
            int child_page = rids[i];
            if (child_page > 0 && child_page < 1000) { // 简单的边界检查
                traverse_btree_file(index_file, child_page, col_tot_len, depth + 1);
            }
        }
    }
    
    file.close();
}

int main() {
    std::string index_file = "build/t1/warehouse_w_id.idx";
    
    // 读取文件头
    std::ifstream file(index_file, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Cannot open index file: " << index_file << std::endl;
        return 1;
    }
    
    char hdr_data[PAGE_SIZE];
    file.read(hdr_data, PAGE_SIZE);
    file.close();
    
    // 解析文件头（需要正确读取序列化格式）
    int offset = 0;
    int tot_len = *reinterpret_cast<int*>(hdr_data + offset);
    offset += sizeof(int);
    int first_free_page_no = *reinterpret_cast<int*>(hdr_data + offset);
    offset += sizeof(int);
    int num_pages = *reinterpret_cast<int*>(hdr_data + offset);
    offset += sizeof(int);
    int root_page = *reinterpret_cast<int*>(hdr_data + offset);
    offset += sizeof(int);
    int col_num = *reinterpret_cast<int*>(hdr_data + offset);
    offset += sizeof(int);

    // 跳过col_types_数组
    offset += col_num * sizeof(int);

    // 读取col_lens_数组并计算col_tot_len
    int col_tot_len = 0;
    for (int i = 0; i < col_num; i++) {
        int col_len = *reinterpret_cast<int*>(hdr_data + offset);
        col_tot_len += col_len;
        offset += sizeof(int);
    }
    
    std::cout << "=== B+Tree Structure for " << index_file << " ===" << std::endl;
    std::cout << "Root page: " << root_page << std::endl;
    std::cout << "Column number: " << col_num << std::endl;
    std::cout << "Column total length: " << col_tot_len << std::endl;
    std::cout << "File header total length: " << tot_len << std::endl;
    std::cout << "Number of pages: " << num_pages << std::endl;
    std::cout << std::endl;
    
    // 遍历整个B+树
    traverse_btree_file(index_file, root_page, col_tot_len);
    
    return 0;
}
