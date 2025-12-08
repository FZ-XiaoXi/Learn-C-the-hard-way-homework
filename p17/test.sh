#!/bin/bash
set -e

echo "=== 开始测试 ex17 ==="

# 清理
rm -f test.db

# 测试创建数据库
echo "测试 1: 创建数据库"
./ex17 test.db c 10 256

# 测试设置数据
echo "测试 2: 设置数据"
./ex17 test.db s 1 "Alice" "alice@example.com"
./ex17 test.db s 2 "Bob" "bob@example.com"

# 测试获取数据
echo "测试 3: 获取数据"
./ex17 test.db g 1 | grep -q "Alice" || (echo "获取Alice失败" && exit 1)
./ex17 test.db g 2 | grep -q "Bob" || (echo "获取Bob失败" && exit 1)

# 测试列出数据
echo "测试 4: 列出数据"
./ex17 test.db l

# 测试删除数据
echo "测试 5: 删除数据"
./ex17 test.db d 1
./ex17 test.db g 1 && echo "错误: 已删除的数据仍可获取" && exit 1 || true

echo "=== 所有测试通过 ==="
