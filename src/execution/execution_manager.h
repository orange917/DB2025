/* Copyright (c) 2023 Renmin University of China
RMDB is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
        http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include <cassert>
#include <cstring>
#include <memory>
#include <string>
#include <vector>

#include "execution_defs.h"
#include "record/rm.h"
#include "system/sm.h"
#include "common/context.h"
#include "common/common.h"
#include "optimizer/plan.h"
#include "executor_abstract.h"
#include "transaction/transaction_manager.h"
#include "optimizer/planner.h"

class Planner;
class AggPlan; // 前向声明

class QlManager {
   private:
    SmManager *sm_manager_;
    TransactionManager *txn_mgr_;
    Planner *planner_;

   public:
    QlManager(SmManager *sm_manager, TransactionManager *txn_mgr, Planner *planner) 
        : sm_manager_(sm_manager),  txn_mgr_(txn_mgr), planner_(planner) {}

    void run_mutli_query(std::shared_ptr<Plan> plan, Context *context);
    void run_cmd_utility(std::shared_ptr<Plan> plan, txn_id_t *txn_id, Context *context);
    void select_from(std::unique_ptr<AbstractExecutor> executorTreeRoot, std::vector<TabCol> sel_cols,
                        Context *context);

    void run_dml(std::unique_ptr<AbstractExecutor> exec);
    
    // 创建聚合执行器
    std::unique_ptr<AbstractExecutor> create_aggregation_executor(
        std::unique_ptr<AbstractExecutor> prev,
        const std::vector<AggFunc>& agg_funcs,
        const std::vector<TabCol>& group_by_cols,
        const std::vector<Condition>& having_conds,
        int limit_val);
    
    // 从AggPlan创建聚合执行器
    std::unique_ptr<AbstractExecutor> create_executor_from_agg_plan(
        std::shared_ptr<AggPlan> agg_plan,
        std::unique_ptr<AbstractExecutor> sub_executor);
};
