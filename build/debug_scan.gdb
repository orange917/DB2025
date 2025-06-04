# debug_scan.gdb - 用于调试RmScan中记录扫描问题的脚本

# 设置输出不限制元素数量
set print elements 0
set print pretty on

# 在check_equal函数中设置断点
break check_equal

# 运行程序到断点处
run

# 创建一个函数来打印mock中所有记录
define print_mock_records
  printf "\n======= Mock中的所有记录 (%d条) =======\n", mock.size()
  set $it = mock.begin()
  set $end = mock.end()
  set $i = 0
  while $it != $end
    set $rid = (*$it).first
    printf "记录 %3d: page_no = %3d, slot_no = %3d\n", $i, $rid.page_no, $rid.slot_no
    set $it = $it + 1
    set $i = $i + 1
  end
  printf "==================================\n\n"
end

# 创建一个函数进行RmScan扫描并打印每一步
define debug_scan
  printf "\n======= 开始RmScan扫描过程 =======\n"
  # 创建一个新的RmScan对象
  set $debug_scan = RmScan(file_handle)
  set $count = 0
  
  # 打印初始RID
  printf "初始RID: page_no = %d, slot_no = %d\n", $debug_scan.rid().page_no, $debug_scan.rid().slot_no
  
  # 循环直到扫描结束
  while !$debug_scan.is_end()
    set $current_rid = $debug_scan.rid()
    set $in_mock = 0
    
    # 检查当前RID是否在mock中
    set $mock_it = mock.begin()
    set $mock_end = mock.end()
    while $mock_it != $mock_end
      set $mock_rid = (*$mock_it).first
      if $mock_rid.page_no == $current_rid.page_no && $mock_rid.slot_no == $current_rid.slot_no
        set $in_mock = 1
        break
      end
      set $mock_it = $mock_it + 1
    end
    
    printf "扫描记录 %3d: page_no = %3d, slot_no = %3d, 在mock中: %s\n", $count, $current_rid.page_no, $current_rid.slot_no, $in_mock ? "是" : "否"
    
    # 继续扫描下一条记录
    call $debug_scan.next()
    set $count = $count + 1
  end
  
  printf "扫描结束，共找到 %d 条记录 (mock.size() = %d)\n", $count, mock.size()
  printf "==================================\n\n"
end

# 创建一个函数来检查哪些记录被跳过
define find_missed_records
  printf "\n======= 检查被跳过的记录 =======\n"
  set $missed_count = 0
  
  # 遍历mock中所有记录
  set $mock_it = mock.begin()
  set $mock_end = mock.end()
  while $mock_it != $mock_end
    set $mock_rid = (*$mock_it).first
    
    # 创建一个RmScan来检查是否能找到此记录
    set $check_scan = RmScan(file_handle)
    set $found = 0
    
    while !$check_scan.is_end() && !$found
      set $scan_rid = $check_scan.rid()
      if $scan_rid.page_no == $mock_rid.page_no && $scan_rid.slot_no == $mock_rid.slot_no
        set $found = 1
        break
      end
      call $check_scan.next()
    end
    
    if !$found
      printf "被跳过的记录: page_no = %3d, slot_no = %3d\n", $mock_rid.page_no, $mock_rid.slot_no
      set $missed_count = $missed_count + 1
    end
    
    set $mock_it = $mock_it + 1
  end
  
  if $missed_count == 0
    printf "没有发现被跳过的记录\n"
  else
    printf "总共有 %d 条记录被跳过\n", $missed_count
  end
  printf "==================================\n\n"
end

# 检查Bitmap::next_bit的行为
define check_next_bit_behavior
  printf "\n======= 测试Bitmap::next_bit行为 =======\n"
  
  # 对部分页面进行测试
  set $test_pages = 3
  set $page_no = RM_FIRST_RECORD_PAGE
  
  while $page_no < file_handle->file_hdr_.num_pages && $page_no < $test_pages+RM_FIRST_RECORD_PAGE
    printf "测试页面 %d:\n", $page_no
    
    # 获取页面句柄
    set $ph = file_handle->fetch_page_handle($page_no)
    
    # 从-1开始，逐步调用next_bit查看结果
    set $slot = -1
    set $i = 0
    while $i < 5  # 最多找5个位
      set $next_slot = Bitmap::next_bit(true, $ph.bitmap, file_handle->file_hdr_.num_records_per_page, $slot)
      
      if $next_slot >= file_handle->file_hdr_.num_records_per_page
        printf "  从slot %3d开始找，结果: 没有更多记录\n", $slot
        break
      end
      
      # 检查这个位是否真的被设置
      set $is_set = Bitmap::test($ph.bitmap, $next_slot)
      printf "  从slot %3d开始找，结果: slot = %3d, 位图中设置: %s\n", $slot, $next_slot, $is_set ? "是" : "否"
      
      # 探索下一次查找的起始位置
      set $slot = $next_slot
      
      # 再尝试找下一个位置
      set $next_slot = Bitmap::next_bit(true, $ph.bitmap, file_handle->file_hdr_.num_records_per_page, $slot)
      printf "  如果从slot %3d开始找，结果: %s\n", 
             $slot, 
             ($next_slot < file_handle->file_hdr_.num_records_per_page) ? "找到下一条记录" : "没有更多记录"
      
      # 更新起始位置
      set $slot = $next_slot
      set $i = $i + 1
    end
    
    # 释放页面
    call file_handle->buffer_pool_manager_->unpin_page({.fd=file_handle->fd_, .page_no=$page_no}, false)
    
    set $page_no = $page_no + 1
    printf "\n"
  end
  
  printf "==================================\n"
end

# 执行所有调试函数
print_mock_records
debug_scan
find_missed_records
check_next_bit_behavior

# 提供修复建议
printf "\n根据以上调试信息，可能的问题修复建议:\n"
printf "1. 检查RmScan::next()方法中对next_bit的调用参数\n"
printf "2. 确保每次调用next()时从当前slot_no+1开始查找\n"
printf "3. 检查Bitmap::next_bit的实现是否正确\n"

# 显示相关函数定义
printf "\n要确认修复，请检查RmScan::next()方法，可能需要将:\n"
printf "rid_.slot_no = Bitmap::next_bit(true, ph.bitmap, file_handle_->file_hdr_.num_records_per_page, rid_.slot_no);\n\n"
printf "修改为:\n"
printf "int next_slot = rid_.slot_no + 1;  // 从下一个位置开始查找\n"
printf "rid_.slot_no = Bitmap::next_bit(true, ph.bitmap, file_handle_->file_hdr_.num_records_per_page, next_slot);\n\n"

# 继续调试
printf "\n输入'c'继续执行程序，或'q'退出调试\n"