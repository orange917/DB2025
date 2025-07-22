# --- 4. name字段测试---
cat > name_test.sql << 'EOF'

EOF

echo "Adding indexes to the table..."
./rmdb_client < add_indexes.sql > /dev/null
echo "Indexes created."
echo ""